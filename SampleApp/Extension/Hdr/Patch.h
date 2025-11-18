/* Generated file by extension.py */

#include "common.h"

int eve_loadpatch(EVE_HalContext *phost);
/* From base patch */
void EVE_CMD_REGION(EVE_HalContext *phost);
void EVE_CMD_ENDREGION(EVE_HalContext *phost, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void EVE_CMD_TOUCHOFFSET(EVE_HalContext *phost, int16_t p1, int16_t p2);
void EVE_CMD_ENDTOUCHOFFSET(EVE_HalContext *phost);