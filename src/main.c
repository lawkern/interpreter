/* (c) copyright 2025 Lawrence D. Kern ////////////////////////////////////// */

#include "shared.c"
#include "lexer.c"
#include "parser.c"

static void execute(memarena *perma, memarena trans, text_stream stream)
{
   if(stream.text[stream.index] == 'e')
   {
      error(0, 0, "Successfully activated error handling.");
   }
   else
   {
      lex(perma, trans, stream);
      parse();
   }
}

static void execute_script(memarena *perma, memarena trans, char *path)
{
   FILE *file = fopen(path, "rb");
   if(file)
   {
      fseek(file, 0, SEEK_END);
      size_t size = ftell(file);
      fseek(file, 0, SEEK_SET);

      char *code = new(perma, char, size + 1);
      size_t read = fread(code, 1, size, file);
      if(read == size)
      {
         code[size] = 0;

         text_stream stream = {0};
         stream.count = size;
         stream.text = code;

         execute(perma, trans, stream);
      }
      else
      {
         fprintf(stderr, "ERROR: Read %ld of %ld bytes of file.\n", read, size);
      }

      fclose(file);
   }
   else
   {
      fprintf(stderr, "ERROR: Failed to open file %s.\n", path);
   }
}

static void execute_commands(memarena *perma, memarena trans)
{
   char command[256];

   printf("Enter commands for interpretation:\n");
   for(;;)
   {
      printf("> ");
      if(fgets(command, countof(command), stdin))
      {
         text_stream stream = {0};
         stream.count = strlen(command);
         stream.text = command;

         execute(perma, trans, stream);
      }
      else
      {
         break;
      }
   }
}

int main(int argument_count, char **arguments)
{
   if(argument_count > 2)
   {
      fprintf(stderr, "Usage: interpreter [source file]\n");
      return(64);
   }

   memindex perma_size = 1 << 24;
   memindex trans_size = 1 << 24;

   u8 *perma_memory = malloc(perma_size);
   u8 *trans_memory = malloc(trans_size);

   memarena perma = {perma_memory, perma_memory + perma_size};
   memarena trans = {trans_memory, trans_memory + trans_size};

   if(argument_count == 2)
   {
      // NOTE: Execute the specifed script file.
      execute_script(&perma, trans, arguments[1]);
   }
   else
   {
      // NOTE: Begin interpreting commands interactively.
      execute_commands(&perma, trans);
   }

   return(0);
}
