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

#include "lcd_page_props.h"

#define TRIM_THICKNESS 10
#define TRIM_MARGIN 1
u32 _bargraph_get_color(s32 val, struct guiBarGraph *graph, struct disp_bargraph *disp)
{
    (void)val;
    (void)graph;
    (void)disp;
    return 1;
}
void _bargraph_trim_horizontal(int x, int y, int width, int height, s32 val, u32 color,
        struct guiBarGraph *graph, struct disp_bargraph *disp, struct guiBox *box)
{
    (void)color;
    (void)disp;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    
    for(int i=box->x + ITEM_SPACE; i < box->x + box->width - ITEM_SPACE; i+= ITEM_SPACE) 
        LCD_PrintCharXY(i, box->y, LCD_CENTER_DOT);
    LCD_PrintCharXY(box->x, box->y, LCD_LEFT_PTR);
    LCD_PrintCharXY(box->x + box->width - ITEM_SPACE, box->y, LCD_RIGHT_PTR);
    LCD_PrintCharXY(box->x + (box->width) / 2, box->y, LCD_HTRIM_CTR);
    
    s16 xpos = 0;
    s32 center = (graph->max + graph->min) / 2;
    s32 val_scale = ((box->width - ITEM_SPACE) / ITEM_SPACE * (val - graph->min) + (graph->max - graph->min) / 2) / (graph->max - graph->min) * ITEM_SPACE;
    xpos = graph->direction == TRIM_HORIZONTAL ? box->x + val_scale : box->x + box->width - ITEM_SPACE - val_scale;
    unsigned c;
    if (val == center) {
        c = LCD_UP_ARROW;
    } else if (val > center) {
        c = LCD_HTRIM_LT;
    } else {
        c = LCD_HTRIM_GT;
    }
    LCD_PrintCharXY(xpos, box->y , c);
}

void _bargraph_trim_vertical(int x, int y, int width, int height, s32 val, u32 color,
        struct guiBarGraph *graph, struct disp_bargraph *disp, struct guiBox *box)
{
    (void)color;
    (void)disp;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    
    for(int i=box->y + LINE_HEIGHT; i < box->y + box->height - LINE_HEIGHT; i+= LINE_HEIGHT) 
        LCD_PrintCharXY( box->x, i, LCD_CENTER_DOT);
    LCD_PrintCharXY(box->x, box->y + (box->height ) / 2,  LCD_VTRIM_CTR);
    LCD_PrintCharXY(box->x, box->y, LCD_UP_ARROW);
    LCD_PrintCharXY(box->x, box->y + box->height - LINE_HEIGHT, LCD_DOWN_ARROW);
	
    s16 ypos = 0;
    s32 center = (graph->max + graph->min) / 2;
    s32 val_scale = ((box->height - LINE_HEIGHT) / LINE_HEIGHT * (val - graph->min) + (graph->max - graph->min) / 2) / (graph->max - graph->min) * LINE_HEIGHT;
    ypos = box->y + box->height - LINE_HEIGHT - val_scale;
    unsigned c;
    if (val == center) {
        c = LCD_RIGHT_PTR;
    } else if (val > center) {
        c = LCD_HTRIM_GT;
    } else {
        c = LCD_HTRIM_LT;
    }
    LCD_PrintCharXY(box->x, ypos, c);
}
