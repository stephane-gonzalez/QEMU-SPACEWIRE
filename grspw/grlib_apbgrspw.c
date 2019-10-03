/********************************************************************************
 *                                                                              *
 *    ▀▄   ▄▀                                                                   *
 *   ▄█▀███▀█▄    Created on: Jun 16, 2015                                      *
 *  █▀███████▀█   Author: Stephane GONZALEZ                                     *
 *  █ █▀▀▀▀▀█ █   email: stephane.gonzalez@gmx.fr                               *
 *     ▀▀ ▀▀                                                                    *
 *                                                                              *
 * Copyright 2015 grlib_grspw.c                                                 *
 * Released under the "CeCILL-C" license                                        *
 * http://www.cecill.info                                                       *
 ********************************************************************************/

#include "hw/sysbus.h"
#include "sysemu/char.h"
#include "trace.h"

#include "grlib_grspw.h"

#include "grlib_typesgrspw.h"
#include "grlib_reggrspw.h"
#include "grlib_loggrspw.h"
#include "grlib_coregrspw.h"

#define TYPE_GRLIB_APB_GRSPW "grlib,grspw"
#define GRLIB_APB_GRSPW(obj) \
    OBJECT_CHECK(GRSPW, (obj), TYPE_GRLIB_APB_GRSPW)

#define GRSPW_REG_SIZE              (0xFF)                                    ///< Size of memory mapped registers
#define GRSPW_GET_COREID(addr)      ((t_grspw_id)((addr >> 8) & 0x3))       ///< Get core selection from address
#define GRSPW_GET_REGOFFSET(addr)   ((t_grspw_reg_offset)(addr & GRSPW_REG_SIZE))  ///< Get register selection from address
#define GRSPW_GET_CORE(addr,grspw)  ((t_grspw_coreptr)(&grspw->core[GRSPW_GET_COREID(addr)]))       ///< Get core selection
#define GRSPW_GET_GRSPWREG(addr,grspw)    ((t_grspw_regptr)&GRSPW_GET_CORE(addr,grspw)->device.grspw_reg)


typedef struct GRSPW
{
  SysBusDevice parent_obj;

  MemoryRegion iomem;
  qemu_irq irq;
  t_grspw_core core[e_grspw_corenb];

  int sys_freq_hz; // system frequency
  int nb_core; // number of spacewire core

} GRSPW, *GRSPWPTR;



static uint64_t grlib_apbgrspw_read(void *opaque, hwaddr addr, unsigned size)
{
  uint64_t value = 0;

  TRACELOGDBG("")
  // trace into the console for debug
  GRSPWPTR grspwptr = opaque;

  t_grspw_id  grspw_id = GRSPW_GET_COREID(addr);
  t_grspw_regptr grspw_regptr = GRSPW_GET_GRSPWREG(addr,grspwptr);
  t_grspw_reg_offset reg_offset = GRSPW_GET_REGOFFSET(addr); // get the selected register


  // action after reading a register : < DO NOTHING >
  grlib_coregrspw_locktxsem();
  value = (uint64_t) grlib_reggrspw_read_registers(grspw_id, grspw_regptr, reg_offset);
  grlib_coregrspw_unlocktxsem();
  return value;

}

static void grlib_apbgrspw_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
{
  TRACELOGDBG("")
// trace into the console for debug

  GRSPWPTR grspwptr = opaque;

  t_grspw_id  grspw_id = GRSPW_GET_COREID(addr);
  t_grspw_regptr grspw_regptr = GRSPW_GET_GRSPWREG(addr,grspwptr);
  t_grspw_reg_offset reg_offset = GRSPW_GET_REGOFFSET(addr); // get the selected register

  //grlib_coregrspw_locktxsem();
  grlib_reggrspw_write_registers( grspw_id, grspw_regptr, reg_offset, (uint32) value);
  grlib_comgrspw_txenable(&GRSPW_GET_CORE(addr,grspwptr)->device);
  //grlib_coregrspw_txwakeup(grspw_regptr);
  //grlib_coregrspw_unlocktxsem();
}
int  grlib_apbgrspw_can_receive (void*  opaque)
{


  return 0;
}

void grlib_apbgrspw_receive  (void*           opaque,
                                          const uint8_t*  data,
                                          int             datalen)
{


}

void grlib_apbgrspw_event(void *opaque, int event)
{

}

static const MemoryRegionOps grlib_apbgrspw_ops = { .write = grlib_apbgrspw_write, .read = grlib_apbgrspw_read,
    .endianness = DEVICE_NATIVE_ENDIAN, };

static int grlib_apbgrspw_init(SysBusDevice *dev)
{
  TRACELOGDBG("")
  GRSPW *grspw = GRLIB_APB_GRSPW(dev);
  t_grspw_id grspw_id;

  TRACELOG_OPEN()


  for (grspw_id = e_grspw_core1; grspw_id < e_grspw_corenb; grspw_id++)
  {
    t_grspw_coreptr core = &grspw->core[grspw_id];
    /* One IRQ line for each core */
    sysbus_init_irq(dev, &core->device.irq);
    core->device.grspw_id = grspw_id;

    core->chr = g_malloc0(sizeof(CharDriverState));

    qemu_chr_add_handlers( core->chr, grlib_apbgrspw_can_receive,
     grlib_apbgrspw_receive, grlib_apbgrspw_event, grspw );

    // initialise registers to default value (XXX all un-define states {X=0} and all reserved bits are set to 0)
    t_grspw_regptr grspw_regptr = &core->device.grspw_reg;

    grlib_reggrspw_reset_registers(grspw_id, grspw_regptr);

  }

  sysbus_init_irq(dev, &grspw->irq);

  memory_region_init_io(&grspw->iomem, OBJECT(grspw), &grlib_apbgrspw_ops, grspw, "grspw",
      (GRSPW_REG_SIZE * e_grspw_corenb) );

  sysbus_init_mmio(dev, &grspw->iomem);

  grlib_coregrspw_init(grspw->core);

  return 0;
}

static void grlib_apbgrspw_reset(DeviceState *d)
{
  TRACELOGDBG("")
}

static Property grlib_apbgrspw_properties[] = { DEFINE_PROP_INT32("frequency", GRSPW, sys_freq_hz, 40000000),
    DEFINE_PROP_CHR("chrdev0", GRSPW, core[0].chr), DEFINE_PROP_CHR("chrdev1", GRSPW, core[1].chr),
    DEFINE_PROP_CHR("chrdev2", GRSPW, core[2].chr), DEFINE_PROP_CHR("chrdev3", GRSPW, core[3].chr),
    DEFINE_PROP_END_OF_LIST(), };

static void grlib_apbgrspw_class_init(ObjectClass *klass, void *data)
{
  TRACELOGDBG("")
  DeviceClass *dc = DEVICE_CLASS(klass);
  SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

  k->init = grlib_apbgrspw_init;
  dc->reset = grlib_apbgrspw_reset;
  dc->props = grlib_apbgrspw_properties;
}

static const TypeInfo grlib_apbgrspw_info = { .name = TYPE_GRLIB_APB_GRSPW, .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(GRSPW), .class_init = grlib_apbgrspw_class_init, };

static void grlib_apbgrspw_register_types(void)
{
  TRACELOGDBG("")
  type_register_static(&grlib_apbgrspw_info);
}

type_init( grlib_apbgrspw_register_types)
