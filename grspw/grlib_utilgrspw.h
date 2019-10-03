/********************************************************************************
 *                                                                              *
 *    ▀▄   ▄▀                                                                   *
 *   ▄█▀███▀█▄    Created on: Jul 5, 2015                                       *
 *  █▀███████▀█   Author: Stephane GONZALEZ                                     *
 *  █ █▀▀▀▀▀█ █   email: stephane.gonzalez@gmx.fr                               *
 *     ▀▀ ▀▀                                                                    *
 *                                                                              *
 * Copyright 2015 grlib_utilgrspw.h                                             *
 * Released under the "CeCILL-C" license                                        *
 * http://www.cecill.info                                                       *
 ********************************************************************************/

#ifndef GRLIB_UTILGRSPW_H_
#define GRLIB_UTILGRSPW_H_

#include <exec/hwaddr.h>

#include "grlib_typesgrspw.h"

void endianness_swap(uint8 dest[], const uint8 src[], const uint32 len);
void read_leon3_ram(const hwaddr addr, uint8 buff[], const uint32 len, const boolean sawp_endian);
void write_leon3_ram(const hwaddr addr, const uint8 buff[], const uint32 len, const boolean sawp_endian);

boolean checkbit(uint32 reg, uint32 mask);
uint32  setbit(uint32 reg, uint32 value);
uint32  clearbit(uint32 reg, uint32 value);
uint32  deposit(uint32 reg, uint32 value, uint32 mask);

#endif /* GRLIB_UTILGRSPW_H_ */
