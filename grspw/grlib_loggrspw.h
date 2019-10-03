/********************************************************************************
 *                                                                              *
 *    ▀▄   ▄▀                                                                   *
 *   ▄█▀███▀█▄    Created on: Jul 5, 2016                                       *
 *  █▀███████▀█   Author: Stephane GONZALEZ                                     *
 *  █ █▀▀▀▀▀█ █   email: stephane.gonzalez@gmx.fr                               *
 *     ▀▀ ▀▀                                                                    *
 *                                                                              *
 * Copyright 2015 grlib_loggrspw.h                                              *
 * Released under the "CeCILL-C" license                                        *
 * http://www.cecill.info                                                       *
 ********************************************************************************/

#ifndef GRLIB_LOGGRSPW_H_
#define GRLIB_LOGGRSPW_H_




#define CHECK_FATAL(x, msg) \
    do { \
        if (x) { \
            fprintf(stderr, "fatal failure %s:%d: %s", __func__, __LINE__,msg); \
            perror(#x); \
            exit(-1); \
        } \
    } while (0) \

#define CHECK_WARNING(x, msg) \
    do { \
        if (x) { \
            fprintf(stderr, "warning %s:%d: %s", __func__, __LINE__,msg); \
            perror(#x); \
        } \
    } while (0) \

#define DEBUG 1
#ifdef DEBUG
#define TRACELOGDBG(msg)   printf("trace debug # %s - %s : %d - %s\n", __FILE__, __FUNCTION__, __LINE__, msg);

#include "grlib_typesgrspw.h"
#include "grlib_reggrspw.h"

void grlib_loggrspw_init(void);

void grlib_loggrspw_trace_readreg(t_grspw_id grspw_id, t_grspw_reg_offset offset, uint32 data);

void grlib_loggrspw_trace_writereg(t_grspw_id grspw_id, t_grspw_reg_offset offset, uint32 data, uint32 result);


#define TRACELOG_OPEN() grlib_loggrspw_init();
#define TRACELOG_READREG(grspw_id, offset, data) grlib_loggrspw_trace_readreg(grspw_id, offset, data);
#define TRACELOG_WRITEREG(grspw_id, offset, data, result) grlib_loggrspw_trace_writereg(grspw_id, offset, data, result);


#else
#define TRACELOGDBG()
#define TRACELOG_OPEN(grspw_id, file)
#define TRACELOG_READREG(grspw_id, offset, data)
#define TRACELOG_WRITEREG(grspw_id, offset, data, result)
#endif

#endif /* GRLIB_LOGGRSPW_H_ */
