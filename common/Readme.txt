================================================================================
 Copyright (c) Bridgetek Pte Ltd.                                               
                                                                                
 THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD. ``AS IS'' AND ANY EXPRESS OR   
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF   
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO     
 EVENT SHALL BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, LOSS OF USE, DATA, OR PROFITS OR  
 BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER  
 IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE     
 POSSIBILITY OF SUCH DAMAGE.                                                    
================================================================================

This common folder contains reusable source code for every demo applications and
sample application. It contains 3 folder: 

1. EVE_HAL: Contains the HAL (Hardware Abstraction Layer) source code, it provide 
unique APIs to control hardware ICs from different host 
platform (PC, Emulator...).

Platform's specific files are suffixed in the file name, for example:
- EVE_HalImpl_FT4222.c: Source code specific for FT4222.
- EVE_HalImpl_MPSSE.c : Source code specific for MPSSE.
- EVE_HalImpl_WIN32.c : Source code specific for WIN32.
- EVE_HalImpl_RP2040.c : Source code specific for RP2040.

2. APPLICATION: Contains the most useful and reusable functions for applications.
This folder can be use as a library for main application. Every functions inside 
this folder are designed to works on every platform.

3. EVE_FLASH: Contains blob files. Blob files are flash header of EVE's internal flash.
                     
Below diagram describe how common folder used in the program:
                     
                -------------------------------------------
                |             Main application            |
                -------------------------------------------
                     |                   |
                     |                   ▼
                     |        -----------------------
                     |        | Common/application  |
                     |        -----------------------
                     |                   |
                     ▼                   ▼
                -------------------------------------------
                |              Common/eve_hal             |
                -------------------------------------------
                                      |
                                      |
                                      ▼
                -------------------------------------------
                |                  EVE IC                 |
                -------------------------------------------
                                      |
                                      |
                                      ▼   
                -------------------------------------------
                |               LCD, speaker              | 
                -------------------------------------------



                                   【END】
