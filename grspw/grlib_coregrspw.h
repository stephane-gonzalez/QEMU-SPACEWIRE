/********************************************************************************
 *                                                                              *
 *    ▀▄   ▄▀                                                                   *
 *   ▄█▀███▀█▄    Created on: May 21, 2016                                      *
 *  █▀███████▀█   Author: Stephane GONZALEZ                                     *
 *  █ █▀▀▀▀▀█ █   email: stephane.gonzalez@gmx.fr                               *
 *     ▀▀ ▀▀                                                                    *
 *                                                                              *
 * Copyright 2015 grlib_coregrspw.c                                             *
 * Released under the "CeCILL-C" license                                        *
 * http://www.cecill.info                                                       *
 ********************************************************************************/
#ifndef GRLIB_COREGRSPW_H_
#define GRLIB_COREGRSPW_H_

#include "grlib_typesgrspw.h"
#include "grlib_grspw.h"

void grlib_coregrspw_init(t_grspw_core core[e_grspw_corenb]);
void grlib_coregrspw_txwakeup(const t_grspw_regptr grspw_regptr);
void grlib_coregrspw_txwaitack(void);

void grlib_coregrspw_locktxsem(void);

void grlib_coregrspw_unlocktxsem(void);

#endif /* GRLIB_COREGRSPW_H_ */
