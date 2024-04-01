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
 * and returns a pointer to an array containing 27 32-bit integers.
 * 
 * @return uint32_t* 
 */
uint32_t* SmartBattery::GetData(){
  
  uint32_t* data = new uint32_t[27];
  
  for (int i = 0; i <= SBCommands_Special::CellVoltage_1 - SBCommands_Special::CellVoltage_14; i++){
    CMD(i + SBCommands_Special::CellVoltage_14);
    memcpy(data + i ,buff, 1);
  }
  
  CMD(SBCommands_Basic::Voltage);
  memcpy(data + 14 ,buff, 1);
  
  CMD(SBCommands_Basic::RelativeStateOfCharge);
  memcpy(data + 15 ,buff, 1);
  
  CMD(SBCommands_Special::StateOfHealth);
  memcpy(data + 16 ,buff, 1);
  
  CMD(SBCommands_Basic::RunTimeToEmpty);
  memcpy(data + 17 ,buff, 1);
  
  CMD(SBCommands_Basic::RemainingCapacity);
  memcpy(data + 18 ,buff, 1);
  
  CMD(SBCommands_Basic::AverageTimeToEmpty);
  memcpy(data +19, buff, 1);
  
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