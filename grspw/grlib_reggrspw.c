/********************************************************************************
 *                                                                              *
 *    ▀▄   ▄▀                                                                   *
 *   ▄█▀███▀█▄    Created on: Jul 5, 2015                                       *
 *  █▀███████▀█   Author: Stephane GONZALEZ                                     *
 *  █ █▀▀▀▀▀█ █   email: stephane.gonzalez@gmx.fr                               *
 *     ▀▀ ▀▀                                                                    *
 *                                                                              *
 * Copyright 2015 grlib_reggrspw.c                                              *
 * Released under the "CeCILL-C" license                                        *
 * http://www.cecill.info                                                       *
 ********************************************************************************/

#include "grlib_reggrspw.h"
#include "grlib_mqgrspw.h"

#include "grlib_utilgrspw.h"
#include "grlib_loggrspw.h"

uint32 grlib_reggrspw_write_ctr(const t_grspw_id grspw_id, t_grspw_regptr grspw_regptr, const uint32 value);
uint32 grlib_reggrspw_write_str(const t_grspw_id grspw_id, t_grspw_regptr grspw_regptr, const uint32 value);
uint32 grlib_reggrspw_write_chn(const t_grspw_id grspw_id, t_grspw_regptr grspw_regptr, const uint32 value);

/*
 * \brief initialize the selected grspw core
 * \param [in]  grspw_id        Spacewire id link selection
 * \param [out] grspw_regptr   Spacewire registers mapping
 * \return None
 * \detail This function initialize the core spacewire by filling these registers with the default value
 */
void grlib_reggrspw_reset_registers(const t_grspw_id grspw_id, t_grspw_regptr grspw_regptr)
{
  // clear the Reset bit at the end of the operation
  // TODO : verify if upon reset the link start bit is set to 1 ... (it's strange) [see datasheet page 120]
  if (grspw_id < e_grspw_core3) // case of RMAP is unavailable
  {
    grspw_regptr->ctr = (grspw_regptr->ctr & C_SPWCTR_NOTRESET_MASK) | 0x00010002UL;
  }
  else // case of RMAP is available
  {
    grspw_regptr->ctr = (grspw_regptr->ctr & C_SPWCTR_NOTRESET_MASK) | 0xA0010002UL;
  }

  // RATIONAL : upon reset, the link state goes to reset state and after the reset it goes to ready state
  // the state is directly set to ready state because it's an fast operation.
  grspw_regptr->str = 0UL;
  grspw_regptr->ndr = 254UL;
  grspw_regptr->clk = 0x404UL;
  grspw_regptr->key = 0UL;
  grspw_regptr->tim = 0UL;
  grspw_regptr->tdr = 0x27140UL;
  grspw_regptr->chn = (grspw_regptr->chn & C_SPWCHN_NOTRESET_MASK) | 0UL;
  grspw_regptr->rxl = 0UL;
  grspw_regptr->rxd = 0UL;
  grspw_regptr->txd = 0UL;

  // clear the Reset bit at the end of the operation
  grspw_regptr->ctr = clearbit(grspw_regptr->ctr, C_SPWCTR_RS);
}

uint32 grlib_reggrspw_write_ctr(const t_grspw_id grspw_id, t_grspw_regptr grspw_regptr, const uint32 value)
{
  uint32 reg      = 0;
  uint32 clearset = 0;
  uint32 set      = 0;

  reg = grspw_regptr->ctr;
  clearset = deposit (reg, C_SPWCTR_CLEARSET_MASK | C_SPWCTR_NOTRESET_MASK, value);
  set = setbit(reg, ( value &  C_SPWCTR_SELFCLEAR_MASK));
  grspw_regptr->ctr = clearset | set ;


  if (checkbit(value, C_SPWCTR_RS))
  { // Reset [W] : 1: Make complete reset of the SpaceWire node. Self clearing
    grlib_reggrspw_reset_registers(grspw_id, grspw_regptr);
  }

  if (checkbit(value, C_SPWCTR_TI)) // Tick In [W] : set to 0 no effect
  {
    // increment the time counter and send the new time value
    uint32 tim      = grspw_regptr->tim;
    grspw_regptr->tim = deposit (tim, C_SPWTIM_TIME_COUNTER, tim + 1U);
    // transmit new time code value
    grlib_mqgrspw_send_spwtimecode(grspw_id, grspw_regptr->tim);
    // clear the Tick In bit at the end of the operation
    grspw_regptr->ctr = clearbit(grspw_regptr->ctr, C_SPWCTR_TI);
  }

  if (checkbit(value, C_SPWCTR_LS)) // Link Start
  {
    // set the link state status in running state
    uint32 str      = grspw_regptr->str;
    grspw_regptr->str = deposit (str, C_SPWSTR_LS, C_SPWSTR_LSRUN);
    // reset the link disable bit
    grspw_regptr->ctr = clearbit(grspw_regptr->ctr, C_SPWCTR_LD);
  }

  if (checkbit(value, C_SPWCTR_LD)) // Link Disable
  {
    // set the link state status in reset state
    uint32 str      = grspw_regptr->str;
    grspw_regptr->str = deposit (str, C_SPWSTR_LS, 0);
    // reset the link start bit
    grspw_regptr->ctr = clearbit(grspw_regptr->ctr, C_SPWCTR_LS);
  }

  return grspw_regptr->ctr;
}

uint32 grlib_reggrspw_write_str(const t_grspw_id grspw_id, t_grspw_regptr grspw_regptr, const uint32 value)
{
    // Clear bit
    if (checkbit(value, C_SPWSTR_EE)) //  clear Early EOP/EEP bit
    {
      // clear the Early EOP/EEP bit at the end of the operation
      grspw_regptr->str = clearbit(grspw_regptr->str, C_SPWSTR_EE);
    }

    if (checkbit(value, C_SPWSTR_IA)) //  clear Invalid Address bit
    {
      // clear the Invalid Address bit at the end of the operation
      grspw_regptr->str = clearbit(grspw_regptr->str, C_SPWSTR_IA);
    }

    if (checkbit(value, C_SPWSTR_WE)) //  clear Write Synchronization Error bit
    {
      // clear the Write Synchronization Error bit at the end of the operation
      grspw_regptr->str = clearbit(grspw_regptr->str, C_SPWSTR_WE);
    }

    if (checkbit(value, C_SPWSTR_PE)) //  clear Parity Error bit
    {
      // clear the Parity Error bit at the end of the operation
      grspw_regptr->str = clearbit(grspw_regptr->str, C_SPWSTR_PE);
    }

    if (checkbit(value, C_SPWSTR_DE)) //  clear Disconnect Error bit
    {
      // clear the Disconnect Error bit at the end of the operation
      grspw_regptr->str = clearbit(grspw_regptr->str, C_SPWSTR_DE);
    }

    if (checkbit(value, C_SPWSTR_ER)) //  clear Escape Error bit
    {
      // clear the Escape Error bit at the end of the operation
      grspw_regptr->str = clearbit(grspw_regptr->str, C_SPWSTR_ER);
    }

    if (checkbit(value, C_SPWSTR_CE)) //  clear Credit Error bit
    {
      // clear the Credit Error bit at the end of the operation
      grspw_regptr->str = clearbit(grspw_regptr->str, C_SPWSTR_CE);
    }

    if (checkbit(value, C_SPWSTR_TO)) //  clear Tick Out bit
    {
      // clear the Tick Out bit at the end of the operation
      grspw_regptr->str = clearbit(grspw_regptr->str, C_SPWSTR_TO);
    }

  return grspw_regptr->str;
}

uint32 grlib_reggrspw_write_chn(const t_grspw_id grspw_id, t_grspw_regptr grspw_regptr, const uint32 value)
{
  uint32 reg      = 0;
  uint32 clearset = 0;
  uint32 set      = 0;

  reg = grspw_regptr->chn;
  clearset = deposit (reg, C_SPWCHN_CLEARSET_MASK | C_SPWCHN_NOTRESET_MASK, value);
  set  = setbit (clearset, ( value & C_SPWCHN_SELFCLEAR_MASK ));
  grspw_regptr->chn = set;

  // Self clearing
 /* if (!checkbit(value, C_SPWCHN_RE)) //  check if the receiver is desactivated
  {
    // clear the no spill flag
    grspw_regptr->chn = clearbit(grspw_regptr->chn, C_SPWCHN_NS);

    // clear the RX descriptor ready
    grspw_regptr->chn = clearbit(grspw_regptr->chn, C_SPWCHN_RD);
  } xxx : workaround for rtems bsp ? is it true ?*/


  if (checkbit(value, C_SPWCHN_AT)) //  clear Abort TX bit
  {
    // disable the transmission
    grspw_regptr->chn = clearbit(grspw_regptr->chn, C_SPWCHN_TE);

    // clear the Abort TX bit at the end of the operation
    grspw_regptr->chn = clearbit(grspw_regptr->chn, C_SPWCHN_AT);
  }

  // Clear bit
  if (checkbit(value, C_SPWCHN_RA)) //  clear RX AHB Error bit
  {
    // clear the RX AHB Error bit at the end of the operation
    grspw_regptr->chn = clearbit(grspw_regptr->chn, C_SPWCHN_RA);
  }

  if (checkbit(value, C_SPWCHN_TA)) //  clear TX AHB Error bit
  {
    // clear the TX AHB Error bit at the end of the operation
    grspw_regptr->chn = clearbit(grspw_regptr->chn, C_SPWCHN_TA);
  }

  if (checkbit(value, C_SPWCHN_PR)) //  clear Packet received bit
  {
    // clear the Packet received bit at the end of the operation
    grspw_regptr->chn = clearbit(grspw_regptr->chn, C_SPWCHN_PR);
  }

  if (checkbit(value, C_SPWCHN_PS)) //  clear Packet sent bit
  {
    // clear the Packet sent bit at the end of the operation
    grspw_regptr->chn = clearbit(grspw_regptr->chn, C_SPWCHN_PS);
  }

  return grspw_regptr->chn;
}
/*
 * \brief write the selected grspw register
 * \param [in]     grspw_id         Spacewire id link selection
 * \param [in/out] grspw_regptr    Spacewire registers mapping
 * \param [in]     reg_offset         Spacewire register offset ( used in order to select the register )
 * \param [in]     value          The value to write
 * \return None
 * \detail This function allows to write the register selected by the offset value according to GRSPW register access specifications.
 * - Clear/Set bits : bits can be set or un-set without restriction
 * - Not reset bits : one times set these bits can only be reset by a GRSPW reset
 * - Clear bits     : bits are reset when a '1' is written
 */
void grlib_reggrspw_write_registers(const t_grspw_id grspw_id, t_grspw_regptr grspw_regptr, const t_grspw_reg_offset reg_offset, const uint32 value)
{

  uint32 result   = 0;

  switch (reg_offset)
  {
  case e_ctr: // SPW Control Register (SPWCTR) 0x0
    result = grlib_reggrspw_write_ctr(grspw_id, grspw_regptr, value);
    break;

  case e_str: // SPW Status Register (SPWSTR)  0x4
      result = grlib_reggrspw_write_str(grspw_id, grspw_regptr, value);
    break;

  case e_ndr: // SPW Node Address Register (SPWNDR)  0x8
    grspw_regptr->ndr = value & C_SPWNDR;
    result = grspw_regptr->ndr;
    break;

  case e_clk: // SPW Clock Divisor Register (SPWCLK) 0xC
    grspw_regptr->clk = value  & (C_SPWCLK_DIVISOR_LINK | C_SPWCLK_DIVISOR_RUN);
    result = grspw_regptr->clk;
    break;

  case e_key: // SPW Destination Key Register (SPWKEY) 0x10
    grspw_regptr->key = value & C_SPWKEY_KEY;
    result = grspw_regptr->key;
    break;

  case e_tim: // SPW Time Register (SPWTIM)  0x14
    grspw_regptr->tim = value & (C_SPWTIM_TIME_CTRL | C_SPWTIM_TIME_COUNTER);
    result = grspw_regptr->tim;
    break;

  case e_tdr: // SPW Timer and Disconnect Register (SPWTDR)  0x18
    grspw_regptr->tdr = value & (C_SPWTDR_DISCONNECT | C_SPWTDR_TIMER64);
    result = grspw_regptr->tdr;
    break;

  case e_chn: // SPW DMA Channel Control and Status Register (SPWCHN)  0x20
    result = grlib_reggrspw_write_chn(grspw_id, grspw_regptr, value);
    break;

  case e_rxl: // SPW DMA Channel Receiver Maximum Length Register (SPWRXL) 0x24
    grspw_regptr->rxl = value & C_SPWRXL_RX_MAX_LENGTH;
    result = grspw_regptr->rxl;
    break;

  case e_txd: // SPW Transmitter Descriptor Table Address Register (SPWTXD) 0x28
    grspw_regptr->txd = value & (C_SPWTXD_TX_BASE_ADDRESS | C_SPWTXD_TX_DESC_SELECT);
    result = grspw_regptr->txd;
    break;

  case e_rxd: // SPW Receive Descriptor Table Address Register (SPWRXD)  0x2C
    grspw_regptr->rxd = value & (C_SPWRXD_RX_BASE_ADDRESS | C_SPWRXD_RX_DESC_SELECT);
    result = grspw_regptr->rxd;
    break;

  default:
    break;
  }

  TRACELOG_WRITEREG( (grspw_id), reg_offset, value, result)
}

/*
 * \brief read the selected grspw register
 * \param [in]     grspw_id         Spacewire id link selection
 * \param [in/out] grspw_regptr    Spacewire registers mapping
 * \param [in]     reg_offset         Spacewire register offset ( used in order to select the register )
 * \return The register value
 * \detail This function allows to read the register selected by the offset value
 */
uint32 grlib_reggrspw_read_registers(const t_grspw_id grspw_id, t_grspw_regptr grspw_regptr, const t_grspw_reg_offset reg_offset)
{
  uint32 data = 0;

  switch (reg_offset)
  {
  case e_ctr: // SPW Control Register (SPWCTR) 0x0
    data = grspw_regptr->ctr;
    break;
  case e_str: // SPW Status Register (SPWSTR)  0x4
    data = grspw_regptr->str;
    break;
  case e_ndr: // SPW Node Address Register (SPWNDR)  0x8
    data = grspw_regptr->ndr;
    break;
  case e_clk: // SPW Clock Divisor Register (SPWCLK) 0xC
    data = grspw_regptr->clk;
    break;
  case e_key: // SPW Destination Key Register (SPWKEY) 0x10
    data = grspw_regptr->key;
    break;
  case e_tim: // SPW Time Register (SPWTIM)  0x14
    data = grspw_regptr->tim;
    break;
  case e_tdr: // SPW Timer and Disconnect Register (SPWTDR)  0x18
    data = grspw_regptr->tdr;
    break;
  case e_chn: // SPW DMA Channel Control and Status Register (SPWCHN)  0x20
    data = grspw_regptr->chn;
    break;
  case e_rxl: // SPW DMA Channel Receiver Maximum Length Register (SPWRXL) 0x24
    data = grspw_regptr->rxl;
    break;
  case e_txd: // SPW Transmitter Descriptor Table Address Register (SPWTXD) 0x28
    data = grspw_regptr->txd;
    break;
  case e_rxd: // SPW Receive Descriptor Table Address Register (SPWRXD)  0x2C
    data = grspw_regptr->rxd;
    break;
  default:

    break;
  }

  TRACELOG_READREG( (grspw_id), reg_offset, data)

  return data;
}
