/* USER CODE BEGIN Header */
/**
	******************************************************************************
	* @file           : main.c
	* @brief          : Main program body
	******************************************************************************
	* @attention
	*
	* <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
	* All rights reserved.</center></h2>
	*
	* This software component is licensed by ST under BSD 3-Clause license,
	* the "License"; You may not use this file except in compliance with the
	* License. You may obtain a copy of the License at:
	*                        opensource.org/licenses/BSD-3-Clause
	*
	******************************************************************************
	*/
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "board.h"
#include "app.h"
#include "gpio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
spibus_t BOARD_SPIDEV_HANDLER;
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

void DelayMs(uint32_t ms) { HAL_Delay(ms); }
uint32_t GetTick(void){ return HAL_GetTick(); }


#ifdef ENABLE_TFT_DISPLAY
static drvlcdspi_t lcd0;

static void displayInit()
{
    //LPC_GPIO1->FIODIR |= LCD_CS|LCD_RS|LCD_WR|LCD_RD|LCD_LED|LCD_RST;
    
    lcd0.spidev.bus = SPI_BUS0;
    lcd0.spidev.flags = SPI_HW_CS;
    lcd0.w = 128;
    lcd0.h = 160;
    lcd0.cs = LCD_CS_PIN;
    lcd0.cd = LCD_CD_PIN;
    lcd0.bkl = LCD_BKL_PIN;
    lcd0.rst = LCD_RST_PIN;

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

void BOARD_LCD_Init(void)
{
    #ifdef ENABLE_TFT_DISPLAY
    displayInit();
    #endif
}

/**
	* @brief  The application entry point.
	* @retval int
	*/
int main(void)
{
	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();	
	RNG_Init();

    SERIAL_Init();

	#ifdef ENABLE_TFT_DISPLAY
    displayInit();
    #endif
	
	App();

	while (1)
	{
	}
}

/**
	* @brief System Clock Configuration
	* @retval None
	*/
#if 0
// 32MHz
void SystemClock_Config(void)
{
		RCC_OscInitTypeDef RCC_OscInitStruct = {0};
		RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
		RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

		/** Configure the main internal regulator output voltage 
	*/
		if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
		{
				Error_Handler();
		}
		/** Initializes the CPU, AHB and APB busses clocks 
	*/
		RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
		RCC_OscInitStruct.MSIState = RCC_MSI_ON;
		RCC_OscInitStruct.MSICalibrationValue = 0;
		RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_10;
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
		if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		{
				Error_Handler();
		}
		/** Initializes the CPU, AHB and APB busses clocks 
	*/
		RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
		RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
		RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
		{
				Error_Handler();
		}
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_USART2;
		PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
		PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
				Error_Handler();
		}
}
#else
//80 MHz
void SystemClock_Config(void)
{
		RCC_OscInitTypeDef RCC_OscInitStruct = {0};
		RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
		RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

		/** Configure the main internal regulator output voltage 
	*/
		if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
		{
				Error_Handler();
		}
		/** Initializes the CPU, AHB and APB busses clocks 
	*/
		RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
		RCC_OscInitStruct.MSIState = RCC_MSI_ON;
		RCC_OscInitStruct.MSICalibrationValue = 0;
		RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
		RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
		RCC_OscInitStruct.PLL.PLLM = 1;
		RCC_OscInitStruct.PLL.PLLN = 40;
		RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
		RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
		if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		{
				Error_Handler();
		}
		/** Initializes the CPU, AHB and APB busses clocks 
	*/
		RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
		RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
		{
				Error_Handler();
		}
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
		PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
				Error_Handler();
		}
}
#endif

/**
	* @brief USART1 Initialization Function
	* @param None
	* @retval None
	*/
static void MX_USART1_UART_Init(void)
{

		/* USER CODE BEGIN USART1_Init 0 */

		/* USER CODE END USART1_Init 0 */

		/* USER CODE BEGIN USART1_Init 1 */

		/* USER CODE END USART1_Init 1 */
		huart1.Instance = USART1;
		huart1.Init.BaudRate = 115200;
		huart1.Init.WordLength = UART_WORDLENGTH_8B;
		huart1.Init.StopBits = UART_STOPBITS_1;
		huart1.Init.Parity = UART_PARITY_NONE;
		huart1.Init.Mode = UART_MODE_TX_RX;
		huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart1.Init.OverSampling = UART_OVERSAMPLING_16;
		huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
		huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
		if (HAL_UART_Init(&huart1) != HAL_OK)
		{
				Error_Handler();
		}
		/* USER CODE BEGIN USART1_Init 2 */

		/* USER CODE END USART1_Init 2 */
}

/**
	* @brief USART2 Initialization Function
	* @param None
	* @retval None
	*/
static void MX_USART2_UART_Init(void)
{

		/* USER CODE BEGIN USART2_Init 0 */

		/* USER CODE END USART2_Init 0 */

		/* USER CODE BEGIN USART2_Init 1 */

		/* USER CODE END USART2_Init 1 */
		huart2.Instance = USART2;
		huart2.Init.BaudRate = 115200;
		huart2.Init.WordLength = UART_WORDLENGTH_8B;
		huart2.Init.StopBits = UART_STOPBITS_1;
		huart2.Init.Parity = UART_PARITY_NONE;
		huart2.Init.Mode = UART_MODE_TX_RX;
		huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart2.Init.OverSampling = UART_OVERSAMPLING_16;
		huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
		huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
		if (HAL_UART_Init(&huart2) != HAL_OK)
		{
				Error_Handler();
		}
		/* USER CODE BEGIN USART2_Init 2 */

		/* USER CODE END USART2_Init 2 */
}

/** 
	* Enable DMA controller clock
	*/
static void MX_DMA_Init(void)
{
		/* DMA controller clock enable */
		__HAL_RCC_DMA1_CLK_ENABLE();
		__HAL_RCC_DMA2_CLK_ENABLE();
}

/**
	* @brief GPIO Initialization Function
	* @param None
	* @retval None
	*/
static void MX_GPIO_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};

		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();

		/*Configure GPIO pin Output Level */
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

		/*Configure GPIO pin : LD3_Pin */
		GPIO_InitStruct.Pin = LD3_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);
}

void SW_Reset(void)
{
    NVIC_SystemReset();
}

/**
	* @brief  This function is executed in case of error occurrence.
	* @retval None
	*/
void Error_Handler(void)
{
		__disable_irq();
		while (1)
		{
		}
}

void __debugbreak(void){
	__asm volatile(
        "bkpt #01 \n"
        "b . \n"
    );
}

void Fault_Handler(void)
{
    volatile uint8_t isr_number = (SCB->ICSR & 255) - 16;
    // See position number on Table 46 from RM0394
    UNUSED(isr_number);

    __asm volatile(
        "bkpt #01 \n"
        "b . \n"
    );
}

typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
}stackframe_t;

void Stack_Dump(stackframe_t *stack){
    //GPIOJ->MODER = (1 << 26);
    //HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET);

    __asm volatile(
        "bkpt #01 \n"
        "b . \n"
    );
}

#ifdef USE_FULL_ASSERT
/**
	* @brief  Reports the name of the source file and the source line number
	*         where the assert_param error has occurred.
	* @param  file: pointer to the source file name
	* @param  line: assert_param error line source number
	* @retval None
	*/
void assert_failed(char *file, uint32_t line)
{
		/* USER CODE BEGIN 6 */
		/* User can add his own implementation to report the file name and line number,
		 tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
		/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
