//*****************************Copyright(c)*************************** 
//** company：         AMAE 
//** project：         016    
//** hardware version：       
//**================================================================== 
//**-------------------------- file information-----------------------
//** file name:      bmp280.h
//** function :      
//** version  :      v0.1
//** author		:      zhangxu
//** data			:      2018.12.8
//**==================================================================
//====================================================================
//头文件
//====================================================================
#ifndef _BMP280_H
#define _BMP280_H
//====================================================================

/* 宏定义------------------------------------------------------------*/
#pragma pack()
#define BMP280_I2C_ADDRESS 			0x77  
#define BMP180_CHIP_ID					0x55
#define BMP280_CHIP_ID					0x58
#define BME280_CHIP_ID					0x60
#define BMP280_SOFT_RESET_VAL		0xB6


/* BMP280 specific registers */

#define BMP280_CHIPID_REG                0xD0  /*Chip ID Register */
#define BMP280_RESET_REG                 0xE0  /*Softreset Register */

#define BMP280_STATUS_REG                0xF3  /*Status Register */
#define BMP280_CTRLMEAS_REG              0xF4  /*Ctrl Measure Register */
#define BMP280_CONFIG_REG                0xF5  /*Configuration Register 配置时间*/

#define BMP280_PRESSURE_MSB_REG          0xF7  /*Pressure MSB Register */
#define BMP280_PRESSURE_LSB_REG          0xF8  /*Pressure LSB Register */
#define BMP280_PRESSURE_XLSB_REG         0xF9  /*Pressure XLSB Register */

#define BMP280_TEMPERATURE_MSB_REG       0xFA  /*Temperature MSB Reg */
#define BMP280_TEMPERATURE_LSB_REG       0xFB  /*Temperature LSB Reg */
#define BMP280_TEMPERATURE_XLSB_REG      0xFC  /*Temperature XLSB Reg */

#define BMP280_REG_HUMIDITY_LSB_REG			 0xFE
#define BMP280_REG_HUMIDITY_MSB_REG			 0xFD

#define BMP280_REG_CALIBREATE_REG			 	 0x88

#define BMP280_SLEEP_MODE								(0x00)
#define BMP280_FORCED_MODE							(0x01)
#define BMP280_NORMAL_MODE							(0x03)

#define BMP280_OVERSAMP_SKIPPED					(0x00)
#define BMP280_OVERSAMP_1X							(0x01)
#define BMP280_OVERSAMP_2X							(0x02)
#define BMP280_OVERSAMP_4X							(0x03)
#define BMP280_OVERSAMP_8X							(0x04)
#define BMP280_OVERSAMP_16X							(0x05)
/*bmp280 气压和温度过采样 工作模式*/
#define BMP280_PRESSURE_OSR					(BMP280_OVERSAMP_16X)
#define BMP280_TEMPERATURE_OSR			(BMP280_OVERSAMP_2X)
#define BMP280_MODE									(BMP280_PRESSURE_OSR<<2|BMP280_TEMPERATURE_OSR<<5|BMP280_NORMAL_MODE)
#define BMP280_MODE_FORCED					(BMP280_PRESSURE_OSR<<2|BMP280_TEMPERATURE_OSR<<5|BMP280_FORCED_MODE)

#define SHIFT_RIGHT_4_POSITION							 4
#define SHIFT_LEFT_2_POSITION                2
#define SHIFT_LEFT_4_POSITION                4
#define SHIFT_LEFT_5_POSITION                5
#define SHIFT_LEFT_8_POSITION                8
#define SHIFT_LEFT_12_POSITION               12
#define SHIFT_LEFT_16_POSITION               16



#define BMP280_FLOAT_TRUE_TEMP_16384	(16384.0)
#define BMP280_FLOAT_TRUE_TEMP_1024		(1024.0)
#define BMP280_FLOAT_TRUE_TEMP_131072	(131072.0)
#define BMP280_FLOAT_TRUE_TEMP_8192		(8192.0)
#define BMP280_FLOAT_TRUE_TEMP_5120		(5120.0)

#define	BMP280_FLOAT_TRUE_PRES_1		(1.0)
#define	BMP280_FLOAT_TRUE_PRES_0		(0.0)
#define	BMP280_FLOAT_TRUE_PRES_2		(2.0)
#define	BMP280_FLOAT_TRUE_PRES_4		(4.0)
#define	BMP280_FLOAT_TRUE_PRES_1_6		(16.0)
#define	BMP280_FLOAT_TRUE_PRES_64000	(64000.0)
#define	BMP280_FLOAT_TRUE_PRES_32768	(32768.0)
#define	BMP280_FLOAT_TRUE_PRES_65536	(65536.0)
#define	BMP280_FLOAT_TRUE_PRES_524288	(524288.0)
#define	BMP280_FLOAT_TRUE_PRES_1048576	(1048576.0)
#define	BMP280_FLOAT_TRUE_PRES_4096		(4096.0)
#define	BMP280_FLOAT_TRUE_PRES_6250		(6250.0)
#define	BMP280_FLOAT_TRUE_PRES_2147483648  (2147483648.0)
/* 类型变量 ---------------------------------------------------------*/
#pragma pack(1)
typedef struct BMP280_parameter
{
	unsigned short dig_T1;
	short dig_T2; 
	short dig_T3; 
	unsigned short dig_P1;
	short dig_P2;
	short dig_P3;
	short dig_P4; 
	short dig_P5;
	short dig_P6;
	short dig_P7;
	short dig_P8;
	short dig_P9;
	long t_fine;
}BMP280_parameter;

/* 私有变量 ---------------------------------------------------------*/
/* 扩展变量 ---------------------------------------------------------*/
typedef struct
{
	BMP280_parameter calib_param;                                        
	unsigned char chip_id;    
}bmp280_t;
/* 私有函数 ---------------------------------------------------------*/
//=====================================================================
void Bmp280_IIC_Init(void);
char CheckBmp180Ok(void);
long getCurEnvPress(void);
float getCurEnvTemp(void);
void Bmp280Task(void);
#endif

