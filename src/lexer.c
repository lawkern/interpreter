/* (c) copyright 2025 Lawrence D. Kern ////////////////////////////////////// */

typedef enum {
   TOKEN_PAREN_OPEN,
   TOKEN_PAREN_CLOSE,
   TOKEN_BRACKET_OPEN,
   TOKEN_BRACKET_CLOSE,
   TOKEN_BRACE_OPEN,
   TOKEN_BRACE_CLOSE,

   TOKEN_COMMA,
   TOKEN_PERIOD,
   TOKEN_SEMICOLON,
   TOKEN_MINUS,
   TOKEN_PLUS,
   TOKEN_SLASH,
   TOKEN_ASTERISK,

   TOKEN_IDENTIFIER,
   TOKEN_STRING,
   TOKEN_INTEGER,
   TOKEN_FLOAT,

   TOKEN_COLON,
   TOKEN_COLONEQUAL,
   TOKEN_COLONCOLON,
   TOKEN_EQUAL,
   TOKEN_NOT,
   TOKEN_NOTEQUAL,
   TOKEN_GT,
   TOKEN_LT,
   TOKEN_GTE,
   TOKEN_LTE,

   TOKEN_STRUCT,
   TOKEN_GLOBAL,
   TOKEN_PROGRAM,
   TOKEN_SUBROUTINE,
   TOKEN_IF,
   TOKEN_ELSE,
   TOKEN_FOR,
   TOKEN_WHILE,
   TOKEN_AND,
   TOKEN_OR,
   TOKEN_RETURN,
   TOKEN_TRUE,
   TOKEN_FALSE,
   TOKEN_NIL,

   TOKEN_COUNT,
} token_kind;

typedef struct {
   int line;
   int column;
} token_location;

typedef struct {
   token_kind kind;
   token_location location;

   string lexeme;
   union
   {
      u64 integer_value;
      double float_value;
   };
} lexical_token;

typedef struct {
   token_location location;

   memindex index;
   memindex count;

   char *text;
} text_stream;

static int token_count;
static lexical_token tokens[1 << 24];

static bool is_decimal(char c)
{
   return(c >= '0' && c <= '9');
}

static bool is_hexadecimal(char c)
{
   return((c >= '0' && c <= '9') ||
          (c >= 'a' && c <= 'f') ||
          (c >= 'A' && c <= 'F'));
}

static bool is_identifier(char c)
{
   return((c >= '0' && c <= '9') ||
          (c >= 'A' && c <= 'Z') ||
          (c >= 'a' && c <= 'z') ||
          (c == '_'));
}

static bool is_newline(char c)
{
   return(c == '\n');
}

static bool is_whitespace(char c)
{
   return((c == ' ')  ||
          (c == '\t') ||
          (c == '\n') ||
          (c == '\r') ||
          (c == '\f') ||
          (c == '\v'));
}

static char peek_text(text_stream *stream)
{
   // TODO: This does not support embedded zeros. Maybe we care about that?

   char result = 0;
   if(stream->index < stream->count)
   {
      result = stream->text[stream->index];
   }
   return(result);
}

static char advance_text(text_stream *stream)
{
   // assert(stream->index < stream->count);

   char result = stream->text[stream->index++];

   if(is_newline(result))
   {
      stream->location.column = 0;
      stream->location.line++;
   }
   else
   {
      stream->location.column++;
   }

   return(result);
}

// static bool is_text(text_stream *stream, char c)
// {
//    bool result = (peek_text(stream) == c);
//    return(result);
// }

static bool match_text(text_stream *stream, char c)
{
   bool result = (peek_text(stream) == c);
   if(result)
   {
      advance_text(stream);
   }
   return(result);
}

static char expect_text(text_stream *stream, char c)
{
   char result = advance_text(stream);
   if(result != c)
   {
      error(stream->location.line, stream->location.column,
            "Expected '%c', found '%c'.", c, result);
   }
   return(result);
}

static void lex(memarena *perma, memarena trans, text_stream stream)
{
   while(stream.index < stream.count)
   {
      if(encountered_error)
      {
         break;
      }

      while(is_whitespace(peek_text(&stream)))
      {
         advance_text(&stream);
      }

      lexical_token *token = tokens + token_count++;
      token->location = stream.location;
      token->lexeme = (string){stream.text+stream.index, 1};

      switch(peek_text(&stream))
      {
         case 0: {} break;

         case '(': {advance_text(&stream); token->kind = TOKEN_PAREN_OPEN;} break;
         case ')': {advance_text(&stream); token->kind = TOKEN_PAREN_CLOSE;} break;
         case '[': {advance_text(&stream); token->kind = TOKEN_BRACKET_OPEN;} break;
         case ']': {advance_text(&stream); token->kind = TOKEN_BRACKET_CLOSE;} break;
         case '{': {advance_text(&stream); token->kind = TOKEN_BRACE_OPEN;} break;
         case '}': {advance_text(&stream); token->kind = TOKEN_BRACE_CLOSE;} break;

         case ',': {advance_text(&stream); token->kind = TOKEN_COMMA;} break;
         case '.': {advance_text(&stream); token->kind = TOKEN_PERIOD;} break;
         case ';': {advance_text(&stream); token->kind = TOKEN_SEMICOLON;} break;
         case '-': {advance_text(&stream); token->kind = TOKEN_MINUS;} break;
         case '+': {advance_text(&stream); token->kind = TOKEN_PLUS;} break;
         case '*': {advance_text(&stream); token->kind = TOKEN_ASTERISK;} break;
         case '/': {advance_text(&stream); token->kind = TOKEN_SLASH;} break;
         case '=': {advance_text(&stream); token->kind = TOKEN_EQUAL;} break;

         case ':': {
            expect_text(&stream, ':');

            token->kind = TOKEN_COLON;
            if(match_text(&stream, '='))
            {
               token->kind = TOKEN_COLONEQUAL;
            }
            else if(match_text(&stream, ':'))
            {
               token->kind = TOKEN_COLONCOLON;
            }
         } break;

         case '!': {
            expect_text(&stream, '!');

            token->kind = TOKEN_NOT;
            if(match_text(&stream, '='))
            {
               token->kind = TOKEN_NOTEQUAL;
            }
         } break;

         case '>': {
            expect_text(&stream, '>');

            token->kind = TOKEN_GT;
            if(match_text(&stream, '='))
            {
               token->kind = TOKEN_GTE;
            }
         } break;

         case '<': {
            expect_text(&stream, '<');

            token->kind = TOKEN_LT;
            if(match_text(&stream, '='))
            {
               token->kind = TOKEN_LTE;
            }
         } break;

         case '_':
         case 'A': case 'B': case 'C': case 'D': case 'E':
         case 'F': case 'G': case 'H': case 'I': case 'J':
         case 'K': case 'L': case 'M': case 'N': case 'O':
         case 'P': case 'Q': case 'R': case 'S': case 'T':
         case 'U': case 'V': case 'W': case 'X': case 'Y':
         case 'Z':
         case 'a': case 'b': case 'c': case 'd': case 'e':
         case 'f': case 'g': case 'h': case 'i': case 'j':
         case 'k': case 'l': case 'm': case 'n': case 'o':
         case 'p': case 'q': case 'r': case 's': case 't':
         case 'u': case 'v': case 'w': case 'x': case 'y':
         case 'z':
         {
            memindex start = stream.index;
            while(stream.index < stream.count && is_identifier(stream.text[stream.index]))
            {
               advance_text(&stream);
            }
            token->lexeme.length = stream.index - start;

            // TODO: This is obviously silly. Implement string interning.
            if(string_equals(token->lexeme, S("struct")))
            {
               token->kind = TOKEN_STRUCT;
            }
            else if(string_equals(token->lexeme, S("global")))
            {
               token->kind = TOKEN_GLOBAL;
            }
            else if(string_equals(token->lexeme, S("program")))
            {
               token->kind = TOKEN_PROGRAM;
            }
            else if(string_equals(token->lexeme, S("sub")))
            {
               token->kind = TOKEN_SUBROUTINE;
            }
            else if(string_equals(token->lexeme, S("if")))
            {
               token->kind = TOKEN_IF;
            }
            else if(string_equals(token->lexeme, S("else")))
            {
               token->kind = TOKEN_ELSE;
            }
            else if(string_equals(token->lexeme, S("for")))
            {
               token->kind = TOKEN_FOR;
            }
            else if(string_equals(token->lexeme, S("while")))
            {
               token->kind = TOKEN_WHILE;
            }
            else if(string_equals(token->lexeme, S("and")))
            {
               token->kind = TOKEN_AND;
            }
            else if(string_equals(token->lexeme, S("or")))
            {
               token->kind = TOKEN_OR;
            }
            else if(string_equals(token->lexeme, S("return")))
            {
               token->kind = TOKEN_RETURN;
            }
            else if(string_equals(token->lexeme, S("true")))
            {
               token->kind = TOKEN_TRUE;
            }
            else if(string_equals(token->lexeme, S("false")))
            {
               token->kind = TOKEN_FALSE;
            }
            else if(string_equals(token->lexeme, S("nil")))
            {
               token->kind = TOKEN_NIL;
            }
            else
            {
               token->kind = TOKEN_IDENTIFIER;
            }

         } break;

         case '"': {
            expect_text(&stream, '"');

            token->kind = TOKEN_STRING;

            while(peek_text(&stream) != '"')
            {
               advance_text(&stream);
            }
            expect_text(&stream, '"');
         } break;

         case '1': case '2': case '3': case '4': case '5':
         case '6': case '7': case '8': case '9': case '0':
         {
            token->kind = TOKEN_INTEGER;

            if((stream.index + 1) < stream.count &&
               stream.text[stream.index + 0] == '0' &&
               stream.text[stream.index + 1] == 'x')
            {
               stream.index += 2;
               while(stream.index < stream.count && (is_hexadecimal(stream.text[stream.index]) || stream.text[stream.index] == '_'))
               {
                  advance_text(&stream);
               }
            }
            else
            {
               while(stream.index < stream.count && (is_decimal(stream.text[stream.index]) || stream.text[stream.index] == '_'))
               {
                  advance_text(&stream);
               }

               if(stream.text[stream.index] == '.')
               {
                  token->kind = TOKEN_FLOAT;
                  advance_text(&stream);

                  while(stream.index < stream.count && (is_decimal(stream.text[stream.index]) || stream.text[stream.index] == '_'))
                  {
                     advance_text(&stream);
                  }
               }
            }
         } break;

         default: {
            char c = advance_text(&stream);
            error(stream.location.line, stream.location.column, "Unexpected character %c.", c);
         } break;
      }

      token->lexeme.length = (stream.text + stream.index) - token->lexeme.data;
   }
}

static void print_tokens(void)
{
   printf("--\n");
   for(int index = 0; index < token_count; ++index)
   {
      lexical_token *token = tokens + index;
      printf("[TOKEN %2d, (%3d, %3d)]: %.*s\n",
             token->kind,
             token->location.line,
             token->location.column,
             (int)token->lexeme.length,
             token->lexeme.data);
   }
}
