/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "smart_battery.h"
#include "string.h"
#include "stdio.h"
#include "Flash_Driver.h"
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
CAN_HandleTypeDef hcan;

I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
uint8_t TxData[8] = {0,};
uint8_t RxData[8] = {0,};
uint32_t TxMailbox = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*Function which export all data from flash and send it to the CAN bus 
  After this function, all flash data will be erased 
  
  TODO: add timer from flight start*/
void CAN_SendDataFromFlash(InternalFLASH* flash){
  

  FlashMap_List *FlashDataRecord = flash->storage;
  // Tx like [num, len, data..] 
  while(FlashDataRecord->Prev != NULL){
    FlashDataRecord = FlashDataRecord->Prev;
    uint8_t* TxData_2_Send = new uint8_t[FlashDataRecord->Meta->len + 2];
    TxData_2_Send[0] = FlashDataRecord->Meta->idx;
    TxData_2_Send[1] = FlashDataRecord->Meta->len;
    memcpy(TxData_2_Send + 2, (uint32_t*)FlashDataRecord->Meta->start, FlashDataRecord->Meta->len);

    TxHeader.StdId = 0x181; //id
    TxHeader.ExtId = 0;
    TxHeader.RTR = CAN_RTR_DATA; //CAN_RTR_REMOTE
    TxHeader.IDE = CAN_ID_STD;   // CAN_ID_EXT
    TxHeader.DLC =  FlashDataRecord->Meta->len + 2;

    if(HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
      {
              HAL_Delay(10);
      }
    delete TxData_2_Send;
    FlashDataRecord = FlashDataRecord->Prev;
  }

  flash.EraseAllRecords();
};

/*Function so send ongoing state

  TODO Check types!!*/
void CAN_SendFlyingData(SmartBattery* battery){
  uint32_t* TxData_2_Send = battery.GetFlightData();
  
  TxHeader.StdId = 0x182; //id
  TxHeader.ExtId = 0;
  TxHeader.RTR = CAN_RTR_DATA; //CAN_RTR_REMOTE
  TxHeader.IDE = CAN_ID_STD;   // CAN_ID_EXT
  TxHeader.DLC =  4;

  if(HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
    {
            HAL_Delay(10);
    }
  delete TxData_2_Send;
};
void CAN_SaveFalshRegular(SmartBattery* battery, InternalFLASH* flash){

  uint32_t time = 0;
  uint32_t id = 0;
  int NewRecord_Len
  char* NewRecord_RawData = new char[29];
  NewRecord_RawData[0] = time;
  NewRecord_RawData[1] = id;
  NewRecord_RawData[2] = (char*)battery.GetData(); //type??? 

  NewRecord_Data.raw = NewRecord_RawData;
  NewRecord_Data.len = NewRecord_Len;

  FlashData* NewRecord_Container = new FlashData(
                                                NewRecord_RawData,
                                                NewRecord_Len
                                                );
  flash.WriteData(NewRecord_Container);
};


void SaveEvent(SmartBattery battery, InternalFLASH* flash){
  uint32_t time = 0;
  uint32_t id = 1;
  int NewRecord_Len;
  char* NewRecord_RawData = new char[29];
  NewRecord_RawData[0] = time;
  NewRecord_RawData[1] = id;
  NewRecord_RawData[2] = (char*)battery.GetData(); //type??? 

  NewRecord_Data.raw = NewRecord_RawData;
  NewRecord_Data.len = NewRecord_Len;

  FlashData* NewRecord_Container = new FlashData(
                                                NewRecord_RawData,
                                                NewRecord_Len
                                                );
  flash.WriteData(NewRecord_Container);
};
void check_tx(){};


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) 
/*CAN recieve command calback*/
{
    if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
      {   
        switch (RxData[0])
        {
          case 0:
            CAN_SendDataFromFlash(); break;
          case 1:
            CAN_SendFlyingData(); break;
          case 2:
            CAN_SaveFalsh(); break;
          default:
            check_tx(); break;
        }
    }
}

extern uint32_t Load$$LR$$LR_IROM1$$Limit;
uint32_t b = (uint32_t)&Load$$LR$$LR_IROM1$$Limit;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  
  uint8_t smbus_address = 0x16;
  uint8_t data[20];
  

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  char test[256];
   // pointer to end of software
  sprintf(test,"Limit %08X\n", (uint32_t)&Load$$LR$$LR_IROM1$$Limit);
  

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  extern uint32_t* __text_end__;
  HAL_Delay(100);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
/* USER CODE BEGIN 2 */
TxHeader.StdId = 0x181; //id
TxHeader.ExtId = 0;
TxHeader.RTR = CAN_RTR_DATA; //CAN_RTR_REMOTE
TxHeader.IDE = CAN_ID_STD;   // CAN_ID_EXT
TxHeader.DLC = 8;

HAL_CAN_Start(&hcan); //start CAN
  
HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

SmartBattery ba(&hi2c1);
  /* USER CODE END 2 */
  uint32_t a = (uint32_t)&Load$$LR$$LR_IROM1$$Limit; // pointer to end of software
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  ba.CMD(SBCommands_Basic::Voltage);
  InternalFLASH flash;
  FlashData datak((char*)ba.buff, 2);
  flash.WriteData(&datak);
  uint32_t* data_readed;
  FlashData* CheckRecord = flash.ReadData();
  memcpy(TxData,CheckRecord->raw,8);
  //HAL_Delay(test);
  while (1)
  {
    data_readed = ba.GetData();
    TxHeader.StdId = 0x0378;
      while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0);

        if(HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
        {
                HAL_Delay(10);
        }

    delete[] data_readed;
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */
CAN_FilterTypeDef  sFilterConfig;
  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN;
  hcan.Init.Prescaler = 4;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = ENABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = ENABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
sFilterConfig.FilterBank = 0;
sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; 
sFilterConfig.FilterIdHigh = 0x0000;
sFilterConfig.FilterIdLow = 0x0000;
sFilterConfig.FilterMaskIdHigh = 0x0000;
sFilterConfig.FilterMaskIdLow = 0x0000;
sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
sFilterConfig.FilterActivation = ENABLE;
//sFilterConfig.SlaveStartFilterBank = 14;

if(HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
{
Error_Handler();
}

  /* USER CODE END CAN_Init 2 */

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
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

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
