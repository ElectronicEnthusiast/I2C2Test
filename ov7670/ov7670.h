/*
 * ov7670.h
 *
 *  Created on: 2017/08/25
 *      Author: take-iwiw
 */

#ifndef OV7670_OV7670_H_
#define OV7670_OV7670_H_

#include <stdint.h>



#define RET         uint32_t
#define RET_OK      0
#define RET_ERROR   1


//Reading image data using parallel bus, but using GPIO in software
//https://www.youtube.com/watch?v=gp0FxbEmRSw
//OV7670 usage with DMA and external flash memory on TFT display
//https://github.com/iwatake2222/DigitalCamera_STM32

#define REG_BATT 0xFF
//#define SLAVE_ADDR 0x42
#define SLAVE_ADDR 0x21
#define SLAVE_ADDRESS SLAVE_ADDR
    //https://forum.arduino.cc/index.php?topic=535819.0
//But the I2C functions need the address without the read/write bit because they handle the read/write adresses automatically.
//So if we shift the bits of the read/write addresses to the right once (1000010 >> 1) we will get the address without the read/write bit: 0x21 (1000010 >> 1 = 0100001).
//So when I write the address 0x21 in the I2C functions it works!
#define Slave_Word_address 0x0A
#define reg Slave_Word_address

#define OV7670_WIDTH  320
#define OV7670_HEIGHT 240

#define own_add 0x20
////#define slv_add 0x10
////#define bsize    16

#define DATA_COUNT  1
#define RCV_DATA_COUNT  1

#define D7_Port     gioPORTB
#define D7_Pin      3
#define D6_Port     gioPORTB
#define D6_Pin      2
#define D5_Port     gioPORTA
#define D5_Pin      7
#define D4_Port     gioPORTA
#define D4_Pin      6
#define D3_Port     gioPORTA
#define D3_Pin      5
#define D2_Port     gioPORTA
#define D2_Pin      2
#define D1_Port     gioPORTA
#define D1_Pin      1
#define D0_Port     gioPORTA
#define D0_Pin      0

#define Vsync_Port      gioPORTB
#define Vsync_Pin       3
#define Href_Port       gioPORTB
#define Href_Pin        2
#define PixClk_Port     gioPORTA
#define PixClk_Pin      3


//OV7670 Connections        |       TI RM57L Pin Connections
//Reset-->10kohm-->3.3V     |
//MCLK                      |       J9.19 (ECLK1) (hardware PLL clk output for camera 15MHz clock source)
//SCL                       |       J10.8 (I2C2, SCK)
//SDA                       |       J10.9 (I2C2, SDA)



//RET ov7670_init(DCMI_HandleTypeDef *p_hdcmi, DMA_HandleTypeDef *p_hdma_dcmi, I2C_HandleTypeDef *p_hi2c);
RET ov7670_init(void);
RET ov7670_config(uint32_t mode);
//RET ov7670_startCap(uint32_t capMode, uint32_t destAddress);
RET ov7670_startCap(uint16_t *destAddress);
RET ov7670_stopCap();
//void ov7670_registerCallback(void (*cbHsync)(uint32_t h), void (*cbVsync)(uint32_t v));
RET ov7670_write(uint8_t regAddr, uint8_t data);
RET ov7670_read(uint8_t regAddr, uint8_t *arrBuff);



#endif /* OV7670_OV7670_H_ */
