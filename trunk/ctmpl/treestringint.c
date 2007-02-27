// Template generated by maketempl from file tree.tmpl
//
// maketempl has been written by Mikio Braun, 2005
// mikio@first.fraunhofer.de
//

#include "treestringint.h"


#line 64 "tree.tmpl"

// binary trees implemented as red-black trees
// as described in Cormen, Leiserson, Rivest: Introduction to algorithms
//
// written by Stefan Harmeling, 2005
//
// templatized by Mikio Braun

#include <stdlib.h>

int treestringint_lastsearchdepth;
static struct treestringint_treenode *nil = NULL;

// all leaves point to nil
// the parent of the root is NULL

void treestringint_init(treestringint_t *t) {
  // is this the first call of treestringint_init?
  if (nil == NULL) {
    nil = (struct treestringint_treenode *) malloc(sizeof(struct treestringint_treenode));
    nil->color = BLACK;
  }

  // the empty tree
  t->root = nil;
  t->size = 0;
}


bool treestringint_isempty(treestringint_t *t) {
  // is the tree empty?
  return (t->root == nil);
}


bool treestringint_node_isempty(struct treestringint_treenode *t) {
  // is the tree empty?
  return (t == nil);
}


void treestringint_node_print(struct treestringint_treenode *t) {
  // prints the t in order
  if (t != nil) {
    treestringint_node_print(t->left);
    (void)(t->key);
    printf(":");
    (void)(t->value);
    printf("\n");
    treestringint_node_print(t->right);
  }
}

void treestringint_print(treestringint_t *t) { 
  treestringint_node_print(t->root); 
}


void treestringint_node_clear(struct treestringint_treenode *t) {
  // clears t
  if (t != nil) {
    treestringint_node_clear(t->left);
    treestringint_node_clear(t->right);
    (t);
  }
}
void treestringint_clear(treestringint_t *t) { 
  treestringint_node_clear(t->root); 
  treestringint_init(t);
}

int treestringint_node_search(struct treestringint_treenode *x, string key) {
  // find the key in x and return its value
  // or NULL if not found (e.g. if x is empty)
  // note that also value might be NULL

  treestringint_lastsearchdepth = 0;
  while (x != nil) {
    if (stringlessthan(key,x->key)) x = x->left;
    else if (stringlessthan(x->key,key)) x = x->right;
    else return x->value;
    treestringint_lastsearchdepth++;
  }
  return (int) NULL;  // not found
}
int treestringint_search(treestringint_t *t, string key) { 
  return treestringint_node_search(t->root, key);
}

struct treestringint_treenode *treestringint_node_minimum(struct treestringint_treenode *x) {
  // returns the node containing the minimum of x or NULL if x is empty
  if (treestringint_node_isempty(x)) return NULL;
  while (x->left != nil)
    x = x->left;
  return x;
}

struct treestringint_treenode *treestringint_maximum(struct treestringint_treenode *x) {
  // returns the node containing the maximum of x or NULL if x is empty
  if (treestringint_node_isempty(x)) return NULL;
  while (x->right != nil)
    x = x->right;
  return x;
}

struct treestringint_treenode *treestringint_node_successor(struct treestringint_treenode *x) {
  if (treestringint_node_isempty(x)) return NULL;
  if (x->right != nil) return treestringint_node_minimum(x->right);
  struct treestringint_treenode *y = x->p;
  while (y != NULL && x == y->right) {
    x = y;
    y = x->p;
  }
  return y;
}

struct treestringint_treenode *treestringint_node_predecessor(struct treestringint_treenode *x) {
  if (treestringint_node_isempty(x)) return NULL;
  if (x->left != nil) return treestringint_maximum(x->left);
  struct treestringint_treenode *y = x->p;
  while (y != NULL && x == y->left) {
    x = y;
    y = x->p;
  }
  return y;
}

struct treestringint_treenode *treestringint_node_insert(treestringint_t *t, string key, int value) {
  // inserts (key,value) in t, if key exists, replaces value

  // find location to insert (key,value) into t
  struct treestringint_treenode *y = NULL;   // NULL is the parent of t
  struct treestringint_treenode *x = t->root;
  while (x != nil) {
    y = x;  // y is the parent of x after the loop
    if (stringlessthan(key,x->key)) x = x->left;
    else if (stringlessthan(x->key,key)) x = x->right;
    else {
      // key exist, only change its value
      x->value = value;
      return NULL;
    }
  }

  // create new node z
  struct treestringint_treenode *z = (struct treestringint_treenode *) malloc(sizeof(struct treestringint_treenode));
  z->key = key;
  z->value = value;
  z->left = nil;
  z->right = nil;
  z->p = y;
  if (y == NULL) t->root = z;
  else {
    if (stringlessthan(z->key,y->key)) y->left = z;
    else y->right = z;
  }
  t->size++;
  return z;
}

void treestringint_node_leftrotate(treestringint_t *t, struct treestringint_treenode *x) {
  // assumption: x->right != nil
  struct treestringint_treenode *y = x->right;         // set y
  x->right = y->left;   // turn y's left subtree into x's right subtree
  if (y->left != nil) y->left->p = x;
  y->p = x->p;          // link x's parent to y
  if (x->p == NULL) t->root = y;
  else { 
    if (x == x->p->left) x->p->left = y;
    else x->p->right = y;
  }
  y->left = x;          // put x on y's left
  x->p = y;
}

void treestringint_node_rightrotate(treestringint_t *t, struct treestringint_treenode *x) {
  // assumption: x->left != nil
  struct treestringint_treenode *y = x->left;         // set y
  x->left = y->right;   // turn y's right subtree into x's left subtree
  if (y->right != nil) y->right->p = x;
  y->p = x->p;          // link x's parent to y
  if (x->p == NULL) t->root = y;
  else { 
    if (x == x->p->right) x->p->right = y;
    else x->p->left = y;
  }
  y->right = x;          // put x on y's right
  x->p = y;
}

void treestringint_insert(treestringint_t *t, string key, int value) {
  // inserts (key,value) into t
  struct treestringint_treenode *x = treestringint_node_insert(t,key,value);
  // to test whether red-black stuff is working replace 
  // if (0 && x!=NULL) ...
  if (x != NULL) {  // if x == NULL then x existed already
    x->color = RED;
    struct treestringint_treenode *y;
    while (x != t->root && x->p->color == RED) {
      if (x->p == x->p->p->left) {
	y = x->p->p->right;
	if (y->color == RED) {
	  x->p->color = BLACK;
	  y->color = BLACK;
	  x->p->p->color = RED;
	  x = x->p->p;
	}
	else {
	  if (x == x->p->right) {
	    x = x->p;
	    treestringint_node_leftrotate(t,x);
	  }
	  x->p->color = BLACK;
	  x->p->p->color = RED;
	  treestringint_node_rightrotate(t,x->p->p);
	}
      }
      else {
	y = x->p->p->left;
	if (y->color == RED) {
	  x->p->color = BLACK;
	  y->color = BLACK;
	  x->p->p->color = RED;
	  x = x->p->p;
	}
	else {
	  if (x == x->p->left) {
	    x = x->p;
	    treestringint_node_rightrotate(t,x);
	  }
	  x->p->color = BLACK;
	  x->p->p->color = RED;
	  treestringint_node_leftrotate(t,x->p->p);
	}
      }
    }
    t->root->color = BLACK;
  }
}

void treestringint_node_deletefixup(treestringint_t *t, struct treestringint_treenode *x) {
  struct treestringint_treenode *w;
  while (x != t->root && x->color == BLACK) {
    if (x == x->p->left) {
      w = x->p->right;
      if (w->color == RED) {
	w->color = BLACK;
	x->p->color = RED;
	treestringint_node_leftrotate(t,x->p);
	w = x->p->right;
      }
      if (w->left->color == BLACK && w->right->color == BLACK) {
	w->color = RED;
	x = x->p;
      }
      else {
	if (w->right->color == BLACK) {
	  w->left->color = BLACK;
	  w->color = RED;
	  treestringint_node_rightrotate(t,w);
	  w = x->p->right;
	}
	w->color = x->p->color;
	x->p->color = BLACK;
	w->right->color = BLACK;
	treestringint_node_leftrotate(t,x->p);
	x = t->root;
      }
    }
    else {
      w = x->p->left;
      if (w->color == RED) {
	w->color = BLACK;
	x->p->color = RED;
	treestringint_node_rightrotate(t,x->p);
	w = x->p->left;
      }
      if (w->right->color == BLACK && w->left->color == BLACK) {
	w->color = RED;
	x = x->p;
      }
      else {
	if (w->left->color == BLACK) {
	  w->right->color = BLACK;
	  w->color = RED;
	  treestringint_node_leftrotate(t,w);
	  w = x->p->left;
	}
	w->color = x->p->color;
	x->p->color = BLACK;
	w->left->color = BLACK;
	treestringint_node_rightrotate(t,x->p);
	x = t->root;
      }
    }
  }
  x->color = BLACK;
}

int treestringint_delete(treestringint_t *t, string key) {
  // deletes key in t and returns its value

  // find key in t
  struct treestringint_treenode *z = t->root;
  while (z != nil) {
    if (stringlessthan(key,z->key)) z = z->left;
    else if (stringlessthan(z->key,key)) z = z->right;
    else break;
  }
  if (z == nil) return (int) NULL;  // not found and not deleted

  // remove z
  struct treestringint_treenode *y;
  if (z->left == nil || z->right == nil) y = z;
  else y = treestringint_node_successor(z);
  struct treestringint_treenode *x;
  if (y->left != nil) x = y->left;
  else x = y->right;
  x->p = y->p;
  if (y->p == NULL) t->root = x;
  else {
    if (y == y->p->left) y->p->left = x;
    else y->p->right = x;
  }
  int value = z->value;
  if (y != z) {
    z->key = y->key;
    z->value = y->value;
  }
  if (y->color == BLACK) treestringint_node_deletefixup(t,x);
  (y);
  t->size--;
  return value;
}

