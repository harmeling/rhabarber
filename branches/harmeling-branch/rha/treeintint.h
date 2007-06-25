// Template generated by maketempl from file ../../include/ctmpl/tree.tmpl
//
// maketempl has been written by Mikio Braun, 2005
// mikio@first.fraunhofer.de
//

#ifndef TREEINTINT
#define TREEINTINT

#include "alloc.h"
#include "intdefs.h"

#line 18 "../../include/ctmpl/tree.tmpl"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


struct treeintint_treenode {
  enum {treeintint_RED, treeintint_BLACK} color;
  int key;
  int value;
  struct treeintint_treenode *left;
  struct treeintint_treenode *right;
  struct treeintint_treenode *p;      // parent
};


typedef struct treeintint_ {
  struct treeintint_treenode *root;
  int size;
} treeintint_t;

struct treeintint_iterstack {
  enum {treeintint_LEFT, treeintint_RIGHT} state;
  struct treeintint_iterstack *prev;
};

typedef struct {
  struct treeintint_treenode *c;
  struct treeintint_iterstack *hist;
} treeintint_iterator_t;

extern int treeintint_lastsearchdepth;

extern void treeintint_init(treeintint_t *t);
// creates the empty tree

extern void treeintint_print(treeintint_t *t);
// prints the t in order

extern void treeintint_clear(treeintint_t *t);
// clears t

extern bool treeintint_isempty(treeintint_t *t);
// is the tree empty?

extern int treeintint_size(treeintint_t *t);
// size of the tree

extern int treeintint_search(treeintint_t *x, int key);
// finds the key in x and return its value
// or NULL if not found (e.g. if x is empty)
// note that also value might be NULL

extern void treeintint_insert(treeintint_t *t, int key, int value);
// inserts (key,value) into t

extern int treeintint_delete(treeintint_t *t, int key);
// deletes key in t and returns its value

// Iterating
extern void treeintint_begin(treeintint_iterator_t *i, treeintint_t *t);
extern bool treeintint_done(treeintint_iterator_t *i);
extern void treeintint_next(treeintint_iterator_t *i);
extern int treeintint_get_key(treeintint_iterator_t *i);
extern int treeintint_get_value(treeintint_iterator_t *i);


#endif
