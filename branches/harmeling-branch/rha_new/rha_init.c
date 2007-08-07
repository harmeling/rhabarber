// WARNING: don't edit here!  
// This file has been automatically generated by 'rha_config.pl'.
// Instead edit 'rha_config.d'.

#include <stdarg.h>
#include "alloc.h"
#include "rha_init.h"

// (1) symbols
symbol_t object_sym;
symbol_t int_sym;
symbol_t bool_fn_sym;
symbol_t bool_not_sym;
symbol_t include_sym;
symbol_t ls_sym;
symbol_t void_sym;
symbol_t print_sym;
symbol_t rmslot_sym;
symbol_t local_sym;
symbol_t clone_sym;
symbol_t quote_sym;
symbol_t root_sym;
symbol_t new_sym;
symbol_t real_sym;
symbol_t fn_sym;
symbol_t eval_sym;
symbol_t this_sym;
symbol_t lsall_sym;
symbol_t symbol_sym;
symbol_t proto_sym;
symbol_t bool_sym;
symbol_t subscribe_sym;
symbol_t bool_equal_sym;
symbol_t lookup_sym;
symbol_t ptype_sym;
symbol_t bool_to_string_sym;
symbol_t assign_sym;
symbol_t mat_sym;


// (2) prototypes
object_t symbol_proto;
object_t object_proto;
object_t fn_proto;
object_t bool_proto;
object_t int_proto;
object_t real_proto;
object_t mat_proto;


// (3) type objects
object_t void_obj;
object_t symbol_obj;
object_t object_obj;
object_t fn_obj;
object_t bool_obj;
object_t int_obj;
object_t real_obj;
object_t mat_obj;


// (4) functions
object_t b_new(tuple_t t) {
  return new();
}
object_t b_clone(tuple_t t) {
  return clone(tuple_get(t, 1));
}
object_t b_ptype(tuple_t t) {
  return ptype(tuple_get(t, 1));
}
object_t b_lookup(tuple_t t) {
  return lookup(tuple_get(t, 1), *RAW(symbol_t, tuple_get(t, 2)));
}
object_t b_assign(tuple_t t) {
  return assign(tuple_get(t, 1), *RAW(symbol_t, tuple_get(t, 2)), tuple_get(t, 3));
}
object_t b_rmslot(tuple_t t) {
  rmslot(tuple_get(t, 1), *RAW(symbol_t, tuple_get(t, 2)));
}
object_t b_print(tuple_t t) {
  print(tuple_get(t, 1));
}
object_t b_include(tuple_t t) {
  include(tuple_get(t, 1), tuple_get(t, 2));
}
object_t b_subscribe(tuple_t t) {
  subscribe(tuple_get(t, 1), tuple_get(t, 2));
}
object_t b_ls(tuple_t t) {
  return ls(tuple_get(t, 1));
}
object_t b_lsall(tuple_t t) {
  return lsall(tuple_get(t, 1));
}
object_t b_eval(tuple_t t) {
  return eval(tuple_get(t, 1), tuple_get(t, 2));
}
object_t b_bool_equal(tuple_t t) {
  return wrap(BOOL_T, bool_proto, bool_equal(*RAW(bool_t, tuple_get(t, 1)), *RAW(bool_t, tuple_get(t, 2))));
}
object_t b_bool_not(tuple_t t) {
  return wrap(BOOL_T, bool_proto, bool_not(*RAW(bool_t, tuple_get(t, 1))));
}
object_t b_bool_to_string(tuple_t t) {
  return wrap(STRING_T, string_proto, bool_to_string(*RAW(bool_t, tuple_get(t, 1))));
}


// (5) init
#define ADD_TYPE(ttt, TTT)   // ttt ## _t
  setptype(ttt ## _proto, TTT ## _T);
  ttt ## _obj = new();
  assign(root, ttt ## _sym, ttt ## _obj);
  assign(ttt ## _obj, proto_sym, ttt ## _proto);

void add_function(object_t module, symbol_t s, int rettype, void *code, int narg, ...)
{
  // create a new object
  object_t o = new_t(FN_T, fn_proto);

  // create a struct containing all info about the builtin function
  fn_t f = ALLOC_SIZE(size_of(fn_t));
  setraw(o, (void *) f);
  f->rettype = rettype;
  f->code = code;
  f->narg = narg;
  f->argtypes = ALLOC_SIZE(narg*size_of(int_t));

  // read out the argument types
  va_list ap;
  va_start(ap, narg);
  for (int i=0; i<narg; i++)
    f->argtypes[i] = va_arg(ap, int_t);
  va_end(ap);

  // finally add it to module
  assign(module, s, o);
}

#define ADD_MODULE(mmm)   // mmm ## .h
  module = new();
  assign(modules, mmm ## _sym, module);

object_t rha_init()
{
  object_t root = new();

  // (5.1) create prototypes (TYPES)
  symbol_proto = new();
  object_proto = new();
  fn_proto = new();
  bool_proto = new();
  int_proto = new();
  real_proto = new();
  mat_proto = new();


  // (5.2) create symbols (SYMBOLS, TYPES, MODULES, functions)
  object_sym = symbol_new("object");
  int_sym = symbol_new("int");
  bool_fn_sym = symbol_new("bool_fn");
  bool_not_sym = symbol_new("bool_not");
  include_sym = symbol_new("include");
  ls_sym = symbol_new("ls");
  void_sym = symbol_new("void");
  print_sym = symbol_new("print");
  rmslot_sym = symbol_new("rmslot");
  local_sym = symbol_new("local");
  clone_sym = symbol_new("clone");
  quote_sym = symbol_new("quote");
  root_sym = symbol_new("root");
  new_sym = symbol_new("new");
  real_sym = symbol_new("real");
  fn_sym = symbol_new("fn");
  eval_sym = symbol_new("eval");
  this_sym = symbol_new("this");
  lsall_sym = symbol_new("lsall");
  symbol_sym = symbol_new("symbol");
  proto_sym = symbol_new("proto");
  bool_sym = symbol_new("bool");
  subscribe_sym = symbol_new("subscribe");
  bool_equal_sym = symbol_new("bool_equal");
  lookup_sym = symbol_new("lookup");
  ptype_sym = symbol_new("ptype");
  bool_to_string_sym = symbol_new("bool_to_string");
  assign_sym = symbol_new("assign");
  mat_sym = symbol_new("mat");


  // (5.3) create type objects (TYPES)
  ADD_TYPE(symbol, SYMBOL);
  ADD_TYPE(object, OBJECT);
  ADD_TYPE(fn, FN);
  ADD_TYPE(bool, BOOL);
  ADD_TYPE(int, INT);
  ADD_TYPE(real, REAL);
  ADD_TYPE(mat, MAT);


  // (5.4) create the void object
  void_obj = new();
  assign(root, void_sym, void_obj);


  // (5.5) add modules (MODULES, functions)
  object_t modules = new();
  assign(root, modules_sym, modules);
  object_t module = 0;

  ADD_MODULE(object);
  add_function(module, new_sym, (void *) b_new, 0);
  add_function(module, clone_sym, (void *) b_clone, 1, OBJECT_T);
  add_function(module, ptype_sym, (void *) b_ptype, 1, OBJECT_T);
  add_function(module, lookup_sym, (void *) b_lookup, 2, OBJECT_T, SYMBOL_T);
  add_function(module, assign_sym, (void *) b_assign, 3, OBJECT_T, SYMBOL_T, OBJECT_T);
  add_function(module, rmslot_sym, (void *) b_rmslot, 2, OBJECT_T, SYMBOL_T);
  add_function(module, print_sym, (void *) b_print, 1, OBJECT_T);
  add_function(module, include_sym, (void *) b_include, 2, OBJECT_T, OBJECT_T);
  add_function(module, subscribe_sym, (void *) b_subscribe, 2, OBJECT_T, OBJECT_T);
  add_function(module, ls_sym, (void *) b_ls, 1, OBJECT_T);
  add_function(module, lsall_sym, (void *) b_lsall, 1, OBJECT_T);

  ADD_MODULE(eval);
  add_function(module, eval_sym, (void *) b_eval, 2, OBJECT_T, OBJECT_T);

  ADD_MODULE(bool_fn);
  add_function(module, bool_equal_sym, (void *) b_bool_equal, 2, BOOL_T, BOOL_T);
  add_function(module, bool_not_sym, (void *) b_bool_not, 1, BOOL_T);
  add_function(module, bool_to_string_sym, (void *) b_bool_to_string, 1, BOOL_T);
}
