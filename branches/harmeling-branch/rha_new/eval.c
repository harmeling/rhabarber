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
object_t eval_sequence(object_t env, tuple_t t);
object_t eval_args_and_call_fun(object_t env, tuple_t expr);
object_t call_fun(object_t env, int_t tlen, tuple_t expr);
void *call_C_fun(int tlen, tuple_t t);
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
  case SYMBOL_T: {
    // symbol
    symbol_t s = UNWRAP_SYMBOL(expr);
    value = lookup(env, s);
    if (!value && s!=void_sym) 
      rha_error("lookup of symbol '%o' failed", expr);
    if (has(value, hasproxy_sym)) {
      value = callslot(value, hasproxy_sym, 0);
    }
    return value;
  }
  case TUPLE_T: 
    // function call
    assert(UNWRAP_PTR(TUPLE_T, expr));
    return eval_args_and_call_fun(env, UNWRAP_PTR(TUPLE_T, expr));
  case STRING_T:
    // string literals (must be copied, since they might be modified)
    return WRAP_PTR(STRING_T, string_copy(UNWRAP_PTR(STRING_T, expr)));
  case BOOL_T:
  case INT_T:
  case REAL_T:
    // other literals generated by the parser
    // (must be copied as well, since they might be modified)
    return copy_pt(expr);
  default:
    if (expr == 0) // void
      // do nothing
      return expr;
    // else don't know!
    rha_error("(eval) don't know how to evaluate '%o'", expr);
    assert(1==0);
  }
  return 0; // make gcc happy!
}

object_t eval_sequence(object_t env, tuple_t t)
{
  // either the value of the last expression is delivered
  // or the value following 'deliver'
  // eval_sequence does not open a new scope
  object_t res = 0;
  int_t tlen = tuple_len(t);
  begin_frame(BLOCK_FRAME)
    // evaluate all, or stop earlier via 'deliver', 'break', 'return'
    // note: the counter begins at 1 to ignore 'do_sym'
    for (int i = 1; i < tlen; i++)
      res = eval(env, tuple_get(t, i));
  end_frame(res);
  // return the result
  return res;
}


object_t eval_args_and_call_fun(object_t env, tuple_t expr)
{
  int tlen = tuple_len(expr);
  //  debug("eval_args: %o\n", WRAP_PTR(TUPLE_T, expr));
  assert(tlen>0);  // otherwise repair 'rhaparser.y'
  object_t f = tuple_get(expr, 0);
  // deal with special stuff
  if (ptype(f)==SYMBOL_T) {
    if (quote_sym == UNWRAP_SYMBOL(f)) {
      assert(tlen==2);  // otherwise repair 'rhaparser.y'
      return tuple_get(expr, 1);
    }
    else if (do_sym == UNWRAP_SYMBOL(f)) {
      return eval_sequence(env, expr);
    }
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
  if (ptype(tuple_get(values, 0))==BUILTIN_T)
    // the function is implemented in C
    return call_C_fun(tlen, values);
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


void *call_C_fun(int tlen, tuple_t t) 
{
  int narg = tlen-1;
  // extract the function
  object_t t0 = tuple_get(t, 0);
  assert(t0 && ptype(t0)==BUILTIN_T);
  builtin_t f = UNWRAP_PTR(BUILTIN_T, t0);

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
  //  begin_frame(FUNCTION_FRAME)
    res = (f->code)(t);
    //  end_frame(res);
  return res;
}


object_t call_matching_impl(object_t local, object_t scope, object_t fnbody,
			    tuple_t signature, tuple_t values)
{
  // add more to the inner scope
  assign(local, parent_sym, scope);  // the defining scope (lexical)

  // assign the arguments
  int nargs = tuple_len(values)-1;
  for(int i = 0; i < nargs; i++) {
    object_t pattern = tuple_get(signature, i);
    if (!pattern)
      rha_error("(eval) signature is not valid");
    object_t s_o = eval(pattern, WRAP_SYMBOL(symbol_sym));
    if (!s_o || ptype(s_o)!=SYMBOL_T)
      rha_error("(eval) symbol expected, found %o", s_o);
    symbol_t s = UNWRAP_SYMBOL(s_o);
    //debug("assigning argument number %d to '%s'\n", i, symbol_name(s));
    assign(local, s, tuple_get(values, i+1));
  }

  // call the function
  object_t res = 0;
  begin_frame(FUNCTION_FRAME)
    // don't put 'return' inside here
    res = eval(local, fnbody);
  end_frame(res);
  return res;
}


bool_t signature_matches(tuple_t signature, tuple_t values)
{
  int_t narg = tuple_len(values) - 1;
  return tuple_len(signature) == narg;
}


object_t find_and_call_matching_impl(object_t local, list_t fn_data_l, tuple_t values)
{
  string_t msg = "(eval) can't call %o, since it has a faulty entry in 'fn_data'";
  // go through the list to find a match
  for (list_it_t it = list_begin(fn_data_l); !list_done(it); list_next(it)) {
    object_t impl = list_get(it);
    if (!impl) rha_error(msg);
    object_t impl0 = eval(impl, WRAP_SYMBOL(signature_sym));
    if (!impl0 || ptype(impl0)!=TUPLE_T)
      rha_error(msg);
    tuple_t signature = UNWRAP_PTR(TUPLE_T, impl0);
    if (signature_matches(signature, values)) {
      // match found!!!
      object_t scope = eval(impl, WRAP_SYMBOL(scope_sym));
      if (!scope)
	rha_error("(eval) can't find defining scope");
      object_t fnbody = eval(impl, WRAP_SYMBOL(fnbody_sym));
      if (!fnbody)
	rha_error("(eval) can't find function body");
      return call_matching_impl(local, scope, fnbody, signature, values);
    }
  }
  // we did not find anything matching
  rha_error("can't call function since no matching signature"
	    "was foundnumber was found");
  return 0;
}



/* Call a rhabarber function 
 *
 * Checks if the number of argument matches, constructs the local
 * environment of the callee and executes the function
 *
 * callable objects must have slots "argnames", "scope", and "fnbody".
 */
object_t call_rha_fun(object_t this, int tlen, tuple_t values)
{
  assert(tlen == tuple_len(values));

  // get the function to be called
  object_t fn = tuple_get(values, 0);

  // look for 'fn_data' which contains all information for the
  // overloaded function
  object_t fn_data = lookup(fn, fn_data_sym);
  list_t fn_data_l = 0;
  if (!fn_data)
    rha_error("(eval) %o can't be called, since it "
	      "has no 'fn_data' slot", fn);
  if (ptype(fn_data)!=LIST_T
      || list_len(fn_data_l=UNWRAP_PTR(LIST_T, fn_data)) == 0)
    rha_error("(eval) %o can't be called, since it "
	      "has a faulty 'fn_data' slot", fn);

  // construct the inner scope
  object_t local = new();
  assign(local, local_sym, local);   // the scope local to the function
  assign(local, this_sym, this);     // the calling scope
  assign(local, static_sym, fn);     // for static variables

  // find and call the matching implementation
  return find_and_call_matching_impl(local, fn_data_l, values);
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
    throw(excp_newf("(call_slot) object %o doesn't have slot '%s'",
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
