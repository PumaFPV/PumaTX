/* @file max7456_symbols.h
 * @brief max7456 symbols for the mwosd font set
 *
 * @author Nathan Tsoi nathan@vertile.com
 *
 * Copyright (C) 2016 Nathan Tsoi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#pragma once

#ifdef USE_MAX7456

#define SYM_RSSI                  0x01 // 001 Icon RSSI
#define SYM_AH_RIGHT              0x02 // 002 Arrow left
#define SYM_AH_LEFT               0x03 // 003 Arrow right
#define SYM_THR                   0x04 // 004 Throttle
#define SYM_AH_DECORATION_UP      0x05 // 005 Arrow up AHI
#define SYM_VOLT                  0x06 // 006 V
#define SYM_MAH                   0x07 // 007 MAH

//                                0x08 // 008 -
//                                0x09 // 009 -
//                                0x0A // 010 -
//                                0x0B // 011 -
//                                0x0C // 012 -

#define SYM_TEMP_F                0x0D // 013 °F
#define SYM_TEMP_C                0x0E // 014 °C
#define SYM_FT                    0x0F // 015 FT

#define SYM_AH_DECORATION_MIN     0x10 // 016 to 021 Scrolling
#define SYM_AH_DECORATION         0x13 // 019 Scrolling
#define SYM_AH_DECORATION_MAX     0x15 // 021 Scrolling
#define SYM_AH_DECORATION_COUNT (SYM_AH_DECORATION_MAX - SYM_AH_DECORATION_MIN + 1) // Scrolling

#define SYM_WIND_HORIZONTAL       0x16 // 022 Air speed horizontal
#define SYM_WIND_VERTICAL         0x17 // 023 Air speed vertical

#define SYM_HEADING_N             0x18 // 024 Heading Graphic north
#define SYM_HEADING_S             0x19 // 025 Heading Graphic south
#define SYM_HEADING_E             0x1A // 026 Heading Graphic east
#define SYM_HEADING_W             0x1B // 027 Heading Graphic west
#define SYM_HEADING_DIVIDED_LINE  0x1C // 028 Heading Graphic 
#define SYM_HEADING_LINE          0x1D // 029 Heading Graphic 

#define SYM_SAT_L                 0x1E // 030 Sats left
#define SYM_SAT_R                 0x1F // 031 Sats right

#define SYM_BLANK                 0x20 // 032 blank (space)

//                                0x21 // 033 ASCII !

#define SYM_TRIP_DIST             0x22 // 034 Icon total distance

//                                0x23 // 035 ASCII #

#define SYM_AH_DECORATION_DOWN    0x24 // 036 Arrow down AHI

//                                0x25 // 037 ASCII %

#define SYM_AH_CH_LEFT            0x26 // 038 Crossair left
#define SYM_AH_CH_RIGHT           0x27 // 039 Crossair right

//                                0x28 // 040 to 062 ASCII

#define SYM_MILLIOHM              0x3F // 063 battery impedance Mohm

//                                0x40 // 064 to 095 ASCII

#define SYM_ARROW_UP              0x60 // 096 Direction arrow 0°
#define SYM_ARROW_2               0x61 // 097 Direction arrow 22.5°
#define SYM_ARROW_3               0x62 // 098 Direction arrow 45°
#define SYM_ARROW_4               0x63 // 099 Direction arrow 67.5°
#define SYM_ARROW_RIGHT           0x64 // 100 Direction arrow 90°
#define SYM_ARROW_6               0x65 // 101 Direction arrow 112.5°
#define SYM_ARROW_7               0x66 // 102 Direction arrow 135°
#define SYM_ARROW_8               0x67 // 103 Direction arrow 157.5°
#define SYM_ARROW_DOWN            0x68 // 104 Direction arrow 180°
#define SYM_ARROW_10              0x69 // 105 Direction arrow 202.5°
#define SYM_ARROW_11              0x6A // 106 Direction arrow 225°
#define SYM_ARROW_12              0x6B // 107 Direction arrow 247.5°
#define SYM_ARROW_LEFT            0x6C // 108 Direction arrow 270°
#define SYM_ARROW_14              0x6D // 109 Direction arrow 292.5°
#define SYM_ARROW_15              0x6E // 110 Direction arrow 315°
#define SYM_ARROW_16              0x6F // 111 Direction arrow 337.5°

#define SYM_ON_H                  0x70 // 112 ON HR
#define SYM_FLY_H                 0x71 // 113 FLY HR

#define SYM_DIRECTION             0x72 // 114 to 121, directional little arrows

#define SYM_HOME_NEAR             0x7A // 122 Home, near

//                                0x7B // 123 to 125 ASCII

#define SYM_AH_CH_CENTER          0x7E // 126 Crossair center

//                                0x7F // 127 -

#define SYM_AH_H_START            0x80 // 128 to 136 Horizontal AHI

#define SYM_3D_KMH                0x89 // 137 KM/H 3D
#define SYM_3D_MPH                0x8A // 138 MPH 3D

//                                0x8B // 139 -
//                                0x8C // 140 -
//                                0x8D // 141 -
//                                0x8E // 142 -
//                                0x8F // 143 -

#define SYM_BATT_FULL             0x90 // 144 Battery full
#define SYM_BATT_5                0x91 // 145 Battery
#define SYM_BATT_4                0x92 // 146 Battery
#define SYM_BATT_3                0x93 // 147 Battery 
#define SYM_BATT_2                0x94 // 148 Battery
#define SYM_BATT_1                0x95 // 149 Battery
#define SYM_BATT_EMPTY            0x96 // 150 Battery empty

#define SYM_AIR                   0x97 // 151 Air speed
//                                0x98 // 152 Home point map                                   
#define SYM_FTS                   0x99 // 153 FT/S
#define SYM_AMP                   0x9A // 154 A
#define SYM_ON_M                  0x9B // 155 On MN
#define SYM_FLY_M                 0x9C // 156 FL MN
#define SYM_MAH_KM_0              0x9D // 157 MAH/KM left
#define SYM_MAH_KM_1              0x9E // 158 MAH/KM right
#define SYM_MS                    0x9F // 159 M/S
#define SYM_HOME_DIST 	          0xA0 // 160 DIS
#define SYM_KMH                   0xA1 // 161 KM/H

#define SYM_VARIO_UP_2A           0xA2 // 162 Vario up up
#define SYM_VARIO_UP_1A           0xA3 // 163 Vario up
#define SYM_VARIO_DOWN_1A         0xA4 // 164 Vario down
#define SYM_VARIO_DOWN_2A         0xA5 // 165 Vario down down

#define SYM_LAT                   0xA6 // 166 GPS LAT
#define SYM_LON                   0xA7 // 167 GPS LON
#define SYM_DEGREES               0xA8 // 168 ° heading angle
#define SYM_HEADING               0xA9 // 169 Compass Heading symbol
#define SYM_ALT                   0xAA // 170 ALT
#define SYM_WH                    0xAB // 171 WH
#define SYM_WH_KM_0               0xAC // 172 WH/KM left
#define SYM_WH_KM_1               0xAD // 173 WH/KM right
#define SYM_WATT                  0xAE // 174 W
#define SYM_SCALE                 0xAF // 175 Map scale
#define SYM_MPH                   0xB0 // 176 MPH
#define SYM_ALT_M                 0xB1 // 177 ALT M
#define SYM_ALT_KM                0xB2 // 178 ALT KM
#define SYM_ALT_FT                0xB3 // 179 ALT FT
#define SYM_ALT_KFT               0xB4 // 180 DIS KFT
#define SYM_DIST_M                0xB5 // 181 DIS M
#define SYM_DIST_KM               0xB6 // 182 DIM KM
#define SYM_DIST_FT               0xB7 // 183 DIS FT
#define SYM_DIST_MI               0xB8 // 184 DIS MI
#define SYM_M                     0xB9 // 185 M
#define SYM_KM                    0xBA // 186 KM
#define SYM_MI                    0xBB // 187 MI

#define SYM_CLOCK                 0xBC // 188 Clock
#define SYM_HDP_L                 0xBD // 189 HDOP left
#define SYM_HDP_R                 0xBE // 190 HDOP right
#define SYM_HOME                  0xBF // 191 Home icon

#define SYM_ZERO_HALF_TRAILING_DOT 0xC0 // 192 to 201 Numbers with trailing dot

#define SYM_AUTO_THR0             0xCA // 202 Auto-throttle left
#define SYM_AUTO_THR1             0xCB // 203 Auto-throttle right

#define SYM_ROLL_LEFT             0xCC // 204 Sym roll left
#define SYM_ROLL_LEVEL            0xCD // 205 Sym roll horizontal
#define SYM_ROLL_RIGHT            0xCE // 206 Sym roll right
#define SYM_PITCH_UP              0xCF // 207 Pitch up

#define SYM_ZERO_HALF_LEADING_DOT 0xD0 // 208 to 217 Numbers with leading dot

#define SYM_AH_CH_AIRCRAFT0       0xDA // 218 Crossair aircraft left
#define SYM_AH_CH_AIRCRAFT1       0xDB // 219 Crossair aircraft 
#define SYM_AH_CH_AIRCRAFT2       0xDC // 220 Crossair aircraft center
#define SYM_AH_CH_AIRCRAFT3       0xDD // 221 Crossair aircraft 
#define SYM_AH_CH_AIRCRAFT4       0xDE // 222 Crossair aircraft right

#define SYM_PITCH_DOWN            0xDF // 223 Pitch down

#define SYM_AH_V_START            0xE0 // 224 to 229 Vertical AHI

#define SYM_BARO_TEMP             0xF0
#define SYM_IMU_TEMP              0xF1
#define SYM_TEMP                  0xF2

#define SYM_TEMP_SENSOR_FIRST     0xF2
#define SYM_TEMP_SENSOR_LAST      0xF7
#define TEMP_SENSOR_SYM_COUNT     (SYM_TEMP_SENSOR_LAST - SYM_TEMP_SENSOR_FIRST + 1)

#define SYM_LOGO_START            0x101 // 257 to 280, INAV logo
#define SYM_LOGO_WIDTH            6
#define SYM_LOGO_HEIGHT           4

#define SYM_CURSOR SYM_AH_LEFT // Menu cursor

#endif // USE_MAX7456
