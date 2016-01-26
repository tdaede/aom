/*Daala video codec
Copyright (c) 2001-2013 Daala project contributors.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/

#if !defined(_entcode_H)
# define _entcode_H (1)
# include <limits.h>
# include <stddef.h>
# include <stdint.h>

/*Enable special features for gcc and compatible compilers.*/
# if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#  define OD_GNUC_PREREQ(maj, min, pat)                             \
  ((__GNUC__ << 16) + (__GNUC_MINOR__ << 8) + __GNUC_PATCHLEVEL__ >= ((maj) << 16) + ((min) << 8) + pat)
# else
#  define OD_GNUC_PREREQ(maj, min, pat) (0)
# endif

#if OD_GNUC_PREREQ(3, 4, 0)
# define OD_WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))
#else
# define OD_WARN_UNUSED_RESULT
#endif

#if OD_GNUC_PREREQ(3, 4, 0)
# define OD_ARG_NONNULL(x) __attribute__((__nonnull__(x)))
#else
# define OD_ARG_NONNULL(x)
#endif

/*Modern gcc (4.x) can compile the naive versions of min and max with cmov if
   given an appropriate architecture, but the branchless bit-twiddling versions
   are just as fast, and do not require any special target architecture.
  Earlier gcc versions (3.x) compiled both code to the same assembly
   instructions, because of the way they represented ((b) > (a)) internally.*/
/*#define OD_MAXI(a, b) ((a) < (b) ? (b) : (a))*/
# define OD_MAXI(a, b) ((a) ^ (((a) ^ (b)) & -((b) > (a))))
/*#define OD_MINI(a, b) ((a) > (b) ? (b) : (a))*/
# define OD_MINI(a, b) ((a) ^ (((b) ^ (a)) & -((b) < (a))))

/*Count leading zeros.
  This macro should only be used for implementing od_ilog(), if it is defined.
  All other code should use OD_ILOG() instead.*/
# if defined(_MSC_VER)
#  include <intrin.h>
#  if !defined(snprintf)
#   define snprintf _snprintf
#  endif
/*In _DEBUG mode this is not an intrinsic by default.*/
#  pragma intrinsic(_BitScanReverse)

static __inline int od_bsr(unsigned long x) {
  unsigned long ret;
  _BitScanReverse(&ret, x);
  return (int)ret;
}
#  define OD_CLZ0 (1)
#  define OD_CLZ(x) (-od_bsr(x))
# elif defined(ENABLE_TI_DSPLIB)
#  include "dsplib.h"
#  define OD_CLZ0 (31)
#  define OD_CLZ(x) (_lnorm(x))
# elif OD_GNUC_PREREQ(3, 4, 0)
#  if INT_MAX >= 2147483647
#   define OD_CLZ0 ((int)sizeof(unsigned)*CHAR_BIT)
#   define OD_CLZ(x) (__builtin_clz(x))
#  elif LONG_MAX >= 2147483647L
#   define OD_CLZ0 ((int)sizeof(unsigned long)*CHAR_BIT)
#   define OD_CLZ(x) (__builtin_clzl(x))
#  endif
# endif
# if defined(OD_CLZ)
#  define OD_ILOG_NZ(x) (OD_CLZ0 - OD_CLZ(x))
/*Note that __builtin_clz is not defined when x == 0, according to the gcc
   documentation (and that of the x86 BSR instruction that implements it), so
   we have to special-case it.
  We define a special version of the macro to use when x can be zero.*/
#  define OD_ILOG(x) (OD_ILOG_NZ(x) & -!!(x))
# else
#  define OD_ILOG_NZ(x) (od_ilog(x))
#  define OD_ILOG(x) (od_ilog(x))
# endif

# if defined(OD_ENABLE_ASSERTIONS)
#  if OD_GNUC_PREREQ(2, 5, 0)
__attribute__((noreturn))
#  endif
void od_fatal_impl(const char *_str, const char *_file, int _line);

#  define OD_FATAL(_str) (od_fatal_impl(_str, __FILE__, __LINE__))

#  define OD_ASSERT(_cond) \
  do { \
    if (!(_cond)) { \
      OD_FATAL("assertion failed: " # _cond); \
    } \
  } \
  while (0)

#  define OD_ASSERT2(_cond, _message) \
  do { \
    if (!(_cond)) { \
      OD_FATAL("assertion failed: " # _cond "\n" _message); \
    } \
  } \
  while (0)

#  define OD_ALWAYS_TRUE(_cond) OD_ASSERT(_cond)

# else
#  define OD_ASSERT(_cond)
#  define OD_ASSERT2(_cond, _message)
#  define OD_ALWAYS_TRUE(_cond) ((void)(_cond))
# endif

/** Copy n elements of memory from src to dst. The 0* term provides
    compile-time type checking  */
#if !defined(OVERRIDE_OD_COPY)
# define OD_COPY(dst, src, n) \
  (memcpy((dst), (src), sizeof(*(dst))*(n) + 0*((dst) - (src))))
#endif

/** Copy n elements of memory from src to dst, allowing overlapping regions.
    The 0* term provides compile-time type checking */
#if !defined(OVERRIDE_OD_MOVE)
# define OD_MOVE(dst, src, n) \
 (memmove((dst), (src), sizeof(*(dst))*(n) + 0*((dst) - (src)) ))
#endif

/*Set this flag 1 to enable a "reduced overhead" version of the entropy coder.
  This uses a partition function that more accurately follows the input
   probability estimates at the expense of some additional CPU cost (though
   still an order of magnitude less than a full division).

  In classic arithmetic coding, the partition function maps a value x in the
   range [0, ft] to a value in y in [0, r] with 0 < ft <= r via
    y = x*r/ft.
  Any deviation from this value increases coding inefficiency.

  To avoid divisions, we require ft <= r < 2*ft (enforcing it by shifting up
   ft if necessary), and replace that function with
    y = x + OD_MINI(x, r - ft).
  This counts values of x smaller than r - ft double compared to values larger
   than r - ft, which over-estimates the probability of symbols at the start of
   the alphabet, and under-estimates the probability of symbols at the end of
   the alphabet.
  The overall coding inefficiency assuming accurate probability models and
   independent symbols is in the 1% range, which is similar to that of CABAC.

  To reduce overhead even further, we split this into two cases:
  1) r - ft > ft - (r - ft).
     That is, we have more values of x that are double-counted than
      single-counted.
     In this case, we still double-count the first 2*r - 3*ft values of x, but
      after that we alternate between single-counting and double-counting for
      the rest.
  2) r - ft < ft - (r - ft).
     That is, we have more values of x that are single-counted than
      double-counted.
     In this case, we alternate between single-counting and double-counting for
      the first 2*(r - ft) values of x, and single-count the rest.
  For two equiprobable symbols in different places in the alphabet, this
   reduces the maximum ratio of over-estimation to under-estimation from 2:1
   for the previous partition function to either 4:3 or 3:2 (for each of the
   two cases above, respectively), assuming symbol probabilities significantly
   greater than 1/32768.
  That reduces the worst-case per-symbol overhead from 1 bit to 0.58 bits.

  The resulting function is
    e = OD_MAXI(2*r - 3*ft, 0);
    y = x + OD_MINI(x, e) + OD_MINI(OD_MAXI(x - e, 0) >> 1, r - ft).
  Here, e is a value that is greater than 0 in case 1, and 0 in case 2.
  This function is about 3 times as expensive to evaluate as the high-overhead
   version, but still an order of magnitude cheaper than a division, since it
   is composed only of very simple operations.
  Because we want to fit in 16-bit registers and must use unsigned values to do
   so, we use saturating subtraction to enforce the maximums with 0.

  Enabling this reduces the measured overhead in ectest from 0.805% to 0.621%
   (vs. 0.022% for the division-based partition function with r much greater
   than ft).
  It improves performance on ntt-short-1 by about 0.3%.*/
# define OD_EC_REDUCED_OVERHEAD (1)

/*OPT: od_ec_window must be at least 32 bits, but if you have fast arithmetic
   on a larger type, you can speed up the decoder by using it here.*/
typedef uint32_t od_ec_window;

# define OD_EC_WINDOW_SIZE ((int)sizeof(od_ec_window)*CHAR_BIT)

/*Unsigned subtraction with unsigned saturation.
  This implementation of the macro is intentionally chosen to increase the
   number of common subexpressions in the reduced-overhead partition function.
  This matters for C code, but it would not for hardware with a saturating
   subtraction instruction.*/
#define OD_SUBSATU(a, b) ((a) - OD_MINI(a, b))

/*The number of bits to use for the range-coded part of unsigned integers.*/
# define OD_EC_UINT_BITS (4)

/*The resolution of fractional-precision bit usage measurements, i.e.,
   3 => 1/8th bits.*/
# define OD_BITRES (3)

extern const uint16_t OD_UNIFORM_CDFS_Q15[135];

/*Returns a Q15 CDF for a uniform probability distribution of the given size.
  n: The size of the distribution.
     This must be at least 2, and no more than 16.*/
# define OD_UNIFORM_CDF_Q15(n) \
   (OD_UNIFORM_CDFS_Q15 + ((n)*((n) - 1) >> 1) - 1)

/*See entcode.c for further documentation.*/

OD_WARN_UNUSED_RESULT uint32_t od_ec_tell_frac(uint32_t nbits_total,
 uint32_t rng);

#endif
