#include "board.h"
#include "spiflash/flash_dev.h"

#ifdef ENABLE_SPI
static spibus_t spibus;
#endif

#ifdef ENABLE_QSPI
static qspibus_t qspibus;
#endif


#ifdef ENABLE_FLASH
static flashdev_t flashdev;
extern const flashdev_info_t gd25lq16;
static void flashSelect(uint8_t en);
#endif

#ifdef ENABLE_DISPLAY
static drvlcdspi_t lcd0;

void BOARD_LCD_Init(void)
{
    lcd0.w = 240;
    lcd0.h = 320;
    lcd0.cs = LCD_CS_PIN;
    lcd0.cd = LCD_CD_PIN;
    lcd0.bkl = LCD_BKL_PIN;
    lcd0.rst = LCD_RST_PIN;
    lcd0.spidev = &spibus;

    GPIO_Config(lcd0.cs, GPO_MS);
    GPIO_Config(lcd0.cd, GPO_MS);
    GPIO_Config(lcd0.rst, GPO_MS);
    GPIO_Config(lcd0.bkl, GPO_MS);
    GPIO_Config(LCD_DI_PIN, GPIO_AF_SPI1_SPI2);
    GPIO_Config(LCD_SCK_PIN, GPIO_AF_SPI1_SPI2);

    LCD_Init(&lcd0);

    LCD_Bkl(1);

    LCD_FillRect(0, 0, lcd0.w, lcd0.h, 0);
}
#endif

#ifdef ENABLE_FLASH
static void flashSelect(uint8_t en)
{
    (void)en;
}
#endif

#ifdef ENABLE_I2C
i2cbus_t board_i2cbus;

uint32_t i2cWrite(uint8_t device, const uint8_t *data, uint16_t len)
{
    return I2C_Write(&board_i2cbus, device, data, len);
}

uint32_t i2cRead(uint8_t device, uint8_t *data, uint16_t len)
{
    return I2C_Read(&board_i2cbus, device, data, len);
}
#endif

#ifdef ENABLE_PWM
void BOARD_PWM_Init(pwmchip_t *pwmchip)
{
    #warning "PWM init not implemented for nucleo"
    /* pwmchip->chip = 3;
    PWM_Init(pwmchip);

    GPIO_Config(PB_0, GPO_HS_AF);
    GPIO_Config(PB_1, GPO_HS_AF); */
}
#endif

#ifdef ENABLE_TONE
enum tone_e TONE_Init(void)
{
    tone_init_t init = {
        .per = TIM16,
        .ch = 1 - 1,
        .pin = PB_6,
        .pin_idle = 0
    };

    return TONE_PwmInit(&init);
}
#endif

void BOARD_Init(void)
{
    SERIAL_Init();

    #ifdef ENABLE_SPI
    spibus.bus = SPI_BUS0;
    spibus.cfg = SPI_CFG_DMA | SPI_CFG_CS;
    spibus.freq = 1000;

    SPI_Init(&spibus);
    #endif

    #ifdef ENABLE_I2C
    board_i2cbus.bus_num = I2C_BUS0;
    board_i2cbus.speed = 100;
    board_i2cbus.cfg = I2C_CFG_PINS; // D4(SDA), D5(SCL)
    I2C_Init(&board_i2cbus);
    #endif

    #ifdef ENABLE_FLASH
    qspibus.bus = QSPI_BUS0;
    qspibus.freq = 1000;
    qspibus.size = gd25lq16.size;
    qspibus.bus = 0;

    QSPI_Init(&qspibus);

    flashdev.bus = &qspibus;
    flashdev.info = &gd25lq16;
    flashdev.select = flashSelect; // Not called with qspi

    flashDevInit(&flashdev);
    #endif

    #ifdef ENABLE_TONE
    TONE_Init();
    #endif
}

#ifdef ENABLE_FLASH
flashdev_res_t flashRead(uint8_t *pbuffer, uint32_t addr, uint16_t len)
{
    return flashDevRead(&flashdev, pbuffer, addr, len);
}

flashdev_res_t flashWrite(uint8_t *pbuffer, uint32_t addr, uint16_t len)
{
    return flashDevWrite(&flashdev, pbuffer, addr, len);
}

flashdev_res_t flashReadId(uint32_t *id)
{
    return flashDevReadId(&flashdev, (uint8_t*)id);
}

flashdev_res_t flashErase(uint32_t addr, enum flashdev_blk_sz size)
{
    return flashDevErase(&flashdev, addr, size);
}
#endif