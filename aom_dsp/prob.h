/*
 * Copyright (c) 2016, Alliance for Open Media. All rights reserved
 *
 * This source code is subject to the terms of the BSD 2 Clause License and
 * the Alliance for Open Media Patent License 1.0. If the BSD 2 Clause License
 * was not distributed with this source code in the LICENSE file, you can
 * obtain it at www.aomedia.org/license/software. If the Alliance for Open
 * Media Patent License 1.0 was not distributed with this source code in the
 * PATENTS file, you can obtain it at www.aomedia.org/license/patent.
 */

#ifndef AOM_DSP_PROB_H_
#define AOM_DSP_PROB_H_

#include <assert.h>
#include <stdio.h>

#include "./aom_config.h"
#include "./aom_dsp_common.h"

#include "aom_ports/bitops.h"
#include "aom_ports/mem.h"

#include "aom_dsp/entcode.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t aom_prob;

// TODO(negge): Rename this aom_prob once we remove vpxbool.
typedef uint16_t aom_cdf_prob;

#define CDF_SIZE(x) ((x) + 1)

#define CDF_PROB_BITS 15
#define CDF_PROB_TOP (1 << CDF_PROB_BITS)

#define AOM_ICDF OD_ICDF

#define AOM_CDF2(a0) AOM_ICDF(a0), AOM_ICDF(CDF_PROB_TOP), 0
#define AOM_CDF3(a0, a1) AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(CDF_PROB_TOP), 0
#define AOM_CDF4(a0, a1, a2) \
  AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(CDF_PROB_TOP), 0
#define AOM_CDF5(a0, a1, a2, a3) \
  AOM_ICDF(a0)                   \
  , AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(CDF_PROB_TOP), 0
#define AOM_CDF6(a0, a1, a2, a3, a4)                        \
  AOM_ICDF(a0)                                              \
  , AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), \
      AOM_ICDF(CDF_PROB_TOP), 0
#define AOM_CDF7(a0, a1, a2, a3, a4, a5)                                  \
  AOM_ICDF(a0)                                                            \
  , AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), AOM_ICDF(a5), \
      AOM_ICDF(CDF_PROB_TOP), 0
#define AOM_CDF8(a0, a1, a2, a3, a4, a5, a6)                              \
  AOM_ICDF(a0)                                                            \
  , AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), AOM_ICDF(a5), \
      AOM_ICDF(a6), AOM_ICDF(CDF_PROB_TOP), 0
#define AOM_CDF9(a0, a1, a2, a3, a4, a5, a6, a7)                          \
  AOM_ICDF(a0)                                                            \
  , AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), AOM_ICDF(a5), \
      AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(CDF_PROB_TOP), 0
#define AOM_CDF10(a0, a1, a2, a3, a4, a5, a6, a7, a8)                     \
  AOM_ICDF(a0)                                                            \
  , AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), AOM_ICDF(a5), \
      AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(a8), AOM_ICDF(CDF_PROB_TOP), 0
#define AOM_CDF11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)                 \
  AOM_ICDF(a0)                                                            \
  , AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), AOM_ICDF(a5), \
      AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(a8), AOM_ICDF(a9),             \
      AOM_ICDF(CDF_PROB_TOP), 0
#define AOM_CDF12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)               \
  AOM_ICDF(a0)                                                               \
  , AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), AOM_ICDF(a5),    \
      AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(a8), AOM_ICDF(a9), AOM_ICDF(a10), \
      AOM_ICDF(CDF_PROB_TOP), 0
#define AOM_CDF13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)          \
  AOM_ICDF(a0)                                                               \
  , AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), AOM_ICDF(a5),    \
      AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(a8), AOM_ICDF(a9), AOM_ICDF(a10), \
      AOM_ICDF(a11), AOM_ICDF(CDF_PROB_TOP), 0
#define AOM_CDF14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)     \
  AOM_ICDF(a0)                                                               \
  , AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), AOM_ICDF(a5),    \
      AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(a8), AOM_ICDF(a9), AOM_ICDF(a10), \
      AOM_ICDF(a11), AOM_ICDF(a12), AOM_ICDF(CDF_PROB_TOP), 0
#define AOM_CDF15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) \
  AOM_ICDF(a0)                                                                \
  , AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), AOM_ICDF(a5),     \
      AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(a8), AOM_ICDF(a9), AOM_ICDF(a10),  \
      AOM_ICDF(a11), AOM_ICDF(a12), AOM_ICDF(a13), AOM_ICDF(CDF_PROB_TOP), 0
#define AOM_CDF16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, \
                  a14)                                                        \
  AOM_ICDF(a0)                                                                \
  , AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), AOM_ICDF(a5),     \
      AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(a8), AOM_ICDF(a9), AOM_ICDF(a10),  \
      AOM_ICDF(a11), AOM_ICDF(a12), AOM_ICDF(a13), AOM_ICDF(a14),             \
      AOM_ICDF(CDF_PROB_TOP), 0

#define MAX_PROB 255

#define BR_NODE 1

#define aom_prob_half ((aom_prob)128)

typedef int8_t aom_tree_index;

#define TREE_SIZE(leaf_count) (-2 + 2 * (leaf_count))

#define MODE_MV_COUNT_SAT 20

/* We build coding trees compactly in arrays.
   Each node of the tree is a pair of aom_tree_indices.
   Array index often references a corresponding probability table.
   Index <= 0 means done encoding/decoding and value = -Index,
   Index > 0 means need another bit, specification at index.
   Nonnegative indices are always even;  processing begins at node 0. */

typedef const aom_tree_index aom_tree[];

static INLINE aom_prob get_prob(unsigned int num, unsigned int den) {
  assert(den != 0);
  {
    const int p = (int)(((uint64_t)num * 256 + (den >> 1)) / den);
    // (p > 255) ? 255 : (p < 1) ? 1 : p;
    const int clipped_prob = p | ((255 - p) >> 23) | (p == 0);
    return (aom_prob)clipped_prob;
  }
}

static INLINE void update_cdf(aom_cdf_prob *cdf, int val, int nsymbs) {
  int rate;
  const int rate2 = 5;
  int i, tmp;
  int diff;

#if 1
#if 0//CONFIG_LV_MAP_MULTI
  // static const int nsymbs2speed[17] = { 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
  // 3, 3, 3, 3, 4 };
  // static const int nsymbs2speed[17] = { 0, 0, 1, 1, 2, 2, 2, 2, 2,
  //                                       2, 2, 2, 3, 3, 3, 3, 3 };
  static const int nsymbs2speed[17] = { 0, 0, 1, 1, 2, 2, 2, 2, 2,
                                        2, 2, 2, 2, 2, 2, 2, 2 };
  assert(nsymbs < 17);
  rate = 3 + (cdf[nsymbs] > 15) + (cdf[nsymbs] > 31) +
         nsymbs2speed[nsymbs];  // + get_msb(nsymbs);
  tmp = AOM_ICDF(0);
  (void)rate2;
  (void)diff;

  // Single loop (faster)
  for (i = 0; i < nsymbs - 1; ++i) {
    tmp = (i == val) ? 0 : tmp;
#if 1
    if (tmp < cdf[i]) {
      cdf[i] -= ((cdf[i] - tmp) >> rate);
    } else {
      cdf[i] += ((tmp - cdf[i]) >> rate);
    }
#else
    cdf[i] += ((tmp - cdf[i]) >> rate);
#endif
  }

#else
  rate = 4 + (cdf[nsymbs] > 31) + get_msb(nsymbs);
#if 0//CONFIG_LV_MAP
  if (nsymbs == 2)
    rate = 4 + (cdf[nsymbs] > 7) + (cdf[nsymbs] > 15) + get_msb(nsymbs);
#endif
  const int tmp0 = 1 << rate2;
  tmp = AOM_ICDF(tmp0);
  diff = ((CDF_PROB_TOP - (nsymbs << rate2)) >> rate) << rate;

  // Single loop (faster)
  for (i = 0; i < nsymbs - 1; ++i, tmp -= tmp0) {
    tmp -= (i == val ? diff : 0);
#if 0//CONFIG_LV_MAP_MULTI
    if (tmp < cdf[i]) {
      cdf[i] -= ((cdf[i] - tmp) >> rate);
    } else {
      cdf[i] += ((tmp - cdf[i]) >> rate);
    }
#else
    cdf[i] += ((tmp - cdf[i]) >> rate);
#endif
  }

#endif

#else
  for (i = 0; i < nsymbs; ++i) {
    tmp = (i + 1) << rate2;
    cdf[i] -= ((cdf[i] - tmp) >> rate);
  }
  diff = CDF_PROB_TOP - cdf[nsymbs - 1];

  for (i = val; i < nsymbs; ++i) {
    cdf[i] += diff;
  }
#endif
  cdf[nsymbs] += (cdf[nsymbs] < 32);
}

#if CONFIG_LV_MAP
static INLINE void update_bin(aom_cdf_prob *cdf, int val, int nsymbs) {
  update_cdf(cdf, val, nsymbs);
}
#endif

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // AOM_DSP_PROB_H_
