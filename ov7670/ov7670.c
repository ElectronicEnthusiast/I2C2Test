/*
 *      Author: original from take-iwiw (used stm32F4 with dedicated hardware caera interface)
 *              stm32 may run at 150MHz fastest speed
 *      This file will test software for the Texas Instruments RM57L launchpad
 *              RM57L microcontroller may run at 330MHz fastest speed
 */
//#include <stdio.h>
#include "ov7670.h"
#include "ov7670Reg.h"
//Specific libraries for RM57L for GPIO and I2C peripherals
//#include "HL_gio.h"
//#include "HL_i2c.h"
////#include <stdint.h>
//#include "bme280.h"
#include "HL_gio.h"
#include "HL_i2c.h"
//#include "bme280.h"
#include "HL_sys_core.h"

//Reading image data using parallel bus, but using GPIO in software
//https://www.youtube.com/watch?v=gp0FxbEmRSw

//OV7670 usage with DMA and external flash memory on TFT display
//https://github.com/iwatake2222/DigitalCamera_STM32

//Hacking OV7670 camera sensor to get grayscale output image
//  (using YUV format and keeping the "Y" portion as the grayscale value)
//http://embeddedprogrammer.blogspot.com/2012/07/hacking-ov7670-camera-module-sccb-cheat.html


//LibJPEG library obtained from this address and is using the LibJPEG-9c release version (01/14/2018)
//https://www.ijg.org/



///*** Internal Static Variables ***/
//static void (* s_cbHsync)(uint32_t h);
//static void (* s_cbVsync)(uint32_t v);
//static uint32_t s_currentH;
//static uint32_t s_currentV;



//create aray to store raw pixel values from ov7670 camera sensor (before compressed via JPEG)
uint8_t pixelDataArray[OV7670_WIDTH * OV7670_WIDTH * 2];




RET ov7670_VerifyI2Cworking(void)
{
    //read registers from ov7670 sensor (page 12 in 0v7670 datasheet)
    //Address     Value       Description
    //0x0A        0x76        Product ID MSB (read only)
    //0x0B        0x73        Product ID LSB (read only)

    RET retVal1 = RET_OK;

    return retVal1;
}




RET ov7670_init(void)
{
//  sp_hdcmi     = p_hdcmi;
//  sp_hdma_dcmi = p_hdma_dcmi;
//  sp_hi2c      = p_hi2c;
//  s_destAddressForContiuousMode = 0;
//
//  HAL_GPIO_WritePin(CAMERA_RESET_GPIO_Port, CAMERA_RESET_Pin, GPIO_PIN_RESET);
//  HAL_Delay(100);
//  HAL_GPIO_WritePin(CAMERA_RESET_GPIO_Port, CAMERA_RESET_Pin, GPIO_PIN_SET);
//  HAL_Delay(100);

//  ov7670_write(0x12, 0x80);  // RESET
//  HAL_Delay(30);
//
//  uint8_t buffer[4];
//  ov7670_read(0x0b, buffer);
//  printf("[OV7670] dev id = %02X\n", buffer[0]);


  return RET_OK;
}

RET ov7670_config(uint32_t mode)
{
  ov7670_stopCap();
  ov7670_write(0x12, 0x80);  // RESET


//  //Delay 30ms
////  HAL_Delay(30);
//  for(i=0; i<100000; i++);
//
//
//  for(int i=0; OV7670_reg[i][0] != REG_BATT; i++)
//  {
//    ov7670_write(OV7670_reg[i][0], OV7670_reg[i][1]);
//    HAL_Delay(1);
//  }

  return RET_OK;
}



////////////////REFERENCING:
//http://embeddedprogrammer.blogspot.com/2012/07/hacking-ov7670-camera-module-sccb-cheat.html/////////////////
//Pixel "Y" data is the grayscale component
//Each Pixel is 16bits with YUV/YCrCb format normally
//Grayscale will "omit" the "Cr" and "Cb" bits
//  "Cb" and "Cr" portions are 4 bits each (Cr&Cb are shared on every two pixels)
//  1Pixel == [Y] + {Cr+Cb} == [8bits] + {4+4} bits == 16bitsPerPixel
//Byte      1       2       3       4       5       6       7       8
//Data      Cb0     Y0      Cr0     Y1      Cb2     Y2      Cr2     Y3
//Pixel     0&1     0       0&1     1       2&3     2       2&3     3

//capture single image frame
RET ov7670_startCap(uint16_t *destAddress)
{
  ov7670_stopCap();
  uint32 VColIndex = 0;
  uint32 HRowIndex = 0;
  uint32 numPixels = OV7670_WIDTH * OV7670_HEIGHT;
  uint32 VsyncPinVal = 1; //low when image frame is valid
  uint32 HrefPinVal = 0; //Href also means Hsync
                          //Href is when each row inside image frame is valid data

  //wait for new image frame
  while(VsyncPinVal != 0)
  {
      VsyncPinVal = gioGetBit(Vsync_Port, Vsync_Pin);
  }

  //NOTE: Vsync is each image frame and Href is sent on each row
  for(VColIndex=0; VColIndex<OV7670_HEIGHT; VColIndex++)
  {
      //wait for new pixel row
      while(HrefPinVal != 1)
      {
          HrefPinVal = gioGetBit(Href_Port, Href_Pin);
      }

      //NOTE: PixClk (PCLK) is always oscillating and is zero on every pixel
      for(HRowIndex=0; HRowIndex<OV7670_WIDTH; HRowIndex++)
      {
          uint32 PixClkPinVal = 0;

          //wait for new Pixel Clock Falling Edge
          //    This is the first Cb byte that needs to be ignored
          while(PixClkPinVal != 0)
          {
              PixClkPinVal = gioGetBit(PixClk_Port, PixClk_Pin);
          }
          //wait for pixel data to be invalid again
          while(PixClkPinVal != 1)
          {
              PixClkPinVal = gioGetBit(PixClk_Port, PixClk_Pin);
          }
          //wait for new Pixel Clock Falling Edge
          //    This is the first Y byte that is the grayscale data
          while(PixClkPinVal != 0)
          {
              PixClkPinVal = gioGetBit(PixClk_Port, PixClk_Pin);
          }

          //Read every even number byte
          uint8 bitArray[8];
          bitArray[7] = (uint8)gioGetBit(D7_Port, D7_Pin);
          bitArray[6] = (uint8)gioGetBit(D6_Port, D6_Pin);
          bitArray[5] = (uint8)gioGetBit(D5_Port, D5_Pin);
          bitArray[4] = (uint8)gioGetBit(D4_Port, D4_Pin);
          bitArray[3] = (uint8)gioGetBit(D3_Port, D3_Pin);
          bitArray[2] = (uint8)gioGetBit(D2_Port, D2_Pin);
          bitArray[1] = (uint8)gioGetBit(D1_Port, D1_Pin);
          bitArray[0] = (uint8)gioGetBit(D0_Port, D0_Pin);

          uint8 pixelValue=0, i=0;
          for(i=0; i<8; i++)
          {
              pixelValue = pixelValue + (bitArray[i]<<i);
          }

          pixelDataArray[VColIndex*HRowIndex] = pixelValue;
      }
  }



//  write_JPEG_file (char * filename, int quality);



  return RET_OK;
}



RET ov7670_stopCap()
{

//  HAL_DCMI_Stop(sp_hdcmi);

    return RET_OK;
}


//DELETE THIS????????????
//void ov7670_registerCallback(void (*cbHsync)(uint32_t h), void (*cbVsync)(uint32_t v))
//{
//  s_cbHsync = cbHsync;
//  s_cbVsync = cbVsync;
//}

//void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
//{
////  printf("FRAME %d\n", HAL_GetTick());
//  if(s_cbVsync)s_cbVsync(s_currentV);
//  if(s_destAddressForContiuousMode != 0) {
////    HAL_DMA_Start_IT(hdcmi->DMA_Handle, (uint32_t)&hdcmi->Instance->DR, s_destAddressForContiuousMode, OV7670_QVGA_WIDTH * OV7670_QVGA_HEIGHT/2);
//  }
//  s_currentV++;
//  s_currentH = 0;
//}

//void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi)
//{
////  printf("VSYNC %d\n", HAL_GetTick());
////  HAL_DMA_Start_IT(hdcmi->DMA_Handle, (uint32_t)&hdcmi->Instance->DR, s_destAddressForContiuousMode, OV7670_QVGA_WIDTH * OV7670_QVGA_HEIGHT/2);
//}
//
////void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
////{
//////  printf("HSYNC %d\n", HAL_GetTick());
////  if(s_cbHsync)s_cbHsync(s_currentH);
////  s_currentH++;
////}





//From Reference Material:
//https://arduino.stackexchange.com/questions/24645/accessing-configuration-registers-of-ov7670-cam-on-i2c
//Wire.begin();
//Wire.beginTransmission(0x21);
//Wire.write(0x12);  // software reset
//Wire.write(0x80);
//Wire.endTransmission();
//delay(500); // wait for reset to complete
//
//Wire.beginTransmission(0x21);
//Wire.write(0x01);  // u now read from reg 0x01
//Wire.endTransmission();
//Wire.requestFrom(0x21, 1);
//while (Wire.available() == 0);  //block till u get something
//while (Wire.available())
//  Serial.println(Wire.read(), HEX);



/*** Internal Function Defines ***/
RET ov7670_write(uint8_t regAddr, uint8_t data)
{
    RET retVal1 = RET_OK;

    uint8  TX_PACK[2];
    uint8  *t_buff = &TX_PACK[0];
    TX_PACK[0] = regAddr;
    TX_PACK[1] = data;
    uint8 buf_size = 2;
    uint32 i = 0;

    /* i2c initialization         */
    i2cInit();

    /* set i2c own address        */
    i2cSetOwnAdd(i2cREG1,own_add);

    /* enable internal loopback   */
    i2cEnableLoopback(i2cREG1);

    /* Initiate Start condition for Transmission  */
    i2cSetStart(i2cREG1);

    /* send data packets          */
    while(buf_size--)
    {
       i2cSendByte(i2cREG1,*t_buff++);
       for(i=0;i<1000000;i++);
    }

    /* Delay to wait for all bytes to be received. */
    for(i=0;i<1000000;i++);

    /* Clear Stop Condition detect flag  */
    i2cClearSCD(i2cREG1);

    return retVal1;
}




RET ov7670_read(uint8 regAddr, uint8 *arrBuff)
{
    RET retVal1 = RET_OK;

    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(i2cREG2, SLAVE_ADDRESS);

    /* Set direction to Transmitter */
    /* Note: Optional - It is done in Init */
    i2cSetDirection(i2cREG2, I2C_TRANSMITTER);

    /* Configure Data count */
    /* Data Count + 1 ( Word Address) */
    i2cSetCount(i2cREG2, DATA_COUNT);

    /* Set mode as Master */
    i2cSetMode(i2cREG2, I2C_MASTER);

        /* Set Stop after programmed Count */
        i2cSetStop(i2cREG2);

        /* Transmit Start Condition */
        i2cSetStart(i2cREG2);

    /* Send the Word Address */
    i2cSendByte(i2cREG2, regAddr);

    /* Wait until Bus Busy is cleared */
        while(i2cIsBusBusy(i2cREG2) == true);

    /* Wait until Stop is detected */
//        while(i2cIsStopDetected(i2cREG2) == 0);

//        /* Clear the Stop condition */
//        i2cClearSCD(i2cREG2);

//        /* Simple Delay before starting Next Block */
//        /* Depends on how quick the Slave gets ready */
        uint32 delay = 0;
        for(delay=0;delay<100000;delay++);


    ///////////////////////////////////////////////////////////////
    //        Master Receive Functionality                      //
    ///////////////////////////////////////////////////////////////

    /*****************************************/
    //// Start receving the data From Slave
    /*****************************************/

    /* wait until MST bit gets cleared, this takes
     * few cycles after Bus Busy is cleared */
    while(i2cIsMasterReady(i2cREG2) != true);

    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(i2cREG2, SLAVE_ADDRESS);

    /* Set direction to receiver */
    i2cSetDirection(i2cREG2, I2C_RECEIVER);

    /* Configure Data count */
    /* Note: Optional - It is done in Init, unless user want to change */
    i2cSetCount(i2cREG2, RCV_DATA_COUNT);

    /* Set mode as Master */
    i2cSetMode(i2cREG2, I2C_MASTER);

    /* Set Stop after programmed Count */
    i2cSetStop(i2cREG2);

    /* Transmit Start Condition */
    i2cSetStart(i2cREG2);

    /* Tranmit DATA_COUNT number of data in Polling mode */
    i2cReceive(i2cREG2, DATA_COUNT, arrBuff);

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(i2cREG2) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(i2cREG2) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(i2cREG2);



  return retVal1;
}



//https://www.youtube.com/watch?v=gp0FxbEmRSw1
//Read one pixel from the OV7670 camera and store as two bytes (RGB is 565-format and 5+6+5=16 bits per pixel)




//jpeg_free_large
//Removed files from LibJPEG-9c:
//Any files that were not ".txt", ".c", ".h", "imageFormat(JPG,BMP,etc...)", ".log" were removed (configs and makefiles were removed)
//jconfig.cfg
//djpeg.c
//cjpeg.c
//cdjpeg.h
//cdjpeg.c
//cderror.h
//jmemansi.c
//ckconfig.c
//jmemname.c
//jpegtran.c

//12/04/2019  11:17 PM            15,872 rdbmp.c
//12/04/2019  11:17 PM             7,102 rdcolmap.c
//12/04/2019  11:17 PM             1,310 rdgif.c
//12/04/2019  11:17 PM            14,981 rdjpgcom.c
//12/04/2019  11:17 PM            16,316 rdppm.c
//12/04/2019  11:17 PM            12,060 rdrle.c
//12/04/2019  11:17 PM            11,172 rdswitch.c
//12/04/2019  11:17 PM            15,761 rdtarga.c
//
//12/04/2019  11:17 PM            67,627 transupp.c
//12/04/2019  11:17 PM             9,784 transupp.h
//
//12/04/2019  11:17 PM            14,381 wrbmp.c
//12/04/2019  11:17 PM            13,320 wrgif.c
//12/04/2019  11:17 PM            17,780 wrjpgcom.c
//12/04/2019  11:17 PM             8,634 wrppm.c
//12/04/2019  11:17 PM             9,577 wrrle.c
//12/04/2019  11:17 PM             7,811 wrtarga.c
//changed example.c to be example.txt


//These files are required:
//10/24/2018  08:45 PM             6,453 jconfig_template.h
//03/13/2019  09:13 AM             4,237 jdata_conf_template.h
//10/24/2018  08:45 PM            17,145 jdct.h
//10/24/2018  08:45 PM            14,581 jerror.h
//03/13/2019  09:22 AM             6,563 jinclude.h
//10/24/2018  08:45 PM             8,264 jmemsys.h
//10/24/2018  08:45 PM            12,952 jmorecfg_template.h
//10/24/2018  08:45 PM            17,232 jpegint.h
//03/09/2019  11:04 PM            48,623 jpeglib.h
//10/24/2018  08:45 PM               396 jversion.h
//
//10/24/2018  08:45 PM             5,082 jaricom.c
//10/24/2018  08:45 PM             9,384 jcapimin.c
//10/24/2018  08:45 PM             5,881 jcapistd.c
//10/24/2018  08:45 PM            28,189 jcarith.c
//10/24/2018  08:45 PM            16,650 jccoefct.c
//10/24/2018  08:45 PM            15,634 jccolor.c
//10/24/2018  08:45 PM            15,773 jcdctmgr.c
//10/24/2018  08:45 PM            48,201 jchuff.c
//10/24/2018  08:45 PM             2,170 jcinit.c
//03/14/2019  06:21 PM             9,333 jcmainct.c
//10/24/2018  08:45 PM            17,726 jcmarker.c
//10/24/2018  08:45 PM            31,376 jcmaster.c
//10/24/2018  08:45 PM             3,110 jcomapi.c
//10/24/2018  08:45 PM            22,009 jcparam.c
//10/24/2018  08:45 PM            12,216 jcprepct.c
//10/24/2018  08:45 PM            19,923 jcsample.c
//10/24/2018  08:45 PM            13,891 jctrans.c
//10/24/2018  08:45 PM            12,707 jdapimin.c
//10/24/2018  08:45 PM             9,350 jdapistd.c
//10/24/2018  08:45 PM            23,942 jdarith.c
//03/13/2019  09:06 AM             8,611 jdatadst.c
//10/24/2018  08:45 PM             9,482 jdatasrc.c
//10/24/2018  08:45 PM            25,428 jdcoefct.c
//10/24/2018  08:45 PM            16,256 jdcolor.c
//10/24/2018  08:45 PM            12,402 jddctmgr.c
//10/24/2018  08:45 PM            48,374 jdhuff.c
//10/24/2018  08:45 PM            25,038 jdinput.c
//10/24/2018  08:45 PM            20,408 jdmainct.c
//10/24/2018  08:45 PM            42,592 jdmarker.c
//10/24/2018  08:45 PM            19,036 jdmaster.c
//10/24/2018  08:45 PM            13,916 jdmerge.c
//10/24/2018  08:45 PM             9,723 jdpostct.c
//10/24/2018  08:45 PM            11,968 jdsample.c
//10/24/2018  08:45 PM             5,053 jdtrans.c
//10/24/2018  08:45 PM             7,834 jerror.c
//10/24/2018  08:45 PM             6,008 jfdctflt.c
//10/24/2018  08:45 PM             7,980 jfdctfst.c
//10/24/2018  08:45 PM           158,678 jfdctint.c
//10/24/2018  08:45 PM             8,319 jidctflt.c
//10/24/2018  08:45 PM            13,170 jidctfst.c
//10/24/2018  08:45 PM           181,900 jidctint.c
//10/24/2018  08:45 PM            41,024 jmemmgr.c
//10/25/2018  07:03 PM             2,604 jmemnobs.c
//10/24/2018  08:45 PM            31,327 jquant1.c
//10/24/2018  08:45 PM            48,471 jquant2.c
//10/24/2018  08:45 PM             6,735 jutils.c

//not sure about these files
//10/24/2018  08:45 PM             6,502 jconfig.h
//08/08/2019  10:03 AM             4,181 jdata_conf.h
//10/24/2018  08:45 PM            12,837 jmorecfg.h
//07/31/2019  06:58 AM             4,169 libjpeg.h
