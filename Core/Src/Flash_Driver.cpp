#include "Flash_Driver.h"
/* part of cool hierarchy*/


/*-------Data---------*/
Data::Data(char* raw_string)
{  
    this->raw = new char;
    memcpy (this->raw, raw_string, strlen(raw_string));
};


FlashData::FlashData(char* raw_string,int len, FlashMeta* meta) : Data(raw_string)
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
    FLASH_Page res;
    for (int page_type = 0; 
        page_type < FLASH_NTYPE_COUNT; 
        page_type++){
        for (int page = 1; 
            page < FLASH_PAGE_NTYPE_COUNTS[page_type];
            page++){
                if (addr < page * FLASH_PAGE_SIZE_NTYPE[page_type])
                    res.page_num = page - 1;
                    res.page_type = page_type;
                    return res;
            }
    }
    return FLASH_Page();    
};

InternalFLASH::InternalFLASH(){
    this->start_page = find_page(Load$$LR$$LR_IROM1$$Limit);
    this->current_page.page_type = this->start_page.page_type;
    this->current_page.page_num = this->start_page.page_num + 1;
    this->current_addres = ADDR_FLASH_PAGE_SIZE_NTYPE_BEGIN[this->start_page.page_type] 
                            + FLASH_PAGE_SIZE_NTYPE[this->start_page.page_type] * this->current_page.page_num;
    this->storage->data = new FlashMeta;
    this->storage->prev = NULL;

}

OperationStatus InternalFLASH::WriteData(FlashData* data){
    uint32_t* buff = new uint32_t;
    FlashMap_List* storage_buff = new FlashMap_List;
    
    if (data->meta->start == 0) data->meta->start = this->current_addres;

    memcpy(buff,data->raw,strlen(data->raw));
    memcpy(this->storage->data, data->meta,sizeof(FlashMeta));
    
    storage_buff->prev = this->storage;
    this->storage = storage_buff;

    HAL_FLASH_Unlock();
    for (int i = 0; i < ceil(((float)strlen(data->raw))/4); i++ ){
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,this->current_addres,buff[i]) != HAL_OK){
            HAL_FLASH_Lock();
          return OperationStatus::ErrorCode;        
        }
        this->current_addres += 4;
    }
    HAL_FLASH_Lock();
    return OperationStatus::OK;    

}