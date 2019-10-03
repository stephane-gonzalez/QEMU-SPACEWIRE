/********************************************************************************
 *                                                                              *
 *    ▀▄   ▄▀                                                                   *
 *   ▄█▀███▀█▄    Created on: Jul 5, 2015                                       *
 *  █▀███████▀█   Author: Stephane GONZALEZ                                     *
 *  █ █▀▀▀▀▀█ █   email: stephane.gonzalez@gmx.fr                               *
 *     ▀▀ ▀▀                                                                    *
 *                                                                              *
 * Copyright 2015 grlib_grspwreg.h                                              *
 * Released under the "CeCILL-C" license                                        *
 * http://www.cecill.info                                                       *
 ********************************************************************************/

#ifndef GRLIB_REGGRSPW_H_
#define GRLIB_REGGRSPW_H_

#include "grlib_typesgrspw.h"

#define ARCH32BIT        (32U)
#define FIELDMASK(len)   (~0U >> (ARCH32BIT-len))

/**
 * \brief registers mapped into APB memory space
 * \ref Registers
 * \defgroup SPW_BASE_ADDR Spacewire links base address
 * @{
 */
#define C_SPW0_BASE_ADDR          (0x80000A00U)  ///< Spacewire link0 base address
#define C_SPW1_BASE_ADDR          (0x80000B00U)  ///< Spacewire link1 base address
#define C_SPW2_BASE_ADDR          (0x80000C00U)  ///< Spacewire link2 base address
#define C_SPW3_BASE_ADDR          (0x80000D00U)  ///< Spacewire link3 base address
/**
 * \brief This structure define the spacewire registers mapped into APB memory space.
 **/
typedef struct t_grspw_reg
{

  /**
   * \brief SPW Control Register
   * \ref SPW_Control_Register
   * \defgroup SPWCTR SPW Control Register
   * @{
   */

#define C_SPWCTR_RESERVED_MASK     (0X1FFCF080)   ///< mask for reserved bits
#define C_SPWCTR_READONLY_MASK     (0XE0000000)   ///< mask for read only bits
#define C_SPWCTR_NOTRESET_MASK     (0X00000007)   ///< mask for not reset bits (these bits can only be reset by a spacewire GRSPW reset)
#define C_SPWCTR_SELFCLEAR_MASK    (0X00000050)   ///< mask for self clear bits (these bits can only be reset by the GRSPW core)
#define C_SPWCTR_CLEARSET_MASK     (0X00030F28)   ///< mask for clear set bits (these bits can be set with a '1' or reset with a '0')

#define C_SPWCTR_RA     (1U << 31)   ///< RMAP Available [R]
#define C_SPWCTR_RX     (1U << 30)   ///< RMAP RX Unaligned Access [R]
#define C_SPWCTR_RC     (1U << 29)   ///< RMAP RMAP CRC Available [R]
#define C_SPWCTR_RD     (1U << 17)   ///< RMAP Buffer Disable [W]
#define C_SPWCTR_RE     (1U << 16)   ///< RMAP Enable [W]
#define C_SPWCTR_TR     (1U << 11)   ///< Time RX Enable [W]
#define C_SPWCTR_TT     (1U << 10)   ///< Time TX Enable [W]
#define C_SPWCTR_LI     (1U <<  9)   ///< Link Error IRQ [W]
#define C_SPWCTR_TQ     (1U <<  8)   ///< Tick-Out IRQ [W]
#define C_SPWCTR_RS     (1U <<  6)   ///< Reset [W]
#define C_SPWCTR_OPM    (1U <<  5)   ///< Open Packet Mode [W]
#define C_SPWCTR_TI     (1U <<  4)   ///< Tick In [W]
#define C_SPWCTR_IE     (1U <<  3)   ///< Interrupt Enable [W]
#define C_SPWCTR_AS     (1U <<  2)   ///< Autostart [W]
#define C_SPWCTR_LS     (1U <<  1)   ///< Link Start [W]
#define C_SPWCTR_LD     (1U <<  0)   ///< Link Disable [W]
  uint32 ctr;        ///< SPW Control Register (SPWCTR) 0x0
  /*@}*/

  /**
   * \brief SPW Status Register
   * \ref SPW_Status_Register
   * \defgroup SPWSTR SPW Status Register
   * @{
   */
#define C_SPWSTR_LSRUN    (5U << 21)               ///< Link State is Running
#define C_SPWSTR_LSREADY  (2U << 21)               ///< Link State is Ready
#define C_SPWSTR_LS       (FIELDMASK(3U) << 21)    ///< Link State [R]
#define C_SPWSTR_EE       (1U << 8)                ///< Early EOP/EEP [RW]
#define C_SPWSTR_IA       (1U << 7)                ///< Invalid Address [RW]
#define C_SPWSTR_WE       (1U << 6)                ///< Write Synchronization Error [RW]
#define C_SPWSTR_PE       (1U << 4)                ///< Parity Error [RW]
#define C_SPWSTR_DE       (1U << 3)                ///< Disconnect Error [RW]
#define C_SPWSTR_ER       (1U << 2)                ///< Escape Error [RW]
#define C_SPWSTR_CE       (1U << 1)                ///< Credit Error [RW]
#define C_SPWSTR_TO       (1U << 0)                ///< Tick Out [RW]
  uint32 str;        ///< SPW Status Register (SPWSTR)  0x4
  /*@}*/

  /**
   * \brief SPW Node Address Register
   * \ref SPW_Node_Address_Register
   * \defgroup SPWNDR SPW Node Address Register
   * @{
   */
#define C_SPWNDR    (FIELDMASK(8U) << 0)    ///< 8-Bit Node Address [W]
  uint32 ndr;        ///< SPW Node Address Register (SPWNDR)  0x8
  /*@}*/

  /**
   * \brief SPW Clock Divisor Register
   * \ref SPW_Clock_Divisor_Register
   * \defgroup SPWCLK SPW Clock Divisor Register
   * @{
   */
#define C_SPWCLK_DIVISOR_LINK  (FIELDMASK(8U) << 8)  ///< 8-Bit Clock Divisor Link State Value [W]
#define C_SPWCLK_DIVISOR_RUN   (FIELDMASK(8U) << 0)  ///< 8-Bit Clock Divisor Run State Value [W]
  uint32 clk;        ///< SPW Clock Divisor Register (SPWCLK) 0xC
  /*@}*/

  /**
   * \brief SPW Destination Key Register
   * \ref SPW_Destination_Key_Register
   * \defgroup SPWKEY SPW Destination Key Register
   * @{
   */
#define C_SPWKEY_KEY   (FIELDMASK(8U) << 0)       ///< RMAP Destination Key. [W]
  uint32 key;        ///< SPW Destination Key Register (SPWKEY) 0x10
  /*@}*/

  /**
   * \brief SPW Time Register
   * \ref SPW_Time_Register
   * \defgroup SPWTIM SPW Time Register
   * @{
   */
#define C_SPWTIM_TIME_CTRL      (FIELDMASK(2U) << 6)      ///< Time Control Flags [R]
#define C_SPWTIM_TIME_COUNTER   (FIELDMASK(6U) << 0)      ///< Time Counter [R]
  uint32 tim;        ///< SPW Time Register (SPWTIM)  0x14
  /*@}*/

  /**
   * \brief SPW Timer and Disconnect
   * \ref SPW_Timer_and_Disconnect
   * \defgroup SPWTDR SPW Timer and Disconnect
   * @{
   */
#define C_SPWTDR_DISCONNECT   (FIELDMASK(10U) << 12)       ///< Disconnect Time Period [W]
#define C_SPWTDR_TIMER64      (FIELDMASK(12U) << 0)        ///< Timer 6.4us and 12.8us [W]
  uint32 tdr;        ///< SPW Timer and Disconnect Register (SPWTDR)  0x18
  /*@}*/

  /**
   * \brief SPW DMA Channel Control and Status Register
   * \ref SPW_DMA_Channel_Control_and_Status_Register
   * \defgroup SPWCHN SPW DMA Channel Control and Status Register
   * @{
   */
#define C_SPWCHN_RESERVED_MASK     (0XFFFEE000)     ///< mask for reserved bits
#define C_SPWCHN_NOTRESET_MASK     (0X0000001C)     ///< mask for not reset bits (these bits can only be reset by a spacewire GRSPW reset)
#define C_SPWCHN_SELFCLEAR_MASK    (0X00000A01)     ///< mask for self clear bits (these bits can only be reset by the GRSPW core)
#define C_SPWCHN_CLEARSET_MASK     (0X00011402)     ///< mask for clear set bits (these bits can be set with a '1' or reset with a '0')

#define C_SPWCHN_LE           (1U << 16)         ///< Link Error Disable [W]
#define C_SPWCHN_NS           (1U << 12)         ///< No Spill [W]
#define C_SPWCHN_RD           (1U << 11)         ///< RX Descriptors Available [RW]
#define C_SPWCHN_RX           (1U << 10)         ///< RX Active [R]
#define C_SPWCHN_AT           (1U <<  9)         ///< Abort TX [W]
#define C_SPWCHN_RA           (1U <<  8)         ///< Abort RX AHB Error [RW]
#define C_SPWCHN_TA           (1U <<  7)         ///< Abort TX AHB Error [RW]
#define C_SPWCHN_PR           (1U <<  6)         ///< Packet Received [RW]
#define C_SPWCHN_PS           (1U <<  5)         ///< Packet Sent [RW]
#define C_SPWCHN_AI           (1U <<  4)         ///< AHB Error Interrupt [W]
#define C_SPWCHN_RI           (1U <<  3)         ///< Receive Interrupt [W]
#define C_SPWCHN_TI           (1U <<  2)         ///< Transmit Interrupt [W]
#define C_SPWCHN_RE           (1U <<  1)         ///< Receiver Enable [W]
#define C_SPWCHN_TE           (1U <<  0)         ///< Transmitter Enable [RW]
  uint32 chn;        ///< SPW DMA Channel Control and Status Register (SPWCHN)  0x20
  /*@}*/

  /**
   * \brief SPW DMA Channel Receiver Maximum Length Register
   * \ref SPW_DMA_Channel_Receiver_Max_Length_Register
   * \defgroup SPWRXL SPW DMA Channel Receiver Maximum Length Register
   * @{
   */
#define C_SPWRXL_RX_MAX_LENGTH   (FIELDMASK(23U) << 2) ///< Receiver Packet Maximum Length [W]
  uint32 rxl;        ///< SPW DMA Channel Receiver Maximum Length Register (SPWRXL) 0x24
  /*@}*/

  /**
   * \brief SPW Transmitter Descriptor Table Address Register
   * \ref SPW_Transmitter_Descriptor_Table_Address_Register
   * \defgroup SPWTXD SPW Transmitter Descriptor Table Address Register
   * @{
   */
#define C_SPWTXD_TX_BASE_ADDRESS  (FIELDMASK(22U) << 10)       ///< Transmitter Descriptor Table Base Address
#define C_SPWTXD_TX_DESC_SELECT   (FIELDMASK(6U) << 4)         ///< Transmitter Descriptor Selector
  uint32 txd;        ///< SPW Transmitter Descriptor Table Address Register (SPWTXD) 0x28
  /*@}*/
  /**
   * \brief SPW Receiver Descriptor Table Address Register
   * \ref SPW_Receiver_Descriptor_Table_Address_Register
   * \defgroup SPWRXD SPW Receiver Descriptor Table Address Register
   * @{
   */
#define C_SPWRXD_RX_BASE_ADDRESS  (FIELDMASK(22U) << 10)       ///< Receiver Descriptor Table Base Address
#define C_SPWRXD_RX_DESC_SELECT   (FIELDMASK(7U) << 3)         ///< Receiver Descriptor Selector
  uint32 rxd;        ///< SPW Receive Descriptor Table Address Register (SPWRXD)  0x2C
  /*@}*/

} t_grspw_reg, *t_grspw_regptr;


typedef enum
{
  e_ctr = 0x00, ///< SPW Control Register (SPWCTR) 0x0
  e_str = 0x04, ///< SPW Status Register (SPWSTR)  0x4
  e_ndr = 0x08, ///< SPW Node Address Register (SPWNDR)  0x8
  e_clk = 0x0C, ///< SPW Clock Divisor Register (SPWCLK) 0xC
  e_key = 0x10, ///< SPW Destination Key Register (SPWKEY) 0x10
  e_tim = 0x14, ///< SPW Time Register (SPWTIM)  0x14
  e_tdr = 0x18, ///< SPW Timer and Disconnect Register (SPWTDR)  0x18
  e_chn = 0x20, ///< SPW DMA Channel Control and Status Register (SPWCHN)  0x20
  e_rxl = 0x24, ///< SPW DMA Channel Receiver Maximum Length Register (SPWRXL) 0x24
  e_txd = 0x28, ///< SPW Transmitter Descriptor Table Address Register (SPWTXD) 0x28
  e_rxd = 0x2C, ///< SPW Receive Descriptor Table Address Register (SPWRXD)  0x2C
} t_grspw_reg_offset;






#define TRACELOGERR(msg)   printf("err # %s - %s : %d - %s\n", __FILE__, __FUNCTION__, __LINE__, msg);


void grlib_reggrspw_reset_registers(const t_grspw_id grspw_id, t_grspw_regptr grspw_regptr);
void grlib_reggrspw_write_registers(const t_grspw_id grspw_id, t_grspw_regptr grspw_regptr, const t_grspw_reg_offset reg_offset, const uint32 value);
uint32 grlib_reggrspw_read_registers(const t_grspw_id grspw_id, t_grspw_regptr grspw_regptr, const t_grspw_reg_offset reg_offset);

#endif /* GRLIB_REGGRSPW_H_ */
