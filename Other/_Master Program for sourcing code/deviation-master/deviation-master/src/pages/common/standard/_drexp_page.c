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

static struct stddrexp_obj * const gui = &gui_objs.u.stddrexp;
static struct mixer_page   * const mp  = &pagemem.u.mixer_page;

typedef enum {
    DREXP_AIL = 0,
    DREXP_ELE,
    DREXP_RUD,
} DREXPType;
static DREXPType drexp_type = DREXP_AIL;
static u8 current_pit_mode = 0;
static const int MAX_SCALAR = 125;

static void _refresh_page();
void update_graph(int graph);

static int get_mixers()
{
    int count;
    if (drexp_type == DREXP_AIL) {
        count = STDMIX_GetMixers(mp->mixer_ptr, mapped_std_channels.aile, 4);
    } else if (drexp_type == DREXP_ELE) {
        count = STDMIX_GetMixers(mp->mixer_ptr, mapped_std_channels.elev, 4);
    } else {
        count = STDMIX_GetMixers(mp->mixer_ptr, mapped_std_channels.rudd, 4);
    }
    return count;
}

static const char *set_type_cb(guiObject_t *obj, int dir, void *data)
{
    (void)obj;
    (void)data;
    u8 changed;
    drexp_type = GUI_TextSelectHelper(drexp_type, DREXP_AIL, DREXP_RUD, dir, 1, 1, &changed);
    if (changed) {
        get_mixers();
        PAGE_ShowHeader_SetLabel(STDMIX_TitleString, SET_TITLE_DATA(PAGEID_DREXP, SWITCHFUNC_DREXP_AIL+drexp_type));
        _refresh_page();
    }
    switch (drexp_type) {
    case DREXP_AIL:
        tempstring_cpy((const char *)_tr("AIL"));
        break;
    case DREXP_ELE:
        tempstring_cpy((const char *)_tr("ELE"));
        break;
    default:
        tempstring_cpy((const char *)_tr("RUD"));
        break;
    }
    return tempstring;
}

static const char *set_dr_cb(guiObject_t *obj, int dir, void *data)
{
    (void)obj;
    u8 pit_mode = (long)data;
    u8 changed = 1;
    mp->mixer_ptr[pit_mode]->scalar = GUI_TextSelectHelper(mp->mixer_ptr[pit_mode]->scalar,
            0, MAX_SCALAR, dir, 1, LONG_PRESS_STEP, &changed);
    if (changed || (GUI_GetSelected() == obj && current_pit_mode != pit_mode)) {
        current_pit_mode = pit_mode;
        update_graph(pit_mode);
    }
    sprintf(tempstring, "%d", mp->mixer_ptr[pit_mode]->scalar);
    strcat(tempstring, "%");
    return tempstring;
}

const char *drexplabel_cb(guiObject_t *obj, const void *data)
{
    (void)obj;
    u8 i = (long)data;
    snprintf(tempstring, sizeof(tempstring), _tr("Position %d"), i);
    return tempstring;
}

static const char *set_exp_cb(guiObject_t *obj, int dir, void *data)
{
    (void)obj;
    u8 pit_mode = (long)data;
    u8 changed = 1;
    struct Curve *curve = &(mp->mixer_ptr[pit_mode]->curve);
    curve->points[0] = GUI_TextSelectHelper(curve->points[0], -100, 100, dir, 1, LONG_PRESS_STEP, &changed);
    if (changed || (GUI_GetSelected() == obj && current_pit_mode != pit_mode))  {
        curve->points[1] = curve->points[0];
        current_pit_mode = pit_mode;
        update_graph(pit_mode);
    }
    if (curve->points[0] == 0)
        tempstring_cpy(_tr("LIN"));
    else {
        sprintf(tempstring, "%d", curve->points[0]);
        strcat(tempstring, "%");
    }
    return tempstring;
}

static u8 curpos_cb(s16 *x, s16 *y, u8 pos, void *data)
{
    (void)data;
    if (pos != 0)
        return 0;
    int idx = data ? ((long)data - 1) : current_pit_mode;
    *x = mp->raw[MIXER_SRC(mp->mixer_ptr[idx]->src)];
    if (*x > CHAN_MAX_VALUE)
        *x = CHAN_MAX_VALUE;
    else if (*x  < CHAN_MIN_VALUE)
        *x = CHAN_MIN_VALUE;
    s16 ymax = CHAN_MAX_VALUE/100 * MAX_SCALAR;
    s16 ymin = -ymax;
    *y = STDMIX_EvalMixerCb(*x, mp->mixer_ptr[idx], ymax, ymin);
    return 1;
}

static s32 show_curve_cb(s32 xval, void *data)
{
    (void)data;
    int idx = data ? ((long)data - 1) : current_pit_mode;
    s32 yval = CURVE_Evaluate(xval, &(mp->mixer_ptr[idx]->curve));
    yval = yval * mp->mixer_ptr[idx]->scalar / 100 ;
    return yval;
}
