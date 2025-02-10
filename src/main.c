/* (c) copyright 2025 Lawrence D. Kern ////////////////////////////////////// */

#include "shared.c"
#include "lexer.c"

static void execute(memarena *perma, memarena trans, char *code)
{
   if(code[0] == 'e')
   {
      error(0, 0, "Successfully activated error handling.");
   }
   else
   {
      lex(perma, trans, code);
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
         execute(perma, trans, code);
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
         execute(perma, trans, command);
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
