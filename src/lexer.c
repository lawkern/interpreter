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
   TOKEN_PLUS,
   TOKEN_MINUS,
   TOKEN_SEMICOLON,
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

static void lex(memarena *perma, memarena trans, char *code)
{
   int line = 1;

   while(*code)
   {
      if(*code != ' ')
      {
         if(*code == '\n')
         {
            line++;
         }
         else
         {
            lexical_token *token = tokens + token_count++;
            token->kind = TOKEN_IDENTIFIER;
            token->lexeme = (string){code, 1};
            token->line = line;
         }
      }

      code++;
   }

   for(int index = 0; index < token_count; ++index)
   {
      lexical_token *token = tokens + index;
      printf("[Line %d]: %.*s\n", token->line, (int)token->lexeme.length, token->lexeme.data);
   }
}
