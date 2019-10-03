/********************************************************************************
 *                                                                              *
 *    ▀▄   ▄▀                                                                   *
 *   ▄█▀███▀█▄    Created on: Jun 16, 2015                                      *
 *  █▀███████▀█   Author: Stephane GONZALEZ                                     *
 *  █ █▀▀▀▀▀█ █   email: stephane.gonzalez@gmx.fr                               *
 *     ▀▀ ▀▀                                                                    *
 *                                                                              *
 * Copyright 2015 grlib_typesgrspw.h                                            *
 * Released under the "CeCILL-C" license                                        *
 * http://www.cecill.info                                                       *
 ********************************************************************************/

#ifndef GRLIB_TYPESGRSPW_H_
#define GRLIB_TYPESGRSPW_H_

typedef enum
{
  e_false = 0, e_true = 1
} boolean;   //< define boolean type

typedef unsigned int uint32;                  //< define unsigned integer 32 bit type
typedef unsigned int* uint32ptr;              //< define pointer on unsigned integer 32 bit type

typedef unsigned short uint16;                //< define unsigned integer 16 bit type
typedef unsigned short* uint16ptr;            //< define pointer on unsigned integer 16 bit type

typedef unsigned char uint8;                  //< define unsigned integer 8 bit type
typedef unsigned char* uint8ptr;              //< define pointer on unsigned integer 8 bit type

typedef signed int int32;                     //< define signed 32 bit type
typedef signed int* int32ptr;                 //< define pointer on signed 32 bit type

typedef signed short int16;                   //< define signed integer 16 bit type
typedef signed short* int16ptr;               //< define pointer on signed integer 16 bit type

typedef signed char int8;                     //< define signed 8 bit type
typedef signed char* int8ptr;                 //< define pointer on signed 8 bit type

typedef void* OPAQUEPTR;                      //< define opaque pointer

/**
 * Spacewire grspw core id definition
 * */
typedef enum
{
  e_grspw_core1 = 0,  ///< grspw core id 1
  e_grspw_core2 = 1,  ///< grspw core id 2
  e_grspw_core3 = 2,  ///< grspw core id 3
  e_grspw_core4 = 3,  ///< grspw core id 4
  e_grspw_corenb = 4   ///< number of grspw core
} t_grspw_id;

#endif /* GRLIB_TYPESGRSPW_H_ */
