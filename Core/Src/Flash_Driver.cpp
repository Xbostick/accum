#include "Flash_Driver.h"
/* part of cool hierarchy*/

uint32_t ADDR_FLASH_PAGE_SIZE_NTYPE_BEGIN[] = {0x8000000U};
uint32_t FLASH_PAGE_SIZE_NTYPE[] = {0x400};
uint32_t FLASH_PAGE_NTYPE_COUNTS[] = {64}; // there is no page 0. Starting from page 1(!!!)
uint8_t  FLASH_NTYPE_COUNT = 1;

/*-------Data---------*/
Data::Data(uint8_t* RawData, uint32_t Data_Raw_Len, uint8_t Data_Id, uint8_t Data_GovernorCode)
{  
    this->Data_Raw = new uint8_t[Data_Raw_Len];
    memcpy (this->Data_Raw, RawData, Data_Raw_Len);
    this->Data_Raw_Len = Data_Raw_Len;
    this->Data_Id = Data_Id;
    this->Data_GovernorCode = Data_GovernorCode;
};

Data::Data(){
    this->Data_Raw = nullptr;
    this->Data_Raw_Len = 0;
    this->Data_Id = 0;
    this->Data_GovernorCode = 0;
}
Data::Data(Data *ExstData){
    this->Data_Raw = ExstData->Data_Raw;
    this->Data_Raw_Len = ExstData->Data_Raw_Len;
    this->Data_Id = ExstData->Data_Id;
    this->Data_GovernorCode = ExstData->Data_GovernorCode;

    delete ExstData;
};

FlashData::FlashData(uint8_t* RawData, uint32_t Data_Raw_Len, FlashMeta* ExistingMeta) : Data(RawData, Data_Raw_Len)
{   
    if (ExistingMeta == nullptr){
        FlashMeta* self_creted_meta = new FlashMeta;
        self_creted_meta->Description = "Automatic generated meta";
        self_creted_meta->DescriptionLen = strlen(self_creted_meta->Description);
        self_creted_meta->idx = 0;
        self_creted_meta->start = 0;
        self_creted_meta->len = ceil(((float)Data_Raw_Len)/4);
        this->FlashData_Meta = self_creted_meta;
    }
    else
        this->FlashData_Meta = ExistingMeta;
}
FlashData::FlashData(Data *ExstData, FlashMeta *ExistingMeta) : Data(ExstData){
    if (ExistingMeta == nullptr){
        FlashMeta* self_creted_meta = new FlashMeta;
        self_creted_meta->Description = "Automatic generated meta";
        self_creted_meta->DescriptionLen = strlen(self_creted_meta->Description);
        self_creted_meta->idx = 0;
        self_creted_meta->start = 0;
        self_creted_meta->len = ceil(((float)ExstData->Data_Raw_Len)/4);
        this->FlashData_Meta = self_creted_meta;
    }
    else
        this->FlashData_Meta = ExistingMeta;
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
    this->storage->Meta = new FlashMeta;
    this->storage->Prev = NULL;

}

/**
 * @brief Function to write data in internal flash
 * @details Casting bytes data to 32bits data, to use WORD program algorythm
 * 
 * @param data 
 * @return OperationStatus 
 */
OperationStatus InternalFLASH::WriteData(FlashData* data){
    uint32_t* data_buff = new uint32_t;
    FlashMap_List* storage_buff = new FlashMap_List;
    
    /*If in MetaData no starting addres write it after previous record*/
    if (data->FlashData_Meta->start == 0) data->FlashData_Meta->start = this->current_addres; 

    /*Automatic indexing records in `FlashData`*/
    if (this->storage->Prev != NULL){
        data->FlashData_Meta->idx = this->storage->Prev->Meta->idx + 1;
    }
    else{
        data->FlashData_Meta->idx = 0;
    }

    memcpy(data_buff,data->Data_Raw,data->Data_Raw_Len);
    memcpy(this->storage->Meta, data->FlashData_Meta,sizeof(FlashMeta));
    
    storage_buff->Prev = this->storage;
    this->storage = storage_buff;

    HAL_FLASH_Unlock();
    /*Using `ceil` for divide with round up*/
    for (int i = 0; i < data->; i++ ){
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

/**
 * @brief Create `FlashData` object with RawData same as in iternal flash on addr in Meta
 * 
 * @param idx if empty(equal -1) read latest records, else read idxed
 * @return FlashData* 
 */
FlashData* InternalFLASH::ReadData(int idx){
    FlashMeta *NewFlashMeta = new FlashMeta;

    if (idx == -1){
        NewFlashMeta->idx = this->storage->Prev->Meta->idx;
    }
    else{
        NewFlashMeta->idx = idx;
    }
    
    FlashMap_List *ExistingFlashDataRecords = this->storage;
    while (ExistingFlashDataRecords->Prev!= NULL){
        ExistingFlashDataRecords = ExistingFlashDataRecords->Prev;
        if (ExistingFlashDataRecords->Meta->idx == NewFlashMeta->idx){
            memcpy(NewFlashMeta,ExistingFlashDataRecords->Meta,sizeof(FlashMeta));
            /*`data_buff` needed to simply cast uint32_t to uint8_t without endian troubles*/
            uint32_t* data_buff = new uint32_t[NewFlashMeta->len];
            memcpy(data_buff,(uint32_t*)NewFlashMeta->start,NewFlashMeta->len);
            FlashData* NewFlashData = new FlashData((uint8_t*)data_buff,NewFlashMeta->len,NewFlashMeta);
            delete data_buff;
            return NewFlashData;
        }    
    }
    return nullptr;
}

OperationStatus InternalFLASH::EraseAllRecords(){
    while (this->storage->Prev!= NULL){
        FlashMap_List *FlashDataRecord = this->storage;
        this->storage = this->storage->Prev;
        delete FlashDataRecord;        
    }

     for (int i = 0; i < FLASH_PAGE_NTYPE_COUNTS[this->start_page.page_type]; i++){
        FLASH_EraseInitTypeDef EraseInitStruct;
        EraseInitStruct.TypeErase     = FLASH_TYPEERASE_PAGES;

        /*Skipping pages with ROM*/
        if (this->start_page.page_type == i){            
            EraseInitStruct.PageAddress   = ADDR_FLASH_PAGE_SIZE_NTYPE_BEGIN[i] + 
                                            (this->start_page.page_num + 1) * FLASH_PAGE_SIZE_NTYPE[i];
            EraseInitStruct.NbPages       = FLASH_PAGE_SIZE_NTYPE[this->start_page.page_type] - 
                                            (this->start_page.page_num + 1);
        }
        else{
            EraseInitStruct.PageAddress   = ADDR_FLASH_PAGE_SIZE_NTYPE_BEGIN[i];
            EraseInitStruct.NbPages       = FLASH_PAGE_SIZE_NTYPE[i];
        }
        

        HAL_FLASH_Unlock();
        uint32_t PageError;
        if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
        {
            return HAL_FLASH_GetError ();
        }
    }
    return OperationStatus::OK;

}

/*
OperationStatus InternalFLASH::EraseRecord(int idx){
    
    if (idx == -1){
        CleanInterval(this->storage->Prev->Meta->start,this->storage->Prev->Meta->len)
    }
    else{
        FlashMeta CurrentFlashDataRecords = this->storage->Meta;
        FlashMeta PreviousFlashDataRecords = this->storage;
    }
    
    while (CurrentFlashDataRecords->Prev!= NULL){
        CurrentFlashDataRecords = CurrentFlashDataRecords->Prev;
        if (CurrentFlashDataRecords->Meta->idx == idx){
            CleanInterval(CurrentFlashDataRecords->Meta->start,
                            CurrentFlashDataRecords->Meta->len);
            
            PreviousFlashDataRecords.Prev = CurrentFlashDataRecords->Prev;
            delete CurrentFlashDataRecords;
            return OperationStatus::OK;
        }
        PreviousFlashDataRecords = CurrentFlashDataRecords;
    }
    return OperationStatus::ErrorCode;
}


OperationStatus InternalFLASH::CleanInterval(uint32_t start, uint32_t len){
    FLASH_Page page = find_page(start);
    if (start + len > 
        ADDR_FLASH_PAGE_SIZE_NTYPE_BEGIN[page.page_type] + 
        FLASH_PAGE_SIZE_NTYPE[page.page_type] * (page.page_num + 1)){
            ;
        }
    else{
        /* all in one page
        float prev_size, follow_size;
        uint32_t page_start = ADDR_FLASH_PAGE_SIZE_NTYPE_BEGIN[page.page_type] + 
                            FLASH_PAGE_SIZE_NTYPE[page.page_type] * page.page_num;
        prev_size = start - page_start; //bytes
        follow_size = page_start + FLASH_PAGE_SIZE_NTYPE[page.page_type] - len - start;
        uint32_t* data_prev = new uint32_t[ceil((prev_size)/sizeof(uint32_t))];
        uint32_t* data_follow = new uint32_t[ceil((follow_size)/sizeof(uint32_t))];
        memcpy(data_prev,(uint32_t*)page_start,ceil((prev_size)/sizeof(uint32_t)));
        memcpy(data_follow,(uint32_t*)(start + len),ceil((follow_size)/sizeof(uint32_t)));
        
        FLASH_EraseInitTypeDef EraseInitStruct;
        EraseInitStruct.TypeErase     = FLASH_TYPEERASE_PAGES;
        EraseInitStruct.PageAddress   = page_start;
        EraseInitStruct.NbPages       = 1;

        uint32_t current_addres

        HAL_FLASH_Unlock();
        uint32_t PageError;
        if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
        {
            return HAL_FLASH_GetError ();
        }

        for (int i = 0; i < ceil((prev_size)/sizeof(uint32_t)); i++ ){
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,this->current_addres,data_buff[i]) != HAL_OK){
                HAL_FLASH_Lock();
            return OperationStatus::ErrorCode;        
            }
            this->current_addres += 4;
        }
        HAL_FLASH_Lock();
        memcpy((uint32_t*)page_start,data_prev,prev_size);
        memcpy((uint32_t*)(start + len),data_follow,follow_size);




    }
}

*/