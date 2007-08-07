// WARNING: don't edit here!  This file is automatically generated by
// 'rha_config.pl'

#include "rha_init.h"
#include "rha_init_utils.h"

// (1) symbols
symbol_t quote_sym;
symbol_t this_sym;
symbol_t root_sym;
symbol_t local_sym;

// (2) prototypes
object_t int_proto;
object_t real_protot;

// (3) functions
//int_t int_plus(int_t, int_t);
object_t b_int_plus(tuple_t t) {
  object_t o = new_t(INT_T, int_proto);
  setraw(o, (void *) &int_plus(*RAW(int_t, tuple_get(t, 1)), *RAW(int_t, tuple_get(t, 2))));
  return o;
}


//object_t eval(object_t env, object_t expr);
object_t b_eval(tuple_t t) {
  return eval(tuple_get(t, 1), tuple_get(t, 2));
}

//void_t tuple_set(tuple_t t, int_t i, object_t);
object_t b_tuple_set(tuple_t t) {
  object_t o = new();
  setptype(o, VOID_T);
  tuple_set(*RAW(tuple_t, tuple_get(t, 1)), *RAW(tuple_t, tuple_get(t, 2)), tuple_get(t, 3));
  return o;
}

// (4) init

object_t rha_init()
{
  object_t global = new();

  // add prototypes
  int_proto = new();

  // add datatypes
  object_t int_obj = new();
  int_proto = new();
  setptype(int_proto, INT_T);
  assign(int_obj, proto_sym, int_proto);
  assign(global, int_sym, int_obj);

  // add functions
  object_t modules = new();
  assign(global, modules_sym, modules);
  
  add_function(modules, b_new_sym, OBJECT_T, (void *) new, 0);
  add_function(modules, b_clone_sym, OBJECT_T, (void *) clone, 1, OBJECT_T);
}
