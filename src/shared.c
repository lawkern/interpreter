/* (c) copyright 2025 Lawrence D. Kern ////////////////////////////////////// */

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
typedef uint8_t u8;
typedef uint64_t u64;

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

typedef struct {
   char *data;
   memindex length;
} string;

#define S(s) (string){s, countof(s)-1}

static bool string_equals(string a, string b)
{
   bool result = false;
   if(a.length == b.length)
   {
      result = (a.length == 0) || !memcmp(a.data, b.data, a.length);
   }

   return(result);
}

static bool encountered_error;

static void error(int line, int column, char *format, ...)
{
   encountered_error = true;

   char message[512];

   va_list arguments;
   va_start(arguments, format);
   {
      vsnprintf(message, sizeof(message), format, arguments);
   }
   va_end(arguments);

   // NOTE: For the sake of simplicity, line is stored 0-indexed.
   fprintf(stderr, "[ERROR (%d, %d)]: %s\n", line + 1, column, message);
}
