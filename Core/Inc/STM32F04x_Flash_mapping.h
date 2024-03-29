#include "stdlib.h"
#include "Flash_Driver.h"

uint32_t ADDR_FLASH_PAGE_SIZE_NTYPE_BEGIN[] = {0x08000000};
uint32_t FLASH_PAGE_SIZE_NTYPE[] = {0x400};
uint32_t FLASH_PAGE_NTYPE_COUNTS[] = {64}; // there is no page 0. Starting from page 1(!!!)
uint8_t  FLASH_NTYPE_COUNT = 1;

