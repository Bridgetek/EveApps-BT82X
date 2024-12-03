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

1. INTRODUCTION
    This project contains example code for animation functionality of EVE.

2. FOLDER Structure
    ├───Project                  
    │   ├───Msvc          | Microsoft Visual Studio C++ 2019 IDE solution file for FT4222 or MPSSE platform
    │   └───Msvc_Emulator | Microsoft Visual Studio C++ 2019 IDE solution file for Eve emulator platform
    ├───Hdr               | Header files
    ├───Src               | Source files
    ├───Test              | Eve specific Assets: bitmap data, flash image, font data etc. 

2. SUPPORTED PLATFORM
    * Microsoft Visual C++ platform with FT4222 or MPSSE
    * Microsoft Visual C++ platform with Emulator
    
    EVE platform: EVE5
    LCD size: WUXGA(1920x1200)

3. SETUP AND RUN
    3.1 CONNECT HARDWARE
        3.1.1 Microsoft Visual C++ platform with FT4222 and MPSSE
            - Connect PC with EVE platform via FT4222 or MPSSE
            - Connect power to EVE platform

        3.1.2 Microsoft Visual C++ platform with Emulator
            - This setup uses window PC only

    3.2 DOWNLOAD ASSETS
        3.2.1 Use EAB to program the board with the flash.bin file located in the Flash folder.

        3.2.2 Copy all files from the SDCard folder to the SD card.

    3.3 BUILD AND RUN
        3.3.1 Microsoft Visual C++ platform with FT4222 and MPSSE
            - Open project in Project\MSVC with Microsoft Visual C++
            - Build (Ctrl + B)
            - Run (F5)

        3.3.2 Microsoft Visual C++ platform with Emulator
            - Open project in Project\Msvc_Emulator with Microsoft Visual C++
            - Build (Ctrl + B)
            - Run (F5)

4. CONFIGURATION INSTRUCTIONS
    Sample application uses the macros to configure the platforms: 
	
    Host platform:
        - Window host: EVE_PLATFORM_FT4222, EVE_PLATFORM_MPSSE
        - Emulator   : EVE_PLATFORM_BT8XXEMU
    
    EVE platform: EVE_GRAPHICS_BT820
    
    Please see common\eve_hal\EVE_Config.h. for more macros.
            
                                   【END】