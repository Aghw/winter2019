/************************************************************************************

Copyright (c) 2001-2016  University of Washington Extension.

Module Name:

    tasks.c

Module Description:

    The tasks that are executed by the test application.

2016/2 Nick Strathy adapted it for NUCLEO-F401RE 

************************************************************************************/
#include <stdarg.h>

#include "bsp.h"
#include "print.h"
#include "mp3Util.h"

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>

Adafruit_ILI9341 lcdCtrl = Adafruit_ILI9341();  // The LCD controller
    
Adafruit_FT6206 touchCtrl = Adafruit_FT6206();  // The touch controller
Adafruit_GFX_Button stop = Adafruit_GFX_Button();
//Adafruit_GFX grpCtrl = Adafruit_GFX();  // The LCD controller

//Rectangle r1 = Rectangle(2, 2, 10, 5);     // The graphics controller

#define PENRADIUS 3

long MapTouchToScreen(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


//#include "train_crossing.h"
#include "AyeSew.h"

#define BUFSIZE 256

/************************************************************************************

   Allocate the stacks for each task.
   The maximum number of tasks the application can have is defined by OS_MAX_TASKS in os_cfg.h

************************************************************************************/

static OS_STK   LcdTouchDemoTaskStk[APP_CFG_TASK_START_STK_SIZE];
static OS_STK   Mp3DemoTaskStk[APP_CFG_TASK_START_STK_SIZE];
/* Public variables ----------------------------------------------------------*/
__IO uint32_t timer;
__IO uint32_t song_status;
//const unsigned char* currentSong []= {Train_Crossing, };
uint16_t song_index = 0;
INT32U volume_level;
#define NORMAL_MODE    0
     
// Task prototypes
void LcdTouchDemoTask(void* pdata);
void Mp3DemoTask(void* pdata);
bool contains(int16_t a, int16_t b);
void ButtonPressed(int16_t x0, int16_t y0);

// Useful functions
void PrintToLcdWithBuf(char *buf, int size, char *format, ...);

OS_EVENT * qMsg;                 // pointer to a uCOS message queue
#define QMAXENTRIES 5            // maximum entries in the queue
void * qMsgVPtrs[QMAXENTRIES];   // an array of void pointers which is the actual queue

//#define BUT_RADIUS         30
//#define VOL_ECHO_X         210
//#define VOL_ECHO_Y         178
//#define VOL_SHIFT          5

// Globals
BOOLEAN nextSong = OS_FALSE;
OS_EVENT * semLcdDriver;
/************************************************************************************

   This task is the initial task running, started by main(). It starts
   the system tick timer and creates all the other tasks. Then it deletes itself.

************************************************************************************/
void StartupTask(void* pdata)
{
    char buf[BUFSIZE];
    PjdfErrCode pjdfErr;
    INT32U length;
    static HANDLE hSD = 0;
    static HANDLE hSPI = 0; 
    song_status = 0;
    
    semLcdDriver = OSSemCreate(1);              //create semLcdDriver as a binary semaphore
    qMsg = OSQCreate(qMsgVPtrs, QMAXENTRIES);
    
    PrintWithBuf(buf, BUFSIZE, "StartupTask: Begin\n");
    PrintWithBuf(buf, BUFSIZE, "StartupTask: Starting timer tick\n");

    // Start the system tick
    OS_CPU_SysTickInit(OS_TICKS_PER_SEC);
    
    // Initialize SD card
    PrintWithBuf(buf, PRINTBUFMAX, "Opening handle to SD driver: %s\n", PJDF_DEVICE_ID_SD_ADAFRUIT);
    hSD = Open(PJDF_DEVICE_ID_SD_ADAFRUIT, 0);
    if (!PJDF_IS_VALID_HANDLE(hSD)) while(1);


    PrintWithBuf(buf, PRINTBUFMAX, "Opening SD SPI driver: %s\n", SD_SPI_DEVICE_ID);
    // We talk to the SD controller over a SPI interface therefore
    // open an instance of that SPI driver and pass the handle to 
    // the SD driver.
    hSPI = Open(SD_SPI_DEVICE_ID, 0);
    if (!PJDF_IS_VALID_HANDLE(hSPI)) while(1);
    
    length = sizeof(HANDLE);
    pjdfErr = Ioctl(hSD, PJDF_CTRL_SD_SET_SPI_HANDLE, &hSPI, &length);
    if(PJDF_IS_ERROR(pjdfErr)) while(1);

    // Create the test tasks
    PrintWithBuf(buf, BUFSIZE, "StartupTask: Creating the application tasks\n");

    // The maximum number of tasks the application can have is defined by OS_MAX_TASKS in os_cfg.h
    OSTaskCreate(Mp3DemoTask, (void*)0, &Mp3DemoTaskStk[APP_CFG_TASK_START_STK_SIZE-1], APP_TASK_TEST2_PRIO);
    OSTaskCreate(LcdTouchDemoTask, (void*)0, &LcdTouchDemoTaskStk[APP_CFG_TASK_START_STK_SIZE-1], APP_TASK_TEST1_PRIO);

    // Delete ourselves, letting the work be done in the new tasks.
    PrintWithBuf(buf, BUFSIZE, "StartupTask: deleting self\n");
    OSTaskDel(OS_PRIO_SELF);
}

bool contains(int16_t coord, int16_t origin) {
    if(coord >= (origin - BTN_RADIUS) && coord <= (origin + BTN_RADIUS + 1))
        return true;
    return false;
}

void ButtonPressed(int16_t x0, int16_t y0)
{ 
    char buf[BUFSIZE];
    lcdCtrl.setCursor(10, 295);
    lcdCtrl.setTextSize(2);
    lcdCtrl.setTextColor(ILI9341_BLACK, ILI9341_OLIVE);
        
    if (stop.contains(x0, y0)) {
      PrintWithBuf(buf, BUFSIZE, "STOP definitely got pressed. You pressed the Stop button\n");
    }
    if(contains(x0, CIRC_X0_COORDINATES) && contains(y0, CIRC_Y0_COORDINATES)) {
        PrintWithBuf(buf, BUFSIZE, "You pressed the Stop button\n");
        PrintToLcdWithBuf(buf, BUFSIZE, "Stopped");
        song_status = 0;
    } else if (contains(x0, CIRC_X1_COORDINATES) && contains(y0, CIRC_Y0_COORDINATES)) {
        PrintWithBuf(buf, BUFSIZE, "You pressed the Play button\n");
        PrintToLcdWithBuf(buf, BUFSIZE, "Playing");
        song_status = 1;
    } else if (contains(x0, CIRC_X2_COORDINATES) && contains(y0, CIRC_Y0_COORDINATES)) {
        PrintWithBuf(buf, BUFSIZE, "You pressed the Pause button\n");
        PrintToLcdWithBuf(buf, BUFSIZE, "Paused ");
        song_status = 2;
    } else if (contains(x0, CIRC_X0_COORDINATES) && contains(y0, CIRC_Y1_COORDINATES)) {
        PrintWithBuf(buf, BUFSIZE, "You pressed the Fast-Backward button\n");
        song_index = 0;
        song_status = 3;
    } else if (contains(x0, CIRC_X1_COORDINATES) && contains(y0, CIRC_Y1_COORDINATES)) {
        PrintWithBuf(buf, BUFSIZE, "You pressed the Fast-Forward button\n");
        song_index = 1;
        song_status = 4;
    } else if (contains(x0, CIRC_X2_COORDINATES) && contains(y0, CIRC_Y1_COORDINATES)) {
        PrintWithBuf(buf, BUFSIZE, "You pressed the Volume-Up button\n");
//        song_status = 5;
        ++volume_level;
    } else if (contains(x0, CIRC_X2_COORDINATES) && contains(y0, CIRC_Y2_COORDINATES)) {
        PrintWithBuf(buf, BUFSIZE, "You pressed the Volume-Down button\n");
//        song_status = 6;
        --volume_level;
    }
}

static void DrawLoginButtons()
{
     char buf[BUFSIZE];
     
     lcdCtrl.setCursor(10, 10);
     lcdCtrl.setTextColor(ILI9341_BLACK, ILI9341_OLIVE);
     PrintToLcdWithBuf(buf, BUFSIZE, "Enter Password:");
     
     lcdCtrl.setCursor(20, 35);
     lcdCtrl.drawRoundRect(10,27,100, 25, 2, ILI9341_MAROON);
     lcdCtrl.setTextSize(2);
//     PrintToLcdWithBuf(buf, BUFSIZE, "7890");
     
     uint16_t Butt_XLoc[] = {CIRC_X0_COORDINATES + 10, CIRC_X1_COORDINATES, CIRC_X2_COORDINATES - 10};
     uint16_t Butt_YLoc[] = {CIRC_Y0_COORDINATES - 25, CIRC_Y1_COORDINATES - 40, CIRC_Y2_COORDINATES - 55, CIRC_Y3_COORDINATES - 15};
     lcdCtrl.setTextSize(4);
     
     for (int j = 0; j < 3; ++j ) {
        for ( int i = 0; i < 3; ++i) {
           lcdCtrl.drawCircle(Butt_XLoc[i], Butt_YLoc[j], BUT_RADIUS, ILI9341_MAROON);
         }
     }
     
     lcdCtrl.setCursor(CIRC_X0_COORDINATES , CIRC_Y0_COORDINATES - 40);
     PrintToLcdWithBuf(buf, BUFSIZE, "1");
     lcdCtrl.setCursor(CIRC_X1_COORDINATES - 10, CIRC_Y0_COORDINATES - 40);
     PrintToLcdWithBuf(buf, BUFSIZE, "2");
     lcdCtrl.setCursor(CIRC_X2_COORDINATES - 20, CIRC_Y0_COORDINATES - 40);
     PrintToLcdWithBuf(buf, BUFSIZE, "3");
     lcdCtrl.setCursor(CIRC_X0_COORDINATES, CIRC_Y1_COORDINATES - 55);
     PrintToLcdWithBuf(buf, BUFSIZE, "4");
     lcdCtrl.setCursor(CIRC_X1_COORDINATES - 10, CIRC_Y1_COORDINATES - 55);
     PrintToLcdWithBuf(buf, BUFSIZE, "5");
     lcdCtrl.setCursor(CIRC_X2_COORDINATES - 20, CIRC_Y1_COORDINATES - 55);
     PrintToLcdWithBuf(buf, BUFSIZE, "6");
     lcdCtrl.setCursor(CIRC_X0_COORDINATES, CIRC_Y2_COORDINATES - 70);
     PrintToLcdWithBuf(buf, BUFSIZE, "7");
     lcdCtrl.setCursor(CIRC_X1_COORDINATES - 10, CIRC_Y2_COORDINATES - 70);
     PrintToLcdWithBuf(buf, BUFSIZE, "8");
     lcdCtrl.setCursor(CIRC_X2_COORDINATES - 20, CIRC_Y2_COORDINATES - 70);
     PrintToLcdWithBuf(buf, BUFSIZE, "9");
     
     lcdCtrl.drawCircle(CIRC_X1_COORDINATES, CIRC_Y3_COORDINATES - 15, BUT_RADIUS, ILI9341_MAROON);
     lcdCtrl.setCursor(CIRC_X1_COORDINATES - 10, CIRC_Y3_COORDINATES - 30);
     PrintToLcdWithBuf(buf, BUFSIZE, "0");
}

static void DrawMp3Buttons()
{   
    uint16_t BTNX_COORD[] = {CIRC_X0_COORDINATES, CIRC_X1_COORDINATES, CIRC_X2_COORDINATES};
    uint16_t BTNY_COORD[] = {CIRC_Y0_COORDINATES, CIRC_Y1_COORDINATES, CIRC_Y2_COORDINATES};
     
    lcdCtrl.drawRoundRect(20,55,200, 12, 2, ILI9341_MAROON);
    lcdCtrl.setCursor(10, 295);
    lcdCtrl.setTextSize(2);
    
    lcdCtrl.drawRoundRect(180,1,58, 20, 1, ILI9341_WHITE);
    for (int j = 0; j < 2; ++j ) {
        for ( int i = 0; i < 3; ++i) {
           lcdCtrl.drawCircle(BTNX_COORD[i], BTNY_COORD[j], BTN_RADIUS, ILI9341_MAROON);
//           lcdCtrl.fillCircle(CIRC_X0_COORDINATES, CIRC_Y0_COORDINATES, BTN_RADIUS,ILI9341_GRAY);
         }
     }
    
    // Stop button
//    stop.initButton(&lcdCtrl, CIRC_X0_COORDINATES, CIRC_Y0_COORDINATES, 2 * BTN_RADIUS + 1, 2 * BTN_RADIUS + 1, ILI9341_WHITE, ILI9341_OLIVE, ILI9341_BLACK, "", 0);
//    stop.drawButton();
    lcdCtrl.fillRoundRect(RECT_X0_COORDINATES,RECT_Y0_COORDINATES,25, 25, 1, ILI9341_WHITE);
    
    // Play button
//    lcdCtrl.drawTriangle(110,RECT_Y0_COORDINATES, 135, 115, 110, 128, ILI9341_WHITE);
    lcdCtrl.fillTriangle(110,RECT_Y0_COORDINATES, 135, 115, 110, 128, ILI9341_WHITE);
    
    // Pause button
//    lcdCtrl.drawRoundRect(186,RECT_Y0_COORDINATES,10, 25, 1, ILI9341_WHITE);
    lcdCtrl.fillRoundRect(186,RECT_Y0_COORDINATES,10, 25, 1, ILI9341_WHITE);
//    lcdCtrl.drawRoundRect(204,RECT_Y0_COORDINATES,10, 25, 1, ILI9341_WHITE);
    lcdCtrl.fillRoundRect(204,RECT_Y0_COORDINATES,10, 25, 1, ILI9341_WHITE);
    
    // Fast-backward button
//    lcdCtrl.drawTriangle(22,195, 38, 183, 38, 208, ILI9341_WHITE);
    lcdCtrl.fillTriangle(22,195, 38, 183, 38, 208, ILI9341_WHITE);
//    lcdCtrl.drawTriangle(38,195, 53, 183, 53, 208, ILI9341_WHITE);
    lcdCtrl.fillTriangle(38,195, 53, 183, 53, 208, ILI9341_WHITE);
    
    // Fast-Forward button
//    lcdCtrl.drawTriangle(107,183, 122, 195, 107, 208, ILI9341_WHITE);
    lcdCtrl.fillTriangle(107,183, 122, 195, 107, 208, ILI9341_WHITE);
//    lcdCtrl.drawTriangle(122,183, 137, 195, 122, 208, ILI9341_WHITE);
    lcdCtrl.fillTriangle(122,183, 137, 195, 122, 208, ILI9341_WHITE);
    
    // Volume-up button
//    lcdCtrl.drawRoundRect(185,188,15, 15, 1, ILI9341_WHITE);
    lcdCtrl.fillRoundRect(185,188,15, 15, 1, ILI9341_WHITE);
//    lcdCtrl.drawTriangle(185,195, 205, 183, 205, 207, ILI9341_WHITE);
    lcdCtrl.fillTriangle(185,195, 205, 183, 205, 207, ILI9341_WHITE);
    lcdCtrl.drawFastVLine(VOL_ECHO_X, VOL_ECHO_Y + 2 * VOL_SHIFT, 15, ILI9341_WHITE);
    lcdCtrl.drawFastVLine(VOL_ECHO_X + VOL_SHIFT, VOL_ECHO_Y + VOL_SHIFT, 26, ILI9341_WHITE);
    lcdCtrl.drawFastVLine(VOL_ECHO_X + 2 * VOL_SHIFT, VOL_ECHO_Y, 36, ILI9341_WHITE);
    
    // Volume-down button
    lcdCtrl.drawCircle(CIRC_X2_COORDINATES, 275, BTN_RADIUS, ILI9341_MAROON);
//    lcdCtrl.drawRoundRect(185,268,15, 15, 1, ILI9341_WHITE);
    lcdCtrl.fillRoundRect(185,268,15, 15, 1, ILI9341_WHITE);
//    lcdCtrl.drawTriangle(185,275, 205, 263, 205, 287, ILI9341_WHITE);
    lcdCtrl.fillTriangle(185,275, 205, 263, 205, 287, ILI9341_WHITE);
    lcdCtrl.drawFastVLine(210, 268, 15, ILI9341_WHITE);
    
    // Mute button
//    Adafruit_GFX_Button mute = Adafruit_GFX_Button();
//    mute.initButton(&lcdCtrl, CIRC_X0_COORDINATES, 275, 80, 30, ILI9341_WHITE, ILI9341_OLIVE, ILI9341_BLACK, "Mute", 2);
//    mute.drawButton();
}

static void DrawLcdContents()
{
//    char buf[BUFSIZE];
    lcdCtrl.fillScreen(ILI9341_OLIVE);
    
    // Print a message on the LCD
    lcdCtrl.setCursor(40, 60);
    lcdCtrl.setTextColor(ILI9341_NAVY);  
    lcdCtrl.setTextSize(2);
    
    DrawMp3Buttons();
//    DrawLoginButtons();
}

/************************************************************************************

   Runs LCD/Touch demo code

************************************************************************************/
void LcdTouchDemoTask(void* pdata)
{
    PjdfErrCode pjdfErr;
    INT32U length;

    char buf[BUFSIZE];
    PrintWithBuf(buf, BUFSIZE, "LcdTouchDemoTask: starting\n");

    PrintWithBuf(buf, BUFSIZE, "Opening LCD driver: %s\n", PJDF_DEVICE_ID_LCD_ILI9341);
    // Open handle to the LCD driver
    HANDLE hLcd = Open(PJDF_DEVICE_ID_LCD_ILI9341, 0);
    
    // Open handle to the I2C driver
    PrintWithBuf(buf, BUFSIZE, "Opening touch panel I2C Protocol driver: %s\n", PJDF_DEVICE_ID_I2C1);
    HANDLE hI2C = Open(PJDF_DEVICE_ID_I2C1, 0);
    if (!PJDF_IS_VALID_HANDLE(hLcd)) while(1);

    PrintWithBuf(buf, BUFSIZE, "Opening LCD SPI driver: %s\n", LCD_SPI_DEVICE_ID);
    // We talk to the LCD controller over a SPI interface therefore
    // open an instance of that SPI driver and pass the handle to 
    // the LCD driver.
    HANDLE hSPI = Open(LCD_SPI_DEVICE_ID, 0);
    if (!PJDF_IS_VALID_HANDLE(hSPI)) while(1);

    length = sizeof(HANDLE);
    pjdfErr = Ioctl(hLcd, PJDF_CTRL_LCD_SET_SPI_HANDLE, &hSPI, &length);
    if(PJDF_IS_ERROR(pjdfErr)) while(1);

    PrintWithBuf(buf, BUFSIZE, "Initializing LCD controller\n");
    lcdCtrl.setPjdfHandle(hLcd);
    lcdCtrl.begin();

    DrawLcdContents();
    PrintWithBuf(buf, BUFSIZE, "Initializing touch panel controller\n");
    touchCtrl.setPjdfHandle(hI2C);
    
    PrintWithBuf(buf, BUFSIZE, "Initializing FT6206 touchscreen controller\n");
    if (! touchCtrl.begin(40)) {  // pass in 'sensitivity' coefficient
        PrintWithBuf(buf, BUFSIZE, "Couldn't start FT6206 touchscreen controller\n");
        while (1);
    }
    
    int currentcolor = ILI9341_RED;

    while (1) { 
        boolean touched = false;
        // TODO: Poll for a touch on the touch panel
        // <Your code here>
        // <hint: Call a function provided by touchCtrl
        touched = touchCtrl.touched();

        if (! touched) {
            OSTimeDly(5);
            continue;
        }
        
        TS_Point rawPoint;
        // TODO: Retrieve a point  
        // <Your code here>
        rawPoint = touchCtrl.getPoint();
        
        if (rawPoint.x == 0 && rawPoint.y == 0)
        {
            continue; // usually spurious, so ignore
        }
        
        // transform touch orientation to screen orientation.
        TS_Point p = TS_Point();
        p.x = MapTouchToScreen(rawPoint.x, 0, ILI9341_TFTWIDTH, ILI9341_TFTWIDTH, 0);
        p.y = MapTouchToScreen(rawPoint.y, 0, ILI9341_TFTHEIGHT, ILI9341_TFTHEIGHT, 0);
        
        // find out which button got pressed
        ButtonPressed(p.x, p.y);
        
        lcdCtrl.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
    }
}
/************************************************************************************

   Runs MP3 demo code

************************************************************************************/
void Mp3DemoTask(void* pdata)
{
    PjdfErrCode pjdfErr;
    INT32U length;

    OSTimeDly(2000); // Allow other task to initialize LCD before we use it.
    
    char buf[BUFSIZE];
    PrintWithBuf(buf, BUFSIZE, "Mp3DemoTask: starting\n");

    PrintWithBuf(buf, BUFSIZE, "Opening MP3 driver: %s\n", PJDF_DEVICE_ID_MP3_VS1053);
    // Open handle to the MP3 decoder driver
    HANDLE hMp3 = Open(PJDF_DEVICE_ID_MP3_VS1053, 0);
    if (!PJDF_IS_VALID_HANDLE(hMp3)) while(1);

    PrintWithBuf(buf, BUFSIZE, "Opening MP3 SPI driver: %s\n", MP3_SPI_DEVICE_ID);
    // We talk to the MP3 decoder over a SPI interface therefore
    // open an instance of that SPI driver and pass the handle to 
    // the MP3 driver.
    HANDLE hSPI = Open(MP3_SPI_DEVICE_ID, 0);
    if (!PJDF_IS_VALID_HANDLE(hSPI)) while(1);

    length = sizeof(HANDLE);
    pjdfErr = Ioctl(hMp3, PJDF_CTRL_MP3_SET_SPI_HANDLE, &hSPI, &length);
    if(PJDF_IS_ERROR(pjdfErr)) while(1);

    // Send initialization data to the MP3 decoder and run a test
    PrintWithBuf(buf, BUFSIZE, "Starting MP3 device test\n");
    Mp3Init(hMp3);
    int count = 0;
    
//    const unsigned char* mysong = currentSong[0];
//    const unsigned char* mysong = Train_Crossing;
    INT32U song_length = sizeof(What_A_Man);
    
    while (1)
    {
//      INT32U currSong_length = (song_length << shift_stream) + song_length;
      INT32U currsong_section = 0.40 * song_length;
      OSTimeDly(500);
      int song_shift = 0;
      
      while (song_shift <= song_length) {
          if (song_status == 1) { // song is playing
            
            PrintWithBuf(buf, BUFSIZE, "Begin streaming sound file  count=%d\n", ++count);
//            Write(hMp3, (void*)BspMp3SetVol1010, &volume_level);
//            WriteVS10xxRegister(SCI_WRAMADDR, 0xc045); /* DAC_VOL*/
//            WriteVS10xxRegister(SCI_WRAM, 0x0101); 
            
            Mp3Stream(hMp3, (INT8U*)(What_A_Man + song_shift), currsong_section); 
            PrintWithBuf(buf, BUFSIZE, "Done streaming sound file  count=%d\n", count);
            
            if (song_length > (song_shift + currsong_section)) {
              song_shift = song_shift + currsong_section; 
            } else {
                song_shift = (song_length - song_shift) + song_shift; 
            }
            int song_percent = song_shift * 200 /song_length;
            
//            lcdCtrl.fillRoundRect(20,55,song_percent, 15, 2, ILI9341_MAROON);
//            if (song_shift + 1 >= song_length) 
//              lcdCtrl.fillRoundRect(20,55,200, 15, 2, ILI9341_OLIVE);
          } else if (song_status == 2) { // song paused 
            if (song_length > (song_shift + currsong_section)) {
//              song_shift = song_shift + currsong_section; 
            } else
              song_shift = 0;
          } else if (song_status == 0) { // song stopped
            song_shift = 0;
          }
          
          song_shift = 0;
      }
      
    }
}


// Renders a character at the current cursor position on the LCD
static void PrintCharToLcd(char c)
{
    lcdCtrl.write(c);
}

/************************************************************************************

   Print a formated string with the given buffer to LCD.
   Each task should use its own buffer to prevent data corruption.

************************************************************************************/
void PrintToLcdWithBuf(char *buf, int size, char *format, ...)
{
    va_list args;
    INT8U err;
    
    va_start(args, format);
//    OSSemPend(semLcdDriver, 0, &err);
    PrintToDeviceWithBuf(PrintCharToLcd, buf, size, format, args);
//    OSSemPost(semLcdDriver);
    va_end(args);
}



