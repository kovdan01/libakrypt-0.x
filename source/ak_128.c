/* ----------------------------------------------------------------------------------------------- */
/*  Copyright (c) 2021 by Daniil Kovalev, kovdan01@yandex.ru                                       */
/*                                                                                                 */
/*  Файл ak_128.c                                                                                  */
/*  - содержит реализации функций для вычислений с 128-битными числами                             */
/* ----------------------------------------------------------------------------------------------- */
 #include <libakrypt.h>

/* ----------------------------------------------------------------------------------------------- */
/*                                  арифметические операции                                        */
/* ----------------------------------------------------------------------------------------------- */

 ak_uint64 ak_128_add( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y )
{
  z[0] = x[0] + y[0];
  z[1] = x[1] + y[1];
  ak_uint64 ret = z[1] < x[1];
  z[1] += (z[0] < x[0]);
  return ret;
}

 void ak_128_add_mod( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y, const ak_uint64 *p )
{
  z[0] = x[0] + y[0];
  z[1] = x[1] + y[1];
  int tmp = z[1] < x[1];
  z[1] += (z[0] < x[0]);

  // if sum is great or equal than p
  tmp = (tmp == 1 ? ak_true : (z[1] < p[1] ? ak_false : (z[0] < p[0] ? ak_false : ak_true)));
  if (tmp == ak_false)
    return;
  tmp = z[0] < p[0];
  z[0] -= p[0];
  z[1] -= p[1];
  z[1] -= tmp;
}

#define umul_ppmm(w1, w0, u, v) \
  __asm__ ("mulq %3" : "=a,a" (w0), "=d,d" (w1) : "%0,0" (u), "r,m" (v))

 void ak_128_mul( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y )
{
  ak_uint64 tmp;

  ak_uint64 x0muly[2]; // два старших разряда, младший - сразу в z[0]
  umul_ppmm( tmp, z[0], x[0], y[0] );
  umul_ppmm( x0muly[1], x0muly[0], x[0], y[1] );
  x0muly[0] += tmp;
  x0muly[1] += x0muly[0] < tmp;

  ak_uint64 x1muly[2]; // два младших разряда, старший - сразу в z[3]
  umul_ppmm( x1muly[1], x1muly[0], x[1], y[0] );
  umul_ppmm( z[3], tmp, x[1], y[1] );
  x1muly[1] += tmp;
  z[3] += x1muly[1] < tmp;

  z[1] = x0muly[0] + x1muly[0];
  z[2] = x0muly[1] + x1muly[1];
  tmp = z[2] < x0muly[1];
  z[2] += z[1] < x0muly[0];
  z[3] += tmp;
}

 static void rshift_1_bit( ak_uint64 *x )
{
  x[0] >>= 1;
  x[0] |= (ak_uint64)(x[1] & 1) << 63;
  x[1] >>= 1;
  x[1] |= (ak_uint64)(x[2] & 1) << 63;
  x[2] >>= 1;
}

 static int sub_2digits( ak_uint64 *x, ak_uint64 *y )
{
  if ( x[1] > y[1] )
  {
    x[1] -= x[0] < y[0];
    x[0] -= y[0];
    x[1] -= y[1];
    return 1;
  }

  if ( x[1] == y[1] && x[0] >= y[0] )
  {
    x[1] = 0;
    x[0] -= y[0];
    return 1;
  }
  return 0;
}

 static void sub_3digits( ak_uint64 *x, ak_uint64 *y )
{
  if ( x[2] > y[2] )
  {
    ak_uint64 need_one = ( x[1] < y[1] ) || ( x[1] == y[1] && x[0] < y[0] );
    x[1] -= x[0] < y[0];
    x[0] -= y[0];
    x[1] -= y[1];
    x[2] -= y[2];
    x[2] -= need_one;
    return;
  }

  if ( x[2] == y[2] )
  {
    if (sub_2digits( x, y ))
      x[2] = 0;
  }
}

 void ak_128_rem( ak_uint64 *r, const ak_uint64 *u, const ak_uint64 *p )
{
  ak_uint64 pshifted[4] = { 0, 0, p[0], p[1] };
  ak_uint64 x[4] = { u[0], u[1], u[2], u[3] };

  // 0
  sub_2digits( x + 2, pshifted + 2 );
  pshifted[1] |= (ak_uint64)(pshifted[2] & 1) << 63;
  pshifted[2] >>= 1;
  pshifted[2] |= (ak_uint64)(pshifted[3] & 1) << 63;
  pshifted[3] >>= 1;

  // 1 - 62
  for ( int i = 0; i < 62; ++i )
  {
    sub_3digits( x + 1, pshifted + 1 );
    rshift_1_bit( pshifted + 1 );
  }

  // 63
  sub_3digits( x + 1, pshifted + 1 );
  pshifted[1] = p[0];
  pshifted[2] = p[1];
  pshifted[3] = 0;

  // 0
  sub_3digits( x + 1, pshifted + 1 );
  pshifted[0] |= (ak_uint64)(pshifted[1] & 1) << 63;
  pshifted[1] >>= 1;
  pshifted[1] |= (ak_uint64)(pshifted[2] & 1) << 63;
  pshifted[2] >>= 1;

  // 1 - 62
  for ( int i = 0; i < 62; ++i )
  {
    sub_3digits( x, pshifted );
    rshift_1_bit( pshifted );
  }

  // 63
  sub_3digits( x, pshifted );
  pshifted[0] = p[0];
  pshifted[1] = p[1];
  pshifted[2] = 0;

  sub_3digits( x, pshifted );

  r[0] = x[0];
  r[1] = x[1];
}

 void ak_128_mul_mod( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y, const ak_uint64 *p )
{
  ak_uint64 tmp[4];
  ak_128_mul( tmp, x, y );
  ak_128_rem( z, tmp, p );
}
