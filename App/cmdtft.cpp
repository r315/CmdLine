#include "cmdtft.h"
#include "board.h"
#include "lcd.h"
#include "rng.h"

#ifdef FEATURE_GIF
#include "AnimatedGIF.h"
#include "badgers.h"

#define GIF_DATA        (uint8_t*)ucBadgers
#define GIF_DATA_SIZE   sizeof(ucBadgers)
AnimatedGIF gif;
void GIFDraw(GIFDRAW *pDraw);
void gifPlayFrame(void);
#endif


typedef struct demo_s {
    void (*setup)(void);
    uint32_t (*loop)(void);
    void (*end)(void);
}demo_t;

static uint16_t tile[512];
static uint16_t seed, scroll;
static int16_t x, y, px,py;
static uint16_t state, step, stepSize, numSteps, tcount, color;
static uint8_t hue = 0;
static uint32_t demo_frames;

static void AmigaBall_Setup(void);
static uint32_t AmigaBall_Loop(void);
static void Spiral_Setup(void);
static uint32_t Spiral_Loop(void);
static void Scroll_Setup(void);
static uint32_t Scroll_Loop(void);
static void RandomColors_Setup(void);
static uint32_t RandomColors_Loop(void);
#ifdef FEATURE_GIF
static void Gif_Setup(void);
static uint32_t Gif_Loop(void);
static void Gif_End(void);
#endif
const demo_t demos[] = {
    {AmigaBall_Setup, AmigaBall_Loop, NULL},
    {Spiral_Setup, Spiral_Loop, NULL},
    {RandomColors_Setup, RandomColors_Loop, NULL},
    {Scroll_Setup, Scroll_Loop, NULL},
#ifdef FEATURE_GIF
    {Gif_Setup, Gif_Loop, Gif_End}
#endif
};

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

uint32_t Tiles_Loop(){
    uint16_t *buf = tile;
    for(uint8_t i = 0; i < LCD_GetHeight()/16; i++){
        for(uint8_t j = 0; j < LCD_GetWidth()/16; j++){
            memset(buf, generateRandomColor(seed), 15 * 15 * 2);            
            LCD_WriteArea(j * 16, i * 16, 15, 15, buf);
            buf = tile + (256 * (j & 1));
        }
    }
    return (++demo_frames) < 300;
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
    console->putString("Usage: tft <option> [params] \n");    
    console->putString("options:");
    console->putString("  init <orientation>,  orientation 0-3");
    console->putString("  clear <color>,       Fill display with color");
    console->putString("  rc [scroll],         Random colors");
    console->putString("  scroll,              Scroll screen");
    console->putString("  hsv <s> <v>,         HSV color");
    console->putString("  block,               Color squares");
    console->putString("  scroll,              Color scroll");
    console->putString("  demo,                Demo sequence");
    console->putChar('\n');
}

char CmdTft::execute(int argc, char **argv){
    int32_t val1;
    char c;

    if(argc == 1){
        help();
        return CMD_OK;
    }

    if(xstrcmp("init", (const char*)argv[1]) == 0){
        if(yatoi(argv[2], (int32_t*)&val1)){
            BOARD_LCD_Init();
		    LCD_SetOrientation(val1 & 3);
		    LCD_FillRect(0, 0, LCD_GetWidth(), LCD_GetHeight(), LCD_BLACK);
		    LCD_Bkl(1);
            return CMD_OK;
        }
    }

    if(xstrcmp("clear", (const char*)argv[1]) == 0){
        if(hatoi(argv[2], (uint32_t*)&val1)){
            uint32_t ms = GetTick();
            LCD_FillRect(0, 0, LCD_GetWidth(), LCD_GetHeight(), val1);
            ms = GetTick() - ms;
            console->print("Time: %dms\n", ms);
            return CMD_OK;
        }
    }

    if(xstrcmp("block", (const char*)argv[1]) == 0){
        uint16_t *buf = tile;
        for(uint8_t i = 0; i < 128/16; i++){
            for(uint8_t j = 0; j < 160/16; j++){
                memset(buf, RNG_Get(), 15 * 15 * 2);
                LCD_WriteArea(j * 16, i * 16, 15, 15, buf);
                buf = tile + (256 * (j & 1));
            }
        }
        return CMD_OK;
    }

    if(xstrcmp("scroll", (const char*)argv[1]) == 0){    
        Scroll_Setup();
        do{           
            console->print("\r%d  ", scroll);
            Scroll_Loop();
            console->getCharNonBlocking(&c);
            DelayMs(16);
        }while(c != '\n' && c != '\r');
        return CMD_OK_LF;
    }

    if(xstrcmp("rc", (const char*)argv[1]) == 0){
        uint16_t f = 0;

        if((const char*)argv[2] == NULL){            
            RandomColors_Loop();
            return CMD_OK;
        }

        seed = RNG_Get() % 256;

        do{
            Tiles_Loop();
            fps();

            if(f == 0){
                scroll = (scroll + 1) % 160;                
                LCD_Scroll(scroll);
                f = 2; // scroll speed
            }

            f--;
            c = '\0';
            if(console->getCharNonBlocking(&c)){
                seed = RNG_Get() % 256;
            }
        }while(c != '\n' && c != '\r');

        return CMD_OK_LF;
    }

    if(xstrcmp("hsv", (const char*)argv[1]) == 0){
        uint8_t h = 0, s, v;
        uint16_t *buf = tile;
        if(yatoi(argv[2], (int32_t*)&s)){
            if(yatoi(argv[3], (int32_t*)&v)){                
                for(uint8_t i = 0; i < LCD_GetHeight()/8; i++){
                    for(uint8_t j = 0; j < LCD_GetWidth()/8; j++){
                        memset16(buf, HsvToRgb(h++, s, v), 64);                        
                        LCD_WriteArea(j * 8, i * 8, 7, 7, buf);
                        buf = tile + (256 * (j & 1));
                    }
                }
                return CMD_OK;
            }
        }
    }

    if(xstrcmp("demo", (const char*)argv[1]) == 0){        
        char c, limit_fps = 1;
        uint32_t time = 0;
        uint8_t state = 0, demo = 0;

        LCD_Scroll(0);

        do{
            switch(state){
                case 0:
                    demo_frames = 0;
                    state = 1;
                    demos[demo].setup();
                    break;

                case 1:
                    time = GetTick();

                    if(demos[demo].loop() == 0){
                        state = 2;                        
                    }

                    fps();

                    time = GetTick() - time;
                    break;

                case 2:
                    if(demos[demo].end != NULL){
                        demos[demo].end();
                    }
                    demo = (demo + 1) % (sizeof(demos) / sizeof(demo_t));
                    state = 0;
                    break;

                default:                   
                    break;
            }

            if(limit_fps && time < 16){
                DelayMs(16 - time);
            }
            
            if(console->getCharNonBlocking(&c)){
                limit_fps ^= 1;
            }

        }while(c != '\n' && c != '\r');

        return CMD_OK_LF;
    }
#ifdef FEATURE_GIF
    if(xstrcmp("gif", (const char*)argv[1]) == 0){
        long lTime;
        int iFrames = 0;        
        
        gif.begin(BIG_ENDIAN_PIXELS);

        console->print("GIF CPU speed benchmark\n");
        if (gif.open(GIF_DATA, GIF_DATA_SIZE, GIFDraw))
        {
            console->print("Successfully opened GIF, starting test...\n");
            lTime = HAL_GetTick();
            while (gif.playFrame(false, NULL))
            {
                iFrames++;
            }
            gif.close();
            lTime = HAL_GetTick() - lTime;
            console->print("Decoded %d frames in %d miliseconds", iFrames, lTime);
        }  
        return CMD_OK_LF;
    }
#endif
    return CMD_BAD_PARAM;
}

void CmdTft::fps(void){
    static uint32_t expire = 0;
    static uint16_t fps = 0;
 
    if(GetTick() > expire){
        console->print("\rfps %d ", fps);
        fps = 0;
        expire = GetTick() + 1000;
        LED1_TOGGLE;
    }
    fps++;
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
#define LINE_XS2    2
#define BALL_WD     64
#define BALL_HT     64

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
uint16_t grid_sx1, vline_h1;
uint8_t nvlines, nhlines;

void drawBall(int x, int y)
{
    static uint8_t bf = 0;
    int i, j, ii;    

    for (j = 0; j < BALL_HT; j++)
    {
        uint16_t *line = linebuffer + (160 * ((bf++) & 1)); // swap buffer

        uint8_t v, *img = (uint8_t *)ball + 16 * 2 + 6 + j * BALL_WD / 2 + BALL_WD / 2;
        
        int yy = y + j;
        if (yy == LINE_YS || yy == LINE_YS + 1 * 10 || yy == LINE_YS + 2 * 10 || yy == LINE_YS + 3 * 10 || yy == LINE_YS + 4 * 10 || yy == LINE_YS + 5 * 10 || yy == LINE_YS + 6 * 10 ||
            yy == LINE_YS + 7 * 10 || yy == LINE_YS + 8 * 10 || yy == LINE_YS + 9 * 10 || yy == LINE_YS + 10 * 10 || yy == LINE_YS + 11 * 10 || yy == LINE_YS + 12 * 10)
        { // ugly but fast
            for (i = 0; i < grid_sx1; i++)
                line[i] = line[SCR_WD - 1 - i] = bgCol; // erase ball outside grid

            for (i = 0; i <= SCR_WD - grid_sx1 * 2; i++)
                line[i + grid_sx1] = lineCol;  // draw grid hline
        }
        else
        {
            for (i = 0; i < SCR_WD; i++)
                line[i] = bgCol; // erase ball on grid

            if (yy > LINE_YS)
                for (i = 0; i < nvlines; i++)
                    line[grid_sx1 + i * 10] = lineCol; // draw vline pixel
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

static void AmigaBall_Setup(void)
{
    uint16_t *pal = (uint16_t *)ball + 3;

    bgCol    = RGB565(200,200,200);
    bgColS   = RGB565(90,90,90);
    lineCol  = RGB565(150,40,150);
    lineColS = RGB565(80,10,80);    

    if(LCD_GetWidth() == 128){
        grid_sx1 = 19;
        vline_h1 = 120;
        nvlines = 10;
        nhlines = 13;
    }else{
        grid_sx1 = 20;
        vline_h1 = 90;
        nvlines = 13;
        nhlines = 10;
    }

    scroll = 0;
    LCD_Scroll(scroll);
    LCD_Clear(bgCol);

    for (uint8_t i = 0; i < 16; i++)
        palette[i] = *pal++;

    for (uint8_t i = 0; i < nvlines; i++){
        LCD_DrawVLine(grid_sx1 + i * 10, LINE_YS, vline_h1, lineCol);
    }

    for (uint8_t i = 0; i < nhlines; i++){
        LCD_DrawHLine(grid_sx1, LINE_YS + i * 10, SCR_WD - grid_sx1 * 2, lineCol);
    }

    int dy = SCR_HT - LINE_YS - (LINE_YS + vline_h1);
    int dx = grid_sx1 - LINE_XS2;
    
    int o = 7 * dx / dy;
    LCD_DrawHLine(LINE_XS2 + o, LINE_YS + vline_h1 + 6 + 4, SCR_WD - LINE_XS2 * 2 - o * 2, lineCol);
    o = (7 + 6) * dx / dy;
    LCD_DrawHLine(LINE_XS2 + o, LINE_YS + vline_h1 + 4, SCR_WD - LINE_XS2 * 2 - o * 2, lineCol);
    o = (7 + 6 + 4) * dx / dy;
    LCD_DrawHLine(LINE_XS2 + o,LINE_YS + vline_h1, SCR_WD - LINE_XS2 * 2 - o * 2, lineCol);

    uint16_t last_w = SCR_WD - (LINE_XS2 * 2);
    LCD_DrawHLine(LINE_XS2, SCR_HT - LINE_YS, last_w, lineCol);

    last_w = last_w / (nvlines - 1);

    for (uint8_t i = 0; i < nvlines; i++){
        LCD_DrawLine(grid_sx1 + i * 10, LINE_YS + vline_h1, LINE_XS2 + i * last_w, SCR_HT - LINE_YS, lineCol);
    }
}


static uint32_t AmigaBall_Loop(void)
{
    static int16_t anim=0, animd=1;
    static int16_t x=0, y=0;
    static int16_t xd=2, yd=1;
    
    for (int i = 0; i < 14; i++)
    {
        palette[i + 1] = ((i + anim) % 14) < 7 ? LCD_WHITE : LCD_RED;
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
    
    if (x >= SCR_WD - BALL_WD)
    {
        x = SCR_WD - BALL_WD;
        xd = -xd;
        animd = -animd;
    }
    
    if (y < 0)
    {
        y = 0;
        yd = -yd;
    }
    
    if (y >= LINE_YS + vline_h1 + 1 - BALL_HT)
    {
        y = LINE_YS + vline_h1 + 1- BALL_HT;
        yd = -yd;
    }

    return (++demo_frames) < 300;
}

#ifdef FEATURE_GIF
#define TILE_W  8
void drawTileLine(uint16_t x, uint16_t y, uint16_t w, uint16_t *line){
    y = y * TILE_W;
    x = x * TILE_W;
    for(uint8_t i = 0; i < w; i++){
        LCD_FillRect(x + (i * TILE_W), y, TILE_W - 0, TILE_W - 0, line[i]);
    }
}

// Draw a line of image directly on the LCD
void GIFDraw(GIFDRAW *pDraw)
{
    uint8_t *s;
    uint16_t *d, *usPalette, usTemp[320];
    int x, y, iWidth;

    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y; // current line
    iWidth = pDraw->iWidth;
    if (iWidth > LCD_GetWidth())
       iWidth = LCD_GetWidth();
    
    s = pDraw->pPixels;

    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
      for (x=0; x<iWidth; x++)
      {
        if (s[x] == pDraw->ucTransparent)
           s[x] = pDraw->ucBackground;
      }
      pDraw->ucHasTransparency = 0;
    }
    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
      uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
      int x, iCount;
      pEnd = s + iWidth;
      x = 0;
      iCount = 0; // count non-transparent pixels
      while(x < iWidth)
      {
        c = ucTransparent-1;
        d = usTemp;
        while (c != ucTransparent && s < pEnd)
        {
          c = *s++;
          if (c == ucTransparent) // done, stop
          {
            s--; // back up to treat it like transparent
          }
          else // opaque
          {
             *d++ = usPalette[c] >> 8 | usPalette[c] << 8;
             iCount++;
          }
        } // while looking for opaque pixels
        if (iCount) // any opaque pixels?
        {
          //pilcdSetPosition(&lcd, pDraw->iX+x, y, iCount, 1, DRAW_TO_LCD);
          //spilcdWriteDataBlock(&lcd, (uint8_t *)usTemp, iCount*2, DRAW_TO_LCD);
          LCD_WriteArea(pDraw->iX+x, y, iCount, 1, usTemp);
          x += iCount;
          iCount = 0;
        }
        // no, look for a run of transparent pixels
        c = ucTransparent;
        while (c == ucTransparent && s < pEnd)
        {
          c = *s++;
          if (c == ucTransparent)
             iCount++;
          else
             s--;
        }
        if (iCount)
        {
          x += iCount; // skip these
          iCount = 0;
        }
      }
    }
    else
    {
      s = pDraw->pPixels;
      // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
      for (x=0; x<iWidth; x++){
            uint16_t color = usPalette[*s++];
            usTemp[x] = color >> 8 | color << 8;
      }
      //spilcdSetPosition(&lcd, pDraw->iX, y, iWidth, 1, DRAW_TO_LCD);
      //spilcdWriteDataBlock(&lcd, (uint8_t *)usTemp, iWidth*2, DRAW_TO_LCD);
      LCD_WriteArea(pDraw->iX, y, iWidth, 1, usTemp);
      //drawTileLine(pDraw->iX, y, iWidth, usTemp);
    }
}

static void Gif_Setup(void){
    gif.begin(BIG_ENDIAN_PIXELS);
    if(gif.open(GIF_DATA, GIF_DATA_SIZE, GIFDraw)){
        demo_frames = gif.getInfo()->iFrameCount * 8;
    }
}

static uint32_t Gif_Loop(void){
    
    if(gif.playFrame(true, NULL) == 0){
        gif.reset();
        gif.playFrame(true, NULL);
    }

    demo_frames--;
    return demo_frames;
}

static void Gif_End(void){
    gif.close();   
}
#endif

static uint8_t isPrime(uint16_t n){
    if (n == 1) return false;
    for (uint16_t i = 2; i < n >> 1; i++){
        if(n % i == 0){
            return false;
        }
    }
    return true;
}

static void Spiral_Setup(void){
    LCD_Clear(LCD_BLACK);
    x = LCD_GetWidth() / 2;
    y = LCD_GetHeight() / 2;

    px = x;
    py = y;

    state = 0;
    step = 1;
    stepSize = 8;
    numSteps = 1;
    tcount = 0;

    color = RNG_Get();
}

static uint32_t Spiral_Loop(void){

    if(isPrime(step)){
        LCD_FillRect(x - (stepSize >> 1), y - (stepSize >> 1), stepSize - 1, stepSize - 1,color);
    }

    LCD_DrawLine(px, py, x, y, color);
    px = x;
    py = y;

    switch (state){
        case 0:
            x += stepSize;
            break;
        case 1:
            y -= stepSize;
            break;
        case 2:
            x -= stepSize;
            break;
        case 3:
            y += stepSize;
            break;
    }

    if(step % numSteps == 0){
        state = (state + 1 ) % 4;
        tcount ++;
        if(tcount % 2 == 0){
            numSteps++;
        }
    }

    step++;

    return (++demo_frames) < 300;
}

static void Scroll_Setup(void){
    scroll = 0;
    hue = RNG_Get();
}

static uint32_t Scroll_Loop(void){
    y = (LCD_GetHeight() - 1) - scroll;
    scroll = (scroll + 1) % LCD_GetHeight();

    LCD_FillRect(0, y, LCD_GetWidth(), 1,  HsvToRgb(hue++, 255, 255));      
    
    LCD_Scroll(scroll);

    return (++demo_frames) < 300;
}

static void RandomColors_Setup(void){

}

static uint32_t RandomColors_Loop(void){
    for(size_t i = 0; i < LCD_GetHeight(); i++){
        uint16_t *buf = tile + (LCD_GetWidth() * (i & 1));
        for (size_t j = 0; j < LCD_GetWidth(); j++){
            buf[j] = RNG_Get();                    
        }                
        LCD_WriteArea(0, i, LCD_GetWidth(), 1, buf);
    }
    return (++demo_frames) < 50;
}