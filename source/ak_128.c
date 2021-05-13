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

 static ak_uint64 sub_2digits( ak_uint64 *x, ak_uint64 *y )
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

 static ak_uint64 sub_3digits( ak_uint64 *x, ak_uint64 *y )
{
  if ( x[2] > y[2] )
  {
    ak_uint64 need_one = ( x[1] < y[1] ) || ( x[1] == y[1] && x[0] < y[0] );
    x[1] -= x[0] < y[0];
    x[0] -= y[0];
    x[1] -= y[1];
    x[2] -= y[2];
    x[2] -= need_one;
    return 1;
  }

  if ( x[2] == y[2] )
  {
    if (sub_2digits( x, y ))
    {
      x[2] = 0;
      return 1;
    }
  }

  return 0;
}

 // assuming u < p * 2^128
 void ak_128_rem( ak_uint64 *r, const ak_uint64 *u, const ak_uint64 *p )
{
  ak_uint64 pshifted[4] = { 0, 0, p[0], p[1] };
  ak_uint64 x[4] = { u[0], u[1], u[2], u[3] };

  // 0
  // sub_2digits( x + 2, pshifted + 2 );
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

 // assuming u < p * 2^128
 void ak_128_div( ak_uint64 *r, ak_uint64 *d, const ak_uint64 *u, const ak_uint64 *p )
{
  ak_uint64 pshifted[4] = { 0, 0, p[0], p[1] };
  ak_uint64 x[4] = { u[0], u[1], u[2], u[3] };
  d[0] = 0;
  d[1] = 0;
  // 0
  pshifted[1] |= (ak_uint64)(pshifted[2] & 1) << 63;
  pshifted[2] >>= 1;
  pshifted[2] |= (ak_uint64)(pshifted[3] & 1) << 63;
  pshifted[3] >>= 1;

  // 1 - 62
  for ( int i = 0; i < 62; ++i )
  {
    d[1] |= sub_3digits( x + 1, pshifted + 1 ) << (63 - i);
    rshift_1_bit( pshifted + 1 );
  }

  // 63
  d[1] |= sub_3digits( x + 1, pshifted + 1 ) << 1;
  pshifted[1] = p[0];
  pshifted[2] = p[1];
  pshifted[3] = 0;

  // 0
  d[1] |= sub_3digits( x + 1, pshifted + 1 );
  pshifted[0] |= (ak_uint64)(pshifted[1] & 1) << 63;
  pshifted[1] >>= 1;
  pshifted[1] |= (ak_uint64)(pshifted[2] & 1) << 63;
  pshifted[2] >>= 1;

  // 1 - 62
  for ( int i = 0; i < 62; ++i )
  {
    d[0] |= sub_3digits( x, pshifted ) << (63 - i);
    rshift_1_bit( pshifted );
  }

  // 63
  d[0] |= sub_3digits( x, pshifted ) << 1;
  pshifted[0] = p[0];
  pshifted[1] = p[1];
  pshifted[2] = 0;

  d[0] |= sub_3digits( x, pshifted );

  r[0] = x[0];
  r[1] = x[1];
}

 void ak_128_mul_mod( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y, const ak_uint64 *p )
{
  ak_uint64 tmp[4];
  ak_128_mul( tmp, x, y );
  ak_128_rem( z, tmp, p );
}

 static void ak_128_gcd( const ak_uint64* a, const ak_uint64* b, ak_uint64 *x, ak_uint64 *y )
{
  if ( a[0] == 0 && a[1] == 0 )
  {
    x[0] = 0;
    x[1] = 0;
    y[0] = 1;
    y[1] = 0;
    return;
  }
  ak_uint64 x1[4] = { 0, 0, 0, 0 };
  ak_uint64 y1[4] = { 0, 0, 0, 0 };
  ak_uint64 b_div_a[4] = { 0, 0, 0, 0 };
  ak_uint64 b_mod_a[4] = { 0, 0, 0, 0 };

  ak_128_div( b_mod_a, b_div_a, b, a );

  ak_128_gcd( b_mod_a, a, x1, y1 );
  ak_uint64 b_div_a_mul_x1[4];
  ak_128_mul( b_div_a_mul_x1, b_div_a, x1 );
  ak_mpzn_sub( x, y1, b_div_a_mul_x1, 4 );
  for (int i = 0; i < 4; ++i)
    y[i] = x1[i];
}

 void ak_128_opposite( ak_uint64 *o, const ak_uint64 *x, const ak_uint64 *p )
{
  ak_uint64 x4[4] = { x[0], x[1], 0, 0 };
  ak_uint64 p4[4] = { p[0], p[1], 0, 0 };
  ak_uint64 ans[4];
  ak_uint64 tmp[4];

  ak_128_gcd( x4, p4, ans, tmp );
  o[0] = ans[0];
  o[1] = ans[1];
}

 static ak_uint64 sub_128( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y )
{
  z[0] = x[0] - y[0];
  z[1] = x[1] - y[1];
  ak_uint64 ans = z[1] > x[1];
  ak_uint64 before = z[1];
  z[1] -= z[0] > x[0];
  ans |= before < z[1];
  return ans;
}

 static void sub_128_mod( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y, const ak_uint64 *p )
{
  if ( sub_128( z, x, y ) )
  {
    z[0] += p[0];
    z[1] += p[1];
    z[1] += p[0] > z[0];
  }
}

 void ak_128_double_point( ak_uint64 *x3, ak_uint64 *y3, const ak_uint64 *x1, const ak_uint64 *y1, const ak_uint64 *p )
{
  // TODO
  x3[0] = 0;
  x3[1] = 0;
  y3[0] = 0;
  y3[1] = 0;
}

 void ak_128_add_points( ak_uint64 *x3, ak_uint64 *y3, const ak_uint64 *x1, const ak_uint64 *y1, const ak_uint64 *x2, const ak_uint64 *y2, const ak_uint64 *p )
{
  if ( x1[0] == x2[0] && x1[1] == x2[1] )
  {
    ak_uint64 y1_plus_y2[2];
    ak_128_add_mod( y1_plus_y2, y1, y2, p );
    if ( y1_plus_y2[0] == 0 && y1_plus_y2[0] == 0 )
    {
      x3[0] = 0;
      x3[1] = 0;
      y3[0] = 0;
      y3[1] = 0;
      return;
    }
    if ( y1[0] == y2[0] && y1[1] == y2[1] )
    {
      ak_128_double_point(x3, y3, x1, y1, p);
    }
  }
  ak_uint64 num[2];
  ak_uint64 den[2];
  sub_128_mod( num, y1, y2, p );
  sub_128_mod( den, x1, x2, p );
  ak_uint64 den_opp[2];
  ak_128_opposite( den_opp, den, p );
  ak_uint64 s[2];
  ak_uint64 s_2[2];
  ak_128_mul_mod( s, num, den_opp, p );
  ak_128_mul_mod( s_2, s, s, p );
  ak_uint64 s_2_minus_x1[2];
  sub_128_mod( s_2_minus_x1, s_2, x1, p );
  sub_128_mod( x3, s_2_minus_x1, x2, p );
  ak_uint64 x3_minus_x1[2];
  sub_128_mod( x3_minus_x1, x3, x1, p );
  ak_uint64 s_mul_x3_minus_x1[2];
  ak_128_mul_mod( s_mul_x3_minus_x1, s, x3_minus_x1, p );
  ak_128_add_mod( y3, y1, s_mul_x3_minus_x1, p );
}
