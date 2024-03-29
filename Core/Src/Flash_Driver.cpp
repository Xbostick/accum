#include "Flash_Driver.h"
/* part of cool hierarchy*/

uint32_t ADDR_FLASH_PAGE_SIZE_NTYPE_BEGIN[] = {0x8000000U};
uint32_t FLASH_PAGE_SIZE_NTYPE[] = {0x400};
uint32_t FLASH_PAGE_NTYPE_COUNTS[] = {64}; // there is no page 0. Starting from page 1(!!!)
uint8_t  FLASH_NTYPE_COUNT = 1;

/*-------Data---------*/
Data::Data(char* raw_string, int len)
{  
    this->raw = new char;
    memcpy (this->raw, raw_string, len);
    this->len = len;
};


FlashData::FlashData(char* raw_string,int len, FlashMeta* meta) : Data(raw_string, len)
{   
    if (meta == nullptr){
        FlashMeta* self_creted_meta = new FlashMeta;
        self_creted_meta->Description = "Automatic generated meta";
        self_creted_meta->DescriptionLen = strlen(self_creted_meta->Description);
        self_creted_meta->idx = 0;
        self_creted_meta->len = len;
        self_creted_meta->start = 0;
        this->meta = self_creted_meta;
    }
    else
        this->meta = meta;
};


/*End of part of cool hierarchy*/


FLASH_Page InternalFLASH::find_page(uint32_t addr){
    FLASH_Page res = {0,0};
    for (int page_type = 0; 
        page_type < FLASH_NTYPE_COUNT; 
        page_type++){
        for (int page = 1; 
            page < FLASH_PAGE_NTYPE_COUNTS[page_type];
            page++){
                if (addr < ADDR_FLASH_PAGE_SIZE_NTYPE_BEGIN[page_type] + (page * FLASH_PAGE_SIZE_NTYPE[page_type])){
                    res.page_num = page - 1;
                    res.page_type = page_type;
                    return res;
                }
            }
    }
    return FLASH_Page();    
};

InternalFLASH::InternalFLASH(){
    this->start_page = find_page((uint32_t)&Load$$LR$$LR_IROM1$$Limit); // ptr externed in .h. It pointing to the end of ROM
    this->current_page.page_type = this->start_page.page_type;
    this->current_page.page_num = this->start_page.page_num + 1;
    this->current_addres = ADDR_FLASH_PAGE_SIZE_NTYPE_BEGIN[this->start_page.page_type] 
                            + FLASH_PAGE_SIZE_NTYPE[this->start_page.page_type] * this->current_page.page_num;
    this->storage = new FlashMap_List;
    this->storage->data = new FlashMeta;
    this->storage->prev = NULL;

}

OperationStatus InternalFLASH::WriteData(FlashData* data){
    uint32_t* data_buff = new uint32_t;
    FlashMap_List* storage_buff = new FlashMap_List;
    
    if (data->meta->start == 0) data->meta->start = this->current_addres;

    if (this->storage->prev != NULL){
        data->meta->idx = this->storage->prev->data->idx + 1;
    }
    else{
        data->meta->idx = 0;
    }

    memcpy(data_buff,data->raw,strlen(data->raw));
    memcpy(this->storage->data, data->meta,sizeof(FlashMeta));
    
    storage_buff->prev = this->storage;
    this->storage = storage_buff;

    HAL_FLASH_Unlock();
    for (int i = 0; i < ceil(((float)strlen(data->raw))/4); i++ ){
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,this->current_addres,data_buff[i]) != HAL_OK){
            HAL_FLASH_Lock();
          return OperationStatus::ErrorCode;        
        }
        this->current_addres += 4;
    }
    HAL_FLASH_Lock();

    delete data_buff, data;  
    return OperationStatus::OK;    

}

FlashData* InternalFLASH::ReadData(int idx){
    uint32_t* data_buff = new uint32_t;
    FlashMeta *NewFlashMeta = new FlashMeta;
    if (idx == -1){
        NewFlashMeta->idx = this->storage->prev->data->meta->idx;
    }
    else{
        NewFlashMeta->idx = idx;
    }
    
    FlashMap_List *ExistingFlashDataRecords = this->storage;
    while (ExistingFlashDataRecords->prev!= NULL){
        ExistingFlashDataRecords = ExistingFlashDataRecords->prev;
        if (ExistingFlashDataRecords->Meta->idx == NewFlashMeta->idx){
            memcpy(NewFlashMeta,ExistingFlashDataRecords->Meta,sizeof(FlashMeta));
            FlashData* NewFlashData = new FlashData(
                NewFlashMeta->start,NewFlashMeta->len,NewFlashMeta);
        }
    
    }
    delete data_buff;
    return NewFlashData;
}