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
   TOKEN_ASSIGNMENT,
   TOKEN_EQUAL,
   TOKEN_NOT,
   TOKEN_NOTEQUAL,
   TOKEN_GT,
   TOKEN_LT,
   TOKEN_GTE,
   TOKEN_LTE,

   TOKEN_STRUCT,
   TOKEN_FUNCTION,
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
static lexical_token tokens[1024];

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
         case ',': {token->kind = TOKEN_COMMA;} break;
         case '.': {token->kind = TOKEN_PERIOD;} break;
         case ';': {token->kind = TOKEN_SEMICOLON;} break;
         case '-': {token->kind = TOKEN_MINUS;} break;
         case '+': {token->kind = TOKEN_PLUS;} break;
         case '*': {token->kind = TOKEN_ASTERISK;} break;
         case '/': {token->kind = TOKEN_SLASH;} break;

         case ':': {token->kind = TOKEN_COLON;} break;

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

         default: {token->kind = TOKEN_IDENTIFIER;} break;
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
