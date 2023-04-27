#include "cmdencoder.h"
#include "board.h"
#include "app.h"
#include "liblcd.h"

#define ENC_TIM             TIM2
#define ENC_MIN_POS         1000
#define ENC_MAX_POS         2000

#define BAR_GRAPH_WIDTH     150
#define BAR_GRAPH_HEIGHT    12
#define BAR_GRAPH_BCOLOR    LCD_BLACK
#define BAR_GRAPH_FCOLOR    LCD_ORANGE
#define BAR_GRAPH_OUTLINE   LCD_WHITE

static uint16_t enc_count = 0;
static uint16_t enc_pos = 0;

uint16_t getEncoder(void){
  
    int16_t diff = ENC_TIM->CNT - enc_count;

    if(diff != 0){
        uint16_t tmp = enc_pos;

        tmp -= diff * 10; // speed

        if(tmp > ENC_MAX_POS){
            tmp = ENC_MAX_POS;
        }else if(tmp < ENC_MIN_POS){
            tmp = ENC_MIN_POS;
        }

        enc_pos = tmp;

        enc_count += diff;
    }

    return enc_pos;
}

void drawEncoder(int16_t val){
    uint16_t x = (LCD_GetWidth()/2) - (BAR_GRAPH_WIDTH/2);
    uint16_t y = (LCD_GetHeight()/2) - (BAR_GRAPH_HEIGHT/2);

    LCD_FillRect(
        x,
        y,
        val,
        BAR_GRAPH_HEIGHT,
        BAR_GRAPH_FCOLOR);

    LCD_FillRect(
        x + val,
        y,
        BAR_GRAPH_WIDTH - val,
        BAR_GRAPH_HEIGHT,
        BAR_GRAPH_BCOLOR);
}

void CmdEncoder::help(void){
    console->print("Usage: encoder <on/off> \n");
    console->print("Reads encoder connected to pins PB3/PA15\n"); 
}

char CmdEncoder::execute(int argc, char **argv){    

    if(argc < 1){
        help();
        return CMD_OK;
    }

    if(xstrcmp("on", (const char*)argv[1]) == 0){
        /* Configure Timer 2 */
        RCC->APB1ENR  |= RCC_APB1ENR_TIM2EN;    // Enable Timer 2
        RCC->APB1RSTR |= RCC_APB1ENR_TIM2EN;    // Reset timer registers
        RCC->APB1RSTR &= ~RCC_APB1ENR_TIM2EN;

        AFIO->MAPR = (AFIO->MAPR & ~(3 << 8)) | (1 << 8);       // Partial remap for TIM2; PA15 -> CH1, PB3 -> CH2 

        ENC_TIM->CR2 = 0;
        ENC_TIM->SMCR = TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0;        // External clock, Encoder mode 3
        ENC_TIM->CCMR1 = (15 << 12) | (15 << 4)                 // Map TIxFP1 to TIx,
                  | TIM_CCMR1_CC2S_0 | TIM_CCMR1_CC1S_0         // and max length if input filter
                  | TIM_CCMR1_IC2PSC_1 | TIM_CCMR1_IC1PSC_1;
        
        ENC_TIM->CCER = 0;                                      // Falling polarity
        ENC_TIM->CR1 = TIM_CR1_CEN;
        ENC_TIM->SR = 0;

        BOARD_GPIO_Init(GPIOB, 3, PIN_IN_PU);
        BOARD_GPIO_Init(GPIOA, 15, PIN_IN_PU);

        BOARD_GPIO_Init(BOARD_SPI_PORT, BOARD_SPI_DI_PIN, PIN_OUT_2MHZ);
        LCD_Init(TFT_SPIDEV);
        LCD_SetOrientation(LCD_LANDSCAPE);
        LCD_FillRect(0, 0, LCD_GetWidth(), LCD_GetHeight(), LCD_BLACK);

        LCD_DrawRect(
            (LCD_GetWidth()/2) - (BAR_GRAPH_WIDTH/2) - 2,
            (LCD_GetHeight()/2) - (BAR_GRAPH_HEIGHT/2) - 2,
            BAR_GRAPH_WIDTH + 3,
            BAR_GRAPH_HEIGHT + 3,
            BAR_GRAPH_OUTLINE
        );

        
        LCD_Bkl(1);
        
        ENC_TIM->CNT = enc_pos = enc_count = ENC_MIN_POS + ((ENC_MAX_POS - ENC_MIN_POS) / 3);
        
        char c = 0;
        do{
            uint16_t raw = getEncoder();
            uint16_t val = map(raw, ENC_MIN_POS, ENC_MAX_POS, 0, BAR_GRAPH_WIDTH);
            console->print("\r%d %d ", raw, val);
            drawEncoder(val);
            DelayMs(20);
            console->getCharNonBlocking(&c);
        }while(c != '\n');
    }

    return CMD_OK_LF;
}