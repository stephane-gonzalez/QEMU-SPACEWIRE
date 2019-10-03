/********************************************************************************
 *                                                                              *
 *    ▀▄   ▄▀                                                                   *
 *   ▄█▀███▀█▄    Created on: May 18, 2016                                      *
 *  █▀███████▀█   Author: Stephane GONZALEZ                                     *
 *  █ █▀▀▀▀▀█ █   email: stephane.gonzalez@gmx.fr                               *
 *     ▀▀ ▀▀                                                                    *
 *                                                                              *
 * Copyright 2015 grlib_comgrspw.c                                              *
 * Released under the "CeCILL-C" license                                        *
 * http://www.cecill.info                                                       *
 ********************************************************************************/

#include "grlib_comgrspw.h"
#include "grlib_typesgrspw.h"
#include "grlib_utilgrspw.h"
#include "grlib_reggrspw.h"
#include "grlib_mqgrspw.h"
#include "grlib_coregrspw.h"
#include "grlib_loggrspw.h"


void grlib_comgrspw_txtimecode(t_grsw_deviceptr deviceptr);

uint8 g_txdata[C_MAX_DATA_LEN];
uint8 g_txheader[C_MAX_HEADER_LEN];

/*
 * \brief send spacewire message on the bus
 * \param [in]     grspw_id      The selected space link
 * \param [in/out] deviceptr     The grspw device
 * \param [in/out] irq           The grspw device irq
 * \return e_true if the transmission has to be stopped
 *         e_false if the transmission has to continue
 * \detail This function send spacewire message on the bus and manage registers and TX descriptor do to this
 */
boolean grlib_comgrspw_txenable(t_grsw_deviceptr deviceptr)
{
  t_grspw_regptr grspw_regptr = &deviceptr->grspw_reg;

  boolean stop = e_false;
  // [GRSPWSim-102] check if the transmission is enable
  if (checkbit(grspw_regptr->chn, C_SPWCHN_TE))
  {

    uint32 txd_cur = deviceptr->txd_cur;
    uint32 txd_addr = grspw_regptr->txd;
    t_grspw_tx_descrptr txd_mapptr = (t_grspw_tx_descrptr) &deviceptr->txd_map[txd_cur];

    // [GRSPWSim-103] read TX descriptor (base address tx descriptor + current descriptor)
    read_leon3_ram(txd_addr, (uint8ptr) txd_mapptr, sizeof(t_grspw_tx_descr), e_true); // copy with little endian format

    // [GRSPWSim-104] check if the Tx descriptor is enable
    if (checkbit(txd_mapptr->ctrl, C_SPWTDW0_EN))
    {

      uint8ptr txdmad_mapptr = (uint8ptr) &deviceptr->txdmad_map[txd_cur];
      uint32 data_len = txd_mapptr->len & C_SPWTDW2_DATA_LENGTH;
      txdmad_mapptr = g_txdata;

      uint8ptr txdmah_mapptr = (uint8ptr) &deviceptr->txdmah_map[txd_cur];
      uint32 header_len = txd_mapptr->ctrl & C_SPWTDW0_HEADER_LENGTH;
      txdmah_mapptr = g_txheader;

      // [GRSPWSim-105] read & copy TX data buffer
      read_leon3_ram(txd_mapptr->data, txdmad_mapptr, data_len, e_false); // straight copy without format

      // [GRSPWSim-106] read & copy TX header buffer
      read_leon3_ram(txd_mapptr->addr_header, txdmah_mapptr, header_len, e_false); // straight copy without format

#if 0
      uint8 node_addr = (uint8) grspw_regptr->ndr;
      uint8 protid = (uint8) checkbit(txd_mapptr->ctrl, C_SPWCTR_RE);
#endif

      // [GRSPWSim-107] send the message on the bus
      grlib_mqgrspw_send_spwpacket(deviceptr->grspw_id, C_MSGQ_DATA, txdmah_mapptr, header_len, txdmad_mapptr, data_len);

      // [GRSPWSim-107] inform a packet has been sent by setting the bit PS of the SPWCHN register
      grspw_regptr->chn = setbit(grspw_regptr->chn, C_SPWCHN_PS);

      // [GRSPWSim-110] check if the current tx descriptor force to wrap around
      if (checkbit(txd_mapptr->ctrl, C_SPWTDW0_WR))
      {
        deviceptr->txd_cur = 0;
      }
      else
      {
        // [GRSPWSim-111] increment and auto wrap the current tx descriptor
        deviceptr->txd_cur = (deviceptr->txd_cur + 1) % C_NB_TX_DESCRIPTOR;
      }

      // [GRSPWSim-xxx] update the TX descriptor table address register with the new index
      grspw_regptr->txd = deposit(grspw_regptr->txd, C_SPWTXD_TX_DESC_SELECT, (deviceptr->txd_cur << 4));

      // [GRSPWSim-xxx] wait message set ack
      grlib_coregrspw_txwaitack();

      // [GRSPWSim-xxx] write the TX descriptor in order to disable the used descriptor
      txd_mapptr->ctrl = clearbit(txd_mapptr->ctrl, C_SPWTDW0_EN);
      write_leon3_ram(txd_addr, (uint8ptr) &txd_mapptr->ctrl, sizeof(txd_mapptr->ctrl), e_true); // don't inverse the byte order

      // [GRSPWSim-109] check if the interrupt is requested and if the interrupt controller is enable
      if (checkbit(txd_mapptr->ctrl, C_SPWTDW0_IE) && checkbit(grspw_regptr->chn, C_SPWCHN_TI))
      {
        // raise the interrupt
        qemu_irq_pulse( deviceptr->irq);
      }

    }
    else
    { // [GRSPWSim-111] otherwise set the TE bit of the SPWCHN to ‘0’
      grspw_regptr->chn = clearbit(grspw_regptr->chn, C_SPWCHN_TE);

      stop = e_true;
    }
  }
  else
  {
    stop = e_true;
  }
  return stop;
}

/*
 * \brief transmit the receive data to the user space
 * \param [in]     grspw_id      The selected space link
 * \param [in/out] deviceptr     The grspw device
 * \param [in/out] irq           The grspw device irq
 * \param [in]     data          The received data
 * \param [in]     len           The length of the received data
 * \param [in]     attptr        The message attribute in order fake error
 * \return None
 * \detail This function transmits the receive data to the user space and manage registers and RX descriptor do to this
 */
void grlib_comgrspw_rxenable(t_grsw_deviceptr deviceptr, const uint8 data[], const uint32 len, const t_recv_attptr attptr)
{

  t_grspw_regptr grspw_regptr = &deviceptr->grspw_reg;

  // [GRSPWSim-002] check if the receiver is enable
  if (checkbit(grspw_regptr->chn, C_SPWCHN_RE))
  {
    uint8 node_addr = data[0];

    // [GRSPWSim-003] the packet is accepted if the receiver is configured in open packet mode or
    // [GRSPWSim-004] if the node address of the incoming matches with the configured node address
    if (checkbit(grspw_regptr->ctr, C_SPWCTR_OPM) || grspw_regptr->ndr == node_addr)
    {
      // [GRSPWSim-005] check if an RD descriptor is available
      if (checkbit(grspw_regptr->chn, C_SPWCHN_RD))
      {
        uint32 rxd_cur = deviceptr->rxd_cur;
        uint32 rxd_addr = grspw_regptr->rxd;
        t_grspw_rx_descptr rxd_mapptr = &deviceptr->rxd_map[rxd_cur];

        // [GRSPWSim-006] read RX descriptor (base address rx descriptor + current descriptor)
        read_leon3_ram(rxd_addr, (uint8*) rxd_mapptr, sizeof(t_grspw_rx_descptr), e_true); // copy with little endian format

        // [GRSPWSim-007] check if the Rx descriptor is enable
        if (checkbit(rxd_mapptr->ctrl, C_SPWTRW0_EN))
        {

          boolean trucated = e_false;
          uint32 length = len;
          // [GRSPWSim-008] check the data length of the incoming packet against the RX_MAX_LENGTH
          if (len > (grspw_regptr->rxl & C_SPWRXL_RX_MAX_LENGTH))
          {
            // [GRSPWSim-009] inform a truncated packet has been received
            trucated = e_true;
            CHECK_WARNING(e_true, "Incoming packet has been truncated");
            length = (grspw_regptr->rxl & C_SPWRXL_RX_MAX_LENGTH);
          }

          // [GRSPWSim-010] set message reception attribute in order to force error for simulation need
          rxd_mapptr->ctrl = setbit(rxd_mapptr->ctrl, (trucated * C_SPWTRW0_TR));
          rxd_mapptr->ctrl = setbit(rxd_mapptr->ctrl, (attptr->dc * C_SPWTRW0_DC));
          rxd_mapptr->ctrl = setbit(rxd_mapptr->ctrl, (attptr->hc * C_SPWTRW0_HC));
          rxd_mapptr->ctrl = setbit(rxd_mapptr->ctrl, (attptr->ep * C_SPWTRW0_EP));

          // [GRSPWSim-xxx] set the SPWSTS register EE bit if a message has been received with an EEP error status
          if(attptr->ep)
          {
            grspw_regptr->str = setbit(grspw_regptr->str, C_SPWSTR_EE);
          }

          // [GRSPWSim-011] copy the data according to according the incoming packet length or the truncated length
          write_leon3_ram(rxd_mapptr->addr, data, length, e_false); // don't inverse the byte order

          // [GRSPWSim-013] inform a packet has been receive by setting the bit PR of the SPWCHN register
          grspw_regptr->chn = setbit(grspw_regptr->chn, C_SPWCHN_PR);

          // [GRSPWSim-012] update the RX descriptor control word
          rxd_mapptr->ctrl = deposit(rxd_mapptr->ctrl, C_SPWTRW0_PACKET_LENGTH, length);

          // [GRSPWSim-xxx] write the RX descriptor in order to disable the used descriptor
          rxd_mapptr->ctrl = clearbit(rxd_mapptr->ctrl, C_SPWTRW0_EN);
          write_leon3_ram(rxd_addr, (uint8ptr) &rxd_mapptr->ctrl, sizeof(rxd_mapptr->ctrl), e_true); // inverse the byte order

          // [GRSPWSim-xxx] check if the current rx descriptor force to wrap around
          if (checkbit(rxd_mapptr->ctrl, C_SPWTRW0_WR))
          {
            deviceptr->rxd_cur = 0;
          }
          else
          {
            // [GRSPWSim-015] increment and auto wrap the current rx descriptor
            deviceptr->rxd_cur = (deviceptr->rxd_cur + 1) % C_NB_RX_DESCRIPTOR;
          }

          // update the RX_DESC_SELECT with the next rx descriptor selection
          grspw_regptr->rxd = deposit(grspw_regptr->rxd, C_SPWRXD_RX_DESC_SELECT, (deviceptr->rxd_cur << 3));

          // [GRSPWSim-014] check if the interrupt is requested and if the interrupt controller is enable
          if (checkbit(rxd_mapptr->ctrl, C_SPWTRW0_IE) && checkbit(grspw_regptr->chn, C_SPWCHN_TI))
          {
            // raise the interrupt
            qemu_irq_pulse( deviceptr->irq );
          }


        }
        else
        { // [GRSPWSim-016] otherwise set the RD bit of the SPWCHN to ‘0’

          grspw_regptr->chn = clearbit(grspw_regptr->chn, C_SPWCHN_RD);
          CHECK_WARNING(e_true,
              "One packet has been receive although the Rx descriptor was enable (SPWCHN : Bit RD is '1') \
                but the current RX descriptor is disable (EN bit of the RX Descriptor Word 0 is set to '0' )");

          // [GRSPWSim-017] check if no spill is disable
          if (e_false == checkbit(grspw_regptr->chn, C_SPWCHN_NS))
          {
            // discard the packet
            CHECK_WARNING(e_true, "Incoming packet has been discarded (SPWCHN : Bit NS is set to '0')");
          }
          else
          { // [GRSPWSim-018] otherwise put the packet in the pending queue
            // TODO : Add the packet in the pending queue
          }
        }
      }
      // [GRSPWSim-019] check if no spill is disable
      if (e_false == checkbit(grspw_regptr->chn, C_SPWCHN_NS))
      {
        // discard the packet
        CHECK_WARNING(e_true, "Incoming packet has been discarded (SPWCHN : Bit NS is set to '0')");
      }
      else
      { // [GRSPWSim-020] otherwise put the packet in the pending queue
        // TODO : Add the packet in the pending queue
      }
    }
    else
    {
      // [GRSPWSim-004] otherwise do nothing, the packet is discarded silently
    }
  }
  else
  {
    // [GRSPWSim-002] otherwise do nothing, all packets are discarded silently
  }

}

/*
 * \brief update the time code counter if needed
 * \param [in/out] deviceptr     The grspw device
 * \param [in]     data          The received data
 * \return None
 * \detail This function updates the time code counter register (SPWTIM) if needed
 */
void grlib_comgrspw_rxtimecode(t_grsw_deviceptr deviceptr, const uint8 timecode)
{
  t_grspw_regptr grspw_regptr = &deviceptr->grspw_reg;

  // [GRSPWSim-xxx] check if the received time code is greater than the current time code
  if (grspw_regptr->tim < (uint32)timecode)
  {
    // [GRSPWSim-xxx] update the time code with the received value
    grspw_regptr->tim = (uint32)timecode;
    // [GRSPWSim-xxx] raise the Tick Out bit in order to notice a new time code has been received
    grspw_regptr->str = setbit (grspw_regptr->str, C_SPWSTR_TO);

    // check if the Tick Out interrupt is enabled
    if(checkbit(grspw_regptr->ctr, C_SPWCTR_TQ))
    {
      // raise the interrupt
      qemu_irq_pulse( deviceptr->irq );
    }
  }
}

