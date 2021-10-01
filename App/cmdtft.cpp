#include "cmdtft.h"
#include "board.h"
#include "lcd.h"

extern StdOut *userio;

uint16_t tile[512];
uint16_t seed;

void AmigaBall_Loop(void);
void AmigaBall_Setup(void);


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

void randomTiles(){
    uint16_t *buf = tile;
    for(uint8_t i = 0; i < LCD_GetHeight()/16; i++){
        for(uint8_t j = 0; j < LCD_GetWidth()/16; j++){
            memset(buf, generateRandomColor(seed), 15 * 15 * 2);
            SPI_WaitEOT(&spi1);
            LCD_WriteArea(j * 16, i * 16, 15, 15, buf);
            buf = tile + (256 * (j & 1));
        }
    }
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
        return CMD_OK_LF;
    }

    if(xstrcmp("rc", (const char*)argv[0]) == 0){
        uint16_t s = 0, f = 0;
        uint16_t *buf = tile;
        char c;

        seed = RNG_Get() % 256;
        do{

            fps(randomTiles);

            if(f == 0){
                s++;
                s = s % 160;
                SPI_WaitEOT(&spi1);
                LCD_Scroll(s);
                f = 2; // scroll speed
            }

            f--;
            c = '\0';
            if(userio->getCharNonBlocking(&c)){
                seed = RNG_Get() % 256;
            }
        }while(c != '\n');

        return CMD_OK_LF;
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

    if(xstrcmp("demo", (const char*)argv[0]) == 0){        
        char c, max_fps = 0;
        uint32_t elapsed;
        LCD_Scroll(0);
        AmigaBall_Setup();

        do{            
            elapsed = fps(AmigaBall_Loop);

            if(!max_fps){
                HAL_Delay(19 - elapsed); // 50fps
            }
            
            if(userio->getCharNonBlocking(&c)){
                max_fps ^= 1;
            }
        }while(c != '\n');

        return CMD_OK_LF;
    }
   
    return CMD_BAD_PARAM;
}

uint32_t CmdTft::fps(void (*func)(void)){
    static uint32_t start, totalms = 0;
    static uint16_t fps = 0;

    start = HAL_GetTick();
    func();
    fps++;
    
    if(HAL_GetTick() - totalms >= 1000){
        console->print("\rfps %d ", fps);
        fps = 0;
        totalms = HAL_GetTick();;
        LED_TOGGLE;
    }

    return HAL_GetTick() - start; // return elapsed time from last call
}

// ST7735 library example
// Amiga Boing Ball Demo
// (c) 2019 Pawel A. Hernik
// YT video: https://youtu.be/KwtkfmglT-c

//https://github.com/cbm80amiga/Arduino_ST7735_Fast/tree/master/examples

#include "ball.h"

#define SCR_WD      LCD_GetWidth() //128
#define SCR_HT      LCD_GetHeight() //160

#define LINE_YS     10
#define LINE_XS1    19
#define LINE_XS2    2

#define BALL_WD     64
#define BALL_HT     64
#define BALL_SWD    128
#define BALL_SHT    131

#define SHADOW      20

// AVR stats:
// with shadow        - 42-43ms/24fps
// without shadow     - 37-38ms/27fps
// without background - 31-32ms/32fps
// SPI transfer only  - 22-23ms/45fps (128x64x16bit)
// STM32 stats:
// with shadow        - 7-8ms/125fps
// without shadow     - 6-7ms/166fps
// without background - 5-6ms/200fps
// SPI transfer only  - 4-5ms/250fps (128x64x16bit)

uint16_t palette[16];
uint16_t linebuffer[160  * 2];
uint16_t bgCol, bgColS, lineCol, lineColS;

void drawBall(int x, int y)
{
    static uint8_t bf = 0;
    int i, j, ii;
    

    for (j = 0; j < BALL_HT; j++)
    {
        uint16_t *line = linebuffer + (160 * ((bf++) & 1));

        uint8_t v, *img = (uint8_t *)ball + 16 * 2 + 6 + j * BALL_WD / 2 + BALL_WD / 2;
        
        int yy = y + j;
        if (yy == LINE_YS || yy == LINE_YS + 1 * 10 || yy == LINE_YS + 2 * 10 || yy == LINE_YS + 3 * 10 || yy == LINE_YS + 4 * 10 || yy == LINE_YS + 5 * 10 || yy == LINE_YS + 6 * 10 ||
            yy == LINE_YS + 7 * 10 || yy == LINE_YS + 8 * 10 || yy == LINE_YS + 9 * 10 || yy == LINE_YS + 10 * 10 || yy == LINE_YS + 11 * 10 || yy == LINE_YS + 12 * 10)
        { // ugly but fast
            for (i = 0; i < LINE_XS1; i++)
                line[i] = line[SCR_WD - 1 - i] = bgCol;
            for (i = 0; i <= SCR_WD - LINE_XS1 * 2; i++)
                line[i + LINE_XS1] = lineCol;
        }
        else
        {
            for (i = 0; i < SCR_WD; i++)
                line[i] = bgCol;
            if (yy > LINE_YS)
                for (i = 0; i < 10; i++)
                    line[LINE_XS1 + i * 10] = lineCol;
        }
        for (i = BALL_WD - 2; i >= 0; i -= 2)
        {
            v = *(--img);
            if (v >> 4)
            {
                line[x + i + 0] = palette[v >> 4];
#if SHADOW
                ii = x + i + 0 + SHADOW;
                if (ii < SCR_WD)
                {
                    if (line[ii] == bgCol)
                        line[ii] = bgColS;
                    else if (line[ii] == lineCol)
                        line[ii] = lineColS;
                }
#endif
            }
            if (v & 0xf)
            {
                line[x + i + 1] = palette[v & 0xf];
#if SHADOW
                ii = x + i + 1 + SHADOW;
                if (ii < SCR_WD)
                {
                    if (line[ii] == bgCol)
                        line[ii] = bgColS;
                    else if (line[ii] == lineCol)
                        line[ii] = lineColS;
                }
#endif
            }
        }
        LCD_WriteArea(0, yy, SCR_WD, 1, line);
    }
}

void AmigaBall_Setup()
{
    int o, i;
    uint16_t *pal = (uint16_t *)ball + 3;

    bgCol    = RGB565(200,200,200);
    bgColS   = RGB565(90,90,90);
    lineCol  = RGB565(150,40,150);
    lineColS = RGB565(80,10,80);

    LCD_Clear(bgCol);

    for (i = 0; i < 16; i++)
        palette[i] = *pal++;

    for (i = 0; i < 10; i++){
        LCD_Line_V(LINE_XS1 + i * 10, LINE_YS, 12 * 10, lineCol);
    }

    for (i = 0; i < 13; i++){
        LCD_Line_H(LINE_XS1, LINE_YS + i * 10, SCR_WD - LINE_XS1 * 2, lineCol);
    }

    LCD_Line_H(LINE_XS2, SCR_HT - LINE_YS, SCR_WD - LINE_XS2 * 2, lineCol);

    int dy = SCR_HT - LINE_YS - (LINE_YS + 10 * 12);
    int dx = LINE_XS1 - LINE_XS2;
    
    o = 7 * dx / dy;
    LCD_Line_H(LINE_XS2 + o, SCR_HT - LINE_YS - 7, SCR_WD - LINE_XS2 * 2 - o * 2, lineCol);
    o = (7 + 6) * dx / dy;
    LCD_Line_H(LINE_XS2 + o, SCR_HT - LINE_YS - 7 - 6, SCR_WD - LINE_XS2 * 2 - o * 2, lineCol);
    o = (7 + 6 + 4) * dx / dy;
    LCD_Line_H(LINE_XS2 + o, SCR_HT - LINE_YS - 7 - 6 - 4, SCR_WD - LINE_XS2 * 2 - o * 2, lineCol);

    for (i = 0; i < 10; i++){
        LCD_Line(LINE_XS1 + i * 10, LINE_YS + 10 * 12, LINE_XS2 + i * (SCR_WD - LINE_XS2 * 2) / 9, SCR_HT - LINE_YS, lineCol);
    }
}


void AmigaBall_Loop()
{
    static int16_t anim=0, animd=1;
    static int16_t x=0, y=0;
    static int16_t xd=2, yd=1;
    
    for (int i = 0; i < 14; i++)
    {
        palette[i + 1] = ((i + anim) % 14) < 7 ? WHITE : RED;
        //int c=((i+anim)%14); // with ping between white and red
        //if(c<6) palette[i+1]=WHITE; else if(c==6 || c==13) palette[i+1]=RGB565(255,128,128); else palette[i+1]=RED;
    }

    drawBall(x, y);
    anim += animd;
    
    if (anim < 0){
        anim += 14;
    }

    x += xd;
    y += yd;
    
    if (x < 0)
    {
        x = 0;
        xd = -xd;
        animd = -animd;
    }
    
    if (x >= BALL_SWD - BALL_WD)
    {
        x = BALL_SWD - BALL_WD;
        xd = -xd;
        animd = -animd;
    }
    
    if (y < 0)
    {
        y = 0;
        yd = -yd;
    }
    
    if (y >= BALL_SHT - BALL_HT)
    {
        y = BALL_SHT - BALL_HT;
        yd = -yd;
    }
}
