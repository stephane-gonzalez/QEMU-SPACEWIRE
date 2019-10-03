/********************************************************************************
 *                                                                              *
 *    ▀▄   ▄▀                                                                   *
 *   ▄█▀███▀█▄    Created on: May 14, 2016                                      *
 *  █▀███████▀█   Author: Stephane GONZALEZ                                     *
 *  █ █▀▀▀▀▀█ █   email: stephane.gonzalez@gmx.fr                               *
 *     ▀▀ ▀▀                                                                    *
 *                                                                              *
 * Copyright 2015 grlib_mqgrspw.c                                               *
 * Released under the "CeCILL-C" license                                        *
 * http://www.cecill.info                                                       *
 ********************************************************************************/

#ifndef GRLIB_MQGRSPW_H_
#define GRLIB_MQGRSPW_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mqueue.h>

  #include "grlib_typesgrspw.h"

#define C_SPWQ  "/spwq" ///< Define message queue name for spacewire transmission (spacewire -> device)
#define C_DEVQ  "/devq" ///< Define message queue name for device transmission (device -> spacewire)
#define C_MAXQ_SIZE    1024

// Protocol word
#define C_MSGQ_ACK     0x00        ///< Define the "acknowledge" protocol word (inform it is the message has been received)
#define C_MSGQ_DATA    0x01        ///< Define the "data" protocol word (inform this message part is a data)
#define C_MSGQ_TIME    0x02        ///< Define the "time code" protocol word (inform it's a time code message)


//
#define C_SPW_ID_IDX   0
#define C_PWORD_IDX    1
#define C_HEADER_IDX   2
#define C_DATA_SIZE    4

  /** Reception message attribute
   * @brief This structure contains all error flag which can be forced when a message is sent by the simulated device
   */
  typedef struct
  {
    boolean dc;         ///< inform the packet has been received with a crc error on the data
    boolean hc;         ///< inform the packet has been received with a crc error on the header
    boolean ep;         ///< inform the packet has been received with an error end of packet
  } t_recv_att, *t_recv_attptr;

  void grlib_mqgrspw_connect(void);
  void grlib_mqgrspw_init(void);
  void grlib_mqgrspw_devdestroy(void);
  void grlib_mqgrspw_spwdestroy(void);

  void grlib_mqgrspw_send_spwtimecode(
      const t_grspw_id grspw_id,
      const uint8 time_code);

  void grlib_mqgrspw_send_spwpacket(
      const t_grspw_id grspw_id,
      const uint8 pword,
      const uint8 header[],
      const uint32 header_len,
      const uint8 data[],
      const uint32 data_len);

  void grlib_mqgrspw_send_devpacket(
      const t_grspw_id grspw_id,
      const uint8 pword,
      const uint8 header[],
      const uint32 header_len,
      const uint8 data[],
      const uint32 data_len);

  void grlib_mqgrspw_receive_spwpacket(
      t_grspw_id * grspw_id,                ///< the spacewire id
      uint8 * pword,                    ///< the protocol word value
      uint8 header[],                   ///< the header content
      uint8 * header_len,              ///< the header length
      uint8 data[],                     ///< the data content
      uint32 * data_len);

  void grlib_mqgrspw_receive_devpacket(
      t_grspw_id * grspw_id,                ///< the spacewire id
      uint8 * pword,                    ///< the protocol word value
      uint8 header[],                   ///< the header content
      uint8 * header_len,              ///< the header length
      uint8 data[],                     ///< the data content
      uint32 * data_len);                ///< the data length

#ifdef __cplusplus
}
#endif

#endif /* GRLIB_MQGRSPW_H_ */
