#ifndef CFG_H
#define CFG_H

#include "hardware/flash.h"

#include <stdbool.h>
#include <stdint.h>

#define CFG_MAGIC "foo"
#define CFG_MAGIC_LEN 3
#define LAST_SECTOR_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

struct cfg {
  float calib[4];
};

int cfg_read(struct cfg *cfg);
void cfg_write(struct cfg *cfg);

#endif // !CFG_H
