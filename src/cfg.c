#include "cfg.h"

#include "hardware/regs/addressmap.h"
#include "hardware/sync.h"

#include <hardware/flash.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static bool cfg_exists()
{
    uint8_t *cfg_start = (uint8_t *)XIP_BASE + LAST_SECTOR_OFFSET;
    return !memcmp(cfg_start, CFG_MAGIC, CFG_MAGIC_LEN);
}

int cfg_read(struct cfg *cfg)
{
    if (!cfg_exists()) { return 1; }
    uint8_t *cfg_start = (uint8_t *)XIP_BASE + LAST_SECTOR_OFFSET + CFG_MAGIC_LEN;
    memcpy(cfg, cfg_start, sizeof(struct cfg));
    return 0;
}

// do NOT call when core 1 is enabled!!
void cfg_write(struct cfg *cfg)
{
    static uint8_t sector[FLASH_SECTOR_SIZE];
    bzero(sector, FLASH_SECTOR_SIZE);
    memcpy(sector, CFG_MAGIC, CFG_MAGIC_LEN);
    memcpy(sector + CFG_MAGIC_LEN, cfg, sizeof(struct cfg));

    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(LAST_SECTOR_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(LAST_SECTOR_OFFSET, sector, FLASH_SECTOR_SIZE);
    restore_interrupts(ints);
}
