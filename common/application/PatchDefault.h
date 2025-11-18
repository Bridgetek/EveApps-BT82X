/* Generated file by extension.py and modified for EveApps */

#include "common.h"

/* From base patch*/
int EVE_LoadDefaultPatch(EVE_HalContext *phost);
#ifdef EVE_FLASH_NAND
/* From patch-nand */
void EVE_CMD_NFLASHERASEBLOCK(EVE_HalContext *phost, uint32_t block);
uint32_t EVE_CMD_NFLASHISBLOCKBAD(EVE_HalContext *phost, uint32_t block, uint32_t status);
void EVE_CMD_NFLASHPARAMETERS(EVE_HalContext *phost, uint32_t a);
uint32_t EVE_CMD_NFLASHSTATUS(EVE_HalContext *phost, uint32_t status);
uint32_t EVE_CMD_NFLASHMAP(EVE_HalContext *phost, uint32_t a, uint32_t page);
void EVE_CMD_NFLASHSETBLOCKBAD(EVE_HalContext *phost, uint32_t block);
#endif