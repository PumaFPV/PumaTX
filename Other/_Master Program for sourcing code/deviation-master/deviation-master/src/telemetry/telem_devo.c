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

s32 _devo_value(struct Telemetry *t, int idx)
{
    switch (idx) {
        case TELEM_DEVO_TEMP1:
        case TELEM_DEVO_TEMP2:
        case TELEM_DEVO_TEMP3:
        case TELEM_DEVO_TEMP4:
            return t->value[idx] == 0xff ? 0xffff // Not connected
            : (s16)t->value[idx];
        default:
            return t->value[idx];
    }
    return 0;
}

const char * _devo_str_by_value(char *str, u8 telem, s32 value)
{
    switch(telem) {
        case TELEM_DEVO_VOLT1:
        case TELEM_DEVO_VOLT2:
        case TELEM_DEVO_VOLT3: _get_value_str(str, value, 1, 'V'); break;
        case TELEM_DEVO_TEMP1:
        case TELEM_DEVO_TEMP2:
        case TELEM_DEVO_TEMP3:
        case TELEM_DEVO_TEMP4: _get_temp_str(str, value ? value - 20 : 0, 0, 'C'); break;
        case TELEM_DEVO_RPM1:
        case TELEM_DEVO_RPM2:  _get_value_str(str, value, 0, '\0'); break;
        default:
            return "";
    }
    return str;
}

const char * _devo_name(char *str, u8 telem)
{
    switch (telem) {
        case TELEM_DEVO_VOLT1:
        case TELEM_DEVO_VOLT2:
        case TELEM_DEVO_VOLT3:
            sprintf(str, "%s%d", _tr("TelemV"), telem - TELEM_DEVO_VOLT1 + 1);
            break;
        case TELEM_DEVO_TEMP1:
        case TELEM_DEVO_TEMP2:
        case TELEM_DEVO_TEMP3:
        case TELEM_DEVO_TEMP4:
            sprintf(str, "%s%d", _tr("TelemT"), telem - TELEM_DEVO_TEMP1 + 1);
            break;
        case TELEM_DEVO_RPM1:
        case TELEM_DEVO_RPM2:
            sprintf(str, "%s%d", _tr("TelemRPM"), telem - TELEM_DEVO_RPM1 + 1);
            break;
        default:
            return "";
    }
    return str;
}

const char * _devo_short_name(char *str, u8 telem)
{
    switch(telem) {
        case 0: strcpy(str, _tr("None")); break;
        case TELEM_DEVO_VOLT1:
        case TELEM_DEVO_VOLT2:
        case TELEM_DEVO_VOLT3: sprintf(str, "%s%d", _tr("Volt"), telem - TELEM_DEVO_VOLT1 + 1); break;
        case TELEM_DEVO_RPM1:
        case TELEM_DEVO_RPM2:  sprintf(str, "%s%d",  _tr("RPM"), telem - TELEM_DEVO_RPM1 + 1);  break;
        case TELEM_DEVO_TEMP1:
        case TELEM_DEVO_TEMP2:
        case TELEM_DEVO_TEMP3:
        case TELEM_DEVO_TEMP4: sprintf(str, "%s%d", _tr("Temp"), telem - TELEM_DEVO_TEMP1 + 1); break;
        default: str[0] = '\0'; break;
    }
    return str;
}

s32 _devo_get_max_value(u8 telem)
{
    switch(telem) {
        case TELEM_DEVO_VOLT1:
        case TELEM_DEVO_VOLT2:
        case TELEM_DEVO_VOLT3:  return 999; 
        case TELEM_DEVO_RPM1:
        case TELEM_DEVO_RPM2:   return 30600;
        case TELEM_DEVO_TEMP1:
        case TELEM_DEVO_TEMP2:
        case TELEM_DEVO_TEMP3:
        case TELEM_DEVO_TEMP4:  return 220;
        default:
            return 0;
    }
}

s32 _devo_get_min_value(u8 telem)
{
    switch(telem) {
        case TELEM_DEVO_VOLT1:
        case TELEM_DEVO_VOLT2:
        case TELEM_DEVO_VOLT3:  return 2;
        case TELEM_DEVO_RPM1:
        case TELEM_DEVO_RPM2:   return 120;
        case TELEM_DEVO_TEMP1:
        case TELEM_DEVO_TEMP2:
        case TELEM_DEVO_TEMP3:
        case TELEM_DEVO_TEMP4:  return -20;
        default:
            return 0;
    }
}
