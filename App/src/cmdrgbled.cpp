#include "cmdrgbled.h"
#include "ws2812b.h"
#include "board.h"

// Helper defines
#define newColor(r, g, b) (((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (b))
#define Red(c) ((uint8_t)((c >> 16) & 0xFF))
#define Green(c) ((uint8_t)((c >> 8) & 0xFF))
#define Blue(c) ((uint8_t)(c & 0xFF))

#define WS2812_BUFFER_COUNT 2

// RGB Framebuffers
static uint8_t frameBuffer[3 * 60];

void CmdRgbled::help(void)
{
    console->println("Usage: rgbled <init|color|demo> [option] \n");
    console->println("\tinit Initialize driver");
    console->println("\tcolor <led idx> <RGB888> Hexadecimal color");
}

char CmdRgbled::execute(int argc, char **argv)
{
    if (!xstrcmp("help", argv[1]) || argc < 2)
    {
        help();
        return CMD_OK_LF;
    }

    if (!xstrcmp("init", argv[1]))
    {
        ws2812_init();
        return CMD_OK;
    }

    if (!xstrcmp("color", argv[1]))
    {
        uint32_t idx, val;
        if (ha2i(argv[2], &idx))
        {
            if (ha2i(argv[3], &val))
            {
                idx *= 3;
                frameBuffer[idx] = val;
                frameBuffer[idx + 1] = val >> 8;
                frameBuffer[idx + 2] = val >> 16;
                ws2812_write(3, frameBuffer);
                return CMD_OK;
            }
        }
    }

    if (!xstrcmp("demo", argv[1]))
    {
        uint32_t timestamp;        
        do{
            if (GetTick() - timestamp > 50)
            {
                timestamp = GetTick();                
                visRainbow(frameBuffer, sizeof(frameBuffer), 15);
                //visDots(frameBuffer, sizeof(frameBuffer), 50, 40);
                ws2812_write(3, frameBuffer);                
            }
        }while(console->available() == 0);

        return CMD_OK;
    }

    return CMD_BAD_PARAM;
}

uint32_t CmdRgbled::Wheel(uint8_t WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return newColor(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        return newColor(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return newColor(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void CmdRgbled::visRainbow(uint8_t *frameBuffer, uint32_t frameBufferSize, uint32_t effectLength)
{
    uint32_t i;
    static uint8_t x = 0;

    x += 1;

    if (x == 256 * 5)
        x = 0;

    for (i = 0; i < frameBufferSize / 3; i++)
    {
        uint32_t color = Wheel(((i * 256) / effectLength + x) & 0xFF);

        frameBuffer[i * 3 + 0] = color & 0xFF;
        frameBuffer[i * 3 + 1] = color >> 8 & 0xFF;
        frameBuffer[i * 3 + 2] = color >> 16 & 0xFF;
    }
}

void CmdRgbled::visDots(uint8_t *frameBuffer, uint32_t frameBufferSize, uint32_t random, uint32_t fadeOutFactor)
{
    uint32_t i;

    for (i = 0; i < frameBufferSize / 3; i++)
    {

        if (rand() % random == 0)
        {
            frameBuffer[i * 3 + 0] = 255;
            frameBuffer[i * 3 + 1] = 255;
            frameBuffer[i * 3 + 2] = 255;
        }

        if (frameBuffer[i * 3 + 0] > fadeOutFactor)
            frameBuffer[i * 3 + 0] -= frameBuffer[i * 3 + 0] / fadeOutFactor;
        else
            frameBuffer[i * 3 + 0] = 0;

        if (frameBuffer[i * 3 + 1] > fadeOutFactor)
            frameBuffer[i * 3 + 1] -= frameBuffer[i * 3 + 1] / fadeOutFactor;
        else
            frameBuffer[i * 3 + 1] = 0;

        if (frameBuffer[i * 3 + 2] > fadeOutFactor)
            frameBuffer[i * 3 + 2] -= frameBuffer[i * 3 + 2] / fadeOutFactor;
        else
            frameBuffer[i * 3 + 2] = 0;
    }
}
