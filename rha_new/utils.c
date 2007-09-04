#include "utils.h"

#include <string.h>
#include "fmt.h"
#include "alloc.h"
#include "object.h"
#include "messages.h"
#include "rha_types.h"
#include "symbol_fn.h"


/* handy function that checks indices and transforms them if negative */
int_t idx(int_t i, int_t len)
{
  if (i >= len || -i > len)
    rha_error("index out-of-bounds");
  if (i < 0) i = len + i;
  return i;
}



/* print an object */
static void fmt_object(STREAM *s, char *fmt, va_list *ap)
{
  strputs(s, to_string_only_in_c(va_arg(*ap, any_t)));
}

void print(const char *fmt, ...)
{
  addfmt('o', fmt_object);
  va_list ap;
  va_start(ap, fmt);
  vprtfmt(cerr, fmt, &ap);
  va_end(ap);
}

void vprint(const char *fmt, va_list *ap)
{
  addfmt('o', fmt_object);
  vprtfmt(cout, fmt, ap);
}

void fprint(FILE *f, const char *fmt, ...)
{
  addfmt('o', fmt_object);
  STREAM *s = fstropen(f);
  va_list ap;
  va_start(ap, fmt);
  vprtfmt(s, fmt, &ap);
  va_end(ap);
  strclose(s);
}

void vfprint(FILE * f, const char *fmt, va_list *ap)
{
  addfmt('o', fmt_object);
  STREAM *s = fstropen(f);
  vprtfmt(s, fmt, ap);
  strclose(s);
}

char *sprint(const char *fmt, ...)
{
  char *retval;
  addfmt('o', fmt_object);
  STREAM *str = sstropen(NULL, 0);

  va_list ap;
  va_start(ap, fmt);
  vprtfmt(str, fmt, &ap);
  va_end(ap);

  retval = gc_strdup(sstrbuffer(str));
  
  strclose(str);

  return retval;
}

char *vsprint(const char *fmt, va_list *ap)
{
  char *retval;
  addfmt('o', fmt_object);
  STREAM *str = sstropen(NULL, 0);

  vprtfmt(str, fmt, ap);

  retval = gc_strdup(sstrbuffer(str));
  
  strclose(str);

  return retval;
}


void printusage(char *str)
{
  printf("usage: %s\n", str);
}
