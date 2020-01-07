/** @file HL_sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com  
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */

#include "HL_sci.h"
#include "HL_gio.h"
#include "HL_i2c.h"
#include "HL_sys_core.h"

#include "ov7670.h"
#include "bme280.h"

/* USER CODE END */

/* Include Files */

#include "HL_sys_common.h"

/* USER CODE BEGIN (1) */
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */

//void millisDelay(unsigned int ms){
//    while (ms--) __delay_cycles(330000);
//} // 330MHz on RM57L

/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */

    sciInit();
    sciSend(sciREG1, 8,(unsigned char*)"Test4!\r\n");

    //toggle LED
    uint32 LEDinitVal1 = 1;
    uint32 LEDinitVal2 = 1;

    gioInit();
    gioSetBit(gioPORTB, 6, LEDinitVal1); //LED2
    gioSetBit(gioPORTB, 7, LEDinitVal2); //LED3

//    while(1)
//    {
//        LEDinitVal2^=1;
//        millisDelay(500);
//        gioSetBit(gioPORTB, 7, LEDinitVal2); //LED2
//
//        sciSend(sciREG1, 1, &count12);
//        count12++;
//    }


    //Testing I2C2
    //Address     Value       Description
    //0x0A        0x76        Product ID MSB (read only)
    //0x0B        0x73        Product ID LSB (read only)
    uint8  RX_PACK[4];
    uint8  *r_buff = &RX_PACK[0];

//    /* Enable CPU Interrupts - FIQ & IRQ */
//        _enable_interrupt_();



    ///////////////////////////////////////////////////////////////
    //        Master Receive Functionality                      //
    ///////////////////////////////////////////////////////////////
    //  // check if sensor, i.e. the chip ID is correct
    //  BME280_ADDRESS = (0x77)
    //  BME280_REGISTER_CHIPID = 0xD0,
    //  BME280_REGISTER_VERSION = 0xD1,
    //  BME280_REGISTER_SOFTRESET = 0xE0,
    //  _sensorID = read8(BME280_REGISTER_CHIPID);
    //  if (_sensorID != 0x60)
    //    return false;


//    /* Enable CPU Interrupts - FIQ & IRQ */
//    _enable_interrupt_();


        /* I2C Init as per GUI
         *  Mode = Master - Transmitter
         *  baud rate = 100KHz
         *  Count = 10
         *  Bit Count = 8bit
         */
        i2cInit();

//        ov7670_read(0xD0, r_buff);
        ov7670_write(0x12, 0x80);
        ov7670_read(0x0A, r_buff); //0x76 constant
        ov7670_read(0x0B, r_buff+1); //0x73 constant
        ov7670_read(0x1C, r_buff+2); //0x7F constant
        ov7670_read(0x1D, r_buff+3); //0xA2 constant




        while(1);


/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */
/* USER CODE END */
