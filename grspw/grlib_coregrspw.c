/********************************************************************************
 *                                                                              *
 *    ▀▄   ▄▀                                                                   *
 *   ▄█▀███▀█▄    Created on: Jul 5, 2015                                       *
 *  █▀███████▀█   Author: Stephane GONZALEZ                                     *
 *  █ █▀▀▀▀▀█ █   email: stephane.gonzalez@gmx.fr                               *
 *     ▀▀ ▀▀                                                                    *
 *                                                                              *
 * Copyright 2015 grlib_coregrspw.c                                             *
 * Released under the "CeCILL-C" license                                        *
 * http://www.cecill.info                                                       *
 ********************************************************************************/

#include <mqueue.h>
#include <pthread.h>

#include "grlib_grspw.h"
#include "grlib_comgrspw.h"
#include "grlib_mqgrspw.h"
#include "grlib_typesgrspw.h"
#include "grlib_reggrspw.h"
#include "grlib_loggrspw.h"
#include "grlib_utilgrspw.h"

pthread_mutex_t txthread_mutex;
pthread_cond_t txthread_wait_cond;
pthread_cond_t txthread_wait_txack;



void * grlib_coregrspw_rxthread(void *arg);
void * grlib_coregrspw_txthread(void *arg);

void grlib_coregrspw_init(t_grspw_core core[e_grspw_corenb])
{
  /* thread id */
  pthread_t rxthread = 0;
  pthread_t txthread = 0;

  grlib_mqgrspw_init();

  /* Initialize mutex and condition variable objects */
  pthread_mutex_init(&txthread_mutex, NULL);
  pthread_cond_init(&txthread_wait_cond, NULL);
  pthread_cond_init(&txthread_wait_txack, NULL);

  // create rx thread
  CHECK_FATAL(0 != pthread_create( &rxthread, NULL, grlib_coregrspw_rxthread, core ), "create rx thread fail");
  // create tx thread
 // CHECK_FATAL(0 != pthread_create( &txthread, NULL, grlib_coregrspw_txthread, core ), "create tx thread fail");
}


uint8 g_rxdata[C_MAX_DATA_LEN];         ///< global rx data buffer
uint8 g_rxheader[C_MAX_HEADER_LEN];       ///< global rx header buffer

void * grlib_coregrspw_rxthread(void *arg)
{
  t_grspw_coreptr core[e_grspw_corenb] = { (t_grspw_coreptr) arg, (t_grspw_coreptr) arg + 1, (t_grspw_coreptr) arg + 2,
      (t_grspw_coreptr) arg + 3 };

  boolean must_stop = e_false;

  t_grspw_id grspw_id;                  ///< the spacewire id
  uint8 pword;                    ///< the protocol word value
  uint8 header_len;               ///< the header length
  uint32 data_len;                 ///< the data length;
  t_recv_att recv_att;
  t_grsw_deviceptr deviceptr;
  do
  {

    grlib_mqgrspw_receive_devpacket(
        &grspw_id,                ///< the spacewire id
        &pword,                    ///< the protocol word value
        g_rxheader,                   ///< the header content
        &header_len,              ///< the header length
        g_rxdata,                     ///< the data content
        &data_len);                ///< the data length;

    deviceptr = &core[grspw_id]->device;

    switch (pword)
    {
      case C_MSGQ_ACK:
        // upon device ack message, continue Tx thread
        pthread_cond_signal(&txthread_wait_txack);
        break;
      case C_MSGQ_DATA:
        // upon device data message, manage Rx incoming message
        grlib_comgrspw_rxenable(deviceptr, g_rxdata, data_len, &recv_att);
        break;
      case C_MSGQ_TIME:
        // upon device time message, up date the time code register
        grlib_comgrspw_rxtimecode(deviceptr, g_rxdata[0]);
        break;
    }
  }
  while (!must_stop);

  /* cleanup */
  grlib_mqgrspw_devdestroy();

  return NULL;
}

void * grlib_coregrspw_txthread(void *arg)
{
  t_grspw_coreptr core[e_grspw_corenb] = { (t_grspw_coreptr) arg, (t_grspw_coreptr) arg + 1, (t_grspw_coreptr) arg + 2,
      (t_grspw_coreptr) arg + 3 };

  t_grspw_id grspw_id;
  boolean stop = e_false;
  t_grsw_deviceptr deviceptr;
  do
  {
    pthread_mutex_lock(&txthread_mutex);
    pthread_cond_wait(&txthread_wait_cond, &txthread_mutex);
    for (grspw_id = e_grspw_core1; grspw_id < e_grspw_corenb; grspw_id++)
    {
      deviceptr = &core[grspw_id]->device;
      do
      {
        stop = grlib_comgrspw_txenable(deviceptr);
      }while(!stop);
      grlib_comgrspw_txenable(deviceptr);
    }
    pthread_mutex_unlock(&txthread_mutex);
  }
  while (e_true);

  return NULL;
}

void grlib_coregrspw_locktxsem()
{
  pthread_mutex_lock(&txthread_mutex);
}

void grlib_coregrspw_unlocktxsem()
{
  pthread_mutex_unlock(&txthread_mutex);
}

void grlib_coregrspw_txwakeup(const t_grspw_regptr grspw_regptr)
{
  // if the TX Enable Bit is set then wake up the TX thread and start the transmission
  if (checkbit(grspw_regptr->chn, C_SPWCHN_TE))
  {
    pthread_cond_signal(&txthread_wait_cond);
  }
}

void grlib_coregrspw_txwaitack(void)
{
  pthread_cond_wait(&txthread_wait_txack, &txthread_mutex);
}
