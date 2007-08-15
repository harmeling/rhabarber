// Template generated by maketempl from file ../../include/ctmpl/tree.tmpl
//
// maketempl has been written by Mikio Braun, 2005
// mikio@first.fraunhofer.de
//

#ifndef TREESYMBOLOBJECT
#define TREESYMBOLOBJECT

#include "alloc.h"
#include "object.h"
#include "symbol_tr.h"

#line 18 "../../include/ctmpl/tree.tmpl"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


struct treesymbolobject_treenode {
  enum {treesymbolobject_RED, treesymbolobject_BLACK} color;
  symbol_tr key;
  object_t value;
  struct treesymbolobject_treenode *left;
  struct treesymbolobject_treenode *right;
  struct treesymbolobject_treenode *p;      // parent
};


typedef struct treesymbolobject_ {
  struct treesymbolobject_treenode *root;
  int size;
} treesymbolobject_t;

struct treesymbolobject_iterstack {
  enum {treesymbolobject_LEFT, treesymbolobject_RIGHT} state;
  struct treesymbolobject_iterstack *prev;
};

typedef struct {
  struct treesymbolobject_treenode *c;
  struct treesymbolobject_iterstack *hist;
} treesymbolobject_iterator_t;

extern int treesymbolobject_lastsearchdepth;

extern void treesymbolobject_init(treesymbolobject_t *t);
// creates the empty tree

extern void treesymbolobject_print(treesymbolobject_t *t);
// prints the t in order

extern void treesymbolobject_clear(treesymbolobject_t *t);
// clears t

extern bool treesymbolobject_isempty(treesymbolobject_t *t);
// is the tree empty?

extern int treesymbolobject_size(treesymbolobject_t *t);
// size of the tree

extern object_t treesymbolobject_search(treesymbolobject_t *x, symbol_tr key);
// finds the key in x and return its value
// or NULL if not found (e.g. if x is empty)
// note that also value might be NULL

extern void treesymbolobject_insert(treesymbolobject_t *t, symbol_tr key, object_t value);
// inserts (key,value) into t

extern object_t treesymbolobject_delete(treesymbolobject_t *t, symbol_tr key);
// deletes key in t and returns its value

// Iterating
extern void treesymbolobject_begin(treesymbolobject_iterator_t *i, treesymbolobject_t *t);
extern bool treesymbolobject_done(treesymbolobject_iterator_t *i);
extern void treesymbolobject_next(treesymbolobject_iterator_t *i);
extern symbol_tr treesymbolobject_get_key(treesymbolobject_iterator_t *i);
extern object_t treesymbolobject_get_value(treesymbolobject_iterator_t *i);


#endif