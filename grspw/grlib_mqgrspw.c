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

#include <string.h>
#include <stdlib.h>
#include <mqueue.h>
#include "grlib_mqgrspw.h"
#include "grlib_typesgrspw.h"
#include "grlib_loggrspw.h"

mqd_t spwq;
mqd_t devq;

#define OVERHEAD (7)
#define C_MAX_HEADER_LEN    (255)   ///< The maximum header length is 255 bytes
#define C_MAX_DATA_LEN      (16 *1024 *1024) ///< The maximum data length is 16MB

char txbuffer[C_MAX_DATA_LEN + C_MAX_HEADER_LEN + OVERHEAD];
char rxbuffer[C_MAX_DATA_LEN + C_MAX_HEADER_LEN + OVERHEAD];

void grlib_mqgrspw_send_packet(const mqd_t mq,
    const t_grspw_id grspw_id,
    const uint8 pword,
    const uint8 header[],
    const uint32 header_len,
    const uint8 data[],
    const uint32 data_len);

void grlib_mqgrspw_receive_packet(const mqd_t mq, ///< the mq reference id
    t_grspw_id * grspw_id,                ///< the spacewire id
    uint8 * pword,                    ///< the protocol word value
    uint8 header[],                   ///< the header content
    uint8 * header_len,              ///< the header length
    uint8 data[],                     ///< the data content
    uint32 * data_len);                ///< the data length

void grlib_mqgrspw_init(void)
{
  /* the queues attributes */
  struct mq_attr attr = { .mq_flags = 0, .mq_maxmsg = 10, .mq_msgsize = C_MAXQ_SIZE, .mq_curmsgs = 0 };

  /* create the message queue ( spacewire -> device) */
  spwq = mq_open(C_SPWQ, O_CREAT | O_WRONLY, 0644, &attr);
  CHECK_FATAL((mqd_t)-1 == spwq, "create spwq fail");

  /* create the message queue ( spacewire <- device) */
  devq = mq_open(C_DEVQ, O_CREAT | O_RDONLY, 0644, &attr);
  CHECK_FATAL((mqd_t)-1 == devq, "create devq fail");
}

void grlib_mqgrspw_connect(void)
{
  /* open the mail queue ( device -> spacewire) */
  devq = mq_open(C_DEVQ, O_WRONLY);
  CHECK_FATAL((mqd_t)-1 == devq, "open the mail queue ( device -> spacewire) fail");

  /* open the mail queue ( device <- spacewire) */
  spwq = mq_open(C_SPWQ, O_RDONLY);
  CHECK_FATAL((mqd_t)-1 == spwq, "open the mail queue ( device <- spacewire) fail");
}

void grlib_mqgrspw_devdestroy(void)
{
  CHECK_FATAL((mqd_t)-1 == mq_close(devq), "mq close devq fail");
  CHECK_FATAL((mqd_t)-1 == mq_unlink(C_DEVQ), "mq unlink fail devq");
}

void grlib_mqgrspw_spwdestroy(void)
{
  CHECK_FATAL((mqd_t)-1 == mq_close(spwq), "mq close spwq fail");
  CHECK_FATAL((mqd_t)-1 == mq_unlink(C_SPWQ), "mq unlink fail spwq");
}

void grlib_mqgrspw_send_spwpacket(
    const t_grspw_id grspw_id,
    const uint8 pword,
    const uint8 header[],
    const uint32 header_len,
    const uint8 data[],
    const uint32 data_len)
{
  grlib_mqgrspw_send_packet(spwq, grspw_id, pword, header, header_len, data, data_len);
}

void grlib_mqgrspw_send_spwtimecode(
    const t_grspw_id grspw_id,
    const uint8 time_code)
{
  grlib_mqgrspw_send_packet(spwq, grspw_id, C_MSGQ_TIME, NULL, 0, &time_code, 1);
}

void grlib_mqgrspw_send_devpacket(
    const t_grspw_id grspw_id,
    const uint8 pword,
    const uint8 header[],
    const uint32 header_len,
    const uint8 data[],
    const uint32 data_len)
{
  grlib_mqgrspw_send_packet(devq, grspw_id, pword, header, header_len, data, data_len);
}

void grlib_mqgrspw_send_packet(const mqd_t mq,
    const t_grspw_id grspw_id,
    const uint8 pword,
    const uint8 header[],
    const uint32 header_len,
    const uint8 data[],
    const uint32 data_len)
{

  uint32ptr dlen;
  uint32 length = OVERHEAD + header_len + data_len;

  txbuffer[C_SPW_ID_IDX] = (uint8) grspw_id;
  txbuffer[C_PWORD_IDX] = pword;
  txbuffer[C_HEADER_IDX] = header_len;
  if (header_len > 0)
    memcpy(&txbuffer[C_HEADER_IDX], header, header_len);
  // set the data size with an uint32 (4 bytes)
  dlen = (uint32ptr)&txbuffer[C_HEADER_IDX + header_len+1];
  *dlen = data_len;
  if (data_len > 0)
    memcpy(&txbuffer[C_HEADER_IDX + header_len + C_DATA_SIZE +1], data, data_len);

  CHECK_FATAL( 0 > mq_send(mq, txbuffer, length, 0), "packet transmission failure");

}

void grlib_mqgrspw_receive_spwpacket(
    t_grspw_id * grspw_id,                ///< the spacewire id
    uint8 * pword,                    ///< the protocol word value
    uint8 header[],                   ///< the header content
    uint8 * header_len,              ///< the header length
    uint8 data[],                     ///< the data content
    uint32 * data_len)                ///< the data length
{
  grlib_mqgrspw_receive_packet(spwq, grspw_id, pword, header, header_len, data, data_len) ;
}

void grlib_mqgrspw_receive_devpacket(
    t_grspw_id * grspw_id,                ///< the spacewire id
    uint8 * pword,                    ///< the protocol word value
    uint8 header[],                   ///< the header content
    uint8 * header_len,              ///< the header length
    uint8 data[],                     ///< the data content
    uint32 * data_len)                ///< the data length
{
  grlib_mqgrspw_receive_packet(devq, grspw_id, pword, header, header_len, data, data_len) ;
}

void grlib_mqgrspw_receive_packet(const mqd_t mq, ///< the mq reference id
    t_grspw_id * grspw_id,                ///< the spacewire id
    uint8 * pword,                    ///< the protocol word value
    uint8 header[],                   ///< the header content
    uint8 * header_len,              ///< the header length
    uint8 data[],                     ///< the data content
    uint32 * data_len)                ///< the data length
{

  int32 bytes_read;
  uint32ptr dlen;

  bytes_read = mq_receive(mq, rxbuffer, C_MAXQ_SIZE, NULL);
  CHECK_FATAL(bytes_read < 0, "packet reception failure");
  *grspw_id = rxbuffer[C_SPW_ID_IDX];
  *pword  = rxbuffer[C_PWORD_IDX];
  *header_len = (uint8)rxbuffer[C_HEADER_IDX];
  if (*header_len > 0)
    memcpy(header, &rxbuffer[C_HEADER_IDX], *header_len);
  // get the data size on an uint32 (4 bytes)
  dlen = (uint32ptr)&rxbuffer[C_HEADER_IDX + *header_len +1];
  *data_len = *dlen ;
  if (*data_len > 0)
    memcpy(data, &rxbuffer[C_HEADER_IDX + *header_len + C_DATA_SIZE +1], *data_len);

  CHECK_FATAL( *grspw_id >= e_grspw_corenb, "receive packet with a spacewire id invalid");

}

