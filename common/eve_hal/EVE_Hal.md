# EVE Hal Library

The EVE Hardware Abstraction Layer consists of the following modules:
* **EVE_Hal**: This is the lowest level layer, which implements writing to and reading from EVE memory space, as well as sending host commands.
* **EVE_Cmd**: Handles writing to the coprocessor command queue, also ensuring coprocessor fault reports are detected.
* **EVE_CoCmd**: Wraps the coprocessor commands. <ins> Recommanded to use </ins>
* **EVE_CoDl**: Wraps the display list commands. <ins> Recommanded to use </ins>
* **EVE_Util**: Utilities for bootup, reset, and other common management.
* **EVE_Platform**: Host platform support.
* **EVE_MediaFifo**: MediaFifo support.
* **EVE_LoadFile**: load file helper function.

# Usage

## Include

```
#include <EVE_Platform.h>
```

## Initialization

```
/* Initialize HAL */
EVE_Hal_initialize();

/* Fetch the default parameters for a device. */
EVE_Hal_defaults(&params);

/* Open the specified device */
EVE_HalContext host = { 0 };
EVE_HalContext *phost = &host;
if (!EVE_Hal_open(phost, &params))
	return false;

/* Bootup and config the device.
Change display resolution if needed. */
EVE_Util_bootupConfig(phost);

/* Show a blank screen */
EVE_Util_clearScreen(phost);
```

## Cleanup

```
EVE_Util_shutdown(phost);
EVE_Hal_close(phost);
EVE_Hal_release();
```

#  EVE Hal

## Initialization functions (Host platform depended)

### EVE_HalImpl_initialize

Initialize HAL platform

### EVE_HalImpl_release

Release HAL platform

### EVE_HalImpl_defaults

Get the default configuration parameters

### EVE_HalImpl_open

Opens a new HAL context

### EVE_HalImpl_close

Close a HAL context

### EVE_HalImpl_idle

Idle

## Intialization helper functions (Host platform independed)

* EVE_Hal_initialize
* EVE_Hal_release
* EVE_Hal_defaults
* EVE_Hal_open
* EVE_Hal_close
* EVE_Hal_idle

## Transfer functions (Host platform depended)

### EVE_Hal_startTransfer

Starts a read or write transfer, using the *EVE_TRANSFER_READ* or *EVE_TRANSFER_WRITE* option respectively. From hardware point of view, this keeps the SPI cable select flagged until *EVE_Hal_endTransfer* is called. Write transfers may be buffered in software, depending on the implementation.

### EVE_Hal_transfer32

Transfers an integer value using the currently open transfer, using the appropriate byte ordering.

### EVE_Hal_transferMem

Transfers a chunk of memory using the currently open transfer.

### EVE_Hal_transferString

Transfers a string using the currently open transfer. The string will be limited to whichever is shorter of the specified maximum length, or the actual string length, but will always be null-terminated, which may add another byte to the specified maximum length. Additionally the string may be padded to the specified padding mask, which may also add additional bytes to the written length. The maximum amount of transferred data shall be no more than *padMask + 1*.

### EVE_Hal_endTransfer

Ends the transfer started by *EVE_Hal_startTransfer*. From a hardware point of view, this turns off the SPI cable select.

Write transfers may be buffered in software, depending on the implementation. In case a write transfer is buffered, it may be transferred during the this function call, or anytime as is most efficient for the underlying implementation.

## Transfer helper functions (Host platform independed)

* EVE_Hal_rd32
* EVE_Hal_rdMem
* EVE_Hal_wr32
* EVE_Hal_wrMem
* EVE_Hal_wrString

## Utility functions (Host platform depended)

* EVE_Hal_spiCommand
* EVE_Hal_powerCycle
* EVE_Hal_setSPI
* EVE_Hal_restoreSPI
* EVE_Hal_getInterrupt

## SPI commands (Host platform independed)

* EVE_Hal_SPICmd_pwr_state
* EVE_Hal_SPICmd_setpllsp1
* EVE_Hal_SPICmd_sysclkdiv
* EVE_Hal_SPICmd_setbootcfg
* EVE_Hal_SPICmd_bootcfgen
* EVE_Hal_SPICmd_setddrtype

## Misc (Host platform depended)

* EVE_Mcu_initialize
* EVE_Mcu_release
* EVE_Millis_initialize
* EVE_Millis_release
* EVE_millis
* EVE_millis64
* EVE_sleep

# EVE Cmd

## Status utility

### EVE_Cmd_rp

Get the current read pointer. Safe to use during ongoing command transaction.

### EVE_Cmd_wp

Get the current write pointer. Updates the cached write pointer when CMDB is not supported. Safe to use during ongoing command transaction.

### EVE_Cmd_space

Get the currently available space. Updates cached available space. Safe to use during ongoing command transaction.

## Command writing

### EVE_Cmd_startFunc

Begin writing a function, keeps the transfer open. The transfer may be closed using *EVE_Cmd_endFunc*.

Functions must be ended, even if a coprocessor fault occurs while writing them. The coprocessor may be reset only after ending the function.

It is safe to call any of the command writing functions while the coprocessor is in fault mode. The coprocessor fault may be checked at the end of transfer.

While a command transaction is ongoing, HAL functions outside of *EVE_Cmd_* must not be used.

### EVE_Cmd_endFunc

End writing a function, closes the transfer.

### EVE_Cmd_wrMem

Write a memory buffer to the command buffer. Waits if there is not enough space in the command buffer. Returns *false* in case a co processor fault occurred.

### EVE_Cmd_wrString

Write a string to the command buffer, null-terminated, padded to 4 bytes. Waits if there is not enough space in the command buffer. Returns *false* in case a coprocessor fault occured.

The value for *maxLength* may be no more than *EVE_CMD_STRING_MAX*, which should also be used as it's default value in case the string length is not known.

### EVE_Cmd_wr16/32

Write a value to the command buffer. Uses a cache to write 4 bytes at once to the command buffer for multiple 16-bit writes. Wire endianness is handled by the transfer. Waits if there is not enough space in the command buffer. Returns *false* in case a coprocessor fault occurred.

### EVE_Cmd_moveWp

Move the write pointer forward by the specified number of bytes. Returns the previous write pointer. This is used to reserve space in the command buffer for reading output from a command. Returns an uneven value in case a coprocessor fault occurred. The return value may be more conveniently validated using *EVE_CMD_FAULT* to check whether a coprocessor fault occurred.

## Queue management

### EVE_Cmd_waitFlush

Wait for the command buffer to fully empty. Returns *false* in case a coprocessor fault occured.

### EVE_Cmd_waitSpace

Wait for the command buffer to have at least the requested amount of free space. Returns *false* in case a coprocessor fault occurred.

### EVE_Cmd_waitLogo

Wait for logo to finish displaying. Waits for both the read and write pointer to go to 0. Returns *false* in case a coprocessor fault occurred.

# EVE_CoCmd

It is recommended to use this when calling coprocess commands, as it wraps the EVE_Cmd function, ensuring forward and backward compatibility, along with some optimizations.
Refer to __EVE_CoCmd.h__.

# EVE_CoDl

It is recommended to use this when calling display list commands, as it wraps the display list functions through EVE_Cmd, ensuring forward and backward compatibility, along with some optimizations.
Refer to __EVE_CoDl.h__.

# EVE Util

* EVE_Util_bootup
* EVE_Util_shutdown
* EVE_Util_clearScreen
* EVE_Util_resetCoprocessor
* EVE_Util_coprocessorFaultRecover

# EVE Platform

Host platform support, working with EVE_HalImpl to provide host-dependent functions for the EVE Hal above.

# EVE_MediaFifo

* EVE_MediaFifo_set
* EVE_MediaFifo_rp
* EVE_MediaFifo_wp
* EVE_MediaFifo_space
* EVE_MediaFifo_wrMem
* EVE_MediaFifo_waitFlush
* EVE_MediaFifo_waitSpace

# EVE_LoadFile

* EVE_Util_loadRawFile
* EVE_Util_loadInflateFile
* EVE_Util_loadImageFile
* EVE_Util_loadCmdFile
* EVE_Util_readFile
* EVE_Util_loadMediaFile
* EVE_Util_loadFile
* EVE_Util_closeFile