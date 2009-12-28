/*
  Copyright (c) 2005 Eliot Dresselhaus

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef included_clib_vector_h
#define included_clib_vector_h

#include <clib/clib.h>

/* Vector types. */

#if defined (__MMX__) || defined (__IWMMXT__)
#define CLIB_HAVE_VEC64
#endif

#if defined (__SSE2__)
#define CLIB_HAVE_VEC128
#endif

#if defined (__ALTIVEC__)
#define CLIB_HAVE_VEC128
#endif

/* 128 implies 64 */
#ifdef CLIB_HAVE_VEC128
#define CLIB_HAVE_VEC64
#endif

#define _vector_size(n) __attribute__ ((vector_size (n)))

#ifdef CLIB_HAVE_VEC64
/* Signed 64 bit. */
typedef char i8x8 _vector_size (8);
typedef short i16x4 _vector_size (8);
typedef int i32x2 _vector_size (8);

/* Unsigned 64 bit. */
typedef unsigned char u8x8 _vector_size (8);
typedef unsigned short u16x4 _vector_size (8);
typedef unsigned int u32x2 _vector_size (8);

/* Floating point 64 bit. */
typedef float f32x2 _vector_size (8);
#endif /* CLIB_HAVE_VEC64 */

#ifdef CLIB_HAVE_VEC128
/* Signed 128 bit. */
typedef char i8x16 _vector_size (16);
typedef short i16x8 _vector_size (16);
typedef int i32x4 _vector_size (16);
typedef long long i64x2 _vector_size (16);

/* Unsigned 128 bit. */
typedef unsigned char u8x16 _vector_size (16);
typedef unsigned short u16x8 _vector_size (16);
typedef unsigned int u32x4 _vector_size (16);
typedef unsigned long long u64x2 _vector_size (16);

typedef float f32x4 _vector_size (16);
typedef double f64x2 _vector_size (16);
#endif /* CLIB_HAVE_VEC128 */

/* Vector word sized types. */
#ifndef CLIB_VECTOR_WORD_BITS
# ifdef CLIB_HAVE_VEC128
#  define CLIB_VECTOR_WORD_BITS 128
# else
#  define CLIB_VECTOR_WORD_BITS 64
# endif
#endif /* CLIB_VECTOR_WORD_BITS */

/* Vector word sized types. */
#if CLIB_VECTOR_WORD_BITS == 128
typedef  i8  i8x _vector_size (16);
typedef i16 i16x _vector_size (16);
typedef i32 i32x _vector_size (16);
typedef i64 i64x _vector_size (16);
typedef  u8  u8x _vector_size (16);
typedef u16 u16x _vector_size (16);
typedef u32 u32x _vector_size (16);
typedef u64 u64x _vector_size (16);
#else /* CLIB_HAVE_VEC64 */
typedef  i8  i8x _vector_size (8);
typedef i16 i16x _vector_size (8);
typedef i32 i32x _vector_size (8);
typedef i64 i64x _vector_size (8);
typedef  u8  u8x _vector_size (8);
typedef u16 u16x _vector_size (8);
typedef u32 u32x _vector_size (8);
typedef u64 u64x _vector_size (8);
#endif

#undef _vector_size

#define VECTOR_WORD_TYPE(t) t##x
#define VECTOR_WORD_TYPE_LEN(t) (sizeof (VECTOR_WORD_TYPE(t)) / sizeof (t))

#if defined (__SSE2__)
#include <clib/vector_sse2.h>
#endif

#if defined (__ALTIVEC__)
#include <clib/vector_altivec.h>
#endif

#if defined (__IWMMXT__)
#include <clib/vector_iwmmxt.h>
#endif

#if (defined(CLIB_HAVE_VEC128) || defined(CLIB_HAVE_VEC64))
#include <clib/vector_funcs.h>
#endif

#endif /* included_clib_vector_h */
