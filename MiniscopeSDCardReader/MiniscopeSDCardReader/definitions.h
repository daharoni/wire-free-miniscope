//Which CMOS sensor configuration is being used
//#define EV76C454
#define EV76C454_SUBSAMP
//#define EV76C541


#define START_SECTOR			1024

#define WRITE_KEY0				0x0D7CBA17
#define WRITE_KEY1				0x0D7CBA17
#define WRITE_KEY2				0x0D7CBA17
#define WRITE_KEY3				0x0D7CBA17

#define HEADER_SECTOR			START_SECTOR - 1
#define HEADER_LENGTH			512
#define HEADER_GAIN_POS			4
#define HEADER_LED_POS			5
#define HEADER_NUM_FRAMES_POS	6

#define GAIN					2
#define LED_POWER				2

#ifdef EV76C541
	#define WIDTH					704/2
	#define HEIGHT					480/2
	#define NUM_SECTOR_PER_FRAME	165
	#define BYTE_PER_SECTOR			512
	#define FOOTER_WORD_LENGTH		4
#endif // EV76C541

#ifdef EV76C454
	#define WIDTH					752
	#define HEIGHT					480
	#define NUM_SECTOR_PER_FRAME	705
	#define BYTE_PER_SECTOR			512
	#define FOOTER_WORD_LENGTH		4
#endif // EV76C454

#ifdef EV76C454_SUBSAMP
	#define WIDTH					320
	#define HEIGHT					320
	#define NUM_SECTOR_PER_FRAME	200
	#define BYTE_PER_SECTOR			512
	#define FOOTER_WORD_LENGTH		4
#endif // EV76C454_SUBSAMP



#define DEFAULT_DRIVE			"PhysicalDrive1"
#define NUM_FRAMES				5
#define NUM_FRAMES_TO_CLEAR		2

#define MAX_FRAMES_PER_FILE		1000
