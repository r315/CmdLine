#include "cmdtft.h"
#include "board.h"
#include "lcd.h"

extern StdOut *userio;
extern "C" void LCD_FillBlock(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
extern "C" void LCD_WriteBlock(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data);

uint16_t tile[512];


uint16_t generateRandomColor(int32_t mix) {
    uint16_t red = RNG_Get() % 32;
    uint16_t green = RNG_Get() % 64;
    uint16_t blue = RNG_Get() % 32;

    // mix the color
    if (mix > 0) {
        red = (red + ((mix & 31) >> 11)) >> 1;
        green = (green + ((mix & 63) >> 5)) >> 1;
        blue = (blue + ((mix & 31) >> 0)) >> 1;
    }
    
    return (red << 11) | (green << 5) | (blue << 0);
}

uint16_t HsvToRgb(uint8_t h, uint8_t s, uint8_t v)
{
    uint8_t region, remainder, p, q, t;
    uint8_t r, g, b;

    if (s == 0){
        r = v;
        g = v;
        b = v;
    }else{

        region = h / 43;
        remainder = (h - (region * 43)) * 6; 

        p = (v * (255 - s)) >> 8;
        q = (v * (255 - ((s * remainder) >> 8))) >> 8;
        t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

        switch (region)
        {
            case 0:
                r = v; g = t; b = p;
                break;
            case 1:
                r = q; g = v; b = p;
                break;
            case 2:
                r = p; g = v; b = t;
                break;
            case 3:
                r = p; g = q; b = v;
                break;
            case 4:
                r = t; g = p; b = v;
                break;
            default:
                r = v; g = p; b = q;
                break;
        }
    }

    r = r >> 3;
    g = g >> 2;
    b = b >> 3;
    return (r << 11) | (g << 5) | b;
}


/**
 * Public API
 * */
void CmdTft::help(void){
    console->xputs("Usage: tft <option> [params] \n");    
    console->xputs("options:");
    console->xputs("  init <orientation>,  orientation 0-3");
    console->xputs("  clear <color>,       Fill display with color");
    console->xputs("  rc,                  Random colors");
    console->xputs("  scroll,              Scroll screen");
    console->xputs("  hsv <s> <v>,         HSV color");
    console->xputchar('\n');
}

char CmdTft::execute(void *ptr){
    int32_t val1;
    char *argv[4];
    int argc;

    argc = strToArray((char*)ptr, argv);

    if(argc < 1){
        help();
        return CMD_OK;
    }

    if(xstrcmp("init", (const char*)argv[0]) == 0){
        if(yatoi(argv[1], (int32_t*)&val1)){
            LCD_Init(&spi1);
		    LCD_Rotation(val1 & 3);
		    LCD_Clear(BLACK);
		    LCD_Bkl(1);
            return CMD_OK;
        }
    }

    if(xstrcmp("clear", (const char*)argv[0]) == 0){
        if(hatoi(argv[1], (uint32_t*)&val1)){
            //LCD_Clear(val1);
            LCD_FillRect(0, 0, LCD_GetWidth(), LCD_GetHeight(), val1);
            SPI_WaitEOT(&spi1);
            return CMD_OK;
        }
    }

    if(xstrcmp("block", (const char*)argv[0]) == 0){
        uint16_t *buf = tile;
        for(uint8_t i = 0; i < 128/16; i++){
            for(uint8_t j = 0; j < 160/16; j++){
                memset(buf, i*3, 15 * 15 * 2);
                SPI_WaitEOT(&spi1);
                LCD_WriteArea(j * 16, i * 16, 15, 15, buf);                
                buf = tile + (256 * (j & 1));
            }
        }
        return CMD_OK;
    }

    if(xstrcmp("scroll", (const char*)argv[0]) == 0){
        uint8_t s = 0;
        char c;
        uint8_t h = 0;
        LCD_Scroll(0);
        //LCD_Clear(BLACK);
        //LCD_Line_H(0, 0, 128, RED);
        //LCD_Line_H(0, 159, 128, GREEN);
        do{           
            LCD_Scroll(s);
            int y = s + 160 - 16;
            if(y >= 160) y -= 160;
            if((s % 16) == 0){
                uint16_t color = HsvToRgb(h++, 255, 255);
                LCD_FillRect(0, y, 128, 16, color);
            }
            console->print("\r%d  ", s);            
            s++;
            s = s % 160;
            DelayMs(16);
            //c = userio->xgetchar();
            userio->getCharNonBlocking(&c);
        }while(c != '\n');
        return CMD_OK;
    }

    if(xstrcmp("rc", (const char*)argv[0]) == 0){
        uint16_t *buf = tile;
        uint16_t seed = RNG_Get() % 256, s = 0, f = 0;
        char c;
        do{
            for(uint8_t i = 0; i < LCD_GetHeight()/16; i++){
                for(uint8_t j = 0; j < LCD_GetWidth()/16; j++){
                    memset(buf, generateRandomColor(seed), 15 * 15 * 2);
                    SPI_WaitEOT(&spi1);
                    LCD_WriteArea(j * 16, i * 16, 15, 15, buf);
                    buf = tile + (256 * (j & 1));
                }
            }

            if(f == 0){
                s++;
                s = s % 160;
                SPI_WaitEOT(&spi1);
                LCD_Scroll(s);
                f = 1;
            }

            f--;

            c = '\0';
            if(userio->getCharNonBlocking(&c)){
                seed = RNG_Get() % 256;
            }
        }while(c != '\n');

        return CMD_OK;
    }

    if(xstrcmp("hsv", (const char*)argv[0]) == 0){
        uint8_t h = 0, s, v;
        uint16_t *buf = tile;
        if(yatoi(argv[1], (int32_t*)&s)){
            if(yatoi(argv[2], (int32_t*)&v)){                
                for(uint8_t i = 0; i < LCD_GetHeight()/8; i++){
                    for(uint8_t j = 0; j < LCD_GetWidth()/8; j++){
                        memset16(buf, HsvToRgb(h++, s, v), 64);
                        SPI_WaitEOT(&spi1);
                        LCD_WriteArea(j * 8, i * 8, 7, 7, buf);
                        buf = tile + (256 * (j & 1));
                    }
                }
                return CMD_OK;
            }
        }
    }
   
    return CMD_BAD_PARAM;
}
