#include "board.h"
#include "cmdtft.h"
#include "rng.h"
#include "drvlcd.h"
#include "wdt.h"

#define ENABLE_DEMO_TILES   1

#ifdef FEATURE_GIF
#include "AnimatedGIF.h"
#include "badgers.h"

#define GIF_DATA        (uint8_t*)ucBadgers
#define GIF_DATA_SIZE   sizeof(ucBadgers)
AnimatedGIF gif;
void GIFDraw(GIFDRAW *pDraw);
void gifPlayFrame(void);
#endif

enum {
    DEMO_SETUP = 0,
    DEMO_RUN,
    DEMO_END
};

typedef struct democtx_s{
    int16_t x, y, xd, yd;
    uint16_t sx, w;
    uint16_t sy, h;
    uint32_t frames, seed;
    uint8_t state, stepSize;
    uint8_t numSteps, tcount;
    uint16_t step, color;
    uint16_t bgColor, gridLineColor;
    uint16_t ballShadowColor, gridLineShadowColor;
    uint16_t value, bidx, scroll;
    uint8_t spacing, nvlines, nhlines;
    uint16_t palette[16];
    uint16_t buf[512];          // ideally 2 * display width
}democtx_t;

typedef struct demo_ops_s {
    void (*setup)(democtx_t *);
    uint32_t (*loop)(democtx_t *);
    void (*cleanup)(democtx_t *);
}demoops_t;

static void Tiles_Setup(democtx_t *);
static uint32_t Tiles_Loop(democtx_t *);
#if ENABLE_DEMO_AMIGA
static void AmigaBall_Setup(democtx_t *);
static uint32_t AmigaBall_Loop(democtx_t *);
#endif
#if ENABLE_DEMO_SPIRAL
static void Spiral_Setup(democtx_t *);
static uint32_t Spiral_Loop(democtx_t *);
#endif
#if ENABLE_DEMO_SCROLL
static void Scroll_Setup(democtx_t *);
static uint32_t Scroll_Loop(democtx_t *);
static void Scroll_Cleanup(democtx_t *);
#endif
#if ENABLE_DEMO_RCOLOR
static void RandomColors_Setup(democtx_t *);
static uint32_t RandomColors_Loop(democtx_t *d);
#endif
#ifdef FEATURE_GIF
static void Gif_Setup(democtx_t *);
static uint32_t Gif_Loop(democtx_t *);
static void Gif_Cleanup(democtx_t *);
#endif

static democtx_t demo_ctx;

const demoops_t demos[] = {
#if ENABLE_DEMO_TILES
    {Tiles_Setup, Tiles_Loop},
#endif
#if ENABLE_DEMO_AMIGA
    {AmigaBall_Setup, AmigaBall_Loop, NULL},
#endif
#if ENABLE_DEMO_SPIRAL
    {Spiral_Setup, Spiral_Loop, NULL},
#endif
#if ENABLE_DEMO_RCOLOR
    {RandomColors_Setup, RandomColors_Loop, NULL},
#endif
#if ENABLE_DEMO_SCROLL
    {Scroll_Setup, Scroll_Loop, Scroll_Cleanup},
#endif
#ifdef FEATURE_GIF
    {Gif_Setup, Gif_Loop, Gif_Cleanup},
#endif
#if ENABLE_DEMO_GRAPH
    {Graph_Setup, Graph_Loop, Graph_Cleanup},
#endif
};

static const uint16_t f_data [] = {
0xffff,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xffff,
0xffff,0xf800,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
0xffff,0xf800,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
0xffff,0xf800,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
0xffff,0xf800,0xf800,0xf800,0xf800,0xffff,0xffff,0xffff,
0xffff,0xf800,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
0xffff,0xf800,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
0xffff,0xf800,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
};

void LCD_Line(uint16_t x1, uint16_t y1,  uint16_t x2, uint16_t y2, uint16_t color)
{
	signed int dy = y2 - y1;
    signed int dx = x2 - x1;
    signed int stepx, stepy;
    signed int fraction;

    if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
    if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }

    if ( x1 == x2 ){
    	//LIB2D_VLine(x1, (y1 < y2)? y1 : y2, dy);
        LCD_FillRect(x1, (y1 < y2)? y1 : y2, 1, dy, color);
    	return;
    }

    if ( y1 == y2 ){
		//LIB2D_HLine((x1 < x2)? x1 : x2, y1, dx);
        LCD_FillRect((x1 < x2)? x1 : x2, y1, dx, 1, color);
		return;
    }

    dy <<= 1;
    dx <<= 1;

    LCD_Pixel(x1, y1, color);

    if (dx > dy)
    {
        fraction = dy - (dx >> 1);
        while (x1 != x2)
        {
            if (fraction >= 0)
            {
                y1 += stepy;
                fraction -= dx;
            }
            x1 += stepx;
            fraction += dy;
            LCD_Pixel(x1, y1, color);
        }
    }
    else
    {
        fraction = dx - (dy >> 1);
        while (y1 != y2)
        {
            if (fraction >= 0)
            {
                x1 += stepx;
                fraction -= dy;
            }
            y1 += stepy;
            fraction += dx;
            LCD_Pixel(x1, y1, color);
        }
    }
}

#if ENABLE_DEMO_SPIRAL
static uint8_t isPrime(uint16_t n){
    if (n == 1) return false;
    for (uint16_t i = 2; i < n >> 1; i++){
        if(n % i == 0){
            return false;
        }
    }
    return true;
}
#endif

static uint16_t RandomColor(int32_t mix)
{
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

/**
 * @brief HSV color space
 *
 * @param h     hue 0-255
 * @param s     saturation 0-255
 * @param v     value 0-255
 * @return      RGB565
 */
uint16_t HsvToRgb(uint8_t h, uint8_t s, uint8_t v)
{
    uint8_t region, remainder, p, q, t;
    uint8_t r, g, b;

    if (s == 0){
        r = g = b = v;
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
    console->println("Usage: tft <option> [params] \n");
    console->println("options:");
    console->println("  init <orientation>,  orientation 0-3");
    console->println("  clear <color>,       Fill display with color");
    console->println("  rc [scroll],         Random colors");
    console->println("  scroll [lines],      Scroll screen");
    console->println("  hsv <s> <v>,         HSV color squares, s/h 0-255");
    console->println("  squares,             Colored squares");
    console->println("  scroll,              Color scroll");
    console->println("  demo,                Demo sequence");
    //console->println("  cmd <reg> <param>    Send command");
    console->printchar('\n');
}

char CmdTft::execute(int argc, char **argv){
    int32_t val1;

    if(argc == 1){
        help();
        return CMD_OK;
    }
#if 0 //TODO: Fix
    if(xstrcmp("cmd", (const char*)argv[1]) == 0){
        if(ha2u(argv[2], (uint32_t*)&val1)){
            uint32_t param;
            if(ha2u(argv[3], (uint32_t*)&param)){
                LCD_CS0;
                LCD_CD0;
                SPI_Transfer(BOARD_SPIDEV, (uint8_t*)&val1, 1);
                LCD_CD1;
                SPI_Transfer(BOARD_SPIDEV, (uint8_t*)&param, 1);
                LCD_CS1;
                return CMD_OK;
            }
        }
    }
#endif
    if(xstrcmp("init", (const char*)argv[1]) == 0){
        if(ia2i(argv[2], (int32_t*)&val1)){
            BOARD_LCD_Init();
		    LCD_FillRect(0, 0, LCD_GetWidth(), LCD_GetHeight(), LCD_BLACK);
		    LCD_SetOrientation((drvlcdorientation_t)(val1 & 3));
		    LCD_Bkl(1);
            return CMD_OK;
        }
    }

    if(xstrcmp("orientation", (const char*)argv[1]) == 0){
        if(ia2i(argv[2], (int32_t*)&val1)){
		    LCD_SetOrientation((drvlcdorientation_t)(val1 & 3));
            LCD_FillRect(0, 0, LCD_GetWidth(), LCD_GetHeight(), LCD_BLACK);
            LCD_WriteArea(0, 0, 8, 8, (uint16_t*)f_data);
            return CMD_OK;
        }
    }

    if(xstrcmp("clear", (const char*)argv[1]) == 0){
        if(ha2u(argv[2], (uint32_t*)&val1)){
            uint32_t ms = GetTick();
            LCD_FillRect(0, 0, LCD_GetWidth(), LCD_GetHeight(), val1);
            ms = GetTick() - ms;
            console->printf("Time: %dms\n", ms);
            return CMD_OK;
        }
    }

    if(xstrcmp("squares", (const char*)argv[1]) == 0){
        Tiles_Loop(&demo_ctx);
        return CMD_OK;
    }
#if ENABLE_DEMO_SCROLL
    if(xstrcmp("scroll", (const char*)argv[1]) == 0){
        if(ia2i(argv[2], (int32_t*)&val1)){
            LCD_Scroll(val1);
        }else{
            Scroll_Setup(&demo_ctx);
            char c = '\0';
            do{
                //console->printf("\r%d  ", scroll);
                Scroll_Loop(&demo_ctx);
                console->getchNonBlocking(&c);
                DelayMs(16);
            }while(c != '\n' && c != '\r');
        }
        return CMD_OK_LF;
    }
#endif
#if ENABLE_DEMO_RCOLOR
    if(xstrcmp("rc", (const char*)argv[1]) == 0){
        uint16_t f = 0;
        char c = '\0';

        if((const char*)argv[2] == NULL){
            RandomColors_Loop(&demo_ctx);
            return CMD_OK;
        }

        demo_ctx.seed = RNG_Get() % 256;

        do{
            Tiles_Loop(&demo_ctx);
            fps();

            if(f == 0){
                demo_ctx.scroll = (demo_ctx.scroll + 1) % LCD_GetHeight();
                LCD_Scroll(demo_ctx.scroll);
                f = 2; // scroll speed
            }

            f--;
            c = '\0';
            if(console->getchNonBlocking(&c)){
                demo_ctx.seed = RNG_Get() % 256;
            }
        }while(c != '\n' && c != '\r');

        return CMD_OK_LF;
    }
#endif
    if(xstrcmp("hsv", (const char*)argv[1]) == 0){
        uint8_t h = 0, s, v;
        uint16_t *buf = demo_ctx.buf;
        if(ia2i(argv[2], &val1)){
            s = val1;
            if(ia2i(argv[3], &val1)){
                v = val1;
                for(uint16_t i = 0; i < LCD_GetHeight()/8; i++){
                    for(uint16_t j = 0; j < LCD_GetWidth()/8; j++){
                        memset16(buf, HsvToRgb(h++, s, v), 64);
                        LCD_WriteArea(j * 8, i * 8, 7, 7, buf);
                        buf = demo_ctx.buf + (256 * (j & 1));
                    }
                }
                return CMD_OK;
            }
        }
    }

    if(xstrcmp("demo", (const char*)argv[1]) == 0){
        char c, limit_fps = 1;
        uint32_t frame_duration = 0, demo_time, stime;
        uint8_t demo = 0, demo_state = DEMO_SETUP;

        LCD_Scroll(0);

        do{
            switch(demo_state){
                case DEMO_SETUP:
                    demo_ctx.frames = 0;
                    demo_state = DEMO_RUN;
                    demos[demo].setup(&demo_ctx);
                    demo_time = 0;
                    stime = GetTick();
                    break;

                case DEMO_RUN:

                    demo_time += GetTick() - stime;
                    stime = GetTick();

                    demos[demo].loop(&demo_ctx);

                    frame_duration = GetTick() - stime;

                    if(demo_time > 3000){
                        uint32_t fps_avg = demo_ctx.frames / (demo_time / 1000);
                        console->printf("\rdemo[%d]: %d frames, avg fps: %d \n\r", demo, demo_ctx.frames, fps_avg);
                        demo_state = DEMO_END;
                        break;
                    }

                    fps();

                    break;

                case DEMO_END:
                    if(demos[demo].cleanup != NULL){
                        demos[demo].cleanup(&demo_ctx);
                    }
                    demo = (demo + 1) % (sizeof(demos) / sizeof(demoops_t));
                    demo_state = DEMO_SETUP;
                    break;

                default:
                    break;
            }

            if(limit_fps && frame_duration < 16){
                DelayMs(16 - frame_duration);
            }

            if(console->getchNonBlocking(&c)){
                limit_fps ^= 1;
            }

            WDT_Reset();

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

    fps++;

    if(GetTick() > expire){
        console->printf("\rfps %d ", fps);
        fps = 0;
        expire = GetTick() + 1000;
    }
}

static void Tiles_Setup(democtx_t *ctx)
{
    LCD_FillRect(0, 0, LCD_GetWidth(), LCD_GetHeight(), LCD_BLACK);
}
/**
 * @brief
 *
 * @param ctx
 * @return uint32_t
 */
static uint32_t Tiles_Loop(democtx_t *ctx)
{
    uint16_t *dbuf;
    for(uint8_t i = 0; i < LCD_GetHeight()/16; i++){
        for(uint8_t j = 0; j < LCD_GetWidth()/16; j++){
            dbuf = ctx->buf + (256 * (j & 1));
            ctx->value = RandomColor(ctx->value);
            memset(dbuf, ctx->value, 15 * 15 * 2);
            LCD_WriteArea(j * 16, i * 16, 15, 15, dbuf);
        }
    }
    return (++ctx->frames) < 50;
}

#if ENABLE_DEMO_AMIGA
// ST7735 library example
// Amiga Boing Ball Demo
// (c) 2019 Pawel A. Hernik
// YT video: https://youtu.be/KwtkfmglT-c

//https://github.com/cbm80amiga/Arduino_ST7735_Fast/tree/master/examples

#include "ball.h"

#define SCR_WD          LCD_GetWidth()
#define SCR_HT          LCD_GetHeight()

#define BALL_WD         64
#define BALL_HT         64

#define SHADOW          20
#define GRID_SPACING    10 // default value in pixel

static void drawBall(democtx_t *ctx)
{
    int i, j, ii;

    for (j = 0; j < BALL_HT; j++)
    {
        uint16_t *line = ctx->buf + (SCR_WD * ((ctx->bidx++) & 1)); // swap buffer

        uint8_t v, *img = (uint8_t *)ball + 16 * 2 + 6 + j * BALL_WD / 2 + BALL_WD / 2;

        int yy = ctx->y + j;
        if (!((yy - ctx->sy)%ctx->spacing))
        {
            for (i = 0; i < ctx->sx; i++)
                line[i] = line[SCR_WD - 1 - i] = ctx->bgColor; // erase ball outside grid

            for (i = 0; i <= SCR_WD - ctx->sx * 2; i++)
                line[i + ctx->sx] = ctx->gridLineColor;   // draw grid hline
        }
        else
        {
            for (i = 0; i < SCR_WD; i++)
                line[i] = ctx->bgColor; // erase ball on grid

            if (yy > ctx->sy)
                for (i = 0; i < ctx->nvlines; i++)
                    line[ctx->sx + i * ctx->spacing] = ctx->gridLineColor; // draw vline pixel
        }

        for (i = BALL_WD - 2; i >= 0; i -= 2)
        {
            v = *(--img);

            if (v >> 4)
            {
                line[ctx->x + i + 0] = ctx->palette[v >> 4];
#if SHADOW
                ii = ctx->x + i + 0 + SHADOW;
                if (ii < SCR_WD)
                {
                    if (line[ii] == ctx->bgColor)
                        line[ii] = ctx->ballShadowColor;
                    else if (line[ii] == ctx->gridLineColor)
                        line[ii] = ctx->gridLineShadowColor;
                }
#endif
            }

            if (v & 0xf)
            {
                line[ctx->x + i + 1] = ctx->palette[v & 0xf];
#if SHADOW
                ii = ctx->x + i + 1 + SHADOW;
                if (ii < SCR_WD)
                {
                    if (line[ii] == ctx->bgColor)
                        line[ii] = ctx->ballShadowColor;
                    else if (line[ii] == ctx->gridLineColor)
                        line[ii] = ctx->gridLineShadowColor;
                }
#endif
            }
        }

        LCD_WriteArea(0, yy, SCR_WD, 1, line);
    }
}

static void AmigaBall_Setup(democtx_t *ctx)
{
    uint16_t *pal = (uint16_t *)ball + 3;

    ctx->xd = 2;
    ctx->yd = 1;

    ctx->bgColor = RGB565(200,200,200);
    ctx->ballShadowColor = RGB565(90,90,90);
    ctx->gridLineColor = RGB565(150,40,150);
    ctx->gridLineShadowColor = RGB565(80,10,80);

    ctx->w = SCR_WD - (SCR_WD * 30 / 100); // 70% of screen width
    ctx->sx = (SCR_WD - ctx->w) / 2;  // centered
    ctx->h = SCR_HT - (SCR_HT * 25 / 100); // 75% of screen high
    ctx->sy = (SCR_HT - ctx->h) / 4;  // 1/4 on top

    ctx->spacing = GRID_SPACING;
    while (ctx->w % ctx->spacing){
        ctx->spacing++;
    }

    ctx->nvlines = (ctx->w / ctx->spacing) + 1;
    ctx->nhlines = (ctx->h / ctx->spacing) + 1;

    // Load palette
    for (uint8_t i = 0; i < 16; i++)
        ctx->palette[i] = *pal++;

    // Background color
    LCD_FillRect(0, 0, LCD_GetWidth(), LCD_GetHeight(), LCD_BLACK);
    LCD_FillRect(0, 0, SCR_WD, SCR_HT, ctx->bgColor);

    // Grid Vlines
    for (uint8_t i = 0; i < ctx->nvlines; i++){
        LCD_FillRect(ctx->sx + i * ctx->spacing, ctx->sy, 1, ctx->h, ctx->gridLineColor);
    }

    // Grid Hlines
    for (uint8_t i = 0; i < ctx->nhlines; i++){
        LCD_FillRect(ctx->sx, ctx->sy + i * ctx->spacing, ctx->w, 1, ctx->gridLineColor);
    }

    // Oblique continuation of vertical lines
    uint16_t ex = SCR_WD * 10 / (ctx->nvlines - 1); // keep a decimal place to reduce error
    for (uint8_t i = 0; i < ctx->nvlines; i++){
        LCD_Line(ctx->sx + i * ctx->spacing,
                 ctx->sy + ctx->h,
                 (i * ex) / 10,
                 SCR_HT - ctx->spacing,
                 ctx->gridLineColor
        );
    }
    // Decrementing horizontal lines for depth illusion
    // m = (y + b) / x
    // y = ctx->sy + ctx->h
    // x = ctx->sx
    // b = SCR_HT - GRID_SPACING
    uint16_t m = ((SCR_HT - ctx->spacing) - (ctx->sy + ctx->h)) / ctx->sx;

    for (uint16_t y = SCR_HT - ctx->spacing, i = ctx->spacing; y > ctx->sy + ctx->h; ){
        uint16_t x = ((SCR_HT - ctx->spacing) - y) / m;
        LCD_Line(x,
                 y,
                 SCR_WD - x,
                 y,
                 ctx->gridLineColor
        );
        i -= (i * 2) / 10;
        y -= i;
    }
}

static uint32_t AmigaBall_Loop(democtx_t *ctx)
{

    static int16_t anim = 0, animd = 1;

    for (int i = 0; i < 14; i++)
    {
        ctx->palette[i + 1] = ((i + anim) % 14) < 7 ? LCD_WHITE : LCD_RED;
        //int c=((i+anim)%14); // with ping between white and red
        //if(c<6) palette[i+1]=WHITE; else if(c==6 || c==13) palette[i+1]=RGB565(255,128,128); else palette[i+1]=RED;
    }

    drawBall(ctx);
    anim += animd;

    if (anim < 0){
        anim += 14;
    }

    ctx->x += ctx->xd;
    ctx->y += ctx->yd;

    if (ctx->x < 0)
    {
        ctx->x = 0;
        ctx->xd = -ctx->xd;
        animd = -animd;
    }

    if (ctx->x >= SCR_WD - BALL_WD)
    {
        ctx->x = SCR_WD - BALL_WD;
        ctx->xd = -ctx->xd;
        animd = -animd;
    }

    if (ctx->y < 0)
    {
        ctx->y = 0;
        ctx->yd = -ctx->yd;
    }

    if (ctx->y >= ctx->sy + ctx->h + 1 - BALL_HT)
    {
        ctx->y = ctx->sy + ctx->h + 1- BALL_HT;
        ctx->yd = -ctx->yd;
    }

    return ++ctx->frames;
}
#endif /* ENABLE_DEMO_AMIGA */

#if ENABLE_DEMO_SPIRAL
static void Spiral_Setup(democtx_t *ctx){
    LCD_FillRect(0, 0, LCD_GetWidth(), LCD_GetHeight(), LCD_BLACK);
    ctx->x = LCD_GetWidth() / 2;
    ctx->y = LCD_GetHeight() / 2;

    ctx->xd = ctx->x;
    ctx->yd = ctx->y;

    ctx->state = 0;
    ctx->step = 1;
    ctx->stepSize = 8;
    ctx->numSteps = 1;
    ctx->tcount = 0;

    ctx->color = RNG_Get();
}

static uint32_t Spiral_Loop(democtx_t *ctx){

    if(isPrime(ctx->step)){
        // Draw square
        LCD_FillRect(ctx->x - (ctx->stepSize >> 1),
                     ctx->y - (ctx->stepSize >> 1),
                     ctx->stepSize - 1, ctx->stepSize - 1, ctx->color);
    }

    uint16_t w, h, sx, sy;
    if(ctx->x > ctx->xd){
        w = ctx->x - ctx->xd;
        sx = ctx->xd;
    }else{
        w = ctx->xd - ctx->x;
        sx = ctx->x;
    }

    if(ctx->y > ctx->yd){
        h = ctx->y - ctx->yd;
        sy = ctx->yd;
    }else{
        h = ctx->yd - ctx->y;
        sy = ctx->y;
    }

    if(w == 0 && h > 0){ w = 1; }
    if(h == 0 && w > 0){ h = 1; }

    // Draw line connecting previous square to next one
    LCD_FillRect(sx, sy, w, h, ctx->color);

    ctx->xd = ctx->x;
    ctx->yd = ctx->y;

    switch (ctx->state){
        case 0:
            ctx->x += ctx->stepSize;
            break;
        case 1:
            ctx->y -= ctx->stepSize;
            break;
        case 2:
            ctx->x -= ctx->stepSize;
            break;
        case 3:
            ctx->y += ctx->stepSize;
            break;
    }

    if(ctx->step % ctx->numSteps == 0){
        ctx->state = (ctx->state + 1 ) % 4;
        ctx->tcount++;
        if(ctx->tcount % 2 == 0){
            ctx->numSteps++;
        }
    }

    ctx->step++;

    return ++ctx->frames;
}
#endif

#if ENABLE_DEMO_SCROLL
static void Scroll_Setup(democtx_t *ctx){
    ctx->value = 0;
    ctx->state = RNG_Get();
}

static void Scroll_Cleanup(democtx_t *ctx){
    LCD_Scroll(0);
}

static uint32_t Scroll_Loop(democtx_t *ctx){
    ctx->y = (LCD_GetHeight() - 1) - ctx->value;
    ctx->value = (ctx->value + 1) % LCD_GetHeight();

    LCD_FillRect(0,ctx->y, LCD_GetWidth(), 1,
                HsvToRgb(ctx->state++, 255, 255));

    LCD_Scroll(ctx->value);

    return ++ctx->frames;
}
#endif

#if ENABLE_DEMO_RCOLOR
static void RandomColors_Setup(democtx_t *ctx){ }

static uint32_t RandomColors_Loop(democtx_t *ctx)
{
    for(size_t i = 0; i < LCD_GetHeight(); i++){
        uint16_t *buf = ctx->buf + (LCD_GetWidth() * (i & 1));
        for (size_t j = 0; j < LCD_GetWidth(); j++){
            buf[j] = RNG_Get();
        }
        LCD_WriteArea(0, i, LCD_GetWidth(), 1, buf);
    }
    return ++ctx->frames;
}
#endif

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

static void Gif_Cleanup(void){
    gif.close();
}
#endif

