/*
** Made by fabien le mentec <texane@gmail.com>
**
** Started on  Sun Nov 15 12:35:57 2009 texane
** Last update Tue Nov 17 19:36:26 2009 texane
*/

#include <string.h>
#include "avr_if.h"
#include "stk500.h"
#include "stk500_proto.h"
#include "board.h"

#define SKIP_DATA(BUF, BUF_SIZE, DATA_SIZE) \
    do                                      \
    {                                       \
        BUF += DATA_SIZE;                   \
        BUF_SIZE += DATA_SIZE;              \
    } while (0)

#define PUSH_DATA(BUF, BUF_SIZE, DATA, DATA_SIZE) \
    do                                            \
    {                                             \
        memcpy(BUF, DATA, DATA_SIZE);             \
        BUF += DATA_SIZE;                         \
        BUF_SIZE += DATA_SIZE;                    \
    } while (0)

#define PUSH_BYTE(BUF, BUF_SIZE, DATA) \
    do                                 \
    {                                  \
        *(BUF) = DATA;                 \
        ++(BUF);                       \
        ++(BUF_SIZE);                  \
    } while (0)

#define EEPROM_MEM_TYPE (uint8_t)'E'
#define FLASH_MEM_TYPE (uint8_t)'F'

#define SPI_SIGNATURE_BYTE_COUNT    3
#define SPI_CALIBRATION_BYTE_COUNT  4
#define STATE_FLAG_HAS_DEV_SETTINGS (1 << 0)

#define STATE_HAS_FLAG(S, F)    ((S)->flags & (1 << STATE_FLAG_##F))
#define STATE_CLEAR_FLAG(S, F)  ((S)->flags &= ~(1 << STATE_FLAG_##F))
#define STATE_SET_FLAG(S, F)    ((S)->flags |= 1 << STATE_FLAG_##F)

#define PROGRAM_PAGE_SIZE 0x100

typedef struct state{
    uint32_t flags;
    uint16_t addr;
}state_t;

#ifdef NO_SERVICE
typedef struct _Service
{
    uint8_t state;
    stk500_error_t error;
    uint32_t isize;
    uint32_t osize;
    uint8_t do_timeout;
    uint8_t buf[STK500_BUF_MAX_SIZE];
} Service;

static Service stkService;
#endif

static state_t state;
static serialops_t *serial;

void write_uint8(uint8_t *data, uint32_t len)
{
    serial->writeBytes(data, len);
}

int read_uint8(uint8_t *c, uint8_t do_timeout)
{
#ifdef NO_SERVICE
    if (!serial->available()){
        return -1;
    }
    *c = (uint8_t)serial->readchar();
    return 0;
#else
    if (do_timeout){
        uint32_t ticks = GetTick();
        while (ElapsedTicks(ticks) < 1000){
            if(serial->available()){
                *c = serial->read();
                return 0;
            }
        }
    }else{
        *c = serial->read();
        return 0;
    }
    return -1;
#endif
}

static int get_param(uint8_t k, uint8_t *v)
{
    switch (k)
    {
    case Parm_STK_HW_VER:
    case Parm_STK_SW_MAJOR:
    case Parm_STK_SW_MINOR:
    {
        /* dummy versions */
        *v = 42;
        break;
    }

    case Parm_STK_LEDS:
    {
        /* leds off */
        *v = 0;
        break;
    }

    case Parm_STK_VTARGET:
    case Parm_STK_VADJUST:
    {
        /* divided by 10 */
        *v = 50;
        break;
    }

    case Parm_STK_OSC_PSCALE:
    case Parm_STK_OSC_CMATCH:
    {
        *v = 0;
        break;
    }

    case Parm_STK_RESET_DURATION:
    {
        *v = 20;
        break;
    }

    case Parm_STK_SCK_DURATION:
    {
        *v = 8;
        break;
    }

    case Parm_STK_BUFSIZEL:
    {
        *v = STK500_BUF_MAX_SIZE & 0xff;
        break;
    }

    case Parm_STK_BUFSIZEH:
    {
        *v = (STK500_BUF_MAX_SIZE >> 8) & 0xff;
        break;
    }

    case Param_STK500_TOPCARD_DETECT:
    {
        *v = 3;
        break;
    }

    default:
    {
        break;
    }
    }

    return 0;
}

static int set_param(uint8_t k, uint8_t v)
{
    return 0;
}

/* device settings */

static void set_dev_settings(uint8_t *buf)
{
    /* buf points on the first setting */

    STATE_SET_FLAG(&state, HAS_DEV_SETTINGS);
}

static void set_ext_settings(uint8_t *buf, uint32_t n)
{

    STATE_SET_FLAG(&state, HAS_DEV_SETTINGS);
}

/* memory programming routines */

static void write_flash_page(uint16_t page_addr, uint8_t *buf, uint32_t size)
{
    /* sequence for writing a page
       foreach page word, LOAD_PROGRAM_MEM_PAGE(lsb7(addr), data)
       WRITE_PROGRAM_MEM_PAGE(msb9(addr))
       wait write completion (polling or Twd_flash, done by spi)
     */

    /* assume size < PROGRAM_PAGE_SIZE */

    uint32_t count;
    uint8_t page_off = 0;
    uint16_t value;

    /* note: page_addr is a byte address
       while spi work on flash with word
       relative addresses.
     */

    for (count = size / 2; count; --count, buf += 2, ++page_off)
    {
        /* skip 0xffff bytes since flash erased */
        if (*(uint16_t *)buf == 0xffff)
            continue;

        value = ((uint16_t)buf[1] << 8) | ((uint16_t)buf[0]);

        avrLoadProgramPage(page_off, value);
    }

    if (size & 1)
    {
        uint8_t tmp[2];

        tmp[0] = *buf;
        tmp[1] = 0;

        avrLoadProgramPage(page_off, *(uint16_t *)tmp);
    }

    /* commit the page */
    avrWriteProgramPage(page_addr);
}

static void write_flash_word(uint16_t addr, uint16_t value)
{
    avrLoadProgramPage((uint8_t)((addr / 2) & 0xff), value);
    avrWriteProgramPage(addr);
}

static void read_flash_page(
    uint16_t page_addr,
    uint8_t *buf,
    uint32_t size)
{
    uint32_t count;
    uint16_t value;

    // page_addr /= 2;  // BUG ?????

    for (count = size / 2; count; --count, ++page_addr, buf += 2)
    {
        value = avrReadProgram(page_addr);

        buf[0] = (uint8_t)(value & 0xff);
        buf[1] = (uint8_t)((value >> 8) & 0xff);
    }

    if (size & 1)
    {
        value = avrReadProgram(page_addr);
        buf[0] = (uint8_t)(value & 0xff);
    }
}

static uint16_t read_flash_word(uint16_t addr)
{
    return avrReadProgram(addr / 2);
}

static void write_eeprom_page(
    uint16_t page_addr,
    uint8_t *buf,
    uint32_t size)
{
    for (; size; --size, ++buf, ++page_addr)
        spi_write_eeprom(page_addr, *buf);
}

static void read_eeprom_page(
    uint16_t page_addr,
    uint8_t *buf,
    uint32_t size)
{
    for (; size; --size, ++buf, ++page_addr)
        *buf = spi_read_eeprom(page_addr);
}

static void read_fuse_bits(uint8_t *bits, uint32_t n)
{
    bits[0] = spi_read_fuse_bits();
    bits[1] = spi_read_fuse_high_bits();

    if (n == 3)
        bits[2] = spi_read_extended_fuse_bits();
}

static uint8_t read_osccal_at(uint8_t i)
{
    uint8_t buf[SPI_CALIBRATION_BYTE_COUNT];
    spi_read_calibration_bytes(buf);
    return buf[i];
}

/* exported */

void stk500_timeout(uint8_t *buf, uint32_t isize, uint32_t *osize)
{
    *osize = 0;
}

void stk500_setup(serialops_t *sp)
{
    state.flags = 0;
    state.addr = 0;
    serial = sp;

#ifdef NO_SERVICE
    stkService.state = STK500_ERROR_SUCCESS;
    stkService.isize = 0;
    stkService.osize = 0;
#endif
}

void stk500_loop(void)
{

#ifdef NO_SERVICE
    switch (stkService.state)
    {

    case STK500_ERROR_MORE_DATA:
        if (read_uint8(stkService.buf + stkService.isize, 0) == -1)
        {
            stk500_timeout(stkService.buf, stkService.isize, &stkService.osize);
            stkService.state = STK500_ERROR_SUCCESS;
            break;
        }
        ++stkService.isize;
        stkService.state = stk500_process(stkService.buf, stkService.isize, &stkService.osize);
        break;

    case STK500_ERROR_SUCCESS:
    default:
        stkService.state = STK500_ERROR_MORE_DATA;
        stkService.isize = 0;
        stkService.osize = 0;
        break;
    }

    if ((stkService.state == STK500_ERROR_SUCCESS) && (stkService.osize))
    {
        serial_write(stkService.buf, stkService.osize);
    }
#else
    stk500_error_t error;
    uint32_t isize;
    uint32_t osize;
    uint8_t do_timeout;
    unsigned char buf[STK500_BUF_MAX_SIZE];

    while (1)
    {
        error = STK500_ERROR_MORE_DATA;

        do_timeout = 0;

        isize = 0;
        osize = 0;

        while (error == STK500_ERROR_MORE_DATA)
        {
            if (read_uint8(buf + isize, do_timeout) == -1)
            {
                /* has timeouted */
                stk500_timeout(buf, isize, &osize);
                error = STK500_ERROR_SUCCESS;
                break;
            }

            do_timeout = 1;

            ++isize;

            error = stk500_process(buf, isize, &osize);
        }

        if ((error == STK500_ERROR_SUCCESS) && (osize)){
            write_uint8(buf, osize);
        }
    }
#endif
}

/**
 * @brief stk500v1 message process
 * see avr061.pdf
 * 
 * @param buf 
 * @param isize 
 * @param osize 
 * @return
 */
stk500_error_t stk500_process(uint8_t *buf, uint32_t isize, uint32_t *osize)
{
    /* assume isize >= 1 */

    stk500_error_t error = STK500_ERROR_SUCCESS;

    *osize = 0;

    switch (*buf)
    {
    case Cmnd_STK_GET_SYNC:
    {
        /* get synchronization back */

        if (isize < 2)
            goto on_more_data;

        goto on_insync_ok;
        break;
    }

    case Cmnd_STK_GET_SIGN_ON:
    {
        /* ping the programmer */

        if (isize < 2)
            goto on_more_data;

        PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);
        PUSH_DATA(buf, *osize, STK_SIGN_ON_MESSAGE, 7);
        PUSH_BYTE(buf, *osize, Resp_STK_OK);

        break;
    }

    case Cmnd_STK_SET_PARAMETER:
    {
        /* get a param value */

        if (isize < 4)
            goto on_more_data;

        PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);

        if (set_param(buf[0], buf[1]) != -1)
        {
            PUSH_BYTE(buf, *osize, Resp_STK_OK);
        }
        else
        {
            PUSH_BYTE(buf, *osize, buf[0]);
            PUSH_BYTE(buf, *osize, Resp_STK_FAILED);
        }

        break;
    }

    case Cmnd_STK_GET_PARAMETER:
    {
        /* get a param value */

        uint8_t v;

        if (isize < 3)
            goto on_more_data;

        PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);

        if (get_param(*buf, &v) != -1)
        {
            PUSH_BYTE(buf, *osize, v);
            PUSH_BYTE(buf, *osize, Resp_STK_OK);
        }
        else
        {
            PUSH_BYTE(buf, *osize, *buf);
            PUSH_BYTE(buf, *osize, Resp_STK_FAILED);
        }

        break;
    }

    case Cmnd_STK_SET_DEVICE:
    {
        /* set programming params */

        if (isize < 22)
            goto on_more_data;

        set_dev_settings(buf + 1);

        goto on_insync_ok;
        break;
    }

    case Cmnd_STK_SET_DEVICE_EXT:
    {
        if (isize < (buf[1] + 2))
            goto on_more_data;

        set_ext_settings(buf + 2, isize - 3);

        goto on_insync_ok;
        break;
    }

    case Cmnd_STK_ENTER_PROGMODE:
    {
        /* enter the programming mode */

        if (isize < 2)
            goto on_more_data;

        if (!STATE_HAS_FLAG(&state, HAS_DEV_SETTINGS))
        {
            PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);
            PUSH_BYTE(buf, *osize, Resp_STK_NODEVICE);
            break;
        }

        if (avrProgrammingEnable(1, 0) != -1)
            goto on_insync_ok;

        error = STK500_ERROR_FAILURE;

        break;
    }

    case Cmnd_STK_LEAVE_PROGMODE:
    {
        /* assume progrmode entered */

        if (isize < 2)
            goto on_more_data;

        avrProgrammingEnable(0, 0);

        goto on_insync_ok;
        break;
    }

    case Cmnd_STK_CHIP_ERASE:
    {
        if (isize < 2)
            goto on_more_data;

        avrChipErase();

        goto on_insync_ok;
        break;
    }

    case Cmnd_STK_CHECK_AUTOINC:
    {
        /* address autoincremented during programming */

        if (isize < 2)
            goto on_more_data;

        if (buf[1] != Sync_CRC_EOP)
            goto on_nosync;

        goto on_insync_ok;
        break;
    }

    case Cmnd_STK_LOAD_ADDRESS:
    {
        if (isize < 4)
            goto on_more_data;

        state.addr = (uint16_t)buf[1] | ((uint16_t)buf[2] << 8);

        goto on_insync_ok;
        break;
    }

    case Cmnd_STK_PROG_FLASH:
    {
        /* program one word at state.addr */

        uint16_t value;

        if (isize < 4)
            goto on_more_data;

        value = ((uint16_t)buf[2] << 8) | ((uint16_t)buf[1]);
        write_flash_word(state.addr, value);
        state.addr += 2;

        goto on_insync_ok;
        break;
    }

    case Cmnd_STK_PROG_DATA:
    {
        /* program one eeprom byte at state.addr */

        if (isize < 3)
            goto on_more_data;

        spi_write_eeprom(state.addr, buf[1]);
        state.addr += 1;

        goto on_insync_ok;
        break;
    }

    case Cmnd_STK_PROG_FUSE:
    {
        if (isize < 4)
            goto on_more_data;

        spi_write_fuse_bits(buf[1]);
        spi_write_fuse_high_bits(buf[2]);

        goto on_insync_ok;
        break;
    }

    case Cmnd_STK_PROG_FUSE_EXT:
    {
        if (isize < 5)
            goto on_more_data;

        spi_write_fuse_bits(buf[1]);
        spi_write_fuse_high_bits(buf[2]);
        spi_write_extended_fuse_bits(buf[3]);

        goto on_insync_ok;
        break;
    }

    case Cmnd_STK_PROG_LOCK:
    {
        if (isize < 3)
            goto on_more_data;

        spi_write_lock_bits(buf[1]);

        goto on_insync_ok;
        break;
    }

    case Cmnd_STK_PROG_PAGE:
    {
        uint32_t block_size;

        if (isize < 5)
            goto on_more_data;

        block_size = ((uint32_t)buf[1] << 8) | (uint32_t)buf[2];

        if ((isize - 5) < block_size)
            goto on_more_data;

        if (buf[3] == FLASH_MEM_TYPE)
            write_flash_page(state.addr, buf + 4, block_size);
        else if (buf[3] == EEPROM_MEM_TYPE)
            write_eeprom_page(state.addr, buf + 4, block_size);

        state.addr += block_size;

        goto on_insync_ok;
        break;
    }

    case Cmnd_STK_READ_FLASH:
    {
        /* read flash word */

        uint16_t value;

        if (isize < 2)
            goto on_more_data;

        PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);

        value = read_flash_word(state.addr);
        state.addr += 2;

        PUSH_BYTE(buf, *osize, value & 0xff);
        PUSH_BYTE(buf, *osize, (value >> 8) & 0xff);

        PUSH_BYTE(buf, *osize, Resp_STK_OK);

        break;
    }

    case Cmnd_STK_READ_DATA:
    {
        /* read eeprom byte */

        if (isize < 2)
            goto on_more_data;

        PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);

        *buf = spi_read_eeprom(state.addr);
        state.addr += 1;

        SKIP_DATA(buf, *osize, 1);
        PUSH_BYTE(buf, *osize, Resp_STK_OK);

        break;
    }

    case Cmnd_STK_READ_FUSE:
    {
        if (isize < 2)
            goto on_more_data;

        PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);

        read_fuse_bits(buf, 2);
        SKIP_DATA(buf, *osize, 2);

        PUSH_BYTE(buf, *osize, Resp_STK_OK);

        break;
    }

    case Cmnd_STK_READ_FUSE_EXT:
    {
        if (isize < 2)
            goto on_more_data;

        PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);

        read_fuse_bits(buf, 3);
        SKIP_DATA(buf, *osize, 3);

        PUSH_BYTE(buf, *osize, Resp_STK_OK);

        break;
    }

    case Cmnd_STK_READ_LOCK:
    {
        if (isize < 2)
            goto on_more_data;

        PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);

        *buf = spi_read_lock_bits();
        SKIP_DATA(buf, *osize, 1);

        PUSH_BYTE(buf, *osize, Resp_STK_OK);

        break;
    }

    case Cmnd_STK_READ_PAGE:
    {
        uint32_t block_size;

        if (isize < 5)
            goto on_more_data;

        block_size = ((uint32_t)buf[1] << 8) | (uint32_t)buf[2];

        PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);

        if (buf[2] == FLASH_MEM_TYPE)
            read_flash_page(state.addr, buf, block_size);
        else if (buf[2] == EEPROM_MEM_TYPE)
            read_eeprom_page(state.addr, buf, block_size);

        state.addr += block_size;

        SKIP_DATA(buf, *osize, block_size);
        PUSH_BYTE(buf, *osize, Resp_STK_OK);

        break;
    }

    case Cmnd_STK_READ_SIGN:
    {
        if (isize < 2)
            goto on_more_data;

        PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);

        avrDeviceCode(buf);
        SKIP_DATA(buf, *osize, SPI_SIGNATURE_BYTE_COUNT);

        PUSH_BYTE(buf, *osize, Resp_STK_OK);

        break;
    }

    case Cmnd_STK_READ_OSCCAL:
    {
        if (isize < 2)
            goto on_more_data;

        PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);

        *buf = read_osccal_at(0);
        SKIP_DATA(buf, *osize, 1);

        PUSH_BYTE(buf, *osize, Resp_STK_OK);

        break;
    }

    case Cmnd_STK_READ_OSCCAL_EXT:
    {
        if (isize < 2)
            goto on_more_data;

        PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);

        *buf = read_osccal_at(*buf);
        SKIP_DATA(buf, *osize, 1);

        PUSH_BYTE(buf, *osize, Resp_STK_OK);

        break;
    }

    case Cmnd_STK_UNIVERSAL:
    {
        if (isize < 6)
            goto on_more_data;

        PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);

        *buf = spi_write_read(buf);
        SKIP_DATA(buf, *osize, 1);

        PUSH_BYTE(buf, *osize, Resp_STK_OK);

        break;
    }

    default:
    {
        error = STK500_ERROR_INVALID_CMD;
        goto on_nosync;
        break;
    }
    }

    /* here we just return */

    goto on_return;

on_more_data:
    error = STK500_ERROR_MORE_DATA;
    goto on_return;

on_nosync:
    PUSH_BYTE(buf, *osize, Resp_STK_NOSYNC);
    goto on_return;

on_insync_ok:
    PUSH_BYTE(buf, *osize, Resp_STK_INSYNC);
    PUSH_BYTE(buf, *osize, Resp_STK_OK);
    goto on_return;

on_return:
    return error;
}

