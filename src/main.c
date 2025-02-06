/* (c) copyright 2025 Lawrence D. Kern ////////////////////////////////////// */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
typedef uint8_t u8;

#include <stddef.h>
typedef ptrdiff_t memindex;

#define countof(a) (sizeof(a) / sizeof((a)[0]))

typedef struct {
   u8 *begin;
   u8 *end;
} memarena;

#define new(a, t, n) (t *)allocate((a), sizeof(t), (n), _Alignof(t))

static void *allocate(memarena *arena, memindex size, int count, int alignment)
{
   int padding = -(uintptr_t)arena->begin & (alignment - 1);
   if((arena->end - arena->begin - padding)/size < count)
   {
      fprintf(stderr, "OOM: Failed to allocate %ld bytes.\n", size*count);
      assert(0);
   }

   void *result = arena->begin + padding;
   arena->begin += (padding + count*size);

   return memset(result, 0, count*size);
}

static void execute(memarena *perma, memarena trans, char *code)
{
   printf("%s\n", code);
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
