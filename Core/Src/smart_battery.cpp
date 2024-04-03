#include "smart_battery.h"

SmartBattery::SmartBattery(I2C_HandleTypeDef* hal_i2c_handler){
  _hal_i2c_handler = hal_i2c_handler;
  _i2cAddr         = ADDR;
}

void SmartBattery::CMD(uint8_t SBCommand){
  HAL_I2C_Master_Transmit(_hal_i2c_handler,_i2cAddr,&SBCommand,1,100);
  HAL_I2C_Master_Receive(_hal_i2c_handler,_i2cAddr,buff,100,100);
}

/**
 * @brief  The SmartBattery::GetData() method retrieves battery data 
 * and returns a pointer to an array containing 10 uint32.
 * @details first 2 bytes are empty. Should be used for technical needs
 * 
 * @return uint32_t* 
 */
uint32_t* SmartBattery::GetData(){
  
  uint32_t* data = new uint32_t[10];

  /* 2 bytes */
  CMD(SBCommands_Basic::AverageTimeToEmpty);
  /* first 16 bytes for technical needs*/
  data[0] += *((uint16_t*)(buff));
  
  /* 2 bytes each, so 2*14 = 28 bytes = 7 uint32*/

  for (int i = 0; i <= SBCommands_Special::CellVoltage_1 - SBCommands_Special::CellVoltage_14; i++){
    CMD(i + SBCommands_Special::CellVoltage_14);
    if (i%2 == 0)
            data[i/2 + 1] = *(uint16_t*)buff << 16;
        else
            data[i/2 + 1] += *(uint16_t*)buff;
  }
  
  /* 2 bytes */
  CMD(SBCommands_Basic::Voltage);
  data[8] +=  *(uint16_t*)buff << 16;
  
  /* 1 byte*/
  CMD(SBCommands_Basic::RelativeStateOfCharge);
  data[8] +=  *buff << 8;
   
  /* 1 byte */
  CMD(SBCommands_Special::StateOfHealth);
  data[8] += *buff;
  
  /* 2 bytes */
  CMD(SBCommands_Basic::RunTimeToEmpty);
  data[9] += *(uint16_t*)buff << 16;

  /* 2 bytes */  
  CMD(SBCommands_Basic::RemainingCapacity);
  data[9] += *(uint16_t*)buff;

  return data;
    
}

/**
 * @brief The SmartBattery::GetFlightData() method retrieves flight data
 * and returns a pointer to an array containing 5 32-bit integers.
 * 
 * @return uint32_t* 
 */
uint32_t* SmartBattery::GetFlightData(){
  uint32_t* data = new uint32_t[5];
  
  CMD(SBCommands_Basic::Voltage);
  memcpy(data ,buff, 1);

  CMD(SBCommands_Basic::RelativeStateOfCharge);
  memcpy(data + 1 ,buff, 1);

  CMD(SBCommands_Basic::RunTimeToEmpty);
  memcpy(data + 2 ,buff, 1);

  CMD(SBCommands_Basic::RemainingCapacity);
  memcpy(data + 3 ,buff, 1);
  
  CMD(SBCommands_Basic::AverageTimeToEmpty);
  memcpy(data +4, buff, 1);

  return data;
};