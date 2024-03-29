#include "Flash_Driver.h"

/* part of cool hierarchy*/


/*-------Data---------*/
Data::Data(char* raw_string)
{  
    this->raw = new char;
    memcpy (this->raw, raw_string, strlen(raw_string));
};


FlashData::FlashData(char* raw_string,int len, FlashMeta* meta = nullptr) : Data(raw_string)
{   
    if (meta == nullptr)
        this->meta = new FlashMeta;
    else
        this->meta = meta;
};


/*End of part of cool hierarchy*/


FLASH_Page InternalFLASH::find_page(uint32_t addr){
    struct page res;
    for (int page_type = 0; 
        page_type < FLASH_NTYPE_COUNT; 
        page_type++){
        for (int page = 1; 
            page < FLASH_PAGE_NTYPE_COUNTS[page_type],
            page++){
                if (addr < page * FLASH_PAGE_SIZE_NTYPE[page_type])
                    res.page_num = page - 1;
                    res.page_type = page_type;
                    return res;
            }
    }
    return 0    
};

InternalFLASH::InternalFLASH(){
    this->start_page = find_page(Load$$LR$$LR_IROM1$$Limit) + 1;
    this->current_page = this->start_page;
    this->curent_addres = 0;
    this->storage.data = new FlashMeta;
    this->storage.prev = NULL;

}

OperationStatus InternalFLASH::WriteData(FlashData* data){
    uint32_t* buff = new uint32_t;
    FlashMap_List* storage_buff = new FlashMap_List;

    memcpy(buff,data->raw,strlen(data->raw));
    memcpy(this->storage.data, data->meta,sizeof(FlashMeta));
    
    storage_buff->prev = this->storage;
    this->storage = storage_buff;

    HAL_FLASH_Unlock();
    for (int i = 0; i < sizeof(buff)/sizeof(buff[0], i++))
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,data->meta.start + (i * 4), buff);

}