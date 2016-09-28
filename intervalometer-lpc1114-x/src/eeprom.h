#ifndef AVALON_EEPROM_H_
#define AVALON_EEPROM_H_
//! @file
//! EEPROM interface.

#include <cstddef>
#include <cstdint>

//! EEPROM address, by byte.
typedef std::uint16_t eeaddr_t;

//! Initialize EEPROM subsystem; don't call until after RTOS is initialized.
void eeprom_init();

//! Write sizeBytes bytes from buf to the EEPROM starting at address addr.
//! @return true on successful write.
bool ee_writen(eeaddr_t addr, const void * buf, std::size_t sizeBytes);

//! Reads sizeBytes to buf from the EEPROM start at address addr.
//! @return true on successful read.
bool ee_readn(eeaddr_t addr, void * buf, std::size_t sizeBytes);


#endif /* AVALON_EEPROM_H_ */
