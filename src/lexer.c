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
   token_kind kind;
   string lexeme;
   union
   {
      u64 integer_value;
      double float_value;
   };

   int line;
} lexical_token;

static int token_count;
static lexical_token tokens[1 << 24];

static void consume_whitespace(char **code, int *line)
{
   while(**code)
   {
      char c = **code;

      if(c == ' ' || c == '\t' || c == '\f' || c == '\v')
      {
         // NOTE: Skip it.
      }
      else if(c == '\n')
      {
         (*line)++;
      }
      else
      {
         break;
      }

      (*code)++;
   }
}

static bool is_digit(char c)
{
   return (c >= '0' && c <= '9');
}

static bool is_identifier(char c)
{
   return ((c >= '0' && c <= '9') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c == '_'));
}

static void lex(memarena *perma, memarena trans, char *code)
{
   int line = 1;

   while(*code)
   {
      consume_whitespace(&code, &line);
      if(*code == 0)
      {
         break;
      }

      lexical_token *token = tokens + token_count++;
      token->line = line;
      token->lexeme = (string){code, 1};

      switch(*code)
      {
         case '(': {token->kind = TOKEN_PAREN_OPEN;} break;
         case ')': {token->kind = TOKEN_PAREN_CLOSE;} break;
         case '[': {token->kind = TOKEN_BRACKET_OPEN;} break;
         case ']': {token->kind = TOKEN_BRACKET_CLOSE;} break;
         case '{': {token->kind = TOKEN_BRACE_OPEN;} break;
         case '}': {token->kind = TOKEN_BRACE_CLOSE;} break;

         case ',': {token->kind = TOKEN_COMMA;} break;
         case '.': {token->kind = TOKEN_PERIOD;} break;
         case ';': {token->kind = TOKEN_SEMICOLON;} break;
         case '-': {token->kind = TOKEN_MINUS;} break;
         case '+': {token->kind = TOKEN_PLUS;} break;
         case '*': {token->kind = TOKEN_ASTERISK;} break;
         case '/': {token->kind = TOKEN_SLASH;} break;
         case '=': {token->kind = TOKEN_EQUAL;} break;

         case ':': {
            token->kind = TOKEN_COLON;
            if(code[1])
            {
               if(code[1] == '=')
               {
                  token->kind = TOKEN_COLONEQUAL;
                  token->lexeme.length = 2;
               }
               else if(code[1] == ':')
               {
                  token->kind = TOKEN_COLONCOLON;
                  token->lexeme.length = 2;
               }
            }
         } break;

         case '!': {
            token->kind = TOKEN_NOT;
            if(code[1] && code[1] == '=')
            {
               token->kind = TOKEN_NOTEQUAL;
               token->lexeme.length = 2;
            }
         } break;


         case '>': {
            token->kind = TOKEN_GT;
            if(code[1] && code[1] == '=')
            {
               token->kind = TOKEN_GTE;
               token->lexeme.length = 2;
            }
         } break;

         case '<': {
            token->kind = TOKEN_LT;
            if(code[1] && code[1] == '=')
            {
               token->kind = TOKEN_LTE;
               token->lexeme.length = 2;
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
            memindex length = 0;
            while(code[length] && is_identifier(code[length]))
            {
               length++;
            }

            token->lexeme.length = length;

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
            token->kind = TOKEN_STRING;

            memindex length = 1;
            while(code[length] && code[length] != '"')
            {
               length++;
            }
            while(code[length] && code[length] == '"')
            {
               length++;
            }

            token->lexeme.length = length;
         } break;

         case '1': case '2': case '3': case '4': case '5':
         case '6': case '7': case '8': case '9': case '0':
         {
            memindex length = 0;
            while(code[length] && is_digit(code[length]))
            {
               length++;
            }

            if(code[length] == '.')
            {
               token->kind = TOKEN_FLOAT;
               length++;

               while(code[length] && is_digit(code[length]))
               {
                  length++;
               }
            }
            else
            {
               token->kind = TOKEN_INTEGER;
            }

            token->lexeme.length = length;
         } break;

         default: {token->kind = TOKEN_NIL;} break;
      }

      code += token->lexeme.length;
   }

   for(int index = 0; index < token_count; ++index)
   {
      lexical_token *token = tokens + index;
      printf("[TOKEN %2d, Line %d]: %.*s\n",
             token->kind,
             token->line,
             (int)token->lexeme.length,
             token->lexeme.data);
   }
}
