/* ----------------------------------------------------------------------------------------------- */
/*  Copyright (c) 2021 by Daniil Kovalev, kovdan01@yandex.ru                                       */
/*                                                                                                 */
/*  Файл ak_128.c                                                                                  */
/*  - содержит реализации функций для вычислений с 128-битными числами                             */
/* ----------------------------------------------------------------------------------------------- */
 #include <libakrypt.h>
 #include <gmp.h>

 inline int ak_128_equal( const ak_uint64 *x, const ak_uint64 *y )
{
  return ( x[0] == y[0] && x[1] == y[1] );
}

 inline int ak_128_is_zero( const ak_uint64 *x )
{
  return ( x[0] == 0 && x[1] == 0 );
}

 inline int ak_128_is_one( const ak_uint64 *x )
{
  return ( x[0] == 1 && x[1] == 0 );
}

 inline void ak_128_set_zero( ak_uint64 *x )
{
  x[0] = 0;
  x[1] = 0;
}

 inline void ak_128_set_one( ak_uint64 *x )
{
  x[0] = 1;
  x[1] = 0;
}

/* ----------------------------------------------------------------------------------------------- */
/*                                  арифметические операции                                        */
/* ----------------------------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------------------------- */
/*! Функция складывает два 128-битных числа и возвращает знак переноса. Результат сложения
   помещается в переменную z. Указатель на z не может совпадать с указателями на слагаемые.

    @param z Указатель на 128-битное число, в которое помещается результат
    @param x Левый аргумент опреации сложения
    @param y Правый аргумент операции сложения
    @return Знак переноса (1, если x + y >= 2^128, 0 в противном случае)                           */
/* ----------------------------------------------------------------------------------------------- */
 inline ak_uint64 ak_128_add( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y )
{
  z[0] = x[0] + y[0];
  z[1] = x[1] + y[1];
  ak_uint64 ret = (z[1] < x[1]) || ((z[1] == 0xffffffffffffffffllu) && (z[0] < x[0]));
  z[1] += (z[0] < x[0]);
  return ret;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция складывает два вычета по заданному 128-битному модулю. Результат сложения помещается
    в переменную z. Указатель на z не может совпадать с указателями на слагаемые и модуль.

    @param z Указатель на вычет, в который помещается результат
    @param x Левый аргумент опреации сложения
    @param y Правый аргумент операции сложения
    @param p Модуль, по которому производятся вычисления                                           */
/* ----------------------------------------------------------------------------------------------- */
 inline void ak_128_add_mod( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y, const ak_uint64 *p )
{
  z[0] = x[0] + y[0];
  z[1] = x[1] + y[1];
  ak_uint64 tmp = z[1] < x[1];
  z[1] += z[0] < x[0];

  // если сумма больше либо равна p
  tmp = (tmp == 1 ? ak_true : (z[1] > p[1] ? ak_true : ((z[1] == p[1] && z[0] > p[0]) ? ak_true : ak_false)));
  if (tmp == ak_false)
    return;
  tmp = z[0] < p[0];
  z[0] -= p[0];
  z[1] -= p[1];
  z[1] -= tmp;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция вычитает одно 128-битное число из другого и возвращает знак переноса. Результат вычитания
    помещается в переменную z. Указатель на z не может совпадать с указателями на операнды.

    @param z Указатель на 128-битное число, в которое помещается результат
    @param x Число, из которого происходит вычитание
    @param y Вычитаемое
    @return Знак переноса (1, если x < y, 0 в противном случае)                                    */
/* ----------------------------------------------------------------------------------------------- */
 inline ak_uint64 ak_128_sub( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y )
{
  z[0] = x[0] - y[0];
  z[1] = x[1] - y[1];
  z[1] -= z[0] > x[0];
  return ((x[1] < y[1]) || (x[1] == y[1] && x[0] < y[0]));
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция реализует операцию вычитания двух вычетов по 128-битному модулю. Результат вычитания
    помещается в переменную z. Указатель на z не может совпадать с указателями на операнды и модуль.

    @param z Указатель на вычет, в который помещается результат
    @param x Вычет, из которого происходит вычитание
    @param y Вычитаемое
    @param p Модуль, по которому производятся вычисления                                           */
/* ----------------------------------------------------------------------------------------------- */
 inline void ak_128_sub_mod( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y, const ak_uint64 *p )
{
  if ( ak_128_sub( z, x, y ) )
  {
    z[0] += p[0];
    z[1] += p[1];
    z[1] += p[0] > z[0];
  }
}

#define umul_ppmm(w1, w0, u, v) \
  __asm__ ("mulq %3" : "=a,a" (w0), "=d,d" (w1) : "%0,0" (u), "r,m" (v))

/* ----------------------------------------------------------------------------------------------- */
/*! Функция перемножает два 128-битных числа. Результат умножения помещается в 256-битное число z.
    Указатель на z не может совпадать с указателями на множители.

    @param z Указатель на 256-битное число, в которое помещается результат
    @param x Левый аргумент опреации умножения
    @param y Правый аргумент операции умножения                                                    */
/* ----------------------------------------------------------------------------------------------- */
 inline void ak_128_mul( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y )
{
  ak_uint64 tmp;

  ak_uint64 x0muly[2];  // два старших разряда, младший - сразу в z[0]
  umul_ppmm( tmp, z[0], x[0], y[0] );
  umul_ppmm( x0muly[1], x0muly[0], x[0], y[1] );
  x0muly[0] += tmp;
  x0muly[1] += x0muly[0] < tmp;

  ak_uint64 x1muly[2];  // два младших разряда, старший - сразу в z[3]
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

/* ----------------------------------------------------------------------------------------------- */
/*! Функция перемножает два вычета по заданному 128-битному модулю. Результат умножения помещается
    в переменную z. Указатель на z не может совпадать с указателями на множители и модуль.

    @param z Указатель на вычет, в который помещается результат
    @param x Левый аргумент опреации умножения
    @param y Правый аргумент операции умножения
    @param p Модуль, по которому производятся вычисления                                           */
/* ----------------------------------------------------------------------------------------------- */
 inline void ak_128_mul_mod( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y, const ak_uint64 *p )
{
  ak_mpzn256 tmp;
  ak_128_mul( tmp, x, y );
  ak_128_rem( z, tmp, p );
}

 static void detail_128_rshift_1_bit( ak_uint64 *x )
{
  x[0] >>= 1;
  x[0] |= (ak_uint64)(x[1] & 1) << 63;
  x[1] >>= 1;
  x[1] |= (ak_uint64)(x[2] & 1) << 63;
  x[2] >>= 1;
}

 inline static ak_uint64 detail_128_try_sub_2_words( ak_uint64 *x, const ak_uint64 *y )
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

 inline static ak_uint64 detail_128_try_sub_3_words( ak_uint64 *x, const ak_uint64 *y )
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
    if (detail_128_try_sub_2_words( x, y ))
    {
      x[2] = 0;
      return 1;
    }
  }

  return 0;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция получает остаток от деления 256-битного числа на 128-битный модуль. Результат помещается
    в переменную r. Указатель на r не может совпадать с указателями на делимое и модуль.
    Предполагается, что p >= 2^127.

    @param r Указатель на вычет, в который помещается остаток
    @param u Делимое
    @param p Модуль, по которому производятся вычисления                                           */
/* ----------------------------------------------------------------------------------------------- */
 void ak_128_rem( ak_uint64 *r, const ak_uint64 *u, const ak_uint64 *p )
{
  ak_mpzn256 pshifted = { 0, 0, p[0], p[1] };
  ak_mpzn256 x = { u[0], u[1], u[2], u[3] };

  // 0
  detail_128_try_sub_2_words( x + 2, pshifted + 2 );
  pshifted[1] |= (ak_uint64)(pshifted[2] & 1) << 63;
  pshifted[2] >>= 1;
  pshifted[2] |= (ak_uint64)(pshifted[3] & 1) << 63;
  pshifted[3] >>= 1;

  // 1 - 62
  for ( int i = 0; i < 62; ++i )
  {
    detail_128_try_sub_3_words( x + 1, pshifted + 1 );
    detail_128_rshift_1_bit( pshifted + 1 );
  }

  // 63
  detail_128_try_sub_3_words( x + 1, pshifted + 1 );
  pshifted[1] = p[0];
  pshifted[2] = p[1];
  pshifted[3] = 0;

  // 0
  detail_128_try_sub_3_words( x + 1, pshifted + 1 );
  pshifted[0] |= (ak_uint64)(pshifted[1] & 1) << 63;
  pshifted[1] >>= 1;
  pshifted[1] |= (ak_uint64)(pshifted[2] & 1) << 63;
  pshifted[2] >>= 1;

  // 1 - 62
  for ( int i = 0; i < 62; ++i )
  {
    detail_128_try_sub_3_words( x, pshifted );
    detail_128_rshift_1_bit( pshifted );
  }

  // 63
  detail_128_try_sub_3_words( x, pshifted );
  pshifted[0] = p[0];
  pshifted[1] = p[1];
  pshifted[2] = 0;

  detail_128_try_sub_3_words( x, pshifted );

  r[0] = x[0];
  r[1] = x[1];
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция получает частное и остаток от деления 256-битного числа на 128-битный модуль. Результат
    помещается в переменную r. Указатель на r не может совпадать с указателями на делимое и модуль.
    Предполагается, что p >= 2^127.

    @param q Указатель на вычет, в который помещаются младшие 128 бит частного
    @param r Указатель на вычет, в который помещается остаток
    @param u Делимое
    @param p Модуль, по которому производятся вычисления
    @return Бит переноса (1, если p * 2^128 <= u, 0 в противном случае)                            */
/* ----------------------------------------------------------------------------------------------- */
 ak_uint64 ak_128_div( ak_uint64 *q, ak_uint64 *r, const ak_uint64 *u, const ak_uint64 *p )
{
  ak_mpzn256 pshifted = { 0, 0, p[0], p[1] };
  ak_mpzn256 x = { u[0], u[1], u[2], u[3] };
  q[0] = 0;
  q[1] = 0;
  // 0
  ak_uint64 ret = detail_128_try_sub_2_words( x + 2, pshifted + 2 );  // бит 2^128
  pshifted[1] |= (ak_uint64)(pshifted[2] & 1) << 63;
  pshifted[2] >>= 1;
  pshifted[2] |= (ak_uint64)(pshifted[3] & 1) << 63;
  pshifted[3] >>= 1;

  // 1 - 62
  for ( int i = 0; i < 62; ++i )
  {
    q[1] |= detail_128_try_sub_3_words( x + 1, pshifted + 1 ) << (63 - i);
    detail_128_rshift_1_bit( pshifted + 1 );
  }

  // 63
  q[1] |= detail_128_try_sub_3_words( x + 1, pshifted + 1 ) << 1;
  pshifted[1] = p[0];
  pshifted[2] = p[1];
  pshifted[3] = 0;

  // 0
  q[1] |= detail_128_try_sub_3_words( x + 1, pshifted + 1 );
  pshifted[0] |= (ak_uint64)(pshifted[1] & 1) << 63;
  pshifted[1] >>= 1;
  pshifted[1] |= (ak_uint64)(pshifted[2] & 1) << 63;
  pshifted[2] >>= 1;

  // 1 - 62
  for ( int i = 0; i < 62; ++i )
  {
    q[0] |= detail_128_try_sub_3_words( x, pshifted ) << (63 - i);
    detail_128_rshift_1_bit( pshifted );
  }

  // 63
  q[0] |= detail_128_try_sub_3_words( x, pshifted ) << 1;
  pshifted[0] = p[0];
  pshifted[1] = p[1];
  pshifted[2] = 0;

  q[0] |= detail_128_try_sub_3_words( x, pshifted );

  r[0] = x[0];
  r[1] = x[1];

  return ret;
}

 static void detail_128_gcd_mpz( const mpz_t a, const mpz_t b, mpz_t x, mpz_t y )
{
  if ( mpz_cmp_ui( a, 0 ) == 0 )
  {
    mpz_set_ui( x, 0 );
    mpz_set_ui( y, 1 );
    return;
  }

  mpz_t x1, y1, b_div_a, b_mod_a;
  mpz_init_set_ui( x1, 0 );
  mpz_init_set_ui( y1, 0 );
  mpz_init( b_div_a );
  mpz_init( b_mod_a );

  mpz_div( b_div_a, b, a );
  mpz_mod( b_mod_a, b, a );

  detail_128_gcd_mpz( b_mod_a, a, x1, y1 );

  mpz_t b_div_a_mul_x1;
  mpz_init_set_ui( b_div_a_mul_x1, 0 );
  mpz_mul( b_div_a_mul_x1, b_div_a, x1 );
  mpz_sub( x, y1, b_div_a_mul_x1 );
  mpz_set( y, x1 );

  mpz_clear( b_div_a_mul_x1 );
  mpz_clear( b_mod_a );
  mpz_clear( b_div_a );
  mpz_clear( y1 );
  mpz_clear( x1 );
}

 static void detail_128_to_mpzn( const mpz_t x_, ak_uint64 *x, const ak_uint64 *p )
{
  if ( x_->_mp_size < 0 )
  {
    ak_mpzn128 tmp = ak_mpzn128_zero;
    memcpy( tmp, x_->_mp_d, -x_->_mp_size*sizeof( ak_uint64 ) );
    ak_128_sub( x, p, tmp );
  }
  else
  {
    memcpy( x, x_->_mp_d, x_->_mp_size*sizeof( ak_uint64 ) );
  }
}

 static void detail_128_gcd( const ak_uint64 *a, const ak_uint64 *b, ak_uint64 *x, ak_uint64 *y )
{
  mpz_t a_, b_, x_, y_;
  mpz_init(a_);
  mpz_init(b_);
  mpz_init(x_);
  mpz_init(y_);

  ak_mpzn_to_mpz(a, ak_mpzn256_size, a_);
  ak_mpzn_to_mpz(b, ak_mpzn256_size, b_);
  ak_mpzn_to_mpz(x, ak_mpzn256_size, x_);
  ak_mpzn_to_mpz(y, ak_mpzn256_size, y_);
  detail_128_gcd_mpz(a_, b_, x_, y_);

  detail_128_to_mpzn(x_, x, b);
  detail_128_to_mpzn(y_, y, b);

  mpz_clear(y_);
  mpz_clear(x_);
  mpz_clear(b_);
  mpz_clear(a_);
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция получает обратный элемент в кольце вычетов по 128-битный модулю. Результат помещается
    в переменную o. Указатель на r не может совпадать с указателями на исходное число и модуль.
    Предполагается, что p >= 2^127.

    @param o Указатель на вычет, в который помещается обратный элемент
    @param x Указатель на вычет, обратный элемент от которого требуется взять
    @param p Модуль, по которому производятся вычисления                                           */
/* ----------------------------------------------------------------------------------------------- */
 inline void ak_128_inverse( ak_uint64 *o, const ak_uint64 *x, const ak_uint64 *p )
{
  ak_mpzn256 x4 = { x[0], x[1], 0, 0 };
  ak_mpzn256 p4 = { p[0], p[1], 0, 0 };
  ak_mpzn256 ans;
  ak_mpzn256 tmp;

  detail_128_gcd( x4, p4, ans, tmp );
  o[0] = ans[0];
  o[1] = ans[1];
}

 void ak_128_point_double( ak_point128 c, const ak_point128 a, const ak_uint64 *p )
{
  // TODO
  ak_128_set_zero(c->x);
  ak_128_set_zero(c->y);
}

 void ak_128_point_add( ak_point128 c, const ak_point128 a, const ak_point128 b, const ak_uint64 *p )
{
  if ( ak_128_equal( a->x, b->x ) )
  {
    ak_mpzn128 y1_plus_y2;
    ak_128_add_mod( y1_plus_y2, a->y, b->y, p );
    if ( ak_128_is_zero( y1_plus_y2 ) )
    {
      ak_128_set_zero( c->x );
      ak_128_set_zero( c->y );
      return;
    }
    if ( ak_128_equal( a->y, b->y ) )
    {
      ak_128_point_double( c, a, p );
      return;
    }
  }
  ak_mpzn128 num;
  ak_mpzn128 den;
  ak_128_sub_mod( num, a->y, b->y, p );
  ak_128_sub_mod( den, a->x, b->x, p );
  ak_mpzn128 den_inv;
  ak_128_inverse( den_inv, den, p );
  ak_mpzn128 s;
  ak_mpzn128 s_2;
  ak_128_mul_mod( s, num, den_inv, p );
  ak_128_mul_mod( s_2, s, s, p );
  ak_mpzn128 s_2_minus_x1;
  ak_128_sub_mod( s_2_minus_x1, s_2, a->x, p );
  ak_128_sub_mod( c->x, s_2_minus_x1, b->x, p );
  ak_mpzn128 x3_minus_x1;
  ak_128_sub_mod( x3_minus_x1, c->x, a->x, p );
  ak_mpzn128 s_mul_x3_minus_x1;
  ak_128_mul_mod( s_mul_x3_minus_x1, s, x3_minus_x1, p );
  ak_128_add_mod( c->y, a->y, s_mul_x3_minus_x1, p );
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция инициализирует контекст с данными, необходимыми для выполнения операций Монтгомери.

    @param ctx Указатель на контекст, данные которого необходимо заполнить. Поле p контекста должно
               содержать модуль, по которому будут производиться вычисления. Функция использует эту
               информацию и заполняет поля r, r2, v.                                               */
/* ----------------------------------------------------------------------------------------------- */
 void ak_128_montgomery_init( ak_montgomery_context_128 *ctx )
{
  ak_mpzn256 r4 = { 0, 0, 1, 0 };
  ak_128_sub( ctx->r, r4, ctx->p );

  ak_mpzn256 tmp = { 0, 0, ctx->r[0], ctx->r[1] };
  ak_128_rem( ctx->r2, tmp, ctx->p);

  ak_mpzn256 p4 = { ctx->p[0], ctx->p[1], 0, 0 };
  ak_mpzn256 ans;
  detail_128_gcd( p4, r4, ans, tmp );
  ak_128_sub( ctx->v, r4, ans );
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция складывает два вычета в форме Монтгомери по заданному 128-битному модулю. Результат
    сложения в форме Монтгомери помещается в переменную z. Указатель на z не может совпадать с
    указателями на слагаемые.

    @param z Указатель на вычет, в который помещается результат
    @param x Левый аргумент опреации сложения
    @param y Правый аргумент операции сложения
    @param ctx Контекст с данными, необходимыми для операций Монтгомери                            */
/* ----------------------------------------------------------------------------------------------- */
 inline void ak_128_montgomery_add( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y,
                                    const ak_montgomery_context_128 *ctx )
{
  ak_128_add_mod( z, x, y, ctx->p );
}

 inline static void detail_128_mul_lower_part_only( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y)
{
  ak_uint64 tmp, _;
  umul_ppmm( _, z[1], y[0], x[1] );
  umul_ppmm( tmp, z[0], y[0], x[0] );
  z[1] += tmp;
  umul_ppmm( _, tmp, y[1], x[0] );
  z[1] += tmp;
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция перемножает два вычета в форме Монтгомери по заданному 128-битному модулю. Результат
    умножения в форме Монтгомери помещается в переменную z. Указатель на z не может совпадать с
    указателями на множители.

    @param z Указатель на вычет, в который помещается результат
    @param x Левый аргумент опреации умножения
    @param y Правый аргумент операции умножения
    @param ctx Контекст с данными, необходимыми для операций Монтгомери                            */
/* ----------------------------------------------------------------------------------------------- */
 void ak_128_montgomery_mul( ak_uint64 *z, const ak_uint64 *x, const ak_uint64 *y,
                             const ak_montgomery_context_128 *ctx )
{
  ak_mpzn256 t;
  ak_128_mul( t, x, y );
  ak_mpzn128 s;
  detail_128_mul_lower_part_only( s, ctx->v, t );
  ak_mpzn256 sp;
  ak_128_mul( sp, s, ctx->p );

  s[1] = t[1] + sp[1];
  ak_uint64 sign;
  if ( s[1] < t[1] || ( s[1] == 0xffffffffffffffffllu && t[0] + sp[0] < t[0] ) )
  {
    sign = ak_128_add( s, t + 2, sp + 2 );
    if ( s[0] == 0xffffffffffffffffllu )
    {
      s[0] = 0;
      if ( s[1] == 0xffffffffffffffffllu )
      {
        s[1] = 0;
        sign = 1;
      }
      else
      {
        ++s[1];
      }
    }
    else
    {
      ++s[0];
    }
  }
  else
  {
    sign = ak_128_add( s, t + 2, sp + 2 );
  }

  if ( sign == 1 )
  {
    ak_128_sub( z, s, ctx->p );
    return;
  }
  if ( s[1] > ctx->p[1] )
  {
    z[1] = s[1] - ctx->p[1] - ( s[0] < ctx->p[0] );
    z[0] = s[0] - ctx->p[0];
    return;
  }
  if ( s[1] == ctx->p[1] && s[0] >= ctx->p[0] )
  {
    z[1] = 0;
    z[0] = s[0] - ctx->p[0];
    return;
  }

  z[1] = s[1];
  z[0] = s[0];
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция приводит вычет из прямого вида в форму Монтгомери. Результат приведения помещается в
    переменную z. Указатель на z не может совпадать с указателем на исходный вычет.

    @param z Указатель на вычет, в который помещается результат
    @param x Вычет, который требуется привести в форму Монтгомери
    @param ctx Контекст с данными, необходимыми для операций Монтгомери                            */
/* ----------------------------------------------------------------------------------------------- */
 inline void ak_128_to_montgomery( ak_uint64 *z, const ak_uint64 *x, const ak_montgomery_context_128 *ctx )
{
  ak_128_montgomery_mul( z, x, ctx->r2, ctx );
}

/* ----------------------------------------------------------------------------------------------- */
/*! Функция приводит вычет из формы Монтгомери к прямому виду. Результат приведения помещается в
    переменную z. Указатель на z не может совпадать с указателем на исходный вычет.

    @param z Указатель на вычет, в который помещается результат
    @param x Вычет, который требуется привести к прямому виду
    @param ctx Контекст с данными, необходимыми для операций Монтгомери                            */
/* ----------------------------------------------------------------------------------------------- */
 inline void ak_128_from_montgomery( ak_uint64 *z, const ak_uint64 *x, const ak_montgomery_context_128 *ctx )
{
  ak_mpzn128 one = ak_mpzn128_one;
  ak_128_montgomery_mul( z, x, one, ctx );
}
