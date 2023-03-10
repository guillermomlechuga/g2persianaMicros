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
#include "main.h"
#include "main.h"
#include<stdlib.h>
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
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
volatile int boton_modo=0, p_arriba=0,p_abajo=1,subiendo=0,bajando=0; //Empieza abajo y en el modo manual.
__IO uint16_t  LDR=0; //Este será el valor al que igualaremos el obtenido por el sensor de luminosidad.
__IO uint16_t  TEM=0; //Este será el valor al que igualaremos el obtenido por el sensor de temperatura.
uint8_t contador=0;
volatile int button_int=0; //DEBOUNCER
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	if (GPIO_Pin==GPIO_PIN_0)  //Boton de usuario
	{
		button_int=1;//DEBOUNCER
		if(contador==0)
		{
			boton_modo=1;  //SI CAMBIA A AUTOMATICO PERO LUEGO NO VUELVE A MANUAL HAY QUE HACER UN IF Y PONERLO A CERO CUANDO ESTA A 1
			contador=1;
		}
		else if (contador==1){
			boton_modo=2;
			contador=2;
		}

		else{
			boton_modo=0;
			contador=0;
		}

		}

	if (GPIO_Pin==GPIO_PIN_1)  //Boton subida

	{

	if(p_abajo==1) // Si esta la persiana abajo se sube.
		subiendo=1;
	}

	if (GPIO_Pin==GPIO_PIN_3)

	{
		if(p_arriba==1) // Si esta la persiana arriba se baja.
			bajando=1;
	}
	//IGUAL DA PROBLEMAS Y HACE FALTA PONER A 0 SUBIENDO Y BAJANDO CUANDO SE REALIZA LA ACCION CONTRARIA.
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
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
  int counter=HAL_GetTick();//DEBOUNCER
  	uint8_t button_count=0;//DEBOUNCER
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //DEBOUNCER
		if (button_int==1){
				if (HAL_GetTick()-counter>=20){
					counter=HAL_GetTick();
					if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)!=1){
						button_count=0;
					}
					else{
						button_count++;
					}
					if (button_count==3){ //Periodo antirebotes
						button_count=0;
						button_int=0;
					}
				}
			}
		//FINAL DEBOUNCER





	  //SUBIMOS PERSIANA
	  while((subiendo==1)&&(p_arriba==0)){ //Subimos persiana
		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,0);
		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,0);
		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,1);
		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,0);


		  for (int i=0;i<=1200;i=i+125){
		  		  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,i);
		  		  HAL_Delay(200);
		  	      }

		//PERSIANA ARRIBA
		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,0);
		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,1);
		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,0);
		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,0);

		  bajando=0;
		  p_arriba=1;
		  subiendo=0;
		  p_abajo=0;
	   }

	  while((bajando==1)&&(p_abajo==0)){	//Bajamos persiana

		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,1); //Bajando
		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,0);
		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,0);
		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,0);


	      for (int i=1200;i>=0;i=i-125){
	      		  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,i);
	      		  HAL_Delay(200);}

		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,0);
		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,0);
		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,0);
		  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,1); //P_abajo

		  bajando=0;
		  p_arriba=0;
		  subiendo=0;
		  p_abajo=1;
	  }
//	MODO CONTROL POR LUZ
	  HAL_ADC_Start(&hadc1);


	  	if (HAL_ADC_PollForConversion(&hadc1,1000000)==HAL_OK){
	  	LDR=HAL_ADC_GetValue(&hadc1);
	  	}

    while(boton_modo==1){

    	if(LDR<50 && p_abajo==1 ){	//Si hay poca luz subimos la persiana
    		 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,0);
    		 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,0);
   		     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,1);
    		 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,0);


    	for (int i=0;i<=1200;i=i+125){
    			  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,i);
    			  HAL_Delay(200);
    		      }
    	     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,0);
    	     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,1);
    	   	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,0);
    	     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,0);

    	     bajando=0;
     		 p_arriba=1;
    	     subiendo=0;
    	     p_abajo=0;
    	}

    	     HAL_ADC_Start(&hadc1);


     if (HAL_ADC_PollForConversion(&hadc1,1000000)==HAL_OK){
     LDR=HAL_ADC_GetValue(&hadc1);}


     if(LDR>50  && p_arriba==1 ){ //Mucha luz bajamos la persiana.
    	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,1);
    	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,0);
    	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,0);
    	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,0);


    	 for (int i=1200;i>=0;i=i-125){
    	         __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,i);
    	     	HAL_Delay(200);
    	     	}
    	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,0);
    	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,0);
    	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,0);
    	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,1);

    	 bajando=0;
    	 p_arriba=0;
         subiendo=0;
    	 p_abajo=1;
     }


    	 HAL_ADC_Start(&hadc1);


    	 if (HAL_ADC_PollForConversion(&hadc1,1000000)==HAL_OK)
    	 {
    	  LDR=HAL_ADC_GetValue(&hadc1);
    	 }


     }

    while(boton_modo==2){ //SI ESTA EN MODO TEMPERATURA
    	 //	MODO CONTROL POR Temperatura
    		  HAL_ADC_Start(&hadc2);
    		  if (HAL_ADC_PollForConversion(&hadc2,1000000)==HAL_OK){
    		  		  	TEM=HAL_ADC_GetValue(&hadc2);
    		  		  	}

    		  if(TEM<40 && p_abajo==1 ){	//Si hay poca temperatura subimos la persiana
    		      		 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,0);
    		      		 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,0);
    		     		 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,1);
    		      		 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,0);


    		      	for (int i=0;i<=1200;i=i+125){
    		      			  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,i);
    		      			  HAL_Delay(200);
    		      		      }
    		      	     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,0);
    		      	     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,1);
    		      	   	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,0);
    		      	     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,0);

    		      	     bajando=0;
    		       		 p_arriba=1;
    		      	     subiendo=0;
    		      	     p_abajo=0;
    		      	}


    		      	     HAL_ADC_Start(&hadc2);


    		       if (HAL_ADC_PollForConversion(&hadc2,1000000)==HAL_OK){
    		           TEM=HAL_ADC_GetValue(&hadc2);}

    		       if(TEM>40  && p_arriba==1 ){ //Mucha temperatura bajamos la persiana.
    		      	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,1);
    		      	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,0);
    		      	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,0);
    		      	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,0);


    		      	 for (int i=1200;i>=0;i=i-125){
    		      	         __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,i);
    		      	     	HAL_Delay(200);
    		      	     	}
    		      	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,0);
    		      	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,0);
    		      	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,0);
    		      	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,1);

    		      	 bajando=0;
    		      	 p_arriba=0;
    		           subiendo=0;
    		      	 p_abajo=1;
    		       }



    		      	 HAL_ADC_Start(&hadc2);


    		      	 if (HAL_ADC_PollForConversion(&hadc2,1000000)==HAL_OK)
    		      	    	 {
    		      	    	  TEM=HAL_ADC_GetValue(&hadc2);
    		      	    	 }



    }


    } //del while








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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_8B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_8B;
  hadc2.Init.ScanConvMode = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 83;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD12 PD13 PD14 PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

}

/* USER CODE BEGIN 4 */

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
