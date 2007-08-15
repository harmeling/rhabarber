/*
 * tuple_fn - tuple functions
 *
 * This file is part of rhabarber.
 *
 * (c) 2005-2007 by Mikio Braun          & Stefan Harmeling
 *                  mikiobraun@gmail.com   harmeling@gmail.com
 *                             
 */

#ifndef TUPLE_FN_H
#define TUPLE_FN_H

#include "rha_types.h"

extern _rha_ tuple_t  tuple_new(int_t len);
extern _rha_ int_t    tuple_len(tuple_t t);
extern _rha_ object_t tuple_get(tuple_t t, int_t i);
extern _rha_ void     tuple_set(tuple_t t, int_t i, object_t s);

extern tuple_t tuple_make(int, ...);

#endif