/********************************************************************************
 *                                                                              *
 *    ▀▄   ▄▀                                                                   *
 *   ▄█▀███▀█▄    Created on: Jul 5, 2016                                       *
 *  █▀███████▀█   Author: Stephane GONZALEZ                                     *
 *  █ █▀▀▀▀▀█ █   email: stephane.gonzalez@gmx.fr                               *
 *     ▀▀ ▀▀                                                                    *
 *                                                                              *
 * Copyright 2015 grlib_loggrspw.c                                              *
 * Released under the "CeCILL-C" license                                        *
 * http://www.cecill.info                                                       *
 ********************************************************************************/



#include <stdio.h>
#include "grlib_typesgrspw.h"
#include "grlib_reggrspw.h"
#include "grlib_loggrspw.h"

void grlib_loggrspw_print_bitfield(t_grspw_id grspw_id, uint32 data);
void grlib_loggrspw_print_chnfield(t_grspw_id grspw_id, uint32 data);
void grlib_loggrspw_print_stsfield(t_grspw_id grspw_id, uint32 data);
void grlib_loggrspw_print_ctrfield(t_grspw_id grspw_id, uint32 data);
void grlib_loggrspw_print_bitheader(t_grspw_id grspw_id);

FILE *grspw_log[e_grspw_corenb];
/**
 * Convert register offset enumerate to string
 */
#define REGISTER_STR(offset)                         \
    (e_ctr == offset ? "SPWCTR 0x00" :                \
     (e_str == offset ? "SPWSTR 0x04" :                \
      (e_ndr == offset ? "SPWNDR 0x08" :                \
       (e_clk == offset ? "SPWCLK 0x0C" :                \
        (e_key == offset ? "SPWKEY 0x10" :                \
         (e_tim == offset ? "SPWTIM 0x14" :                \
          (e_tdr == offset ? "SPWTDR 0x18" :                \
           (e_chn == offset ? "SPWCHN 0x20" :                \
            (e_rxl == offset ? "SPWRXL 0x24" :                \
             (e_txd == offset ? "SPWTXD 0x28" :                \
              (e_rxd == offset ? "SPWRXD 0x2C" : "unknown")))))))))))

const char * fgrspw0 = "/home/loky/workspace/spw_test/log_grspw0.txt";
const char * fgrspw1 = "/home/loky/workspace/spw_test/log_grspw1.txt";
const char * fgrspw2 = "/home/loky/workspace/spw_test/log_grspw2.txt";
const char * fgrspw3 = "/home/loky/workspace/spw_test/log_grspw3.txt";

void grlib_loggrspw_init(void)
{
  grspw_log[e_grspw_core1] = fopen(fgrspw0, "w");
  grspw_log[e_grspw_core2] = fopen(fgrspw1, "w");
  grspw_log[e_grspw_core3] = fopen(fgrspw2, "w");
  grspw_log[e_grspw_core4] = fopen(fgrspw3, "w");
}

void grlib_loggrspw_trace_readreg(t_grspw_id grspw_id, t_grspw_reg_offset offset, uint32 data)
{
  FILE * file = grspw_log[grspw_id];
  fprintf(file, "READ  [ CORE %d ] REGISTER %s = %x\n",grspw_id, REGISTER_STR(offset), data); fflush(file);
  switch (offset)
  {
    case e_ctr:
      grlib_loggrspw_print_ctrfield(grspw_id, data);
    break;
    case e_str:
      grlib_loggrspw_print_stsfield(grspw_id, data);
    break;
    case e_chn:
      grlib_loggrspw_print_chnfield(grspw_id, data);
    break;
    default:
      break;
  }
}

void grlib_loggrspw_trace_writereg(t_grspw_id grspw_id, t_grspw_reg_offset offset, uint32 data, uint32 result)
{
  FILE * file = grspw_log[grspw_id];
  fprintf(file, "WRITE [ CORE %d ] REGISTER %s = %x -> %x\n",grspw_id, REGISTER_STR(offset), data, result); fflush(file);
  switch (offset)
  {
    case e_ctr:
      grlib_loggrspw_print_ctrfield(grspw_id, data);
      grlib_loggrspw_print_bitfield(grspw_id, result);
    break;
    case e_str:
      grlib_loggrspw_print_stsfield(grspw_id, data);
      grlib_loggrspw_print_bitfield(grspw_id, result);
    break;
    case e_chn:
      grlib_loggrspw_print_chnfield(grspw_id, data);
      grlib_loggrspw_print_bitfield(grspw_id, result);
    break;
    default:
      break;
  }
}

void grlib_loggrspw_print_bitheader(t_grspw_id grspw_id)
{
  FILE * file = grspw_log[grspw_id];
  fprintf(file, "31\t30\t29\t28\t27\t26\t25\t24\t23\t22\t21\t20\t19\t18\t17\t16\t15\t14\t13\t12\t11\t10\t9\t8\t7\t6\t5\t4\t3\t2\t1\t0\n"); fflush(file);
}

void grlib_loggrspw_print_ctrfield(t_grspw_id grspw_id, uint32 data)
{
  FILE * file = grspw_log[grspw_id];
  fprintf(file, "SPWCTR\n");
  grlib_loggrspw_print_bitheader(grspw_id);
  fprintf(file, "RA\tRX\tRC\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\tRD\tRE\t-\t-\t-\t-\tTR\tTT\tLI\tTQ\t-\tRS\tOPM\tTI\tIE\tAS\tLS\tLD\n"); fflush(file);
  grlib_loggrspw_print_bitfield(grspw_id, data);
}

void grlib_loggrspw_print_stsfield(t_grspw_id grspw_id, uint32 data)
{
  FILE * file = grspw_log[grspw_id];
  fprintf(file, "SPWSTS\n");
  grlib_loggrspw_print_bitheader(grspw_id);
  fprintf(file, "-\t-\t-\t-\t-\t-\t-\t-\tLS\t\t\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\tEE\tIA\tWE\t-\tPE\tDE\tER\tCE\tTO\n"); fflush(file);
  grlib_loggrspw_print_bitfield(grspw_id, data);
}

void grlib_loggrspw_print_chnfield(t_grspw_id grspw_id, uint32 data)
{
  FILE * file = grspw_log[grspw_id];
  fprintf(file, "SPWCHN\n");
  grlib_loggrspw_print_bitheader(grspw_id);
  fprintf(file, "-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\tLE\t-\t-\t-\tNS\tRD\tRX\tAT\tRA\tTA\tPR\tPS\tAI\tRI\tTI\tRE\tTE\n"); fflush(file);
  grlib_loggrspw_print_bitfield(grspw_id, data);
}

#define GETBIT(data, no) (data >> no & 1)

void grlib_loggrspw_print_bitfield(t_grspw_id grspw_id, uint32 data)
{
  FILE * file = grspw_log[grspw_id];

  fprintf(file, "%d\t%d\t%d\t%d\t\
      %d\t%d\t%d\t%d\t%d\t%d\t%d\t\
      %d\t%d\t%d\t%d\t%d\t%d\t%d\t\
      %d\t%d\t%d\t%d\t%d\t%d\t%d\t\
      %d\t%d\t%d\t%d\t%d\t%d\t%d\t\n",\
      GETBIT(data,31),GETBIT(data,30),GETBIT(data,29),GETBIT(data,28),\
      GETBIT(data,27),GETBIT(data,26),GETBIT(data,25),GETBIT(data,24),\
      GETBIT(data,23),GETBIT(data,22),GETBIT(data,21),GETBIT(data,20),\
      GETBIT(data,19),GETBIT(data,18),GETBIT(data,17),GETBIT(data,16),\
      GETBIT(data,15),GETBIT(data,14),GETBIT(data,13),GETBIT(data,12),\
      GETBIT(data,11),GETBIT(data,10),GETBIT(data,9),GETBIT(data,8),\
      GETBIT(data,7),GETBIT(data,6),GETBIT(data,5),GETBIT(data,4),\
      GETBIT(data,3),GETBIT(data,2),GETBIT(data,1),GETBIT(data,0));
  fflush(file);
}
