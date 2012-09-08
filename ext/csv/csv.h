#include <stddef.h>

typedef struct Csv_t Csv;
typedef void* (*Csv_alloc_f)(void* ptr, size_t sz);
typedef int (*Csv_buffer_f)(void* ptr, char* c, size_t sz);
typedef void (*Csv_collect_f)(void *ptr, size_t line, size_t column, const char* str);

Csv* Csv_openfile(const char* filename);
Csv* Csv_openstring(const char* str);
Csv* Csv_openbuffer(Csv_buffer_f buf, void* bufptr);
Csv* Csv_open(Csv_buffer_f buf, void* bufptr, Csv_alloc_f alloc);
void Csv_close(Csv* C);

const char* Csv_get(Csv* C, size_t line, size_t col);
const char* Csv_getheader(Csv* C, size_t col);
size_t Csv_numlines(Csv* C);
size_t Csv_numcols(Csv* C);
void Csv_collect(Csv *C, Csv_collect_f fn, void *ptr);
