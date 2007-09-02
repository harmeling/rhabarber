#include "mat_fn.h"

#include <assert.h>

#include <cblas.h>
#include <clapack.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "messages.h"
#include "utils.h"
#include "string_fn.h"
#include "tuple_fn.h"

mat_t mat_new(int r, int c)
{
  mat_t m = ALLOC_SIZE(sizeof(struct _mat_t_));
  m->array = ALLOC_SIZE(sizeof(real_t)*r*c);
  m->rows = r;
  m->cols = c;
  m->len  = r*c;
  return m;
}

int_t mat_rows(mat_t m)
{
  return m->rows;
}

int_t mat_cols(mat_t m)
{
  return m->cols;
}


int_t mat_len(mat_t m)
{
  return m->len;
}

tuple_t mat_size(mat_t m)
{
  return tuple_make(2, WRAP_INT(m->rows), WRAP_INT(m->cols));
}

/*
 *
 * Element access
 *
 */

real_t mat_getl(mat_t m, int_t i)
{
  return m->array[i];
}

void mat_setl(mat_t m, int_t i, real_t v)
{
  m->array[i] = v;
}


real_t mat_get(mat_t m, int_t i, int_t j)
{
  int_t c = i + j*(m->rows);
  return m->array[c];
}


void mat_set(mat_t m, int_t i, int_t j, real_t v)
{
  int_t c = i + j*(m->rows);
  m->array[c] = v;
}


void mat_fillfromarray(mat_t m, real_t *array)
{
  memcpy(m->array, array, sizeof(*m->array) * m->len);
}


void mat_fillwithfunction(mat_t m, real_t (*f)())
{
  for(int_t i = 0; i < m->len; i++) {
    m->array[i] = (*f)();
  }
}


mat_t mat_copy(mat_t m)
{
  mat_t a = mat_new(m->rows, m->cols);
  memcpy(a->array, m->array, sizeof(*m->array) * m->len);
  return a;
}


mat_t mat_eye(int_t n)
{
  mat_t I = mat_new(n, n);
  for(int_t i = 0; i < n; i++)
    for(int_t j = 0; j < n; j++)
      mat_set(I, i, j, 0);
  for(int_t i = 0; i < n; i++)
    mat_set(I, i, i, 1);
  return I;
}


mat_t mat_transpose(mat_t a)
{
  mat_t b = mat_new(a->cols, a->rows);
  for(int_t i = 0; i < a->rows; i++)
    for(int_t j = 0; j < a->cols; j++) 
      mat_set(b, j, i, mat_get(a, i, j));
  return b;
}


real_t mat_norm(mat_t a)
{
  // Frobenius norm
  return cblas_ddot(a->len, a->array, 1, a->array, 1);
}


static real_t drand()
{
  return ((real_t)rand())/RAND_MAX;
}


static real_t drandn()
{
  real_t pi = 3.1415926536;
  real_t r, phi;
  while (!(r = drand())) ;    // r must be != 0
  while (!(phi = drand())) ;  // phi must be != 0
  return cos(2*pi*phi)*sqrt(-2*log(r));
}


mat_t mat_rand(int_t m, int_t n)
{
  mat_t X = mat_new(m, n);
  mat_fillwithfunction(X, drand);
  return X;
}


mat_t mat_randn(int_t m, int_t n)
{
  mat_t X = mat_new(m, n);
  mat_fillwithfunction(X, drandn);
  return X;
}


void mat_scale(mat_t a, real_t b)
{
  cblas_dscal(a->len, b, a->array, 1);
}


real_t mat_max(mat_t m)
{
  
  real_t cand = m->array[0];
  for (int_t i = 1; i < m->len; i++) {
    real_t dum = m->array[i];
    cand = (cand<dum) ? dum : cand;
  }
  return cand;
}


real_t mat_min(mat_t m)
{
  real_t cand = m->array[0];
  for (int_t i = 1; i < m->len; i++) {
    real_t dum = m->array[i];
    cand = (cand>dum) ? dum : cand;
  }
  return cand;
}


mat_t mat_solve(mat_t A, mat_t B)
{
  rha_warning("(mat_solve) please check implementation in mat.c:mat_solve()");
  mat_t AA = mat_copy(A); //mikio not sure about this!
  mat_t C = mat_copy(B);
  int_t *P = malloc(sizeof(int_t) * AA->rows);
  // where is it?
  //  clapack_dgesv(CblasColMajor, 
  //		AA->rows, C->cols,
  //		AA->array, AA->rows, 
  //		P,
  //		C->array, C->rows);
  free(P);		
  return C;
}


int lessthan_real_t(const void *a, const void *b) { 
  real_t *ap = (real_t *) a;
  real_t *bp = (real_t *) b;
  return *ap < *bp; 
}


mat_t mat_sort(mat_t A)
{
  extern int_t lessthan_real_t();
  mat_t B = mat_copy(A);
  //qsort(B->array, B->len, sizeof(real_t), lessthan_real_t);
  for(int_t i = 0; i < B->len; i++)
    for(int_t j = 0; j < B->len; j++) {
      if(B->array[i] > B->array[j]) {
	real_t swap = B->array[i];
	B->array[i] = B->array[j];
	B->array[j] = swap;
      }
    }
  return B;
}


void mat_map(mat_t m, real_t (*f)(real_t ))
{
  for(int_t i = 0; i < m->len; i++) {
    m->array[i] = (*f)(m->array[i]);
  }
}


void copy_col(mat_t a, mat_t u, int_t j)
{
  for (int_t i = 0; i < a->rows; i++) {
    real_t aij = mat_get(a, i, j);
    u->array[i] = aij;
  }
}


mat_t mat_repmat(mat_t a, int_t r, int_t c)
{
  // allocate memory for the result
  mat_t res = mat_new(r*(a->rows), c*(a->cols));
  int_t len = sizeof(*a->array) * a->rows;  // copy column by column
  for (int_t k=0; k<a->cols; k++) {
    real_t *inspot = &(a->array[k*(a->rows)]);
    for (int_t i=0; i<r; i++)
      for (int_t j=0; j<c; j++) {
	real_t *outspot = &(res->array[i*(a->rows) + j*(a->cols)*(a->rows)*r + k*(a->rows)*r]);
	memcpy(outspot, inspot, len);
      }
  }
  return res;
}


mat_t mat_reshape(mat_t a, int_t r, int_t c)
{
  // does the new size match the mat
  if (a->len != r*c)
    rha_error("(mat) reshape must not change size of the matrix");
  // change the shape
  a->rows = r;
  a->cols = c;
  return a;
}


mat_t mat_minus(mat_t a)
{
  mat_t m = mat_copy(a);
  for (int_t i=0; i<m->len; i++) m->array[i] = -m->array[i];
  return m;
}


mat_t mat_dotdivide_mat(mat_t a, mat_t b)
{
  // checks dims
  if (a->cols!=b->cols || a->rows!=b->rows)
    rha_error("(mat) dimension must agree for hadamard divide");
  mat_t c = mat_copy(a);
  for (int_t i=0; i<a->len; i++) c->array[i] = c->array[i] / b->array[i];
  return c;
}


mat_t mat_dottimes_mat(mat_t a, mat_t b)
{
  // checks dims
  if (a->cols!=b->cols || a->rows!=b->rows)
    rha_error("(mat) dimension must agree for hadamard produce");
  mat_t c = mat_copy(a);
  for (int_t i=0; i<a->len; i++) c->array[i] = c->array[i] * b->array[i];
  return c;
}


mat_t mat_plus_mat(mat_t a, mat_t b)
{
  // checks dims
  if (a->cols!=b->cols || a->rows!=b->rows)
    rha_error("(mat) dimension must agree for matrix sum");
  mat_t c = mat_copy(a);
  cblas_daxpy(b->len, 1.0, b->array, 1, c->array, 1);
  return c;
}


mat_t mat_plus_real(mat_t a, real_t r)
{
  mat_t m = mat_copy(a);
  for (int_t i=0; i<m->len; i++) m->array[i] = m->array[i] + r;
  return m;
}


mat_t real_plus_mat(real_t r, mat_t a)
{
  return mat_plus_real(a, r);
}


mat_t mat_minus_mat(mat_t a, mat_t b)
{
  // checks dims
  if (a->cols!=b->cols || a->rows!=b->rows)
    rha_error("(mat) dimension must agree for matrix minus");
  mat_t c = mat_copy(a);
  cblas_daxpy(b->len, -1.0, b->array, 1, c->array, 1);
  return c;
}


mat_t mat_minus_real(mat_t a, real_t r)
{
  mat_t m = mat_copy(a);
  for (int_t i=0; i<m->len; i++) m->array[i] = m->array[i] - r;
  return m;
}


mat_t real_minus_mat(real_t r, mat_t a)
{
  mat_t m = mat_copy(a);
  for (int_t i=0; i<m->len; i++) m->array[i] = r - m->array[i];
  return m;
}


mat_t mat_times_mat(mat_t a, mat_t b)
{
  // check dims
  if (a->cols != b->rows)
    rha_error("(mat) inner dimensions must match for matrix multiplication");
  mat_t c = mat_new(a->rows, b->cols);
  if(b->cols == 1) {
    // mat-vector multiplication
    cblas_dgemv(CblasColMajor, CblasNoTrans,
		a->rows, a->cols, 
		1.0, a->array, a->rows,
		b->array, 1,
		0.0, c->array, 1);
  }
  else {
    // mat-mat multiplication
    cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
		c->rows, c->cols, a->cols,
		1.0, a->array, a->rows,
		b->array, b->rows,
		0.0, c->array, c->rows);
  }
  return c;
}


mat_t mat_times_real(mat_t a, real_t r)
{
  mat_t res = mat_copy(a);
  mat_scale(res, r);
  return res;
}


mat_t mat_divide_real(mat_t a, real_t r)
{
  mat_t res = mat_copy(a);
  mat_scale(res, 1.0/r);
  return res;
}


mat_t real_times_mat(real_t r, mat_t a)
{
  return mat_times_real(a, r);
}


mat_t mat_exp(mat_t a)
// component-wise exponential
{
  mat_t r = mat_new(a->rows, a->cols);
  for (int_t i = 0; i < mat_len(a); i++) 
    mat_setl(r, i, exp(mat_getl(a, i)));
  return r;
}


mat_t mat_sin(mat_t a)
// component-wise sin
{
  mat_t r = mat_new(a->rows, a->cols);
  for (int_t i = 0; i < mat_len(a); i++) 
    mat_setl(r, i, sin(mat_getl(a, i)));
  return r;
}


mat_t mat_sinc(mat_t a)
// component-wise sinc
{
  mat_t r = mat_new(a->rows, a->cols);
  for (int_t i = 0; i < mat_len(a); i++) {
    real_t ddd = mat_getl(a, i);
    if (ddd==0) mat_setl(r, i, 1);
    else mat_setl(r, i, sin(ddd)/ddd);
  }
  return r;
}


string_t mat_to_string(mat_t m)
{
  string_t s = sprint("");
  for(int i = 0; i < m->rows; i++) {
    for(int j = 0; j < m->cols; j++) {
      if (j) s = string_concat(s, "  ");
      s = string_concat(s, to_string(WRAP_REAL(mat_get(m, i, j))));
    }
    s = string_concat(s, "\n");
  }
  return s;
}
