#pragma once

#include <kernel/config.h>
#ifdef DEVICE_BUILD_BOARD_raspi

#include <stdint.h>

// Mailbox Addresses
#define MMIO_VIDCORE_MBOX 0xB880
#define MBOX_READ         (MMIO_VIDCORE_MBOX + 0x00)
#define MBOX_POLL         (MMIO_VIDCORE_MBOX + 0x10)
#define MBOX_SENDER       (MMIO_VIDCORE_MBOX + 0x14)
#define MBOX_STATUS       (MMIO_VIDCORE_MBOX + 0x18)
#define MBOX_CONFIG       (MMIO_VIDCORE_MBOX + 0x1C)
#define MBOX_WRITE        (MMIO_VIDCORE_MBOX + 0x20)

// Mailbox Codes
#define MBOX_REQUEST  0
#define MBOX_RESPONSE 0x80000000
#define MBOX_FULL     0x80000000
#define MBOX_EMPTY    0x40000000

// Mailbox Channels
#define MBOX_CH_POWER 0
#define MBOX_CH_FB    1
#define MBOX_CH_VUART 2
#define MBOX_CH_VCHIQ 3
#define MBOX_CH_LEDS  4
#define MBOX_CH_BTNS  5
#define MBOX_CH_TOUCH 6
#define MBOX_CH_COUNT 7
#define MBOX_CH_PROP  8

// Mailbox Tags
#define MBOX_TAG_GETSERIAL  0x10004
#define MBOX_TAG_SETCLKRATE 0x38002
#define MBOX_TAG_LAST       0

#define GPFSEL0   0x00200000
#define GPFSEL1   0x00200004
#define GPFSEL2   0x00200008
#define GPFSEL3   0x0020000C
#define GPFSEL4   0x00200010
#define GPFSEL5   0x00200014
#define GPSET0    0x0020001C
#define GPSET1    0x00200020
#define GPCLR0    0x00200028
#define GPLEV0    0x00200034
#define GPLEV1    0x00200038
#define GPEDS0    0x00200040
#define GPEDS1    0x00200044
#define GPHEN0    0x00200064
#define GPHEN1    0x00200068
#define GPPUD     0x00200094
#define GPPUDCLK0 0x00200098
#define GPPUDCLK1 0x0020009C

namespace kernel {
namespace arm {
namespace rpi {

/// Core class for the raspberry pi.
/// Holds raspberry pi specific information and detection routines.
class rpi {
   public:
    static rpi&  get();                           ///< Returns the global pi object.
    void         init();                          ///< Grabs data from the hardware about this Pi and caches it.
    int          get_version();                   ///< Returns the version of this Pi (3, 4..)
    uintptr_t    mmio_base();                     ///< Returns the base address of the MMIO region for this Raspberry Pi
    unsigned int mmio_read(unsigned int offset);  ///< Reads an int from [mmio_base + offset]
    void         mmio_write(unsigned int offset, unsigned int value);  ///< Writes an int to [mmio_base + offset]

    int mbox_call(unsigned char channel);  ///< Send a mailbox call from the mbox buffer to channel.
    volatile unsigned int __attribute__((aligned(16))) mbox[36];  ///< Mailbox buffer

   private:
    int       raspi_version;  ///< The raspberry pi version number
    uintptr_t mmio_addr;      ///< The address of the MMIO.
};
}  // namespace rpi
}  // namespace arm
}  // namespace kernel

#else

#error "This header should not be included for this architecture. Check build configuration"

#endif