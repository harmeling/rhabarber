// -*- C -*-
// this file is used by rha_stub.pl
// and all c-code that should be accessible in rhabarber

//MODULES
#include "object.h"
#include "eval.h"
#include "core.h"   // must go before prules.h, see prules_init()
#include "prules.h" // must go before parse.h, see parse_init()
#include "parse.h"
#include "symbol_fn.h"
#include "tuple_fn.h"
#include "list_fn.h"
#include "int_fn.h"
#include "real_fn.h"
#include "string_fn.h"
#include "bool_fn.h"
#include "mat_fn.h"
#include "messages.h"

//TYPES
#include <stdbool.h>
#include "gtuple.h"
#include "glist.h"
struct rha_object;
typedef int                       _rha_ symbol_t;
typedef struct rha_object *       _rha_ any_t;
typedef int                       _rha_ int_t; 
typedef bool                      _rha_ bool_t;
typedef struct gtuple*            _rha_ tuple_t;
typedef any_t (*_builtin_t)(tuple_t);
typedef _builtin_t                _rha_ builtin_t;
typedef double                    _rha_ real_t;
typedef struct _mat_t_ { 
  real_t *array;
  int_t  rows;
  int_t  cols;
  int_t  len;
}*                                _rha_ mat_t;
typedef char *                    _rha_ string_t;
typedef void *                    _rha_ address_t;
typedef struct glist *            _rha_ list_t;
typedef glist_iterator_t *        _rha_ list_it_t;

//SYMBOLS
// the most basic symbols
extern symbol_t parent_sym;
extern symbol_t proto_sym;
extern symbol_t do_sym;
extern symbol_t quote_sym;
extern symbol_t type_sym;
extern symbol_t ptype_sym;
extern symbol_t this_sym;
extern symbol_t static_sym;
extern symbol_t root_sym;
extern symbol_t local_sym;
extern symbol_t void_sym;
extern symbol_t parent_sym;

// special slots elsewhere
extern symbol_t fn_data_sym;
extern symbol_t scope_sym;
extern symbol_t signature_sym;
extern symbol_t argnames_sym;
extern symbol_t fnbody_sym;
extern symbol_t priority_sym;
extern symbol_t hasproxy_sym;
extern symbol_t iter_sym;
extern symbol_t done_sym;
extern symbol_t get_sym;
extern symbol_t next_sym;
extern symbol_t msg_sym;
extern symbol_t ismacro_sym;
extern symbol_t check_sym;
extern symbol_t name_sym;
extern symbol_t args_sym;
extern symbol_t varargs_sym;
extern symbol_t no_frame_sym;

// special slots in root
extern symbol_t modules_sym;
extern symbol_t keywords_sym;
extern symbol_t prules_sym;

// special symbols for parsing
extern symbol_t curlied_sym;
extern symbol_t squared_sym;
extern symbol_t rounded_sym;
extern symbol_t tuple_forced_sym;

// other special symbols
extern symbol_t method_call_sym;

// special function (not directly defined in C)
extern symbol_t not_fn_sym;
extern symbol_t and_fn_sym;
extern symbol_t or_fn_sym;
extern symbol_t plus_fn_sym;
extern symbol_t minus_fn_sym;
extern symbol_t neg_fn_sym;
extern symbol_t times_fn_sym;
extern symbol_t divide_fn_sym;
extern symbol_t dottimes_fn_sym;
extern symbol_t dotdivide_fn_sym;
extern symbol_t equalequal_fn_sym;
extern symbol_t notequal_fn_sym;
extern symbol_t less_fn_sym;
extern symbol_t lessequal_fn_sym;
extern symbol_t greater_fn_sym;
extern symbol_t greaterequal_fn_sym;
extern symbol_t pattern_sym;
extern symbol_t signature_sym;
extern symbol_t apostrophe_fn_sym;

//MACROS
#define UNWRAP_INT(o)     (unwrap_int(INT_T, o))
#define UNWRAP_BOOL(o)    (unwrap_int(BOOL_T, o))
#define UNWRAP_SYMBOL(o)  (unwrap_int(SYMBOL_T, o))
#define UNWRAP_REAL(o)    (unwrap_double(REAL_T, o))
#define UNWRAP_BUILTIN(o) (unwrap_builtin(BUILTIN_T, o))
#define UNWRAP_PTR(pt, o) (unwrap_ptr(pt, o))

#define WRAP_INT(i)       (wrap_int(INT_T, i))
#define WRAP_BOOL(b)      (wrap_int(BOOL_T, b))
#define WRAP_SYMBOL(s)    (wrap_int(SYMBOL_T, s))
#define WRAP_REAL(d)      (wrap_double(REAL_T, d))
#define WRAP_BUILTIN(p)   (wrap_builtin(BUILTIN_T, p))
#define WRAP_PTR(pt, p)   (wrap_ptr(pt, p))

//PRULES
