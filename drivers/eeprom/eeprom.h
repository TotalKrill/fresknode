/**
 * @file eeprom.h
 * @brief  Functions for accessing eeprom on uwb-node
 * @author Kristoffer Ödmark
 * @version 0.1
 * @date 2015-09-14
 */
#ifndef EEPROM_H_
#define EEPROM_H_
#include "hal.h"
#include "ch.h"


#define EEPROM_SPI_PORT     GPIOB

#define EEPROM_SPI_CS       12
#define EEPROM_SPI_SCK      13
#define EEPROM_SPI_MISO     14
#define EEPROM_SPI_MOSI     15

#define EEPROM_SPI_BUS      SPID2

#define EEPROM_UID_ADDRESS  0xFA


static const SPIConfig eeprom_spicfg = {
  NULL,
  GPIOB,
  EEPROM_SPI_CS,
  SPI_CR1_BR_1
};

void eeprom_init(void);
void eeprom_write(uint8_t address, uint8_t *data, uint32_t length);
void eeprom_read(uint8_t address, uint8_t *data, uint32_t length);

#endif
