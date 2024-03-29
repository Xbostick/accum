/* Originally this lib is for BQ78350DBT, however it is using Smart Battery protocol. 
    So, i suppose it will fit every device woth same protocol
    http://sbs-forum.org/specs/sbdata10.pdf
    */

#ifndef MESSAGE_CONTROLLER_H
#define MESSAGE_CONTROLLER_H
#ifndef __cplusplus
extern "C++" {
#endif
#include "stm32f0xx_hal.h"
#include <string.h>

#define ADDR 0x16

/*Basic protocol commands*/
enum SBCommands_Basic {
  ManufacturerAccess      = 0x00,
  RemainingCapacityAlarm  = 0x01,
  RemainingTimeAlarm      = 0x02,
  BatteryMode             = 0x03,
  AtRate                  = 0x04,
  AtRateTimeToFull        = 0x05,
  AtRateTimeToEmpty       = 0x06,
  AtRateOK                = 0x07,
  Temperature             = 0x08,
  Voltage                 = 0x09,
  Current                 = 0x0A,
  AverageCurrent          = 0x0B,
  MaxError                = 0x0C,
  RelativeStateOfCharge   = 0x0D,
  AbsoluteStateOfCharge   = 0x0E,
  RemainingCapacity       = 0x0F,
  FullChargeCapacity      = 0x10,
  RunTimeToEmpty          = 0x11,
  AverageTimeToEmpty      = 0x12,
  AverageTimeToFull       = 0x13,
  BatteryStatus           = 0x16,
  CycleCount              = 0x17,
  DesignCapacity          = 0x18,
  DesignVoltage           = 0x19,
  SpecificationInfo       = 0x1A,
  ManufactureDate         = 0x1B,
  SerialNumber            = 0x1C,
  ManufacturerName        = 0x20,
  DeviceName              = 0x21,
  DeviceChemestry         = 0x22,
  ManufacturerData        = 0x23,
};


/*  Special enum for BQ78350DBT. Can be find in DataSheet  */
enum SBCommands_Special {
  HostFETControl          = 0x2B,
  GPIOStatus              = 0x2C,
  GPIOControl             = 0x2D,
  VAUXVoltage             = 0x2E,
  ManufacturerInput       = 0x2F,
  CellVoltage_1           = 0x3F,
  CellVoltage_2           = 0x3E,
  CellVoltage_3           = 0x3D,
  CellVoltage_4           = 0x3C,
  CellVoltage_5           = 0x3B,
  CellVoltage_6           = 0x3A,
  CellVoltage_7           = 0x39,
  CellVoltage_8           = 0x38,
  CellVoltage_9           = 0x37,
  CellVoltage_10          = 0x36,
  CellVoltage_11          = 0x35,
  CellVoltage_12          = 0x34,
  CellVoltage_13          = 0x33,
  CellVoltage_14          = 0x32,
  CellVoltage_15          = 0x31,
  DynamicPower            = 0x4C,
  ExtAveCellVoltage       = 0x4D,
  PendingEDV              = 0x4E,
  StateOfHealth           = 0x4F,
  SafetyAlert             = 0x50,
  SafetyStatus            = 0x51,
  PFAlert                 = 0x52,
  PFStatus                = 0x53,
  OperationStatus_SB      = 0x54,
  ChargingStatus          = 0x55,
  GaugingStatus           = 0x56,
  ManufacturingStatus     = 0x57,
  AFEStatus               = 0x58,
  AFEConfig               = 0x59,
  AFEVCx                  = 0x5A,
  AFEData                 = 0x5B,
  LifetimeDataBlock_1     = 0x60,
  LifetimeDataBlock_2     = 0x61,
  LifetimeDataBlock_3     = 0x62,
  LifetimeDataBlock_4     = 0x63,
  LifetimeDataBlock_5     = 0x64,
  LifetimeDataBlock_6     = 0x65,
  LifetimeDataBlock_7     = 0x66,
  ManufacturerInfo        = 0x70,
  DAStatus1               = 0x71,
  DAStatus2               = 0x72,
  CUVSnapshot             = 0x80,
  COVSnapshot             = 0x81
};

class SmartBattery{
  private:
    I2C_HandleTypeDef*  _hal_i2c_handler;
    uint8_t             _i2cAddr; 

  
  public:
    uint8_t             buff[8];
    
    SmartBattery(I2C_HandleTypeDef* hal_i2c_handler);
    void CMD(uint8_t SBCommand);
    uint32_t* GetData();
  
    
};

#ifndef __cplusplus
}
#endif
#endif


