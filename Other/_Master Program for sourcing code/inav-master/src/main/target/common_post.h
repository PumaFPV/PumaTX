/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

// Touch up configuration

#pragma once

// Targets with built-in vtx do not need external vtx
#if defined(VTX) || defined(USE_RTC6705)
# undef USE_VTX_CONTROL
#endif

// Backward compatibility for I2C OLED display
#if !defined(USE_I2C)
# undef USE_DASHBOARD
# undef USE_OLED_UG2864
#endif

// Enable MSP_DISPLAYPORT for F3 targets without builtin OSD,
// since it's used to display CMS on MWOSD
#if !defined(USE_MSP_DISPLAYPORT) && (FLASH_SIZE > 128) && !defined(USE_OSD)
#define USE_MSP_DISPLAYPORT
#endif
