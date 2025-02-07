/* (c) copyright 2025 Lawrence D. Kern ////////////////////////////////////// */

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

static bool encountered_error;

static void error(int line, char *message)
{
   encountered_error = true;
   fprintf(stderr, "[ERROR (%d)]: %s\n", line, message);
}
