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
#include "../pages.h"
#include "gui/gui.h"
#include "config/model.h"
#include "standard.h"
#if HAS_STANDARD_GUI
#include "../../common/standard/_subtrim_page.c"

static const struct page_defs subtrim_defs = {
    _tr_noop("Subtrim"),
    subtrim_cb,
    NULL,
};
void PAGE_SubtrimInit(int page)
{
    (void)page;
    STANDARD_Init(&subtrim_defs);
}
#endif //HAS_STANDARD_GUI
