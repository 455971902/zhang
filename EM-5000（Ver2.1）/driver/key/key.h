

#ifndef _KEY_H_

#define _KEY_H_


#define UPKEY     0X02
#define DOWNKEY   0X04
#define LEFTKEY   0X08
#define RIGHTKEY  0X10
#define CENTERKEY 0X20
#define CANCELKEY 0x40

#define KEY_NONE  0X40
#define KEY_POWER 0x1f

//#define PAUSE     0XFF
#define LOCK_KEY   0X4F
#define UNLOCK_KEY    0X5F

#define LENTERKEY   0XCE
//#define CANCELKEY   0XBE

void initKey(void);

void scanKey(void);    //¶ÁÈ¡¼üÖµº¯Êý

char getKey(void);

void clrKeyBuf(void);

#endif