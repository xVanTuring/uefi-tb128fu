/** @file
  Volume-up key of the Lenovo TB128FU through the TLMM GPIO block.

  The board device tree's gpio-keys node puts volume up on TLMM GPIO 5, active low
  (`gpios = <&tlmm 5 1>`, linux,code 115). The west TLMM tile is at 0x00500000 and
  each GPIO occupies 0x1000 bytes; GPIO_IN is +0x4. The PM6125 power/volume-down
  keys stay on the SPMI v5 arbiter and are not read here.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef TB128FU_KEYS_H_
#define TB128FU_KEYS_H_

#include <stdint.h>

enum {
  SK_KEY_POWER = 1u << 0,     // volume up, mapped to Enter by the driver
  SK_KEY_VOLDOWN = 1u << 1,
  SK_KEY_VOLUP = 1u << 2
};

#define TB128FU_TLMM_GPIO5_IN 0x00405004UL

struct sk_arbiter {
  int unused;
};

struct sk_keys {
  int gpio;
};

static inline int sk_open (const struct sk_arbiter *a, struct sk_keys *k)
{
  (void) a;
  k->gpio = 1;
  return 0;
}

static inline int sk_sample (const struct sk_arbiter *a, const struct sk_keys *k, uint8_t *pressed)
{
  (void) a;
  (void) k;
  uint32_t value = *(volatile uint32_t *)(uintptr_t)TB128FU_TLMM_GPIO5_IN;
  // Active low: bit 0 clear while the key is held.
  *pressed = (value & 1u) ? 0 : SK_KEY_POWER;
  return 0;
}

// Debounce and auto-repeat, copied from the sheng key driver: a key counts after two
// identical samples; volume keys repeat while held.
struct sk_filter {
  uint8_t last, stable;
  uint16_t held[3];
};

static inline uint8_t sk_step (struct sk_filter *f, uint8_t sample, uint16_t delay, uint16_t period)
{
  uint8_t out = 0;
  if (sample == f->last) {
    uint8_t fresh = (uint8_t)(sample & ~f->stable);
    f->stable = sample;
    out = fresh;
    for (unsigned i = 0; i < 3; ++i) {
      uint8_t bit = (uint8_t)(1u << i);
      if (!(sample & bit)) { f->held[i] = 0; continue; }
      if (fresh & bit) { f->held[i] = 0; continue; }
      ++f->held[i];
      if (bit != SK_KEY_POWER && f->held[i] >= delay && (f->held[i] - delay) % period == 0) {
        out |= bit;
      }
    }
  }
  f->last = sample;
  return out;
}

#endif
