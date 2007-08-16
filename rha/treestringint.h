// Template generated by maketempl from file ../../include/ctmpl/tree.tmpl
//
// maketempl has been written by Mikio Braun, 2005
// mikio@first.fraunhofer.de
//

#ifndef TREESTRINGINT
#define TREESTRINGINT

#include "alloc.h"
#include "stringdefs.h"

#line 18 "../../include/ctmpl/tree.tmpl"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


struct treestringint_treenode {
  enum {treestringint_RED, treestringint_BLACK} color;
  string_t key;
  int value;
  struct treestringint_treenode *left;
  struct treestringint_treenode *right;
  struct treestringint_treenode *p;      // parent
};


typedef struct treestringint_ {
  struct treestringint_treenode *root;
  int size;
} treestringint_t;

struct treestringint_iterstack {
  enum {treestringint_LEFT, treestringint_RIGHT} state;
  struct treestringint_iterstack *prev;
};

typedef struct {
  struct treestringint_treenode *c;
  struct treestringint_iterstack *hist;
} treestringint_iterator_t;

extern int treestringint_lastsearchdepth;

extern void treestringint_init(treestringint_t *t);
// creates the empty tree

extern void treestringint_print(treestringint_t *t);
// prints the t in order

extern void treestringint_clear(treestringint_t *t);
// clears t

extern bool treestringint_isempty(treestringint_t *t);
// is the tree empty?

extern int treestringint_size(treestringint_t *t);
// size of the tree

extern int treestringint_search(treestringint_t *x, string_t key);
// finds the key in x and return its value
// or NULL if not found (e.g. if x is empty)
// note that also value might be NULL

extern void treestringint_insert(treestringint_t *t, string_t key, int value);
// inserts (key,value) into t

extern int treestringint_delete(treestringint_t *t, string_t key);
// deletes key in t and returns its value

// Iterating
extern void treestringint_begin(treestringint_iterator_t *i, treestringint_t *t);
extern bool treestringint_done(treestringint_iterator_t *i);
extern void treestringint_next(treestringint_iterator_t *i);
extern string_t treestringint_get_key(treestringint_iterator_t *i);
extern int treestringint_get_value(treestringint_iterator_t *i);


#endif