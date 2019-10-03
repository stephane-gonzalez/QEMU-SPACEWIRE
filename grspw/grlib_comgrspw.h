/*
 * grlib_comgrspw.h
 *
 *  Created on: May 18, 2016
 *      Author: loky
 */

#ifndef GRLIB_COMGRSPW_H_
#define GRLIB_COMGRSPW_H_

#include <hw/irq.h>

#include "grlib_typesgrspw.h"
#include "grlib_reggrspw.h"
#include "grlib_mqgrspw.h"


/**
 * \brief Receiver DMA engine
 * \ref Setting_up_the_descriptor_table_address
 * \defgroup Setting up the descriptor table address
 * @{
 */
#define C_ONE_KILO_BYTE     (1024U)    ///< 1 kB aligned address

/**
 * \brief This structure define SpaceWire Receive Descriptor.
 **/
typedef struct t_grspw_rx_desc
{
  /**\ref SpaceWire_Receive_Descriptor_Word_0
   * \defgroup SPWTRW0 SpaceWire Receive Descriptor Word 0
   * @{
   */
#define C_SPWTRW0_TR              (1U << 31)   ///< Truncated [R]
#define C_SPWTRW0_DC              (1U << 30)   ///< Data CRC [R]
#define C_SPWTRW0_HC              (1U << 29)   ///< Header CRC [R]
#define C_SPWTRW0_EP              (1U << 28)   ///< EEP Termination [R]
#define C_SPWTRW0_IE              (1U << 27)   ///< Interrupt Enable [R]
#define C_SPWTRW0_WR              (1U << 26)   ///< Wrap [R]
#define C_SPWTRW0_EN              (1U << 25)   ///< Enable Descriptor [R]
#define C_SPWTRW0_PACKET_LENGTH   (FIELDMASK(25U) << 0)  ///< The number of bytes received by the buffer [R]
  uint32 ctrl;        ///< SpaceWire Receive Descriptor Word 0
  /*@}*/

  uint32 addr;        ///< SpaceWire Receive Descriptor Word 1
} t_grspw_rx_desc, *t_grspw_rx_descptr;

/**
 * \brief Transmitter DMA engine
 * \ref Setting_up_the_descriptor_table_address
 * \defgroup Setting up the descriptor table address
 * @{
 */



typedef struct
{
  /**\ref SpaceWire_Transmitter_Descriptor_Word_0
   * \defgroup SPWTDW0 SpaceWire Transmitter Descriptor Word 0
   * @{
   */
#define C_SPWTDW0_DC                    (1U << 17)             ///< Data CRC
#define C_SPWTDW0_HC                    (1U << 16)             ///< Header CRC
#define C_SPWTDW0_LE                    (1U << 15)             ///< Link Error
#define C_SPWTDW0_IE                    (1U << 14)             ///< Interrupt Enable
#define C_SPWTDW0_WR                    (1U << 13)             ///< Wrap
#define C_SPWTDW0_EN                    (1U << 12)             ///< Enable
#define C_SPWTDW0_NON_CRC_BYTES         (FIELDMASK(4U) << 8)   ///< Sets the number of bytes in the beginning of the header which should not be included in the CRC calculation.
#define C_SPWTDW0_HEADER_LENGTH         (FIELDMASK(8U) << 0)   ///< Header length in bytes.
  uint32 ctrl;        ///< SpaceWire Transmitter Descriptor Word 0
  /*@}*/
  uint32 addr_header;        ///< SpaceWire Transmitter Descriptor Word 1
  /**\ref SpaceWire_Transmitter_Descriptor_Word_2
   * \defgroup SPWTDW2 SpaceWire Transmitter Descriptor Word 2
   * @{
   */
#define C_SPWTDW2_DATA_LENGTH           (FIELDMASK(24U) << 0)   ///< Data length in bytes.
  uint32 len;        ///< SpaceWire Transmitter Descriptor Word 2
  /*@}*/
  uint32 data;        ///< SpaceWire Transmitter Descriptor Word 3
} t_grspw_tx_descr, *t_grspw_tx_descrptr;


typedef enum
{
  e_ls_reset,
  e_ls_wait,
  e_ls_ready,
  e_ls_started,
  e_ls_connect,
  e_ls_run
} t_grspw_link_state;

#define C_NB_RX_DESCRIPTOR  (128U)    ///< maximum number of RX descriptors is 128
#define C_NB_TX_DESCRIPTOR  (64U)    ///< maximum number of TX descriptors is 64
#define C_MAX_HEADER_LEN    (255)   ///< The maximum header length is 255 bytes
#define C_MAX_DATA_LEN      (16 *1024 *1024) ///< The maximum data length is 16MB


typedef struct
{
  qemu_irq irq;             ///< spacewire irq mapping
  t_grspw_id grspw_id;
  t_grspw_link_state link_state;

  t_grspw_reg grspw_reg;        ///< spacewire registers mapped into APB memory space.
  t_grspw_rx_desc rxd_map[C_NB_RX_DESCRIPTOR];
  t_grspw_rx_desc txd_map[C_NB_TX_DESCRIPTOR];
  uint32 txd_cur;
  uint32 rxd_cur;
  uint8ptr txdmad_map[C_NB_TX_DESCRIPTOR]; ///< tx descriptor for dma data
  uint8ptr txdmah_map[C_NB_TX_DESCRIPTOR]; ///< tx descriptor for dma header
  uint8ptr rxdmad_map[C_NB_RX_DESCRIPTOR]; ///< rx descriptor for dma data
} t_grsw_device, * t_grsw_deviceptr;

void grlib_comgrspw_rxenable(t_grsw_deviceptr deviceptr, const uint8 data[], const uint32 len, const t_recv_attptr attptr);
boolean grlib_comgrspw_txenable(t_grsw_deviceptr deviceptr);
void grlib_comgrspw_rxtimecode(t_grsw_deviceptr deviceptr, const uint8 timecode);

#endif /* GRLIB_COMGRSPW_H_ */
