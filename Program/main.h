#ifndef MAIN_H
#define MAIN_H

#include "includes.h"

#define GSM_GPS_DDR				DDRC
#define GSM_GPS_PORT			PORTC
#define GSM_GPS_PIN				PC4

#define INTDDR					DDRD
#define INTPORT					PORTD
#define INT0_PIN				PD2
#define INT1_PIN				PD3

#define MOTOR_DDR				DDRC
#define MOTOR_PORT				PORTC
#define MOT_ANODE_PIN			PC2
#define MOT_CATHODE_PIN			PC3

#define SW_DDR					DDRD
#define SW_PORT					PORTD
#define SW_PIN					PIND
#define TOP_SW_PIN				PD4
#define BOT_SW_PIN				PD5


//DEFINE CONSTANT
#define MAGIC_NO				96
#define MAX_CNT					3

//DEFINE MACROS
#define StartTmr()			TCCR0  	|= _BV(CS01)
#define StopTmr()			TCCR0  	&= ~_BV(CS01)


#define GSMEn()				do {									\
								GSM_GPS_PORT |= _BV(GSM_GPS_PIN); 	\
								dlyms(1000);						\
							} while(0)

#define GPSEn()				do {									\
							GSM_GPS_PORT &= ~_BV(GSM_GPS_PIN);		\
								dlyms(1000);						\
							} while(0)

//FUNCTION PROTOTYPES
static void		 init		(void);
static void 	 disptitl 	(void);
static void 	tmr1init	(void);
static void 	sendOTP		(void);
static void		 genOTP		(void);
static void 	chkEEPROM	(void);
static void 	EXTINTinit	(void);
static void 	StoreLoc	(void);
static void 	TrackLoc	(void);
static void 	read_pass	(char  *pass_temp);
static int8u 	verpass 	(char  *pmsg);
static void 	moton		(void);
static void 	motoff		(void);
static void 	WriteLatLon	(int8u lat[], int8u lon[]);
static void 	ReadLatLon	(int8u lat[], int8u lon[]);
static void 	ReadLoc		(int8u lat[], int8u lon[]);
static int8u 	CompLoc		(int8u lat[], int8u lon[]);

#endif
