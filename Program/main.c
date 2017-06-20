#include "main.h"

#define TESTING

#ifdef TESTING
	#define PhNum	 "9980237552"
#else
	#define PhNum	 "9686016522"
#endif

#define LAT_DEVI		0.03000
#define LON_DEVI		0.03000

int8u EEMEM MAGIC_ADDRESS;
int8u EEMEM RANDOM_ADDRESS;

int8u EEMEM LAT_ADDR[11];
int8u EEMEM LON_ADDR[11];

int8u OTP[5];

int8u RandomNumber;

const char *MSG[] = { "Your OTP for unlocking is:",
					  "Locker trying to open. please take action"
					};

struct  {
	volatile int8u vibr:1;
	volatile int8u sw:1;
	volatile int8u found:1;
}AppFlags;

extern int8u lcdptr;

int main(void)
{
	init();

	AppFlags.found = 0;
	AppFlags.sw = 0;
	AppFlags.vibr = 0;

	while (TRUE) {

		if (!AppFlags.found) {
			TrackLoc();
			beep(1,75);
			lcdclrr(1);
		}

		if (AppFlags.vibr) {
			lcdclrr(1);
			lcdws("Vibra'n Occurred");
			beep(1,250);
			GSMsndmsg(PhNum, MSG[1]);
			AppFlags.vibr = 0;
			lcdclrr(1);
		}

		if (AppFlags.sw) {
			lcdclrr(1);
			lcdws("Stor'g Location");
			beep(1,100);
			StoreLoc();
			AppFlags.sw = 0;
			lcdclrr(1);

		}

	}
	return 0;
}
static void TrackLoc(void)
{
	int8u lat[11];
	int8u lon[11];

	int8u InVldCnt = 0;
	int8u pass[5];
	int8u i;

	for (i = 0; i < 10; i++)
		lat[i] = lon[i] = '0';

	lat[10] = lon[10] = '\0';

	lcdclrr(1);
	lcdws("Find'g Location");

	ReadLoc(lat,lon);

	if (CompLoc(lat,lon)) {
		lcdclrr(1);
		lcdws("Location Found");
		beep(1,100);
		dlyms(1000);
		sendOTP();
		AppFlags.found = 1;
CHK_PASS:
		lcdclr();
		lcdws(" ENTER PASSWORD");
		beep(1,100);
		read_pass(pass);
		switch(verpass(pass)) {
			case 1:
				beep(1,100);
				lcdws("    Verified");
				dlyms(1000);
				moton();
				lcdclr();
				lcdws(" Collect Items");
				dlyms(5000);
				motoff();
				dlyms(1000);
				InVldCnt = 0;
				lcdclr();
				lcdws("   Thank You");
				ledoff();
				cli();
				for(;;);
				break;
		default:
				buzon();
				lcdclr();
				lcdws("Wrong Password");
				beep(1,250);
				dlyms(500);
				buzoff();
				if (++InVldCnt >= MAX_CNT) {
					lcdclr();
					lcdws(" System Locked!");
					ledon();
					cli();
					for(;;);
				} else goto CHK_PASS;
				break;
		}
		
	}
}
static void moton(void)
{
	MOTOR_PORT |= _BV(MOT_ANODE_PIN);
	while (SW_PIN & _BV(TOP_SW_PIN));
	MOTOR_PORT &= ~_BV(MOT_ANODE_PIN);
}
static void motoff(void)
{
	MOTOR_PORT |= _BV(MOT_CATHODE_PIN);
	while (SW_PIN & _BV(BOT_SW_PIN));
	MOTOR_PORT &= ~_BV(MOT_CATHODE_PIN);
}
static int8u verpass (char  *pmsg)
{
	return (strcmp(OTP,pmsg)) ? 0:1;
	
}
static void read_pass(char  *pass_temp)
{
	int8u i;
	
	lcdr2();
	lcdwc(0xC6);
	for (i = 0; i < 4; i++) {  
		*pass_temp++ = get_data();	
		lcdwd('*');
		beep(1,75);
	}
	*pass_temp = '\0';

}
static void ReadLoc(int8u lat[], int8u lon[])
{
	GPSEn();
	dlyms(500);
	GPSgetloc(lat,lon);
}
static int8u CompLoc(int8u lat[], int8u lon[])
{
	double LatRef;
	double LonRef;
	double latD;
	double lonD;

	int8u latEE[11];
	int8u lonEE[11];

	int8u InVldCnt = 0;
	int8u pass[5];
	int8u i;

	for (i = 0; i < 10; i++)
		latEE[i] = lonEE[i] = '0';

	latEE[10] = lonEE[10] = '\0';

	ReadLatLon(latEE,lonEE);		/* Read REF from EEPROM */	

	LatRef = atof(latEE);
	LonRef = atof(lonEE);

	latD = atof(lat);
	lonD = atof(lon);

	if ((latD >= (LatRef - LAT_DEVI)) && (latD <= (LatRef + LAT_DEVI))) 
		if ((lonD >= (LonRef - LON_DEVI)) && (lonD <= (LonRef + LON_DEVI))) 
			return 1;
	return 0;
}	
static void StoreLoc(void)
{
	int8u lat[11];
	int8u lon[11];
	double latD;
	double lonD;

	int8u i;


	for (i = 0; i < 10; i++)
		lat[i] = lon[i] = '0';

	lat[10] = lon[10] = '\0';

	GPSEn();
	dlyms(500);
	GPSgetloc(lat,lon);

	latD = atof(lat);
	lonD = atof(lon);

	WriteLatLon(lat,lon);
}
static void init(void)
{
	buzinit();
	ledinit();

	GSM_GPS_DDR  |= _BV(GSM_GPS_PIN);
	GSM_GPS_PORT &= ~_BV(GSM_GPS_PIN);
	
	MOTOR_DDR	|= _BV(MOT_ANODE_PIN) | _BV(MOT_CATHODE_PIN);
	MOTOR_PORT	&= ~(_BV(MOT_ANODE_PIN) | _BV(MOT_CATHODE_PIN));
	
	SW_DDR  &= ~(_BV(TOP_SW_PIN) | _BV(BOT_SW_PIN));
	SW_PORT |= (_BV(TOP_SW_PIN) | _BV(BOT_SW_PIN));
				
	beep(2,100);
	lcdinit();
	uartinit();
	EXTINTinit();
	GSMEn();
	GSMinit();
	chkEEPROM();
	tmr1init();
	disptitl();
	sei();
	StartTmr();
	beep(1,100);
	
}
static void EXTINTinit(void)
{
	INTDDR 	&= ~_BV(INT0_PIN);
	INTPORT |= _BV(INT0_PIN);

	INTDDR 	&= ~_BV(INT1_PIN);
	INTPORT |= _BV(INT1_PIN);

	GICR |= _BV(INT0) | _BV(INT1);			//ENABLE EXTERNAL INTERRUPT
	MCUCR |= _BV(ISC01) | _BV(ISC11);		//FALLING EDGE INTERRUPT

}
static void sendOTP(void)
{
	int8u i, GSMmsg[40];
	
 	for(i = 0; i < 40; i++)
		GSMmsg[i] = '\0';
	
	GSMEn();
	genOTP();
	strcat(GSMmsg, MSG[0]);
	strcat(GSMmsg, OTP);
	GSMsndmsg(PhNum, GSMmsg);
}
static void chkEEPROM(void)
{
	
	int8u MagicNumber;
	int8u Temporaryvariable;

	#if EEPROM_DISPLAY > 0	           						  
		lcdclr();
		lcdws("CHECKING EEPROM");
		lcdr2();
		dlyms(1000);
	#endif

	MagicNumber = eeprom_read_byte (&MAGIC_ADDRESS); 		  
	if (MagicNumber == MAGIC_NO) {
		#if EEPROM_DISPLAY > 0	 
			lcdws("SUCCESSFULL.....");
			dlyms(1000);
		#endif
 		RandomNumber = eeprom_read_byte (&RANDOM_ADDRESS);	  
		Temporaryvariable = RandomNumber + 1;
		eeprom_write_byte(&RANDOM_ADDRESS, Temporaryvariable); 	
	} else  {  
		#if EEPROM_DISPLAY > 0
    		lcdws("  NEW EEPROM");
			dlyms(1000);
		#endif
		MagicNumber = MAGIC_NO;							
	    eeprom_write_byte(&MAGIC_ADDRESS, MagicNumber);		
		RandomNumber = 56;
	 	eeprom_write_byte(&RANDOM_ADDRESS, RandomNumber );   
	 } 
	#if EEPROM_DISPLAY > 0
		 lcdclr();
	#endif
}

static void genOTP(void)
{
	int8u i;
	OTP[4] = '\0';

	srand (RandomNumber);
	
	#if OTP_DISPLAY > 0
		lcdclr();
		lcdws("GENERATING......");
		lcdr2();
		lcdws("PASSWORD:");
	#else
		dlyms(500);
	#endif
	
	for (i = 0; i < 4; i++){
		OTP[i] = '0' + (rand() % 10);
		#if OTP_DISPLAY > 0
			LCDWriteData(OTP[i]);
		#endif
	}
	#if OTP_DISPLAY > 0
		dlyms(1000);
		lcdclr();
	#endif
}
static void disptitl(void)
{
	lcdclrr(0);
	lcdws("Loc'n B'd unLock");
}
		
static void tmr1init(void)
{
	TCNT1H   = 0xD3;
	TCNT1L   = 0x00;
	TIMSK   |= _BV(TOIE1);			//ENABLE OVERFLOW INTERRUPT
	TCCR1A   = 0x00;					
	TCCR1B  |= _BV(CS10) | _BV(CS11); /* PRESCALAR BY 16 */
}

/* overflows at every 100msec */
ISR(TIMER1_OVF_vect) 
{ 
	static int8u i,j,k;

	TCNT1H = 0xD3;
	TCNT1L = 0x00;
	
	if (++i >= 50) i = 0;
	switch(i) {
		case 0: case 2: ledon(); break;
		case 1: case 3: ledoff(); break;
	} 
}
ISR(INT0_vect) 
{ 
	AppFlags.vibr = 1;
	GICR |= _BV(INT0);
}
ISR(INT1_vect) 
{ 

	AppFlags.sw = 1;
	GICR |= _BV(INT1);
}
void WriteLatLon(int8u lat[], int8u lon[])
{
	eeprom_update_block ((const void *)lat, (void *)LAT_ADDR, 11);
	eeprom_update_block ((const void *)lon, (void *)LON_ADDR, 11);
}
void ReadLatLon(int8u lat[], int8u lon[])
{
	eeprom_read_block ((void *)lat , (const void *)LAT_ADDR, 11) ;
	eeprom_read_block ((void *)lon, (const void *)LON_ADDR, 11) ;
		
}


