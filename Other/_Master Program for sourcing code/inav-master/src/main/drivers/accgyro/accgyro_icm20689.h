/*
 * This file is part of iNavFlight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "drivers/bus.h"

#define ICM20689_BIT_RESET                  (0x80)

#if (defined(USE_GYRO_ICM20689) || defined(USE_ACC_ICM20689))

bool icm20689AccDetect(accDev_t *acc);
bool icm20689GyroDetect(gyroDev_t *gyro);

#endif
