#ifndef CPLX_H__
#define CPLX_H__


typedef struct {
  double x;
  double y;
} cplx_t;


static inline __attribute__((nonnull(1,2,3)))
void cplx_prod(cplx_t *res, const cplx_t *a, const cplx_t *b)
{
  if (res == a || res == b)
    abort();

  res->x = a->x*b->x - a->y*b->y;
  if (a == b)
    res->y = 2 * a->x*b->y;
  else
    res->y = a->x*b->y + a->y*b->x;
}

static inline __attribute__((nonnull(1,2,3)))
void cplx_add(cplx_t *res, const cplx_t *a, const cplx_t *b)
{
  if (a == b) {
    res->x = 2*a->x;
    res->y = 2*a->y;
  } else {
    res->x = a->x + b->x;
    res->y = a->y + b->y;
  }
}

static inline __attribute__((nonnull(1)))
double cplx_mod2(const cplx_t *a)
{
  return a->x*a->x + a->y*a->y;
}


#endif /** CPLX_H__ */

