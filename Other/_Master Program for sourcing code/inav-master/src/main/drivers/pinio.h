/*
 * This file is part of Cleanflight, Betaflight and INAV
 *
 * Cleanflight, Betaflight and INAV are free software. You can 
 * redistribute this software and/or modify this software under 
 * the terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License, 
 * or (at your option) any later version.
 *
 * Cleanflight, Betaflight and INAV are distributed in the hope that 
 * they will be useful, but WITHOUT ANY WARRANTY; without even the 
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 * PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "drivers/io_types.h"

#define PINIO_COUNT             4
#define PINIO_FLAGS_INVERTED    0x80

typedef struct pinioHardware_s {
    ioTag_t     ioTag;
    ioConfig_t  ioMode;
    uint16_t    flags;
} pinioHardware_t;

extern const pinioHardware_t pinioHardware[];
extern const int pinioHardwareCount;

void pinioInit(void);
void pinioSet(int index, bool on);
