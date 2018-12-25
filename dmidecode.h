#ifndef __DMIDECODE_H
#define __DMIDECODE_H

#include <stdint.h>

#define EFI_NOT_FOUND   (-1)
#define EFI_NO_SMBIOS   (-2)

int dmi_decode_init();
uint8_t *dmi_get_system_uuid();
uint8_t *dmi_get_system_serial_no();
uint8_t *dmi_get_base_board_serial_no();
uint8_t *dmi_get_chassis_serial_no();

#endif
