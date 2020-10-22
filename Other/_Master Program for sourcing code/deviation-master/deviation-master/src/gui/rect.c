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
#define ENABLE_GUIOBJECT
#include "gui.h"
#include "config/display.h"

guiObject_t *GUI_CreateRect(guiRect_t *rect, u16 x, u16 y, u16 width, u16 height, const struct LabelDesc *desc)
{
    struct guiObject *obj = (guiObject_t *)rect;
    struct guiBox    *box;
    CLEAR_OBJ(rect);

    box = &obj->box;
    box->x = x;
    box->y = y;
    box->width = width;
    box->height = height;

    obj->Type = Rect;
    OBJ_SET_TRANSPARENT(obj, 0);  //Deal with transparency during drawing
    OBJ_SET_SELECTABLE(obj, 0);
    connect_object(obj);

    rect->desc = *desc;

    return obj;
}

guiObject_t *GUI_CreateRectCB(guiRect_t *rect, u16 x, u16 y, u16 width, u16 height, const struct LabelDesc *desc, void (*Callback)(guiObject_t *, const void *), const void *data)
{
    guiObject_t *obj = GUI_CreateRect(rect, x, y, width, height, desc);
    rect->CallBack = Callback;
    rect->cbData = data;
    return obj;
}
    
void GUI_DrawRect(struct guiObject *obj)
{
    struct guiRect *rect = (struct guiRect *)obj;
    if (rect->desc.style == LABEL_TRANSPARENT) {
        GUI_DrawBackground(obj->box.x, obj->box.y, obj->box.width, obj->box.height);
    } else {
        LCD_FillRect(obj->box.x, obj->box.y, obj->box.width, obj->box.height, rect->desc.fill_color);
    }
    if (rect->desc.style == LABEL_TRANSPARENT || rect->desc.fill_color != rect->desc.outline_color) {
        LCD_DrawRect(obj->box.x, obj->box.y, obj->box.width, obj->box.height, rect->desc.outline_color);
    }
    if (rect->CallBack) {
        rect->CallBack(obj, rect->cbData);
    }
}
