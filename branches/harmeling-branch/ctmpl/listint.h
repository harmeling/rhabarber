// Template generated by maketempl from file list.tmpl
//
// maketempl has been written by Mikio Braun, 2005
// mikio@first.fraunhofer.de
//

#ifndef LISTINT
#define LISTINT


#line 17 "list.tmpl"

#include <stdbool.h>

struct nodeint {
  struct nodeint *next;
  struct nodeint *prev;
  int data;
};

struct listint {
  int len;
  struct nodeint *first;
  struct nodeint *last;
};

// Basic access
typedef struct listint listint_t;
typedef struct nodeint *listint_iterator_t;
typedef void listint_proc(int data);

extern void listint_init(listint_t *l);
extern void listint_clear(listint_t *l);

extern void listint_append(listint_t *l, int data);
extern void listint_prepend(listint_t *l, int data);

extern int listint_remove(listint_t *l);

extern int listint_front(listint_t *l);

extern bool listint_isempty(listint_t *l);
extern int listint_length(listint_t *l);

// Iterating
extern void listint_foreach(listint_t *l, listint_proc *p);

extern void listint_begin(listint_iterator_t *i, listint_t *l);
extern bool listint_done(listint_iterator_t *i); 
extern void listint_next(listint_iterator_t *i);
extern int listint_get(listint_iterator_t *i);


#endif