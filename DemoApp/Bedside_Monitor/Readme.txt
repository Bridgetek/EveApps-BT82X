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
    This project contains example code for the Bedside Monitor demo application.

2. FOLDER Structure
    ├───Project                  
    │   ├───MM2040EV_BT81x_C | MM2040EV platform with Rp2040 and BT817
    │   ├───Msvc             | Microsoft Visual Studio C++ 2019 IDE solution file for FT4222 or MPSSE platform
    │   └───Msvc_Emulator    | Microsoft Visual Studio C++ 2019 IDE solution file for Eve emulator platform
    ├───Hdr                  | Header files
    ├───Src                  | Source files
    ├───Test                 | Eve specific Assets: bitmap data, flash image, font data etc. 

2. SUPPORTED PLATFORM
    * Microsoft Visual C++ platform with FT4222 or MPSSE
    * Microsoft Visual C++ platform with Emulator
    * MM2040EV
    
    EVE platform: BT817
    LCD size: WXGA(1280x800)

3. SETUP AND RUN
    3.1 CONNECT HARDWARE
        3.1.1 Microsoft Visual C++ platform with FT4222 and MPSSE
            - Connect PC with EVE platform via FT4222 or MPSSE
            - Connect power to EVE platform

        3.1.2 Microsoft Visual C++ platform with Emulator
            - This setup uses window PC only
        
        3.1.3 MM2040EV
            - Connect PC with MM2040EV via USB cable

    3.2 DOWNLOAD ASSETS

        Copy all files from the Flash folder to the SD card.
        
    3.3 BUILD AND RUN
        3.3.1 Microsoft Visual C++ platform with FT4222 and MPSSE
            - Open project in Project\MSVC with Microsoft Visual C++
            - Build (Ctrl + B)
            - Run (F5)

        3.3.2 Microsoft Visual C++ platform with Emulator
            - Open project in Project\Msvc_Emulator with Microsoft Visual C++
            - Build (Ctrl + B)
            - Run (F5)
        
        3.3.3 MM2040EV
            - Open folder MM2040EV_BT81x_C with VScode
            - Build with Cmake extension, the result is build/MM2040EV_BT81x_C.uf2
            - Copy build/MM2040EV_BT81x_C.uf2 into RPI 

4. CONFIGURATION INSTRUCTIONS
    Sample application uses the macros to configure the platforms: 
	
    Host platform:
        - Window host: EVE_PLATFORM_FT4222, EVE_PLATFORM_MPSSE
        - Emulator   : EVE_PLATFORM_BT8XXEMU
    
    EVE platform: EVE_GRAPHICS_BT817
    
    Please see common\eve_hal\EVE_Config.h. for more macros.
            
                                   【END】