#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>
#define FLASH                       ((FLASH_TypeDef *) FLASH_BASE)
#define FLASH_BASE                  (uint16_t)0x5050

#define FLASH_RASS_KEY1             ((uint8_t)0x56)        /*!< First RASS key */
#define FLASH_RASS_KEY2             ((uint8_t)0xAE)        /*!< Second RASS key */
#define FLASH_CLEAR_BYTE            ((uint8_t)0x00)
#define FLASH_SET_BYTE              ((uint8_t)0xFF)
#define EEPROM_START_ADDRESS        ((uint16_t)0x4000)     /* Address of start EEPROM */
#define EEPROM_END_ADDRESS          ((uint16_t)0x4080)     /* Address of end EEPROM */


void EEPROM_Unlock();
void EEPROM_Lock();
void EEPROM_ProgramByte(uint32_t Address, uint8_t Data);
uint8_t EEPROM_ReadByte(uint32_t Address);
void EEPROM_EraseByte(uint32_t Address);

int __eeprom_wait_for_last_operation(void);
void __eeprom_program_byte(unsigned char __near * dst, unsigned char v);
void __eeprom_program_long(unsigned char __near * dst, unsigned long v);

#endif