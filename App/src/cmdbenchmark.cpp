#include "board.h"
#include "cmdbenchmark.h"
#include "lib2d.h"
#include "drvlcd.h"
#include "app.h"

#if defined(ENABLE_BENCHMARK_DHRYSTONE)
#include "dhry.h"
#endif

#define PI_ITERATIONS   500000L
#define UPDATE_DISPLAY  7500
#define PBAR_WIDTH      80
#define PBAR_HEIGHT     20
#define PBAR_POSY       20

#if defined(ENABLE_TFT_DISPLAY)
static void setupDisplay(void){
    BOARD_LCD_Init();
    LIB2D_Init();
    LIB2D_FillRect(0, 0, LCD_GetWidth(), LCD_GetHeight());
    LIB2D_String("PI Benchmark");
    uint16_t x = (LCD_GetWidth() - PBAR_WIDTH) / 2;
    LIB2D_SetFcolor(LCD_FBBLUE);
    LIB2D_Rect(x, PBAR_POSY, PBAR_WIDTH, PBAR_HEIGHT);
    LCD_Bkl(1);
}

static void updateDisplay(uint32_t *progress){
    static uint32_t update_counter = 0;

    update_counter++;

    if (update_counter > UPDATE_DISPLAY) {
        update_counter = 0;
        progress[0]++;
        uint32_t percent = progress[0] * 100 / (PI_ITERATIONS / UPDATE_DISPLAY);
        uint16_t x = (LCD_GetWidth() - PBAR_WIDTH) / 2;
        LCD_FillRect(x + 3, PBAR_POSY + 2, map(percent, 0, 100, 5, PBAR_WIDTH - 4), PBAR_HEIGHT - 3, LCD_RED);
    }
}
#endif

static uint32_t piBenchmark(uint32_t iterations, float *calc_pi){
    uint32_t start, time;
    float x = 1.0f;
    float pi = 1.0f;
    #if defined(ENABLE_TFT_DISPLAY)
    uint32_t progress = 1;
    #endif

    start = GetTick();  
    for (uint32_t i = 2; i < iterations; i++) {
        x *= -1.0;
        pi += x / (2.0f * (float)i - 1.0f);
        #if defined(ENABLE_TFT_DISPLAY)
        updateDisplay(&progress);
        #endif
    }
    time = GetTick() - start;
 
    *calc_pi = pi * 4.0;

    return time;   
}

void CmdBenchmark::help(void){
    console->println("Usage: benchmark <option>\n");
    console->print("\t pi   - PI benchmark\n");
    #if defined(ENABLE_BENCHMARK_DHRYSTONE)
    console->print("\t dhry - Dhrystone benchmark\n");
    #endif
}

char CmdBenchmark::execute(int argc, char **argv){

    if(argc < 2){
        help();
        return CMD_BAD_PARAM;
    }
    
#if defined(ENABLE_BENCHMARK_DHRYSTONE)
    if(xstrcmp("dhry", (const char*)argv[1]) == 0){
        float res = dhry(1000000) / 1757.0f;
        if(res){
            console->printf("%.2f DMIPS/MHz\n",  res / (SystemCoreClock / 1000000));
        }
        return CMD_OK;
    }
#endif

    if(xstrcmp("pi", (const char*)argv[1]) == 0){
        console->print("PI Benchmark\n");
        #if defined(ENABLE_TFT_DISPLAY)
        setupDisplay();
        #endif

        float pi;
        uint32_t time = piBenchmark(PI_ITERATIONS, &pi);
        //double pi_d = pi;
        console->printf("Pi calculated: %f\n", pi);
        console->printf("Iterations: %u\n", PI_ITERATIONS);
        console->printf("Time: %ums\n", time);

        #if defined(ENABLE_TFT_DISPLAY)
        LIB2D_SetPos(0, PBAR_POSY + 30);
        LIB2D_Printf("PI: %f\n", pi);
        LIB2D_Printf("Iter: %u\n", PI_ITERATIONS);
        LIB2D_Printf("Time: %ums\n", time);
        #endif
    }

    return CMD_OK;
}