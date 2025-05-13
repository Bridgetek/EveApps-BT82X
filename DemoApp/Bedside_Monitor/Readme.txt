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
    │   ├───Msvc             | Microsoft Visual Studio C++ 2019 IDE solution file for FT4222 or MPSSE platform
    ├───Hdr                  | Header files
    ├───Src                  | Source files
    ├───Test                 | Eve specific Assets: bitmap data, flash image, font data etc. 

2. SUPPORTED PLATFORM
    * Microsoft Visual C++ platform with FT4222 or MPSSE
    * Microsoft Visual C++ platform with Emulator
    * MM2040EV
    
    EVE platform: BT820 only
    LCD size: WUXGA(1920x1200)

3. SETUP AND RUN
    3.1 CONNECT HARDWARE
        - Connect PC with EVE platform via FT4222 or MPSSE
        - Connect power to EVE platform
       
    3.2 BUILD AND RUN
            - Open project in Project\MSVC with Microsoft Visual C++
            - Build (Ctrl + B)
            - Run (F5)

4. Demo functionalities
        4.1 Date and time setting: Users can access date and time settings at initialize.

        4.2 Zoom in / out: Tap the zoom button to zoom the graph from level 1 to 8 (pixels per sample).

        4.3 Date and time formatting:

            - Tap on the date to switch the date format (hh-mm-yyyy, hh-mmm-yyyy, hh-month-yyyy).
            - Tap on the time to switch the time format (hh:mm, hh:mm:ss, hh:mm:ss:ms).

5. CONFIGURATION INSTRUCTIONS
    Sample application uses the macros to configure the platforms: 
	
    Host platform:
        - Window host: EVE_PLATFORM_FT4222, EVE_PLATFORM_MPSSE
    
    EVE platform: EVE_GRAPHICS_BT820
    
    Please see common\eve_hal\EVE_Config.h. for more macros.
            
                                   【END】