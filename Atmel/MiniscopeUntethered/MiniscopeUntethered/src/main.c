/**
 * \file
 *
 * \brief Empty user application template
 *
 */

 //Untethered code for E2V Jade CMOS sensor
#include <asf.h>
#include "sdImaging.h"
#include "time_tick.h"


// ----- GPIO Definitions
#define LED_PIN		PIO_PD1_IDX //Red LED on PCB

#define STANDBY_PIN PIO_PB0_IDX //For E2V Jade Sensor
#define RESETB_PIN	PIO_PB1_IDX //For E2V Jade Sensor
#define TRIG_PIN	PIO_PB2_IDX //For E2V Jade Sensor

#define ENT_PIN		PIO_PB12_IDX //LED Driver enable

// ----- DAC Stuff that can be removed once PWM is made
#define DACC_WPKEY	0x444143
#define DACC_PIN	PIO_PD0_IDX

// ----- PWM Stuff 
//PWM0 PWMC0_PWMH0 PA0 A
#define PWM_LED_PIN		 PIO_PA0_IDX
#define PWM_LED_MODE	 IOPORT_MODE_MUX_A
#define PWM_LED_CHANNEL  PWM_CHANNEL_0
#define PWM_FREQUENCY	 1000
#define PWM_PERIOD_VALUE 255	
#define INIT_DUTY_VALUE  5

// ---- TWI Definitions
#define TWCK_PIN	PIO_PB5_IDX
#define TWCK_MODE	IOPORT_MODE_MUX_A
#define TWD_PIN		PIO_PB4_IDX
#define TWD_MODE	IOPORT_MODE_MUX_A

// ----- Bit Bang Spi ----
#define SPI_BB_MOSI_PIN TWD_PIN
#define SPI_BB_SCK_PIN	TWCK_PIN

//------- DACC for Channel 1 ---------------------------
//void DACC_init();
//void DACC_updateOutput(uint32_t value,uint32_t channel);
//------------------------------------------------------

//------ TWIHS -----------------------------------------
//void TWIHS_init();
//------------------------------------------------------

volatile uint32_t time_ms;
volatile uint32_t tick_start;
volatile uint32_t sdCount = 0;
volatile uint32_t sdCount2 = 0;
volatile uint32_t valueLED = 0; //PWM duty value
volatile uint32_t test[128];

//Holds PWM channel information
pwm_channel_t g_pwm_channel_led;

void PWM_Init() {
	pmc_enable_periph_clk(ID_PWM0);

	matrix_set_system_io(matrix_get_system_io() | CCFG_SYSIO_SYSIO12);
	ioport_set_pin_mode(PWM_LED_PIN,PWM_LED_MODE); //Sets GPIO PIN to PWM Peripheral
	ioport_disable_pin(PWM_LED_PIN);

	pwm_channel_disable(PWM0, PWM_LED_CHANNEL);
	
	pwm_clock_t clock_setting = {
		.ul_clka = PWM_FREQUENCY * PWM_PERIOD_VALUE,
		.ul_clkb = 0,
		.ul_mck = sysclk_get_cpu_hz()/2 //Might need or not need to divide by 2 to get the correct clock
	};

	pwm_init(PWM0, &clock_setting);

	/* Initialize PWM channel for LED0 */
	/* Period is left-aligned */
	g_pwm_channel_led.alignment = PWM_ALIGN_LEFT;
	/* Output waveform starts at a high level */
	g_pwm_channel_led.polarity = PWM_HIGH;
	/* Use PWM clock A as source clock */
	g_pwm_channel_led.ul_prescaler = PWM_CMR_CPRE_CLKA;
	/* Period value of output waveform */
	g_pwm_channel_led.ul_period = PWM_PERIOD_VALUE;
	/* Duty cycle value of output waveform */
	g_pwm_channel_led.ul_duty = INIT_DUTY_VALUE;
	g_pwm_channel_led.channel = PWM_LED_CHANNEL;
	pwm_channel_init(PWM0, &g_pwm_channel_led);

	/* Disable channel counter event interrupt */
	pwm_channel_disable_interrupt(PWM0, PWM_LED_CHANNEL, 0);
	//Enable PWM output
	pwm_channel_enable(PWM0,PWM_LED_CHANNEL);
	
}
void DACC_init() {
	pmc_enable_periph_clk(ID_DACC);
	DACC->DACC_WPMR = DACC_WPKEY<<DACC_WPMR_WPKEY_Pos; //Disables write protection of DAC
	DACC->DACC_CR = DACC_CR_SWRST; //resets the DAC
	DACC->DACC_MR = DACC_MR_MAXS0_TRIG_EVENT|DACC_MR_WORD_DISABLED|DACC_MR_DIFF_DISABLED|DACC_MR_PRESCALER(0);//After reset DACC_MR is set to 0x0000
	DACC->DACC_TRIGR = 0x0000; //Puts DAC in free-running mode. Should already be set to 0x0000 after reset but doing it anyways
	DACC->DACC_CHER = DACC_CHER_CH1; //Enables channel 1 of DACC
	while ((DACC->DACC_CHSR & DACC_CHSR_DACRDY1) != DACC_CHSR_DACRDY1) {
		//wait for DAC channel to be ready to receive data
	}

}

void DACC_updateOutput(uint32_t value,uint32_t channel) {
	while ((DACC->DACC_ISR & DACC_ISR_TXRDY1) != DACC_ISR_TXRDY1) {
		//wait for DACC to be ready to receive new data
	}
	DACC->DACC_CDR[channel] = value&0x0FFF;
}

void TWIHS_init() { //Make sure you have correct TWIHS 
	twihs_options_t twihsOpt;
	twihsOpt.master_clk = sysclk_get_cpu_hz()/2; //make sure this is the correct clock to be checking
	twihsOpt.speed = 100000; //100KHz
	
	//matrix_set_system_io(matrix_get_system_io() | CCFG_SYSIO_SYSIO4 | CCFG_SYSIO_SYSIO5);
	//pmc_enable_periph_clk(ID_PIOB);
	//ioport_set_pin_dir(TWCK_PIN, IOPORT_DIR_INPUT);
	////ioport_set_pin_level(TWCK_PIN, 0);
	//ioport_set_pin_dir(TWD_PIN, IOPORT_DIR_INPUT);
	////ioport_set_pin_level(TWD_PIN, 0);
	//while(1) {
		////tick_start = time_tick_get();
		////while (time_tick_calc_delay(tick_start, time_tick_get()) <10) {}
		////ioport_toggle_pin_level(TWCK_PIN);
		////ioport_toggle_pin_level(TWD_PIN);
//
	//}

	matrix_set_system_io(matrix_get_system_io() | CCFG_SYSIO_SYSIO4 | CCFG_SYSIO_SYSIO5);
	ioport_set_pin_mode(TWCK_PIN,TWCK_MODE);
	ioport_disable_pin(TWCK_PIN);
	ioport_set_pin_mode(TWD_PIN,TWD_MODE);
	ioport_disable_pin(TWD_PIN);

	pmc_enable_periph_clk(ID_TWIHS1);

	twihs_master_init(TWIHS1,&twihsOpt);
	//twihs_enable_master_mode(TWIHS1);
}

void SPI_BitBag_init() {
	pmc_enable_periph_clk(ID_PIOB);
	matrix_set_system_io(matrix_get_system_io() | CCFG_SYSIO_SYSIO4 | CCFG_SYSIO_SYSIO5);
	ioport_set_pin_dir(SPI_BB_MOSI_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(SPI_BB_MOSI_PIN, 0);
	ioport_set_pin_dir(SPI_BB_SCK_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(SPI_BB_SCK_PIN, 0);
}

int main (void)
{

	uint32_t writeNum = 0;
	uint32_t writeFrameNum = 0;
	WDT->WDT_MR = WDT_MR_WDDIS; //Disables WDT

	//SCB_EnableICache();
	//SCB_EnableDCache();

	irq_initialize_vectors();
	cpu_irq_enable();

	sysclk_init();
	board_init(); //Sets up sdcard slot pins
	ioport_init(); //turns on all peripheral clocks. could remove this to save power

	//----- CMOS Sensor Stuff
	enableSourceClk(); //Turns on SCK1 from MCU to drive CMOS sensor
	//

	time_tick_init();
	PWM_Init();
	//DACC_init(); //The DAC cannot run at 1.8V. This needs to be replaced with PWM to drive the LED current source
	//DACC_updateOutput(0x0FFF,1);
	#ifdef EV76C541
		SPI_BitBag_init();
	#else
		TWIHS_init();
	#endif
	//tick_start = time_tick_get();
	//time_ms = time_tick_calc_delay(tick_start, time_tick_get());

	//----- Enable control of LED
	pmc_enable_periph_clk(ID_PIOD);	
	ioport_set_pin_dir(LED_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_PIN, 0);
	//-----

	tick_start = time_tick_get();
	while (time_tick_calc_delay(tick_start, time_tick_get()) <100) {}

	//----- Enable control of enable pin for LED Driver
	pmc_enable_periph_clk(ID_PIOB);
	ioport_set_pin_dir(ENT_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(ENT_PIN, 0);

	//-----

	//----- Enable control of sensor control pins
	pmc_enable_periph_clk(ID_PIOB);

	ioport_set_pin_dir(RESETB_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(RESETB_PIN, 0); //not sure if this can stay high the whole time.

	tick_start = time_tick_get();
	while (time_tick_calc_delay(tick_start, time_tick_get()) <100) {}
	ioport_set_pin_level(RESETB_PIN, 1);

	ioport_set_pin_dir(STANDBY_PIN, IOPORT_DIR_OUTPUT);
	#ifdef EV76C541
		ioport_set_pin_level(STANDBY_PIN, 1); //Used for SPI chip select
	#else
		ioport_set_pin_level(STANDBY_PIN, 0); //Should be low for CMOS to acquire. standby bit at address = 0x0F needs to be set low to run
	#endif

	ioport_set_pin_dir(TRIG_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(TRIG_PIN, 0); //Should be high for CMOS to acquire or can be left 0 and controlled by TWI (address = 0x0F)
	//-----

	//pmc_switch_pck_to_mck(ID_PIOA, 1);


	//----- SD Init Stuff
	sd_mmc_init();
	//

	//--------------------------------------------Remove comments below after debugging I2C
	while (sd_mmc_check(SD_SLOT_NB) != SD_MMC_OK) {
		////Maybe turn LED off
	}
	//
//
	if (sd_mmc_get_type(SD_SLOT_NB) == (CARD_TYPE_SD|CARD_TYPE_HC)) {
	////This is the correct type of card (SDHC)
	//
	}
	//
	//-------------------------------------------------------------------------------------------
	uint32_t sdCapacity = sd_mmc_get_capacity(SD_SLOT_NB); //in KB
//
	////if (sd_mmc_is_write_protected(SD_SLOT_NB) == true){
	//////This is a problem. Maybe turn off the LED
	////}
	//-----

	//fillBuffer();//Used for iniitial testing of sd card writes
	
	////sd_mmc_init_write_blocks(SD_SLOT_NB,STARTING_BLOCK,NB_BLOCKS_PER_FRAME);
	//////-------------Loop this by numWritesPerFrame
	////sd_mmc_start_write_blocks(&imageBuffer[0],NB_BLOCKS_PER_FRAME);//NB_BLOCKS_PER_WRITE
	////sd_mmc_wait_end_of_write_blocks(true);
	////-------------
	////while (1) {
	////ioport_toggle_pin_level(LED_PIN);
	////	testPoint++;
	////	testPoint++;
	////}
	////----- CMOS Sensor Stuff
	////enableSourceClk(); //Turns on SCK1 from MCU to drive CMOS sensor

	imagingSensorLoadHeader();
	if (ledValue >= PWM_PERIOD_VALUE)
		ledValue = 0;
	pwm_channel_update_duty(PWM0, &g_pwm_channel_led, ledValue);

	imagingSensorSetup(); //sets interrupts, configs IO pins for DMA CMOS sensor

	#ifdef EV76C541
		imagingSensorConfigureEV76C541();
	#else
		imagingSensorConfigure(); //I2C config sensor
	#endif

	tick_start = time_tick_get();
	while (time_tick_calc_delay(tick_start, time_tick_get()) <5000) {}


	



	//ledValue = 5;
	//numFramesToRecord  =300;
	
	//DACC_updateOutput(ledValue,1);
//
	
	sd_mmc_init_write_blocks(SD_SLOT_NB,STARTING_BLOCK,50*NB_BLOCKS_PER_FRAME);	
	uint32_t curBlock = STARTING_BLOCK;
	uint32_t writeLineCount = 0;
	uint32_t writeCount = 0;
	tick_start = time_tick_get();
	startRecording = 1;
	ioport_set_pin_level(TRIG_PIN, 1); //Starts acq of imaging sensor
	//Enable PWM LED COntroll Driver
	ioport_set_pin_level(ENT_PIN, 1);
	ioport_set_pin_level(LED_PIN,1);
	while (1) {
		
		if (frameNumber > writeFrameNum) {
			
			#ifdef EV76C454
				imageBuffer[buffSize-2] = time_tick_calc_delay(tick_start, time_tick_get());
				sd_mmc_start_write_blocks(&imageBuffer[0*NB_BLOCKS_PER_WRITE*128],NB_BLOCKS_PER_WRITE*5);//NB_BLOCKS_PER_WRITE
			#endif

			#ifdef EV76C454_SUBSAMP
				switch (writeFrameNum%3)
				{
				case (0):
					imageBuffer0[buffSize-2] = time_tick_calc_delay(tick_start, time_tick_get());
					sd_mmc_start_write_blocks(&imageBuffer0[0],NB_BLOCKS_PER_WRITE);//NB_BLOCKS_PER_WRITE
					break;
				case (1):
					imageBuffer1[buffSize-2] = time_tick_calc_delay(tick_start, time_tick_get());
					sd_mmc_start_write_blocks(&imageBuffer1[0],NB_BLOCKS_PER_WRITE);//NB_BLOCKS_PER_WRITE
					break;
				case (2):
					imageBuffer2[buffSize-2] = time_tick_calc_delay(tick_start, time_tick_get());
					sd_mmc_start_write_blocks(&imageBuffer2[0],NB_BLOCKS_PER_WRITE);//NB_BLOCKS_PER_WRITE
					break;
				}				
			#endif

			#ifdef EV76C541
				switch (writeFrameNum%3)
				{
					case (0):
					imageBuffer0[buffSize-2] = time_tick_calc_delay(tick_start, time_tick_get());
					sd_mmc_start_write_blocks(&imageBuffer0[0],NB_BLOCKS_PER_WRITE);//NB_BLOCKS_PER_WRITE
					break;
					case (1):
					imageBuffer1[buffSize-2] = time_tick_calc_delay(tick_start, time_tick_get());
					sd_mmc_start_write_blocks(&imageBuffer1[0],NB_BLOCKS_PER_WRITE);//NB_BLOCKS_PER_WRITE
					break;
					case (2):
					imageBuffer2[buffSize-2] = time_tick_calc_delay(tick_start, time_tick_get());
					sd_mmc_start_write_blocks(&imageBuffer2[0],NB_BLOCKS_PER_WRITE);//NB_BLOCKS_PER_WRITE
					break;
				}			
			#endif

			sd_mmc_wait_end_of_write_blocks(false);
			writeFrameNum++;	
			sdImageWriteFrameNum = writeFrameNum;

			#ifdef EV76C541
				startRecording = 1;
			#endif
			#ifdef EV76C454_SUBSAMP
				//if (frameNumber>sdImageWriteFrameNum +1) {
					startRecording = 1;
				//}
			#endif
			#ifdef EV76C454
				startRecording = 1;
			#endif
			
			if (writeFrameNum%50 == 0) {
				curBlock+= 50*NB_BLOCKS_PER_FRAME;
				sd_mmc_init_write_blocks(SD_SLOT_NB,curBlock,50*NB_BLOCKS_PER_FRAME);
			}

		}


		//if ((frameNumber == writeFrameNum) && ((lineCount*IMAGE_HEIGHT) > ((writeNum+1)*NB_BLOCKS_PER_WRITE*512))){ //Writing frame as it is being acquired
			//sd_mmc_start_write_blocks(&imageBuffer[writeNum*NB_BLOCKS_PER_WRITE*128],NB_BLOCKS_PER_WRITE);//NB_BLOCKS_PER_WRITE
			//sd_mmc_wait_end_of_write_blocks(false);
			//writeNum++;		
		//}
		//if ((frameNumber > writeFrameNum) && (writeNum<4)){ //Writing frame after it has been acquired
			//sd_mmc_start_write_blocks(&imageBuffer[writeNum*NB_BLOCKS_PER_WRITE*128],NB_BLOCKS_PER_WRITE);//NB_BLOCKS_PER_WRITE
			//sd_mmc_wait_end_of_write_blocks(false);
			//writeNum++;
		//}
		//if ((frameNumber > writeFrameNum) && (writeNum==4)){ //Writing last block of frame
			//imageBuffer[buffSize-2] = time_tick_calc_delay(tick_start, time_tick_get());
			//sd_mmc_start_write_blocks(&imageBuffer[writeNum*NB_BLOCKS_PER_WRITE*128],NB_BLOCKS_PER_WRITE);//NB_BLOCKS_PER_WRITE
			//sd_mmc_wait_end_of_write_blocks(false);
			//writeFrameNum++;
			//startRecording = 1;
			//if (writeFrameNum%50 == 0) {
				//curBlock+= 50*NB_BLOCKS_PER_FRAME;
				//sd_mmc_init_write_blocks(SD_SLOT_NB,curBlock,50*NB_BLOCKS_PER_FRAME);
			//}
			//writeNum = 0;
		//}

		
		//test[(frameNumber-1)*4] = imageBuffer[0];
		//test[(frameNumber-1)*4+1] = imageBuffer[1];
		//test[(frameNumber-1)*4+2] = imageBuffer[2];
		//test[(frameNumber-1)*4+3] = imageBuffer[3];

		//for (writeNum=0;writeNum <5;writeNum++){
			//sd_mmc_start_write_blocks(&imageBuffer[writeNum*NB_BLOCKS_PER_WRITE*128],NB_BLOCKS_PER_WRITE);//NB_BLOCKS_PER_WRITE
			//sd_mmc_wait_end_of_write_blocks(false);
		//}
		//ioport_toggle_pin_level(LED_PIN);
		//ioport_toggle_pin_level(ENT_PIN);

		//valueLED = (valueLED+400)&0x0FFF; //higher value less bright
		//DACC_updateOutput(valueLED,1);

		//if (frameNumber>=numFramesToRecord){ //end at number of frames
		if (time_tick_calc_delay(tick_start, time_tick_get())>=numFramesToRecord*1000){
			//sd_mmc_init_write_blocks(SD_SLOT_NB,STARTING_BLOCK-1,1);
			//sd_mmc_start_write_blocks(&test[0],1);//NB_BLOCKS_PER_WRITE
			//sd_mmc_wait_end_of_write_blocks(false);
			ioport_set_pin_level(LED_PIN, 0);
			ioport_set_pin_level(ENT_PIN, 0);
			while(1){}
		}
//			startRecording = 1;


		testPoint = 0;	
					
	}
	
}