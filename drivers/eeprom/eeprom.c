/**
 * @file eeprom.c
 * @brief functions for accessing eeprom on uwb-node
 * @author Kristoffer Ödmark
 * @version 0.1
 * @date 2015-09-14
 */
#include "eeprom.h"

void eeprom_select(void){
    spiAcquireBus(&EEPROM_SPI_BUS);  /* Acquire ownership of the bus.*/
    spiSelect(&EEPROM_SPI_BUS);      /* Slave Select assertion. */

}

void eeprom_unselect(void){
    spiUnselect(&EEPROM_SPI_BUS);       /* Slave Select de-assertion.*/
    spiReleaseBus(&EEPROM_SPI_BUS);     /* Ownership release.*/

}
void eeprom_init(void){
    palSetPad(GPIOB, EEPROM_SPI_CS);
    spiStart(&SPID2, &eeprom_spicfg);
}

void eeprom_write(uint8_t address,  uint8_t *data, uint32_t length){
    eeprom_select();
    uint8_t header [2] = {0x02, address};
    spiSend(&EEPROM_SPI_BUS, 2, header);
    spiSend(&EEPROM_SPI_BUS, length, data);
    eeprom_unselect();

}
void eeprom_read(uint8_t address, uint8_t *data, uint32_t length){
    eeprom_select();
    uint8_t header [2] = {0x03, address};
    spiSend(&EEPROM_SPI_BUS, 2, header);
    spiReceive(&EEPROM_SPI_BUS, length, data);
    eeprom_unselect();

}
