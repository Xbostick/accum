/*This is prototype of internal FLASH controller. In future should be rewriteen to new lib structure*/

#ifndef FLASH_CONTROLLER_H
#define FLASH_CONTROLLER_H
#ifndef __cplusplus
extern "C++" {
#endif

#include "stm32f0xx_hal.h"
#include "string.h"
#include <stdint.h>
#include "math.h"
//#include "STM32F04x_Flash_mapping.h"


extern uint32_t Load$$LR$$LR_IROM1$$Limit; // pointer to end of software
/* part of cool hierarchy*/
typedef enum OperationStatus
{
	OK = 0,
	Not_OK,
	ErrorCode
} OperationStatus;


/**
 * @brief Main type of connections in this library
 * @details This class creating, deleting and hold information. It is used by every element in this library.
 * 
 */
class Data
{   
    protected:
    int actual_index = 0;
    
    public:
    uint8_t Data_GovernorCode;
    uint8_t Data_Id;

    uint8_t* Data_Raw;
    uint32_t Data_Raw_Len = 0; //bytes count

    /**
     * @brief Construct a new Data object
     * 
     * @param RawData Array of raw bytes.
     * @param Data_Raw_Len Size of array also size of it in bytes
     * @param Data_Id Field to specify data usage or type
     * @param Data_GovernorCode  Code for acknowlage 
     */
    Data(uint8_t* RawData, uint32_t Data_Raw_Len, uint8_t Data_Id = 0, uint8_t Data_GovernorCode = 0);

    /**
     * @brief Construct a new Data object
     * @details Creates empty data field with nullptr in RawData field
     * 
     */
    Data();

    /**
     * @brief Construct a new Data object
     * @details Constructor to child classes needs
     * 
     * @param ExstData 
     */
    Data(Data* ExstData);

    //virtual OperationStatus CheckSum() = 0;

    // char* NextIter();

    // int GetActualIdx();

    
};

/**
 * @brief Structure to hold meta information about flash memory record
 * 
 * @param start Address in flash memory
 * 
 * @param len Length of data in 32 bits 
 * @param Name Field to specify data name * 
 * @param NameLen Field to specify data name length
 * @param idx indexing records in internal memory
 * @param Description Field to specify data
 * @param DescriptionLen Field to specify data description length
 */
struct FlashMeta{
    uint32_t 			start; // adress in flash memory
    uint32_t            len; //size in 32_bits units
    char*               Name;
    int                 NameLen;
    int                 idx;// internal usage now
    char*               Description;
    int                 DescriptionLen;
};

/**
 * @brief Child from Data with extra struct `FlashMeta`
 * 
 */
class FlashData : public Data{   

    public:
        FlashMeta*   FlashData_Meta;
        FlashData(uint8_t* RawData, uint32_t Data_Raw_Len, FlashMeta* ExistingMeta = nullptr);
        FlashData(Data* ExstData, FlashMeta* ExistingMeta = nullptr);
       // Делается кастом(cast) FlashData(Data* data);
};



struct FlashMap_List{

    FlashMeta*        Meta;
    FlashMap_List* 		Prev;    
};


/*End of part of cool hierarchy*/

/**
 * @brief 
 * 
 * @param page_num
 * @param page_type 
 */
struct FLASH_Page{
    uint32_t page_num;
    uint8_t page_type;
};

class InternalFLASH{
  private:
    FLASH_Page current_page;
    FLASH_Page start_page;
    uint32_t current_addres; // nearest free address
    

    FLASH_Page find_page(uint32_t addr);
    //OperationStatus CleanInterval(uint32_t beggin, uint32_t len);

  
  public:
    InternalFLASH();
    OperationStatus WriteData(FlashData* data);
    FlashData* ReadData(int idx = -1);
    FlashData* ReadAll(FlashMeta* data);
    OperationStatus EraseAllRecords();

    /*After each action `storage->Meta` should be `new Meta`*/
    FlashMap_List* storage; 
    //OperationStatus EraseRecord(int idx = -1);
};
  

#ifndef __cplusplus
}
#endif
#endif