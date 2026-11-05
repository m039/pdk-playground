#include <pdk/device.h>
#include <stdint.h>
#include "auto_sysclock.h"
#include "i2c.h"

/* 6-byte Unique ID payload returned on I2C read requests */
uint8_t ID[6] = { 0x22, 0x55, 0x33, 0x11, 0x33, 0x53 };

static uint16_t tick;

void main(void)
{
    i2c_init(ID, sizeof(ID) / sizeof(ID[0]));

    uint8_t clkmd_old = CLKMD;

    while (1)
    {
        if (i2c_is_idle())
        {
            /* Stay awake for 65536 idle passes after the last bus activity,
               then enter deep sleep (stopsys). Clock is switched to ILRC and
               IHRC is disabled for sub-microamp standby. Upon wake-up by
               SCL/SDA toggle, IHRC is restored to clkmd_old. */
            if (++tick == 0)
            {
                CLKMD = 0xF4;
                CLKMD &= ~CLKMD_ENABLE_IHRC;
                __asm stopsys __endasm;
                __asm nop __endasm;
                CLKMD = clkmd_old;
            }
        }
        else
        {
            tick = 0; /* Bus active: restart the idle countdown */
        }

        i2c_process();
    }
}

/*
  Startup code - setup and calibrate internal system clock.
*/
unsigned char _sdcc_external_startup(void)
{
    AUTO_INIT_SYSCLOCK();
    AUTO_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);
    return 0;
}
