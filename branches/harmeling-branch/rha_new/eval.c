#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <setjmp.h>
#include "eval.h"
#include "object.h"
#include "rha_types.h"
#include "messages.h"
#include "debug.h"
#include "alloc.h"
#include "utils.h"
#include "excp.h"
#include "list_fn.h"
#include "tuple_fn.h"
#include "symbol_fn.h"

void eval_init(object_t root, object_t module)
{
  // the only two functions we need to pull by hand (and not in prelude.rha)
  assign(root, eval_sym, lookup(module, eval_sym));
  assign(root, callslot_sym, lookup(module, callslot_sym));
}

// forward declarations
object_t eval_sequence(object_t env, list_t source);
object_t eval_args_and_call_fun(object_t env, tuple_t expr);
object_t call_fun(object_t env, int_t tlen, tuple_t expr);
void *call_C_fun(object_t env, int tlen, tuple_t t);
object_t call_rha_fun(object_t this, int narg, tuple_t expr);

// some code for bug-tracking
object_t eval_currentlocation = 0;

#define ENTER     object_t savedloc = eval_currentlocation; eval_currentlocation = expr
#define RETURN(x) do { object_t retval = (x); \
                           eval_currentlocation = savedloc; \
                           return retval; } while(0)

/*************************************************************
 *
 * Frames for 'return', 'break'
 *
 ************************************************************/

// Frames can be opened by functions, loops.  The keyword
// 'return' finishes the most recent function and returns its
// argument, 'break' the most recent loop

jmp_buf frame_stack[FRAME_MAX_NESTING];
object_t frame_value[FRAME_MAX_NESTING];
int frame_type[FRAME_MAX_NESTING];
int frame_tos = -1;

// Note, that these stacks must be only changed via the macros defined
// in eval.h.


object_t eval(object_t env, object_t expr)
{
  //debug("eval(env=%p, expr=%o)\n", env, expr);

  object_t value;
  switch (ptype(expr)) {
  case SYMBOL_T:
    // symbols
    value = lookup(env, UNWRAP_SYMBOL(expr));
    if (!value) rha_error("lookup of symbol '%o' failed", expr);
    object_t excp;
    try {
      value = callslot(value, proxy_sym, 0);
    }
    catch(excp) {
      // ignore it
    }
    return value;
  case TUPLE_T: 
    // function call
    assert(UNWRAP_PTR(TUPLE_T, expr));
    return eval_args_and_call_fun(env, UNWRAP_PTR(TUPLE_T, expr));
  case LIST_T:
    // sequence of expressions, e.g. { x=17; y=42 }
    assert(UNWRAP_PTR(LIST_T, expr));
    return eval_sequence(env, UNWRAP_PTR(LIST_T, expr));
  default:
    // literal
    return expr;
  }
}


object_t eval_sequence(object_t env, list_t source)
{
  // either the value of the last expression is delivered
  // or the value following 'deliver'
  // eval_squence does not open a new scope
  object_t res = void_obj;
  list_it it;
  begin_frame(BLOCK_FRAME)
    // evaluate all, or stop earlier via 'deliver', 'break', 'return'
    for (list_begin(source, &it); !list_done(&it); list_next(&it))
      res = eval(env, list_get(&it));
  end_frame(res);
  // return the result
  return res;
}


object_t eval_args_and_call_fun(object_t env, tuple_t expr)
{
  int tlen = tuple_len(expr);
  //  debug("eval_args: %o\n", WRAP_PTR(TUPLE_T, tuple_proto, expr));
  assert(tlen>0);  // otherwise repair 'rhaparser.y'
  object_t f = tuple_get(expr, 0);
  // deal with 'quote'
  if ((ptype(f)==SYMBOL_T) && symbol_equal(quote_sym, UNWRAP_SYMBOL(f))) {
    assert(tlen==2);  // otherwise repair 'rhaparser.y'
    return tuple_get(expr, 1);
  }

  // otherwise a usual function
  tuple_t values = tuple_new(tlen);
  for (int i=0; i<tlen; i++)
    tuple_set(values, i, eval(env, tuple_get(expr, i)));

  // finally call the function
  return call_fun(env, tlen, values);
}

object_t call_fun(object_t env, int_t tlen, tuple_t values)
{
  if (ptype(tuple_get(values, 0))==FUNCTION_T)
    // the function is implemented in C
    return call_C_fun(env, tlen, values);
  else
    // the function is pure rhabarber
    return call_rha_fun(env, tlen, values);
}


void check_ptypes(object_t o, enum ptypes pt)
{
  // note that OBJECT_T matches anything!
  if ((pt != OBJECT_T) && (ptype(o) != pt))
    rha_error("argument primtype missmatch (expected: %s, got: %s)",
	      ptype_names[pt], ptype_names[ptype(o)]);
}


void *call_C_fun(object_t env, int tlen, tuple_t t) 
{
  int narg = tlen-1;
  // extract the function
  object_t t0 = tuple_get(t, 0);
  assert(t0 && ptype(t0)==FUNCTION_T);
  function_t f = UNWRAP_PTR(FUNCTION_T, t0);

  // is the argument number correct?
  // we distinguish depending on whether variable length argument
  // lists are allowed
  if (f->varargs) {
    if (f->narg > narg)
      rha_error("wrong number of arguments (expected at least %d, got %d)",
		f->narg, narg);
  }
  else {
    if (f->narg != narg)
      rha_error("wrong number of arguments (expected %d, got %d)", 
		f->narg, narg);
  }
  // check the types of the args
  for (int i = 0; i < narg; i++)
    if (i < f->narg)
      check_ptypes(tuple_get(t, i+1), f->argptypes[i]);
    else
      // all optional arguments must be OBJECT_T
      check_ptypes(tuple_get(t, i+1), OBJECT_T);

  // finally call 'f'
  object_t res = 0;
  begin_frame(FUNCTION_FRAME)
    res = (f->code)(env, t);
  end_frame(res);
  return res;
}

/* Call a rhabarber function 
 *
 * Checks if the number of argument matches, constructs the local
 * environment of the callee and executes the function
 *
 * callable objects must have slots "argnames", "scope", and "fnbody".
 */
object_t call_rha_fun(object_t this, int tlen, tuple_t expr)
{
  object_t fn = tuple_get(expr, 0);

  // check if number of args is the same
  int nargs = tlen - 1;
  object_t _argnames = lookup(fn, argnames_sym);
  if (!_argnames) {
    rha_error("the object %o is not callable", fn);
    return 0;
  }
  tuple_t argnames = UNWRAP_PTR(TUPLE_T, _argnames);

  if (tuple_len(argnames) != nargs) {
    rha_error("Function called with wrong number of arguments");
  }

  // construct the inner scope
  object_t local = new();
  assign(local, local_sym, local);
  assign(local, this_sym, this);
  assign(local, parent_sym, lookup(fn, scope_sym));

  // assign the arguments
  for(int i = 0; i < nargs; i++) {
    symbol_t s = UNWRAP_SYMBOL(tuple_get(argnames, i));
    //debug("assigning argument number %d to '%s'\n", i, symbol_name(s));
    assign(local, s, tuple_get(expr, i+1));
  }

  // execute the function body
  object_t fnbody = lookup(fn, fnbody_sym);
  object_t res = 0;
  begin_frame(FUNCTION_FRAME)
    res = eval(local, fnbody);
  end_frame(res);
  return res;
}




// note on "variable argument lists" in rhabarber:
// to pull in a function like 'callslot' with '...' automatically, we
// need to implement a function 'vcallclot' with the '...' replaced by
// a 'list_t' argument:
object_t vcallslot(object_t obj, symbol_t slotname, int_t narg, list_t args)
{
  // note that 'callslot' assumes that all arguments are already
  // evaluated in the outer calling scope.  this is important since we
  // have no longer access to the outer calling scope.  the inner
  // calling scope ('this') will be set to 'obj'
  object_t slot = lookup(obj, slotname);
  if (!slot)
    throw(excp_newf("(call_slot) object %o doesn't have slot %s",
		    obj, symbol_name(slotname)));
  
  // construct the call
  list_prepend(args, slot);

  // call the slot with the scope being the object
  return call_fun(obj, narg+1, list_to_tuple(args));
}


// note that:
// * calling 'callslot' in C calls 'callslot'
// * calling 'callslot' in Rhaberber calls 'vcallslot'
object_t callslot(object_t obj, symbol_t slotname, int_t narg, ...)
{
  va_list ap;
  va_start(ap, narg);
  list_t args = list_new();
  for (int i = 0; i < narg; i++)
    list_append(args, va_arg(ap, object_t));
  va_end(ap);
  
  return vcallslot(obj, slotname, narg, args);
}
