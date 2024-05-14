/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include<stdbool.h>
#include<string.h>
#include<stdlib.h>
#include <inttypes.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define EEPROM_ADDRESS 0xA0  //base address for the eeprom
#define EEPROM_PAGESIZE 16	//16 bytes per page
#define EEPROM_PAGES 8		//8 pages
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 FMPI2C_HandleTypeDef hfmpi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t animcounter=0;
uint8_t eepromanimcounter=0;
uint8_t eepromcurrentanim[16]={0};
uint8_t greenvalue=0;
uint8_t bluevalue=0;
bool currentInputUB=true;
bool currentInputLB=true;
bool interruptaccepted=false;
bool interruptrecieved=false;
bool greencanchange=true;
bool bluecanchange=true;
bool uartrecieved=false;
uint8_t inputcheckcounter=0;
uint8_t dataRead[128]={0};
uint8_t dataWrite[128];
bool eepromanimON=false;
char lastrecievedbyte=0;
uint8_t animindex=8;
uint8_t rxBuffer[200]={0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_FMPI2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */
static void setPWM(TIM_HandleTypeDef, uint32_t, uint16_t, uint16_t);
static void HandleUart();
void GetEepromAnim(uint8_t);
void blackout();
void blue();
void green();
void moving();
void animation();
void eepromAnimSetup(uint8_t);
void clearEeprom(uint8_t);
void setEeprom(uint8_t);

int round_closest(int, int);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//PWM beállítása a paraméterben megkapott timer channel részére
static void setPWM(TIM_HandleTypeDef timer,uint32_t channel,uint16_t period ,uint16_t pulse)
{
	HAL_TIM_PWM_Stop(&timer, channel);
	TIM_OC_InitTypeDef sConfigOC;
	/*timer.Init.Period = period;
	HAL_TIM_PWM_Init(&timer);*/

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = pulse;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&timer, &sConfigOC, channel) != HAL_OK)
	  {
	    Error_Handler();
	  }
	HAL_TIM_PWM_Start(&timer, channel);

}

//uarton érkező adat kezelése
static void HandleUart(){
	uint16_t sum=0;
	uint16_t controlamount;
	if(uartrecieved){									//abban az esetben ha érkezett adatt
		if(lastrecievedbyte!=0){						//és a beérkezett adat első eleme nem 0
			controlamount=rxBuffer[17]<<8;				//ellenőrző összeg visszaalakítása
			controlamount=controlamount|rxBuffer[18];
			switch(lastrecievedbyte){
			case '@':									//új animáció hozzáadásához tartozó kód
					for(int i=1;i<17;i++){
						sum+=rxBuffer[i];
					}
					if(sum==controlamount){				//ha megegyezik az ellenőrző összeg akkor csinálunk bármit is.
						animindex=rxBuffer[16];
						setEeprom(animindex);
						char letter[]={"OK"};
						HAL_UART_Transmit(&huart2, (uint8_t*)letter, 2, 100);
					}
					else{
						char letter[]={"ERROR"};			//ha nem így van akkor visszaküldjük a PC-nek hogy hiba van
						HAL_UART_Transmit(&huart2, (uint8_t*)letter, 5, 100);
					}
					break;
				case 'c':								//animáció törléséhez tartozó kód
					for(int i=1;i<17;i++){
						sum+=rxBuffer[i];
					}
					sum+='c';
					animindex=rxBuffer[16];
					if((animindex!=8)&&(sum==controlamount)){		//ha megegyezik az ellenőrző összeg akkor csinálunk bármit is.
						clearEeprom(animindex);
						char letter[]={"OK"};
						HAL_UART_Transmit(&huart2, (uint8_t*)letter, 2, 100);
					}
					else{											//ha nem így van akkor visszaküldjük a PC-nek hogy hiba van
						char letter[]={"ERROR\n\r"};
						HAL_UART_Transmit(&huart2, (uint8_t*)letter, 5, 100);
					}
					break;
				}
				lastrecievedbyte=0;
			}
			for(int i=0;i<20;i++){		//buffer kinullázása, habár nem szükséges
				rxBuffer[i]=0;
			}
			uartrecieved=false;
			HAL_UART_Receive_IT(&huart2, (uint8_t*)rxBuffer, 19);		//új parancsra várakozás
			}
}
//következő animáció kiválasztása, megkeresi a következő animációt az eepromban
void GetEepromAnim(uint8_t animcounter){
	uint8_t pos=animcounter*EEPROM_PAGESIZE;
	HAL_FMPI2C_Mem_Read(&hfmpi2c1, EEPROM_ADDRESS, pos, 1, eepromcurrentanim, EEPROM_PAGESIZE, 100);
	int counter=0;
	while(eepromcurrentanim[0]==0 && counter<8){// abban az esetben ha a kiválasztott oldalon nem lenne animáció akkor megkeressük a következőt ahol van
		counter++;
		eepromanimcounter++;
		if(eepromanimcounter==8)
			eepromanimcounter=0;
		pos=eepromanimcounter*EEPROM_PAGESIZE;
		HAL_FMPI2C_Mem_Read(&hfmpi2c1, EEPROM_ADDRESS, pos, 1, eepromcurrentanim, EEPROM_PAGESIZE, 100);
		if(eepromcurrentanim[0]!=0){ //ha megtaláltuk kilépünk

			break;
		}

	}
	return;
}
int round_closest(int divident, int divider){
	return ((divident+(divider/2))/divider);
}
//kitörli a paraméterként megkapott eeprom oldalt
void clearEeprom(uint8_t index){
	uint8_t tmp[16]={0};
	if(index!=7){
		for(int i=index;i<7;i++){				//ha nem az utolsó oldalt töröljük, akkor az azt követőeket egyel előrébb tesszük
			int pos=EEPROM_PAGESIZE*i;
			HAL_FMPI2C_Mem_Read(&hfmpi2c1, EEPROM_ADDRESS, pos+EEPROM_PAGESIZE, 1, tmp,EEPROM_PAGESIZE, 1000);
			HAL_FMPI2C_Mem_Write(&hfmpi2c1, EEPROM_ADDRESS, pos, 1, tmp,EEPROM_PAGESIZE, 1000);
			HAL_Delay(5);
		}
	}
	else{
		int pos=EEPROM_PAGESIZE*index;
		HAL_FMPI2C_Mem_Write(&hfmpi2c1, EEPROM_ADDRESS, pos, 1, tmp,EEPROM_PAGESIZE, 1000);
		HAL_Delay(5);
	}
	eepromanimON=false;
	eepromanimcounter=0;
	animcounter=1;
}
//a paraméterként megkapott oldalt betölti az uarton kapott adatokkal
void setEeprom(uint8_t index){
	int pos=EEPROM_PAGESIZE*index;
	uint8_t tmp[16]={0};
	for(int i=0;i<EEPROM_PAGESIZE;i++){
			tmp[i]=rxBuffer[i+1];
	}
	HAL_FMPI2C_Mem_Write(&hfmpi2c1, EEPROM_ADDRESS, pos, 1, tmp, EEPROM_PAGESIZE, 1000);
	HAL_Delay(5);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_FMPI2C1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_FMPI2C_Mem_Read(&hfmpi2c1, EEPROM_ADDRESS, 0, 1, dataRead, 128, 1000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  HandleUart();
	  animation();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief FMPI2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FMPI2C1_Init(void)
{

  /* USER CODE BEGIN FMPI2C1_Init 0 */

  /* USER CODE END FMPI2C1_Init 0 */

  /* USER CODE BEGIN FMPI2C1_Init 1 */

  /* USER CODE END FMPI2C1_Init 1 */
  hfmpi2c1.Instance = FMPI2C1;
  hfmpi2c1.Init.Timing = 0x00A0A3F7;
  hfmpi2c1.Init.OwnAddress1 = 0;
  hfmpi2c1.Init.AddressingMode = FMPI2C_ADDRESSINGMODE_7BIT;
  hfmpi2c1.Init.DualAddressMode = FMPI2C_DUALADDRESS_DISABLE;
  hfmpi2c1.Init.OwnAddress2 = 0;
  hfmpi2c1.Init.OwnAddress2Masks = FMPI2C_OA2_NOMASK;
  hfmpi2c1.Init.GeneralCallMode = FMPI2C_GENERALCALL_DISABLE;
  hfmpi2c1.Init.NoStretchMode = FMPI2C_NOSTRETCH_DISABLE;
  if (HAL_FMPI2C_Init(&hfmpi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_FMPI2CEx_ConfigAnalogFilter(&hfmpi2c1, FMPI2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FMPI2C1_Init 2 */

  /* USER CODE END FMPI2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 255;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 420-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 2-1;		////1 ms-os periódusidő beállítása az egyszerű beállítás miatt.
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 42000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */
  HAL_NVIC_SetPriority(TIM3_IRQn, 1, 0);
  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */

static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 2-1;					//1 ms-os periódusidő beállítása az egyszerű beállítás miatt.
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 42000-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */
HAL_NVIC_SetPriority(TIM4_IRQn, 1, 1);
  /* USER CODE END TIM4_Init 2 */

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
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
  HAL_UART_Receive_IT(&huart2, (uint8_t*)rxBuffer, 19);			//várunk az első küldött uart üzenetre
  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pins : B1_Pin Buttonupper_Pin Buttonlower_Pin */
  GPIO_InitStruct.Pin = B1_Pin|Buttonupper_Pin|Buttonlower_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

}

/* USER CODE BEGIN 4 */

//Amikor az uarton keresztül beolvastunk 19 bájtot akkor fut le
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *handle)
{
		lastrecievedbyte=rxBuffer[0];	//a kapott adatok közül ez lesz a parncsunk
		uartrecieved=true;
}
//Ha megnyomjuk valamelyik gombot akkor fut le
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN){
	if(!interruptrecieved)
	{
		interruptrecieved=true;
		if(GPIO_PIN == Buttonupper_Pin){	// ha  a megfelelő gpio pinről érkezett megszakítás akkor elindítjuk a 2-es timert
			HAL_TIM_Base_Start_IT(&htim2);
		}
		else if(GPIO_PIN == Buttonlower_Pin){
			HAL_TIM_Base_Start_IT(&htim2);
		}
	}

}
//bármely timer megszakításkezelője
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if(htim==&htim2){			//pergésmentesítés megoldása és animációk közötti váltás
	inputcheckcounter++;
	currentInputUB=HAL_GPIO_ReadPin(GPIOC, Buttonupper_Pin);
	currentInputLB=HAL_GPIO_ReadPin(GPIOC, Buttonlower_Pin);
	if(inputcheckcounter>=10&&(currentInputUB==false)){		//megvizsgáljuk hogy 10*5 ms-el később még mindig le van e nyomva a gomb és ha igen akkor változtatunk
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		interruptaccepted=true;
		if(eepromanimON==true){
		eepromanimON=false;
		animcounter=0;
		blackout();
		}
		else
			animcounter++;
		inputcheckcounter=0;
		eepromanimcounter=0;
				if(animcounter==4){
				    animcounter=0;
				    blackout();
				    }
		HAL_TIM_Base_Stop_IT(&htim2);
	}
	else if(inputcheckcounter>=10&&(currentInputLB==false)){
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		interruptaccepted=true;
		inputcheckcounter=0;
		if(eepromanimON==false){
			eepromanimON=true;
			eepromanimcounter=0;
		}
		else{
			eepromanimcounter++;
		}
		animcounter=0;
			if(eepromanimcounter==8){
				eepromanimcounter=0;
				blackout();
			}
		HAL_TIM_Base_Stop_IT(&htim2);
	}
	else if(inputcheckcounter>30){
		inputcheckcounter=0;
		interruptrecieved=false;
		HAL_TIM_Base_Stop_IT(&htim2);
	}
	}
	if(htim==&htim3){									//zöld led PWM beállítása
		 setPWM(htim1,TIM_CHANNEL_1 ,255,greenvalue);
		 greencanchange=true;
	}
	if(htim==&htim4){									//kék led PWM beállítása
		setPWM(htim1, TIM_CHANNEL_3, 255, bluevalue);
		bluecanchange=true;
	}

}
void blackout(){			//mindegyik LED kikapcsolása
	setPWM(htim1, TIM_CHANNEL_1, 255, 0);
	setPWM(htim1, TIM_CHANNEL_2, 255, 0);
	setPWM(htim1, TIM_CHANNEL_3, 255, 0);
	return;
}
void blue(){				//alap kék fény
	blackout();
	setPWM(htim1, TIM_CHANNEL_3, 255, 255);
	setPWM(htim1, TIM_CHANNEL_1, 255, 0);
	setPWM(htim1, TIM_CHANNEL_2, 255, 0);
	return;
}
void green(){				//alap zöld fény
	blackout();
	setPWM(htim1, TIM_CHANNEL_1, 255, 255);
	setPWM(htim1, TIM_CHANNEL_2, 255, 0);
	setPWM(htim1, TIM_CHANNEL_3, 255, 0);
	return;
}
void moving(){				//kék és zöld folyamatos váltogatása
	int g=0,b=255,dirg,dirb;
	while(animcounter==3&&!uartrecieved){ //egyszerű transition effekt, amíg nem kapunk uart üzenetet vagy nem változtatjuk meg az animációt
		if(g==255)
		  	dirg=-1;
		 if(g==0)
		  	dirg=1;
		 setPWM(htim1,TIM_CHANNEL_1 ,255,g);
		 g+=dirg;
		 if(b==255)
		  	dirb=-1;
		 if(b==0)
		  	dirb=1;
		 setPWM(htim1,TIM_CHANNEL_3 ,255,b);
		  	b+=dirb;
		HAL_Delay(10);
		  }
	return;

}

// animáció kiválasztása
void animation(){
	if(interruptaccepted)
	{
		interruptaccepted=false;
		interruptrecieved=false;
	}
	if(!eepromanimON){			//ha nem az eepromról akarunk játszani animációt
	switch(animcounter){
	//case 0:blackout(); return;
	case 1:blue(); return;
	case 2:green(); return;
	case 3:moving();return;
	}
	}
	else{
		GetEepromAnim(eepromanimcounter);	//megkeressük a következő animációt
		switch(eepromanimcounter){
			case 0:eepromAnimSetup(eepromanimcounter);return;
			case 1:eepromAnimSetup(eepromanimcounter);return;
			case 2:eepromAnimSetup(eepromanimcounter);return;
			case 3:eepromAnimSetup(eepromanimcounter);return;
			case 4:eepromAnimSetup(eepromanimcounter);return;
			case 5:eepromAnimSetup(eepromanimcounter);return;
			case 6:eepromAnimSetup(eepromanimcounter);return;
			case 7:eepromAnimSetup(eepromanimcounter);return;
			}
	}
	return;
}
//Az épp kiválasztott eeprom animációhoz tartozó beállítások
void eepromAnimSetup(uint8_t eanimcounter){
	if(eepromcurrentanim[0]==0){		//Ha az épp kiválasztott animációban nincsenek értelmes adatok amit az első bájt jelez akkor kikapcsoljuk a ledeket és kikapcsoljuk az eeprom animációkat.
		eepromanimON=false;
		blackout();
		return;
	}
	int dirg=1,dirb=1;				//beállítások kiolvasása az eepromcurrentanim-ból
	int dirgstart=dirg;
	int dirbstart=dirb;
	uint8_t greenstart=eepromcurrentanim[4],greenstop=eepromcurrentanim[5],greendirmode=eepromcurrentanim[6];
	uint8_t bluestart=eepromcurrentanim[7],bluestop=eepromcurrentanim[8],bluedirmode=eepromcurrentanim[9];
	greenvalue=greenstart;
	bluevalue=bluestart;
	int greenanimtype=eepromcurrentanim[13];
	int blueanimtype=eepromcurrentanim[14];
	int animationlength=eepromcurrentanim[10]*1000+eepromcurrentanim[11]*10;
	int greendelaytime;
	int bluedelaytime;
	if(!greenanimtype){		//zöld led timer megszakítások periódusidejének számítása.
		if(eepromcurrentanim[6])
			greendelaytime=animationlength/(2*abs(eepromcurrentanim[4]-eepromcurrentanim[5]));
		else
			greendelaytime=animationlength/abs(eepromcurrentanim[4]-eepromcurrentanim[5]);
	}
	else{
		greendelaytime=animationlength/greenanimtype;
	}

	if(!blueanimtype){		//kék led timer megszakítások periódusidejének számítása.
		if(eepromcurrentanim[9])
			bluedelaytime=animationlength/(2*abs(eepromcurrentanim[7]-eepromcurrentanim[8]));
		else
			bluedelaytime=animationlength/abs(eepromcurrentanim[7]-eepromcurrentanim[8]);
	}
	else{
		bluedelaytime=animationlength/blueanimtype;
	}
	htim3.Init.Prescaler = (2*greendelaytime);		//timer osztások beállítása

	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	  {
	    Error_Handler();
	  }
	htim4.Init.Prescaler=(2*bluedelaytime);

	if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
		  {
		    Error_Handler();
		  }
	if(eepromcurrentanim[4]>eepromcurrentanim[5])
		dirgstart=-1;
	if(eepromcurrentanim[7]>eepromcurrentanim[8])
		dirbstart=-1;
	setPWM(htim1,TIM_CHANNEL_1 ,255,greenstart);		//kezdőállapotok beállítása
	setPWM(htim1,TIM_CHANNEL_3 ,255,bluestart);
	dirg=dirgstart;
	dirb=dirbstart;
	HAL_TIM_Base_Start_IT(&htim3);				//Timer IT-k indítása
	HAL_TIM_Base_Start_IT(&htim4);
	while(((eepromanimcounter==eanimcounter)&&eepromanimON)&&!uartrecieved){			//Ha nem kaptunk üzenetet épp vagy nem választottunk ki másik animációt
		if(greencanchange==true){				// ha engedélyezve van, akkor változtathatunk a led beállításokon, hogy ne ugorjunk át állapotot két timer megszakítás között										//akkor egy végtelen ciklusban folyamatosan frissítjük a ledeket
			if(!greenanimtype){
			greenvalue+=dirg;
			if(greenvalue==greenstop){			// ha elérjük valamelyik végállapotot akkor váltunk annak megfelelően hogy milyen az animáció típusa(négyszög vagy háromszögjel)
				if(!greendirmode)
					greenvalue=greenstart;
				else
					dirg=-1*dirg;
			}
			if(greenvalue==greenstart){
				dirg=dirgstart;
			}
			}
			else{
				if(greenvalue==greenstart)
					greenvalue=greenstop;
				else
					greenvalue=greenstart;
			}
			greencanchange=false;
		}
		if(bluecanchange==true){			// ha engedélyezve van, akkor változtathatunk a led beállításokon, hogy ne ugorjunk át állapotot két timer megszakítás között
			if(!blueanimtype){
				bluevalue+=dirb;
				if(bluevalue==bluestop){
					if(!bluedirmode)
						bluevalue=bluestart;
					else
						dirb=-1*dirb;
				}
				if(bluevalue==bluestart){
					dirb=dirbstart;
				}
			}
			else{
				if(bluevalue==bluestart){
					bluevalue=bluestop;
				}
				else
					bluevalue=bluestart;
			}
					bluecanchange=false;
		}
	}
	HAL_TIM_Base_Stop_IT(&htim3);			//Timer megszakítások kikapcsolása
	HAL_TIM_Base_Stop_IT(&htim4);
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
