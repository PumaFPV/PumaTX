/*
 This project is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Deviation is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Deviation.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common.h"
#include "pages.h"
#include "gui/gui.h"

#define OVERRIDE_PLACEMENT
enum {
    LABEL_X      = 0,
    LABEL_Y      = 3*LINE_HEIGHT,
    LABEL_WIDTH  = LCD_WIDTH,
    LABEL_HEIGHT = LCD_HEIGHT-3*LINE_HEIGHT
};

#include "../128x64x1/usb_page.c"

