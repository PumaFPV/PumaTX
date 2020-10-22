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

static struct mixer_page   * const mp  = &pagemem.u.mixer_page;
static struct stdthold_obj * const gui = &gui_objs.u.stdthold;

static const char *throhold_cb(guiObject_t *obj, int dir, void *data)
{
    (void)obj;
    (void)data;
    u8 changed = 1;
    u8 throhold_state = 0;
    if (Model.limits[mapped_std_channels.throttle].safetysw)
        throhold_state = 1; // here we set it either 1 or 0
    throhold_state = GUI_TextSelectHelper(throhold_state, 0, 1, dir, 1, 1, &changed);
    if (changed) {
        if (throhold_state == 1) {
            Model.limits[mapped_std_channels.throttle].safetysw = // bug fix: must use inverse position for hold switch
                    mapped_std_channels.switches[SWITCHFUNC_HOLD]
                         ? 0x80 | mapped_std_channels.switches[SWITCHFUNC_HOLD] // inverse of '0'
                         : 0;
            if (Model.limits[mapped_std_channels.throttle].safetyval == 0)
                Model.limits[mapped_std_channels.throttle].safetyval = -110;
        } else
            Model.limits[mapped_std_channels.throttle].safetysw = 0;
    }
    if (throhold_state == 1) {
        tempstring_cpy((const char *)_tr("On"));
        GUI_TextSelectEnable(&gui->value, 1);
    }
    else {
        tempstring_cpy((const char *)_tr("Off"));
        GUI_TextSelectEnable(&gui->value, 0);
    }
    return tempstring;
}

static const char *holdpostion_cb(guiObject_t *obj, int dir, void *data)
{
    (void)obj;
    (void)data;
    if (!GUI_IsTextSelectEnabled((guiObject_t *)&gui->value) || !Model.limits[mapped_std_channels.throttle].safetysw)
        return _tr("Off");
    Model.limits[mapped_std_channels.throttle].safetyval =
            GUI_TextSelectHelper(Model.limits[mapped_std_channels.throttle].safetyval,
                    -SAFETYVALUE_RANGE, SAFETYVALUE_RANGE, dir, 1, LONG_PRESS_STEP, NULL);
    sprintf(tempstring, "%d", Model.limits[mapped_std_channels.throttle].safetyval);
    return tempstring;
}
