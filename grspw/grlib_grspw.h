/********************************************************************************
 *                                                                              *
 *    ▀▄   ▄▀                                                                   *
 *   ▄█▀███▀█▄    Created on: Jul 5, 2015                                       *
 *  █▀███████▀█   Author: Stephane GONZALEZ                                     *
 *  █ █▀▀▀▀▀█ █   email: stephane.gonzalez@gmx.fr                               *
 *     ▀▀ ▀▀                                                                    *
 *                                                                              *
 * Copyright 2015 grlib_apbgrspw.h                                              *
 * Released under the "CeCILL-C" license                                        *
 * http://www.cecill.info                                                       *
 ********************************************************************************/

#ifndef GRLIB_APBGRSPW_H_
#define GRLIB_APBGRSPW_H_

#include "hw/sysbus.h"

#include "grlib_typesgrspw.h"
#include "grlib_reggrspw.h"
#include "grlib_comgrspw.h"


/**
 * Spacewire links core definition
 * */
typedef struct {
  MemoryRegion iomem;       ///< spacewire memory region mapping

  CharDriverState *chr;

  t_grsw_device device;

} t_grspw_core, *t_grspw_coreptr;


#endif /* GRLIB_APBGRSPW_H_ */
