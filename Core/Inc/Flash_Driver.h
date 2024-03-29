/*This is prototype of internal FLASH controller. In future should be rewriteen to new lib structure*/

#ifndef FLASH_CONTROLLER_H
#define FLASH_CONTROLLER_H
#ifndef __cplusplus
extern "C++" {
#endif


#include "stm32f0xx_hal.h"
#include "string.h"

#include "STM32F04x_Flash_mapping.h"

extern uint32_t Load$$LR$$LR_IROM1$$Limit; // pointer to end of software
/* part of cool hierarchy*/
enum OperationStatus
{
	OK = 0,
	Not_OK,
	ErrorCode
};


class Data
{   
    protected:
    int actual_index = 0;
    
    public:
    char* governor_code; //мб в протектед занести
    char *raw;
    int* idxs;
    int num_idx = 0;

    Data(char* raw_string);

    virtual OperationStatus CheckSum() = 0;

    // char* NextIter();

    // int GetActualIdx();

    
};

struct FlashMeta{
    int32_t 			start; // adress in flash memory
    int32_t 			len;
    char*               Name;
    int                 NameLen;
    int                 idx;//нужен ли?
    char*               Description;
    int                 DescriptionLen;
};

struct FlashMap_List{

    FlashMeta*          data;
    FlashMap_List* 		prev;    
};

class FlashData : public Data
{   

    public:
        FlashMeta*   meta;
        FlashData(char* raw_string, int len, FlashMeta* meta = nullptr);
       // Делается кастом(cast) FlashData(Data* data);
};

/*End of part of cool hierarchy*/

struct FLASH_Page{
    uint32_t page_num;
    uint8_t page_type;
}

class InternalFLASH{
  private:
    FLASH_Page current_page;
    FLASH_Page start_page;
    uint32_t current_addres; // nearest free address
    FlashMap_List storage = NULL;

    FLASH_Page find_page(uint32_t addr);

  
  public:
    InternalFLASH();
    OperationStatus WriteData(FlashData* data);
    FlashData* ReadData(FlashMeta* data);
    OperationStatus Erase(FlashMeta* data);
}
  

#ifndef __cplusplus
}
#endif
#endif