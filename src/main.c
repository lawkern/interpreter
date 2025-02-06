/* (c) copyright 2025 Lawrence D. Kern ////////////////////////////////////// */

#include <stdio.h>
#include <stdlib.h>

#define countof(a) (sizeof(a) / sizeof((a)[0]))

static void execute(char *code)
{
   printf("%s\n", code);
}

static void execute_script(char *path)
{
   FILE *file = fopen(path, "rb");
   if(file)
   {
      fseek(file, 0, SEEK_END);
      size_t size = ftell(file);
      fseek(file, 0, SEEK_SET);

      char *code = malloc((size * sizeof(*code)) + 1);
      size_t read = fread(code, 1, size, file);
      if(read == size)
      {
         code[size] = 0;
         execute(code);
      }
      else
      {
         fprintf(stderr, "ERROR: Read %d of %d bytes of file.\n", read, size);
      }

      fclose(file);
   }
   else
   {
      fprintf(stderr, "ERROR: Failed to open file %s.\n", path);
   }
}

static void execute_commands(void)
{
   char command[256];

   printf("Enter commands for interpretation:\n");
   for(;;)
   {
      printf("> ");
      if(fgets(command, countof(command), stdin))
      {
         execute(command);
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
      puts("Usage: interpreter [source file]");
   }
   else if(argument_count == 2)
   {
      char *path = arguments[1];
      execute_script(path);
   }
   else
   {
      execute_commands();
   }

   return(0);
}
