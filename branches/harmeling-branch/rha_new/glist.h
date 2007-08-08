/*
 * glist - Generic doubly linked LISTs
 *
 * (c) 2005 by Mikio Braun
 */

/*
 * This is an edited version of a list generated by maketmpl using  list.tmpl
 */

#ifndef UTIL_GLIST_H
#define UTIL_GLIST_H

#include <stdbool.h>
#include <stdint.h>

struct glistnode {
  struct glistnode *next;
  struct glistnode *prev;
  intptr_t data;
};

struct glist {
  int len;
  struct glistnode *first;
  struct glistnode *last;
};

// Basic access
typedef struct glist glist_t;
typedef struct glistnode *glist_iterator_t;
typedef void glist_proc(intptr_t data);

// initialization
extern void glist_init(glist_t *l);
extern void glist_clear(glist_t *l);

// adding elements and concatenating glists
extern void glist_append_(glist_t *l, intptr_t data);
extern void glist_prepend_(glist_t *l, intptr_t data);
extern void glist_appendlist(glist_t *l1, glist_t *l2);

// removing elements from head and tail
extern intptr_t glist_pop_(glist_t *l);
extern intptr_t glist_remove_(glist_t *l);

// getting the element from the head
extern intptr_t glist_front_(glist_t *l);
extern intptr_t glist_back_(glist_t *l);

// check sizes
extern bool glist_isempty(glist_t *l);
extern int  glist_length(glist_t *l);

// Iterating
extern void glist_foreach(glist_t *l, glist_proc *p);

extern glist_iterator_t glist_begin(glist_t *l);
extern bool glist_done(glist_iterator_t *i); 
extern void glist_next(glist_iterator_t *i);
extern intptr_t glist_get_(glist_iterator_t *i);

// macros with implicit casting
#define glist_append(l, d) glist_append_(l, (intptr_t) (d) )
#define glist_prepend(l, d) glist_prepend_(l, (intptr_t) (d) )

#define glist_popi(l) ((int) glist_pop_(l))
#define glist_popp(l) ((void*) glist_pop_(l))
#define glist_removei(l) ((int) glist_remove_(l))
#define glist_removep(l) ((void*) glist_remove_(l))
#define glist_fronti(l) ((int) glist_front_(l))
#define glist_frontp(l) ((void*) glist_front_(l))
#define glist_backi(l) ((int) glist_back_(l))
#define glist_backp(l) ((void*) glist_back_(l))
#define glist_geti(l) ((int) glist_get_(l))
#define glist_getp(l) ((void*) glist_get_(l))

#endif
