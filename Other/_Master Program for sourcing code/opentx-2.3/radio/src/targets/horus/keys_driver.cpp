/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"

uint32_t readKeys()
{
  uint32_t result = 0;

#if defined(PCBX12S)
  if (~KEYS_GPIO_REG_PGUP & KEYS_GPIO_PIN_PGUP)
    result |= 1 << KEY_PGUP;
#endif
  if (~KEYS_GPIO_REG_PGDN & KEYS_GPIO_PIN_PGDN)
    result |= 1 << KEY_PGDN;
  if (~KEYS_GPIO_REG_ENTER & KEYS_GPIO_PIN_ENTER)
    result |= 1 << KEY_ENTER;
  if (~KEYS_GPIO_REG_UP & KEYS_GPIO_PIN_UP)
    result |= 1 << KEY_MODEL;
  if (~KEYS_GPIO_REG_DOWN & KEYS_GPIO_PIN_DOWN)
    result |= 1 << KEY_EXIT;
  if (~KEYS_GPIO_REG_RIGHT & KEYS_GPIO_PIN_RIGHT)
    result |= 1 << KEY_TELEM;
  if (~KEYS_GPIO_REG_LEFT & KEYS_GPIO_PIN_LEFT)
    result |= 1 << KEY_RADIO;

  // TRACE("readKeys(): %x", result);

  return result;
}

uint32_t readTrims()
{
  uint32_t result = 0;

  if (~TRIMS_GPIO_REG_LHL & TRIMS_GPIO_PIN_LHL)
    result |= 0x01;
  if (~TRIMS_GPIO_REG_LHR & TRIMS_GPIO_PIN_LHR)
    result |= 0x02;
  if (~TRIMS_GPIO_REG_LVD & TRIMS_GPIO_PIN_LVD)
    result |= 0x04;
  if (~TRIMS_GPIO_REG_LVU & TRIMS_GPIO_PIN_LVU)
    result |= 0x08;
  if (~TRIMS_GPIO_REG_RVD & TRIMS_GPIO_PIN_RVD)
    result |= 0x10;
  if (~TRIMS_GPIO_REG_RVU & TRIMS_GPIO_PIN_RVU)
    result |= 0x20;
  if (~TRIMS_GPIO_REG_RHL & TRIMS_GPIO_PIN_RHL)
    result |= 0x40;
  if (~TRIMS_GPIO_REG_RHR & TRIMS_GPIO_PIN_RHR)
    result |= 0x80;
  if (~TRIMS_GPIO_REG_LSD & TRIMS_GPIO_PIN_LSD)
    result |= 0x100;
  if (~TRIMS_GPIO_REG_LSU & TRIMS_GPIO_PIN_LSU)
    result |= 0x200;
  if (~TRIMS_GPIO_REG_RSD & TRIMS_GPIO_PIN_RSD)
    result |= 0x400;
  if (~TRIMS_GPIO_REG_RSU & TRIMS_GPIO_PIN_RSU)
    result |= 0x800;
  // TRACE("readTrims(): result=0x%04x", result);

  return result;
}

bool trimDown(uint8_t idx)
{
  return readTrims() & ((uint32_t)1 << idx);
}

bool keyDown()
{
  return readKeys() || readTrims();
}

/* TODO common to ARM */
void readKeysAndTrims()
{
  uint32_t i;

  uint8_t index = 0;
  uint32_t keys_input = readKeys();
  for (i = 0; i < TRM_BASE; i++) {
    keys[index++].input(keys_input & (1 << i));
  }

  uint32_t trims_input = readTrims();
  for (i = 1; i <= 1 << (TRM_LAST-TRM_BASE); i <<= 1) {
    keys[index++].input(trims_input & i);
  }

  if ((keys_input || trims_input) && (g_eeGeneral.backlightMode & e_backlight_mode_keys)) {
    // on keypress turn the light on
    backlightOn();
  }
}

#define ADD_2POS_CASE(x) \
  case SW_S ## x ## 0: \
    xxx = SWITCHES_GPIO_REG_ ## x  & SWITCHES_GPIO_PIN_ ## x ; \
    break; \
  case SW_S ## x ## 2: \
    xxx = ~SWITCHES_GPIO_REG_ ## x  & SWITCHES_GPIO_PIN_ ## x ; \
    break
#define ADD_INV_2POS_CASE(x) \
  case SW_S ## x ## 2: \
    xxx = SWITCHES_GPIO_REG_ ## x  & SWITCHES_GPIO_PIN_ ## x ; \
    break; \
  case SW_S ## x ## 0: \
    xxx = ~SWITCHES_GPIO_REG_ ## x  & SWITCHES_GPIO_PIN_ ## x ; \
    break
#define ADD_3POS_CASE(x, i) \
  case SW_S ## x ## 0: \
    xxx = (SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H); \
    if (IS_3POS(i)) { \
      xxx = xxx && (~SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
    } \
    break; \
  case SW_S ## x ## 1: \
    xxx = (SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H) && (SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
    break; \
  case SW_S ## x ## 2: \
    xxx = (~SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H); \
    if (IS_3POS(i)) { \
      xxx = xxx && (SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
    } \
    break
#define ADD_INV_3POS_CASE(x, i) \
  case SW_S ## x ## 2: \
    xxx = (SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H); \
    if (IS_3POS(i)) { \
      xxx = xxx && (~SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
    } \
    break; \
  case SW_S ## x ## 1: \
    xxx = (SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H) && (SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
    break; \
  case SW_S ## x ## 0: \
    xxx = (~SWITCHES_GPIO_REG_ ## x ## _H & SWITCHES_GPIO_PIN_ ## x ## _H); \
    if (IS_3POS(i)) { \
      xxx = xxx && (SWITCHES_GPIO_REG_ ## x ## _L & SWITCHES_GPIO_PIN_ ## x ## _L); \
    } \
    break

#if !defined(BOOT)
uint32_t switchState(uint8_t index)
{
  uint32_t xxx = 0;

  switch (index) {
#if defined(PCBX12S)
    ADD_3POS_CASE(A, 0);
    ADD_3POS_CASE(B, 1);
    ADD_3POS_CASE(C, 2);
    ADD_3POS_CASE(D, 3);
    ADD_3POS_CASE(E, 4);
    ADD_INV_2POS_CASE(F);
    ADD_3POS_CASE(G, 6);
    ADD_2POS_CASE(H);
#else
    ADD_3POS_CASE(A, 0);
    ADD_INV_3POS_CASE(B, 1);
    ADD_3POS_CASE(C, 2);
    ADD_INV_3POS_CASE(D, 3);
    ADD_INV_3POS_CASE(E, 4);
    ADD_2POS_CASE(F);
    ADD_3POS_CASE(G, 6);
    ADD_2POS_CASE(H);
    ADD_2POS_CASE(GMBL);
    ADD_2POS_CASE(GMBR);
#endif
    default:
      break;
  }

  // TRACE("switch %d => %d", index, xxx);
  return xxx;
}
#endif

void keysInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;

  GPIO_InitStructure.GPIO_Pin = KEYS_GPIOB_PINS;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = KEYS_GPIOC_PINS;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = KEYS_GPIOD_PINS;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = KEYS_GPIOE_PINS;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = KEYS_GPIOG_PINS;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = KEYS_GPIOH_PINS;
  GPIO_Init(GPIOH, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = KEYS_GPIOI_PINS;
  GPIO_Init(GPIOI, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = KEYS_GPIOJ_PINS;
  GPIO_Init(GPIOJ, &GPIO_InitStructure);
}
