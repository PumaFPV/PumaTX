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

#ifndef _FONTS_H_
#define _FONTS_H_

#if defined(COLORLCD)

#if !defined(BOOT)
#define FONT_TABLE_SIZE 16
#else
#define FONT_TABLE_SIZE 1
#endif

extern const uint16_t * const fontspecsTable[FONT_TABLE_SIZE];
extern const uint8_t * fontsTable[FONT_TABLE_SIZE];
extern BitmapBuffer *  fontCache[2];

void loadFontCache();
void loadFonts();

#else

extern const unsigned char font_5x7[];
extern const unsigned char font_10x14[];

#if defined(BOLD_FONT) &&  !defined(BOOT)
  #define BOLD_SPECIFIC_FONT
  extern const unsigned char font_5x7_B[];
#endif

extern const unsigned char font_3x5[];
extern const unsigned char font_4x6[];
extern const unsigned char font_8x10[];
extern const unsigned char font_22x38_num[];
extern const unsigned char font_5x7_extra[];
extern const unsigned char font_10x14_extra[];
extern const unsigned char font_4x6_extra[];

#endif

#endif // _FONTS_H_
