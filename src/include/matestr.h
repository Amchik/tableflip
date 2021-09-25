/* VIM BEST EDITOR
 * vim: ft=c
 */
#ifndef __8zc1kg_matestr_h
#define __8zc1kg_matestr_h

#include <sys/types.h>

/*
 * Mate string. Do not modify this struct
 */
typedef struct {
  /*
   * C-string instance
   */
  char* cstr;
  /*
   * Length of string
   */
  u_int32_t length;

  /*
   * Allocated bytes for the string
   */
  u_int32_t __allocated;
} matestr;

/*
 * Creates new instance of matestr.
 */
matestr matestralloc(size_t n);

/*
 * Creates new instance of matestr.
 * Calculates length of cstr, if cstr == 0,
 * passes empty allocated string
 * If len == 0 it will be ignored
 */
matestr matestr_new(const char* cstr, size_t len);
/*
 * Normalizes cstr in matestr
 */
void    matestr_normalize(matestr* self);
/*
 * Optimizes memory in matestr
 */
void    matestr_optimize(matestr* self);
/*
 * Copy string to matestr
 * If len == 0 it will be ignored
 */
void    matestr_append(matestr* self, const char* cstr, size_t len);
/*
 * Allocs matestr for n bytes
 */
void    matestr_appendm(matestr* self, size_t n);
void    matestr_replacec(matestr* self, char oldc, char newc);

#endif
