
#define NOHRD
//#define SPRINTBOK
//#define BIKE
//#define CICLOTTE

#include <asf.h>
#include "avr/interrupt.h"
#include "avr\io.h"
#include "avr\sleep.h"
#include "stdio.h"
#include "enums.h"
#include "string.h"
#include <stdbool.h>
#include <stdint.h>
#include "AddStrings.h"
#include "math.h"
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <util/delay.h>
#ifdef BIKE
#ifndef CICLOTTE
//#include ""
#endif
#endif

////////////////// UNIQUE DEFINES //////////////////////
#ifdef BIKE
#define MAX_DISTANCE 65000
#define MIN_DISTANCE 100
#define MAX_TIME 60000
#define MIN_TIME 60
#define DISTANCE_GAIN_0 100
#define DISTANCE_GAIN_1 1000
#define DISTANCE_GAIN_2 1000
#define DISTANCE_GAIN_3 5000
#define DISTANCE_THRESH 10000


#define TIME_GAIN_0 1*60
#define TIME_GAIN_1 5*60
#define TIME_GAIN_2 10*60
#define TIME_GAIN_3 30*60
#define TIME_THRESH 60*60
#else
#define MAX_DISTANCE 100000
#define MIN_DISTANCE 100
#define MAX_TIME 60000
#define MIN_TIME 60
#define DISTANCE_GAIN_0 100
#define DISTANCE_GAIN_1 1000
#define DISTANCE_GAIN_2 1000
#define DISTANCE_GAIN_3 5000
#define DISTANCE_THRESH 10000

#define TIME_GAIN_0 1*60
#define TIME_GAIN_1 5*60
#define TIME_GAIN_2 10*60
#define TIME_GAIN_3 30*60
#define TIME_THRESH 60*60

#endif

//////////////// COMMON DEFINES /////////////////////////////
#ifdef NOHRD
#define BAUD0 9600
#define MY_UBRR0 129
#define MY_UBRR1 21

#define BUFFEROVERFLOWLIMIT 120
#define MAX_IMAGES_PER_PAGE 10
#define MAX_BTNS_PER_PAGE 8
#define MAX_TEXTS_PER_PAGE 7
#define FW_VERSION "FW 3.3.5\0"
#define GFIT_VERSION "0000000000 \0"
#define DFW_VERSION "DFW 1.0\0"
#define BASE_RMR 1568/(24*3600)
#define BAUD_RATE 9600
#define GFIT_MAX_LENGTH 22
#define WAIT_AFTER_RST_HIGH 1500
#define NO_OF_LANGUAGES 4

#define DISPLAYRXBUFFERSIZE 512

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#endif


#define RST_DIR_REG DDRB
#define RST_PORT PORTB
#define RST_PIN 0

#define DISPLAY_DIR_REG DDRA
#define DISPLAY_PORT PORTA
#define DISPLAY_PIN (1<<1)

#define DISPLAY_RST_DIR_REG DDRD
#define DISPLAY_RST_PORT PORTD
#define DISPLAY_RST_PIN (1<<5)

#define LED_DIR_REG DDRA
#define LED_PORT PORTA
#define LED_PIN (1<<4)

#define PBWAKE_DIR_REG DDRA
#define PBWAKE_PORT PORTA
#define PBWAKE_PIN (1<<2)
#define PBWAKE_DURATION 750

#define GFIT_DIR_REG DDRB
#define GFIT_PORT PORTB
#define GFIT_SLEEP_PIN (1<<1)
#define GFIT_RST_PIN (1<<0)
#define GFIT_ENABLE_PIN (1<<3)
#endif


#define POWER_SOURCE_BATTERY false
#define POWER_SOURCE_GRID true
#define CONTRAST_HIGH 15
#define CONTRAST_MID 11
#define CONTRAST_LOW 7
#define BATTERY_TIMEOUT 15*500
#define GRID_TIMEOUT 60*500

////////////////// TYPEDEFS /////////////////////////////
#ifdef NOHRD

typedef struct {
	uint8_t no_of_targets;
	uint8_t *adresses[4];
	uint8_t buffer[4][11];
	uint8_t idx;
	uint8_t max_idx;
	void (*function_pointer[3])(uint8_t);
} Ring_t;
typedef struct {
	uint8_t button_id;
	uint8_t action_id;
	uint8_t press_image_id;
	uint8_t image_id;
	uint8_t text_field_id;
	bool active;
	uint8_t translated_text_id;
	uint8_t button_text_id;
	uint8_t top_layer;
} Button_t;
typedef struct {
	uint8_t page_id;
	uint8_t Images[MAX_IMAGES_PER_PAGE];
	uint8_t No_Of_Images;
	uint8_t Translated_String_IDs[MAX_TEXTS_PER_PAGE];
	uint8_t Translated_String_Field_IDs[MAX_TEXTS_PER_PAGE];
	uint8_t Common_String_IDs[5];
	uint8_t Common_String_Field_IDs[5];
	uint8_t No_Of_Rings;
	Ring_t* Rings[3];
	bool Do_Buttons;
	uint8_t No_Of_Common_Strings;
	uint8_t No_Of_Translated_Strings;
	uint8_t No_Of_Buttons;
	uint8_t Button_ids[MAX_BTNS_PER_PAGE];
	uint8_t Button_press_image_ids[MAX_BTNS_PER_PAGE];
	uint8_t Button_action_ids[MAX_BTNS_PER_PAGE];
	uint8_t A_Command;
	uint8_t Gifs[2];
	uint8_t No_Of_Gifs;
	Button_t Buttons[];
	
} Page_t;

typedef struct {
	uint16_t size; // Die Größe des Ringbuffers
	volatile uint16_t start; // Der Index des ersten Elements
	volatile uint16_t end; // Der Index des letzten Elements
	volatile uint8_t buffer[DISPLAYRXBUFFERSIZE];
	
} ring_buffer_t;


typedef enum {
	PENDING,
	COLLECTING,
	FINISHED,
	ABORTED,
	STARTED
} parse_state_t;

// Define the message structure.
typedef struct {
	uint8_t prefix;
	uint8_t payload[40]; // Adjust size as needed.
	uint8_t suffix[4];
	uint8_t payload_length;
	uint8_t expected_length;
	bool complete;
} message_t;

#define PAYLOAD_ABORT_LENGTH 38

// Parser context structure.
typedef struct {
	parse_state_t state;
	message_t current_message;
	uint8_t payload_index;
} parser_context_t;


#endif

/////////////// COMMON VARIABLES ////////////////////
#ifdef NOHRD

//	q					0		1				2		3			4			5				6		7			8
char* Button_Strings[9]={"","+1 min ","+5 min ","-1 min ","-5 min ","+100 m ","+1000 m","-100 m ","-1000 m"};
//								0	1			2		3		4			5			6		7		8			9		10		   11		  12		13		  14			15			16		  17		18			19			  20		21		    22			    23
//char* Common_Strings[26]={"",DAP_VERSION,"5000"," 30 ","01:23","10.000k","0000","0123","1111","TI:ME ","CAD0 ","DIST ","RES ","CAL0 ","PACEGG ",FW_VERSION,DFW_VERSION,"ANT+ ","Bluetooth","Sensor   ","1234 ","AV:PA ","0:00:00 ","--k Projection   "};



#define STRING_LENGTH_GFIT_VERSION 12
#define STRING_LENGTH_DISTANCE_PRESELECT 5
#define STRING_LENGTH_TIME_PRESELECT 5
#define STRING_LENGTH_TIME_LEFT 6
#define STRING_LENGTH_DISTANCE_LEFT 8
#define STRING_LENGTH_WATTS 5
#define STRING_LENGTH_SPEED 5
#define STRING_LENGTH_HEARTRATE 5
#define STRING_LENGTH_TIME 7
#define STRING_LENGTH_CADENCE 6
#define STRING_LENGTH_DISTANCE 6
#define STRING_LENGTH_RESISTANCE 5
#define STRING_LENGTH_CALORIES 6
#define STRING_LENGTH_PACE 7
#define STRING_LENGTH_FIRMWARE_VERSION 10
#define STRING_LENGTH_DISPLAY_VERSION 8
#define STRING_LENGTH_ANT 6
#define STRING_LENGTH_BLUETOOTH 10
#define STRING_LENGTH_HRM_NAME 10
#define STRING_LENGTH_AVG_SPEED 6
#define STRING_LENGTH_AVG_PACE 7
#define STRING_LENGTH_PROJECTION 7
#define STRING_LENGTH_XXK_PROJECTION 15



char gfit_version[] = GFIT_VERSION;
char distance_preselect[] = "5000\0";
char time_preselect[] = " 30 \0";
char value_time_left[] = "01:23\0";
char value_distance_left[] = "10.000k\0";
char value_watts[] = "0000\0";
char value_speed[] = "0123\0";
char value_heartrate[] = "1111\0";
char value_time[] = "TI:ME \0";
char value_cadence[] = "CAD0 \0";
char value_distance[] = "DIST \0";
char value_resistance[] = "RES \0";
char value_calories[] = "CAL0 \0";
char value_pace[] = "PACEGG\0";
char version_firmware[] = FW_VERSION;
char version_display_fw[] = DFW_VERSION;
char ant[] = "ANT+ \0";
char bluetooth[] = "Bluetooth\0";
char hrm_name[] = "Sensor    \0";
char value_avg_speed[] = "1234 \0";
char value_avg_pace[] = "AV:PA \0";
char value_projection[] = "0:00:00  \0";
char xxk_projection[] = "--k Projection\0";

// Array of pointers to these character arrays
char* Common_Strings[] = {
	
	gfit_version,		//0
	distance_preselect,	//1
	time_preselect,		//2
	value_time_left,	//3
	value_distance_left,//4
	value_watts,		//5
	value_speed,		//6
	value_heartrate,	//7
	value_time,			//8
	value_cadence,		//9
	value_distance,		//10
	value_resistance,	//11
	value_calories,		//12
	value_pace,			//13
	version_firmware,	//14
	version_display_fw,	//15
	ant,				//16
	bluetooth,			//17
	hrm_name,			//18
	value_avg_speed,	//19
	value_avg_pace,		//20
	value_projection,	//21
	xxk_projection		//22
};

uint16_t DistanceGain = DISTANCE_GAIN_0, DistanceBigGain = DISTANCE_GAIN_1, TimeGain = TIME_GAIN_0, TimeBigGain = TIME_GAIN_1;
volatile bool toggler = false;
volatile uint8_t timer_correction = 0;
volatile bool do_send = false;
uint8_t pack1[] = {0x80,0x84};
uint8_t pack2[] = {0x80,0x85};
bool App_Darkmode = false;
unsigned char String_buffer[25];
unsigned char String_buffer2[25];
unsigned char String_buffer3[25];
volatile uint16_t ms_counter = 0;
bool time_blocked = false;
uint8_t block_time = 0;
uint8_t channel_mode = CHANNELMODE_BOTH;
bool inSession = false;
uint8_t DEBUGCTR = 0;
uint16_t b = 0;
uint8_t WatchDoggy = 0;
uint32_t IdleDoggy = 0;
uint16_t PBDoggy = 0, PBTurnOn = 2800, PBDuration = 250;
volatile bool deepsleep = false, wasAsleep = false;
uint16_t HRDoggy = 0;
uint16_t HRDoggyTimeOut = 10*500;
bool HRDevicePaired = false;
uint8_t App_Max_Brightness = 15;
uint8_t App_Brightness = 200;
uint8_t IdleState = IDLE_STATE_IDLE;
uint32_t timeout = 0;
uint8_t APP_STATE_CALLBACK = 0;
uint16_t tick_tock_timer,tick_tock_timeout = 0;
uint8_t ss_image = 0;
uint16_t rest;
uint32_t distanceTicks = 0;
double Interval = 0;
uint16_t projected_difference,projected_time;
bool onoff = true;
#ifdef BIKE
#ifdef CICLOTTE
const uint8_t gf1[] PROGMEM = {5, 0xe2, 0xe0, 25,(1<<0)|(1<<1),(1<<4)};
#else
const uint8_t gf1[] PROGMEM = {5, 0xe2, 0xe0, 25,0,(1<<4)};
#endif
const uint8_t gf2[] PROGMEM = {21,0xe2, 0xe1, 0x2a,0x01,0x01,0x2b,0x01,0x01,0x22,0x01,0x01,0x33,0x00,0x00,0x21,0x00,0x00,0x60,0x01,0x01,0x01};
#endif
#ifdef SPRINTBOK
const uint8_t gf1[] PROGMEM = {3, 0xe2, 0xe0, 19};
const uint8_t gf2[] PROGMEM = {9,0xe2, 0xe1, 0x60,0x01,0x01,0x01, 0x21, 0x00, 0x00};
#endif
const uint8_t gf3[] PROGMEM = {4,0xe2, 0xd5, 0xb6,0xa6};
const uint8_t gf4[] PROGMEM = {3,0xe2, 0xd2, 0x00};
const uint8_t gf5[] PROGMEM = {3,0xe2, 0xd2, 0x01};
const uint8_t gf6[] PROGMEM = {3,0xe2, 0xd2, 0x02};
const uint8_t gf7[] PROGMEM = {3,0xe2,0xe2,0xC8};
	 
const uint8_t gf8[] PROGMEM = {4,0xe2,0xd1,0x03,0b11111010};   //BLE
const uint8_t gf9[] PROGMEM = {4,0xe2,0xd1,0x03,0b11110101};   //ANT
const uint8_t gf10[] PROGMEM = {4,0xe2,0xd1,0x03,0b11101111};	//BOTH

const uint8_t gf11[] PROGMEM = {2,0x4a,0x00};
const uint8_t gf12[] PROGMEM = {4,0xe1,0xfe,0xe2,0xc0};

#ifdef BIKE
	#ifdef CICLOTTE
		const uint8_t gf13[] PROGMEM = {10,0xe2, 0xe4, 0x05, 0x4e, 0x4f, 0x48, 0x52, 0x44,0x4e,0x43};
	#else
		const uint8_t gf13[] PROGMEM = {10,0xe2, 0xe4, 0x05, 0x4e, 0x4f, 0x48, 0x52, 0x44,0x4e,0x42};
	#endif
#endif

#ifdef SPRINTBOK
const uint8_t gf13[] PROGMEM = {10,0xe2, 0xe4, 0x05, 0x4e, 0x4f, 0x48, 0x52, 0x44,0x53,0x50};
#endif
const uint8_t gf14[] PROGMEM = {18,0xe2, 0xe4, 0x06, 0x57, 0x61, 0x74, 0x65, 0x72, 0x72, 0x6f, 0x77, 0x65, 0x72, 0x20, 0x47, 0x6d, 0x62, 0x48};

const uint8_t gf15[] PROGMEM = {5,0xe2, 0xe4, 0x04, 0x03, 0x03};  //FIRMWARE VERSION STRING
const uint8_t gf16[] PROGMEM = {4,0xe2, 0xe4, 0x02, 0x03};        //HARDWARE REVISION
const uint8_t gf17[] PROGMEM = {2,0xe2,0xd7};
const uint8_t gf18[] PROGMEM = {3,0xe2,0xe5,0x00};
const uint8_t gf19[] PROGMEM = {3,0xe2,0xe5,0x01};
const uint8_t gf20[] PROGMEM = {3,0xe2, 0xd0, 0x01};
const uint8_t gf21[] PROGMEM = {3,0xe2, 0xd0, 0x02};
const uint8_t gf22[] PROGMEM = {3,0xe2, 0xd0, 0x03};
const uint8_t gf23[] PROGMEM = {3,0xe2, 0xd0, 0x04};
const uint8_t gf24[] PROGMEM = {5,0x02, 0x01, 0x01, 0x00, 0x00};
const uint8_t gf25[] PROGMEM = {8,0xe2, 0xb0, 0xfe, 0x01, 0x01, 0x01, 0x00, 0x00};
const uint8_t gf26[] PROGMEM = {8,0xe2, 0xb0, 0xfe, 0x01, 0x02, 0x01, 0x00, 0x00};
const uint8_t gf27[] PROGMEM = {3,0xe2, 0xd9, 0x00};
const uint8_t gf28[] PROGMEM = {8,0xe2,0xb0,0xfe,0x40,0x01,0x00,0x00,0x00};
const uint8_t gf29[] PROGMEM = {8,0xe2,0xb0,0xfe,0x40,0x02,0x00,0x00,0x00};
const uint8_t gf30[] PROGMEM = {5,0x02,0xe6,0x06,0x01,0x00};
const uint8_t* gfPointers[] = {
	gf1, gf2, gf3, gf4, gf5, gf6, gf7, gf8, gf9, gf10,
	gf11, gf12, gf13, gf14, gf15, gf16, gf17, gf18, gf19, gf20,
	gf21, gf22, gf23, gf24, gf25, gf26, gf27, gf28, gf29, gf30
};

uint8_t brightness;
uint8_t gfit_buffer[GFIT_MAX_LENGTH];
volatile bool UpdateTick = 0;
volatile bool UpdateRes = 0;
double avgInterval[]={0,0,0,0,0,0,0,0,0,0};
bool devMode = false;

uint8_t devModeUnlockCounter = 0;
uint8_t DisplayStartIdx=0,DisplayIdx=0;
uint8_t Display_RX_State = RX_IDLE;
uint8_t last_App_State = APP_STATE_BOOT, App_State = APP_STATE_BOOT, CallBackState = 0;
uint8_t last_User_State = USER_IDLE, User_State = USER_IDLE;
uint8_t UsartStartIdx=0,UsartIdx=0;
uint8_t Usart_RX_State = RX_IDLE;
bool wakeUp=false;
uint16_t lastcount=0;
uint16_t call2=0,call=0;

static uint8_t Display_Buffer[128];
volatile uint16_t tick=0,tock=0,tickctr=0;


uint8_t App_Language = lEN;

uint8_t gfit_startup_step=0;
uint8_t saveLightMode = 0;
bool saveSessionType = false;
uint8_t buttonPressed = 0x00;
uint8_t Timer2Ticks_HalfSecond = 0;
bool gfit_memorizeLength = false;
bool BT_on_startup = false;
uint8_t gfit_message_length=0;
bool BT_connected = false;
bool HR_connected = false;
uint8_t gfit_message_buffer[40];
uint16_t Timer2_Overflow_Counter = 0;


uint8_t Session_type = SESSION_ID_NONE;
uint8_t gfit_index = 0,gfit_last_index=0;
bool gfit_message_complete = false;
bool GFIT_boot_success = false;
uint8_t GFIT_boot_ctr = 0;
uint8_t gfit_MSG_Stat = GFIT_MSG_STAT_UNKNOWN;
uint8_t gfit_RX_ChkSum = 0x00;
double Speed_Meters_per_second = 0.0,Distance_Meters = 0.0;
uint16_t SecTimer = 0, lastSecMoved = 0;
uint16_t SecsWithoutHR = 0;
bool AppPowerSource = false;
uint8_t app_response_id = 0x00, app_response_code = 0x00;
char* SerialSendBuffer[20];
bool bluetoothEngaged = false;
bool doOnce = true;
double Calories_Accumulated;
uint32_t Watts_Accumulated;
uint32_t Heartrate_Accumulated;
uint8_t Heartrate_BPM=0,max_Heartrate_BPM=0;

uint16_t HR_Search_counter;
uint8_t HR_Update_Counter=0,BT_Update_Counter=0,dev_mode_update_counter=0;
uint8_t Heartrate_RSSI;
bool Heartrate_BLE;
char Heartrate_Name[10];

uint8_t Cadence_RPM;
uint8_t Resistance_Percent;
uint16_t Custom_Time = 30*60,Custom_Distance=5000;
  uint8_t setAngle[] = {53,67,82,97,110,115,123,130,150,180};
//uint8_t setAngle[] = {0 ,13,26,39, 53, 68, 84,102,125,180};
uint8_t setAngleIndex = 0;
uint8_t testAngle =0;
uint16_t Resistance_High_value = 980;
uint16_t Resistance_Low_value = 210;
double Power_Watts;

volatile ring_buffer_t UsartRX_Buffer;
volatile ring_buffer_t GFITRX_Buffer;

parser_context_t Display_Parser;
parser_context_t GFIT_Parser;

#endif

///////////////// UNIQUE VARIABLES //////////////////

#ifdef BIKE
uint8_t SumStrings[] = {STRING_AVG_WATTAGE, STRING_MAX_HEARTRATE, STRING_AVG_HEARTRATE, STRING_AVG_SPEED, STRING_AVG_PACE, STRING_TIME, STRING_DISTANCE, STRING_CALORIES, STRING_HEARTRATE};
double ResistanceNom = 0.0;
uint16_t adc = 0;
uint8_t SessStrings[] = {STRING_WATTS, STRING_CADENCE, STRING_RESISTANCE, STRING_SPEED, STRING_PACE, STRING_TIME, STRING_DISTANCE, STRING_CALORIES, STRING_HEARTRATE};
double a1,a2,a3;

#else
uint8_t SessStrings[] = {STRING_SPEED,STRING_AVG_SPEED,STRING_PACE,STRING_AVG_PACE,STRING_DISTANCE,STRING_TIME,STRING_CALORIES,STRING_PROJECTION,STRING_HEARTRATE};
uint8_t SumStrings[] = {STRING_AVG_HEARTRATE, STRING_AVG_SPEED, STRING_MAX_HEARTRATE, STRING_AVG_PACE, STRING_DISTANCE, STRING_TIME, STRING_CALORIES, STRING_PROJECTION ,STRING_HEARTRATE};
double avgInterval[10];
uint16_t projections[] = {1000,5000,10000,15000,20100,40200};
uint8_t projections_index=10;
uint8_t currproj = 10;
double inclineCorrFactor=1.0;
uint8_t inclineCorrStep = 0;
uint8_t inclineMenuUnlock = 0;
#endif

////////////////// UNIQUE FUNCTION DECLARATIONS /////////////////
#ifdef BIKE
double newton(double aero, double hw, double tr, double tran, double p);
double SpeedFromWatts(double P);
void actResUpdate(void);
#ifndef CICLOTTE
uint16_t readADC(void);
#endif
#endif
////////////////// COMMON FUNCTION DECLARATIONS /////////////////

// Function prototypes
void USART_Init(void);
void USART_Transmit(unsigned char data, unsigned char usart_num);
unsigned char USART_Receive(unsigned char usart_num);
void GFIT_send(unsigned char arr[], unsigned int len);
void Display_send(unsigned char arr[], unsigned int len);
void DisplayCMD(const uint8_t* string, uint8_t length);
void DisplayText(uint8_t field, const char* text, uint8_t length);
void DisplayImage(uint8_t image);
void deactivateButton(uint8_t button);
void DisplayActivateButton(uint8_t button);
void DisplayActivate9ACMD(uint8_t button);
void RemoveImage(uint8_t image);
uint8_t StringToHex(const char* string);
unsigned short Rx_CRC_CCITT(const uint8_t * puchMsg, unsigned int usDataLen);
void DisplayCommonStringByStringId(uint8_t Fieldid);

void DisplayTranslatedStringByStringId(uint8_t Fieldid);
void DisplayGif(uint8_t image);
const char* get_translated_string(uint8_t lang, uint8_t index);
void Construct_Page(Page_t *page);
void set_Ring(Ring_t *ring, uint8_t set);
void update_Ring(Ring_t *ring);
void update_Ringbuffer(Ring_t *ring);
void left_Cycle_Ring(Ring_t *ring);
void cycle_Ring(Ring_t *ring);
void Showbutton(Button_t *btn,bool pressed);
void activateSwipe(void);
void EnableDisablePBWake(bool stat);
void EnableDisablePWM(bool stat);
void Setup_Registers(void);
void sleep_1ms(uint16_t ms);

void StartStopTimer2(bool start);
void writeByteToEEPROM(uint16_t address, uint8_t data);
uint8_t readByteFromEEPROM(uint16_t address);
void setBrightness(uint8_t br);
void updateHRIndicator(void);
void actDisplay(void);
void actGFIT(void);
void parser_init(parser_context_t *context);
void ring_buffer_init(ring_buffer_t *ring_buffer, int size);
bool ring_buffer_is_full(ring_buffer_t *ring_buffer);
bool ring_buffer_is_empty(ring_buffer_t *ring_buffer);
uint8_t ring_buffer_read(ring_buffer_t *ring_buffer);
void ring_buffer_write(ring_buffer_t *ring_buffer, uint8_t element);
bool parse_Display(ring_buffer_t *ring_buffer, parser_context_t *context);
bool parse_GFIT(ring_buffer_t *ring_buffer, parser_context_t *context);
void EnableDisableGFIT(bool stat);
void EnableDisableUsarts(bool stat);
void EnableDisableDisplay(bool stat);
void EnableDisableLED(bool stat);
void Serial_SendByte(uint8_t byte);

char* get_gfit_command(uint8_t index);
void gfit_send_command(uint8_t cmd[]);
void startup_bluetooth_new(uint8_t step, bool show);
void ticktocker(uint8_t callback, uint16_t timeout);
void EnableDisableDetect(bool enable);
void fillValues(void);
void zeroValues(void);
void actTickUpdate(bool standstill);

void setServoAngle(uint8_t angleIndex);

//////////////////// PAGES /////////////////////////////
#ifdef NOHRD

Page_t generic_Page = {
	.page_id = 0,
	.Images = {IMG_BG_GRID_LIGHT,IMG_TILE0_NOHRD_LOGO,IMG_HR_INDICATOR_ICON_OFF},
	.No_Of_Images = 3,
	.No_Of_Translated_Strings = 1,
	.Translated_String_Field_IDs={SF_LABEL0_DARK},
	.Translated_String_IDs={0},
	.Do_Buttons = false,
	.No_Of_Common_Strings = 0,
	.A_Command = 0x01,
	.Common_String_Field_IDs = {},
	.Common_String_IDs = {},
	.No_Of_Rings=0,
	.Rings= {},
	.No_Of_Buttons = 5,
	.No_Of_Gifs=0,
	.Gifs={},
	.Buttons = {
		{ .button_id = BTN_TILE1,
			.action_id = BTN_ACTION_GOTO_TIME_SELECTION,
			.image_id = IMG_TILE1_BLANK_LIGHT,
			.press_image_id = IMG_TILE1_FRAME_LIGHT,
			.text_field_id = SF_LABEL1_LIGHT,
			.translated_text_id = STRING_TIME,
			.button_text_id = 0,
			.top_layer = IMG_TILE1_TIME_ICON_LIGHT
		},
		{ .button_id = BTN_TILE2,
			.action_id = BTN_ACTION_GOTO_DISTANCE_SELECTION,
			.image_id = IMG_TILE2_BLANK_LIGHT,
			.press_image_id = IMG_TILE2_FRAME_LIGHT,
			.text_field_id = SF_LABEL2_LIGHT,
			.translated_text_id = STRING_DISTANCE,
			.button_text_id = 0,
			.top_layer = IMG_TILE2_DISTANCE_ICON_LIGHT
		},
		{ .button_id = BTN_TILE3,
			.action_id = BTN_ACTION_GOTO_FREE_SESSION,
			.image_id = IMG_TILE3_BLANK_LIGHT,
			.press_image_id = IMG_TILE3_FRAME_LIGHT,
			.text_field_id = SF_LABEL3_LIGHT,
			#ifdef BIKE
			.translated_text_id = STRING_CYCLE,
			#else
			.translated_text_id = STRING_RUN,
			#endif
			
			.button_text_id = 0,
			.top_layer = IMG_TILE3_FREE_ICON_LIGHT
		},
		{	.button_id = BTN_TILE4,
			.action_id = BTN_ACTION_GOTO_SETTINGS,
			.image_id = IMG_TILE4_BLANK_LIGHT,
			.press_image_id = IMG_TILE4_FRAME_LIGHT,
			.text_field_id = SF_LABEL4_LIGHT,
			.translated_text_id = STRING_SETTINGS,
			.button_text_id = 0,
			.top_layer = IMG_TILE4_SETTINGS_ICON_LIGHT,
		},
		{	.button_id = BTN_TILE0,
			.action_id = 0,
			.image_id = 0,
			.press_image_id = 0,
			.text_field_id = 0,
			.translated_text_id = 0,
			.button_text_id = 0,
			.top_layer = 0,
		}
	}
};

Page_t page_Information = {
	.page_id = PAGE_ID_INFO,
	.No_Of_Images = 7,
	.Images = {IMG_BG_GRID_LIGHT,IMG_TILE0_INFO_ICON,IMG_TILE1_NOHRD_INFO,IMG_TILE3_QRCODE,IMG_HALFTILE41_HOME_ICON_LIGHT,IMG_HALFTILE40_VERSION_NUMBER,IMG_HR_INDICATOR_ICON_OFF},
	.No_Of_Common_Strings = 2,
	.Common_String_Field_IDs = {SF_HALFTILE40_LINE2_LIGHT,SF_HALFTILE40_LINE3_LIGHT},
	.Common_String_IDs = {STRING_VERSION_FIRMWARE,STRING_VERSION_DISPLAY_FW},
	.No_Of_Translated_Strings = 6,
	.Translated_String_Field_IDs = {SF_LABEL0_DARK,SF_INFO_LINE0_LIGHT,SF_INFO_LINE1_LIGHT,SF_INFO_LINE2_LIGHT,SF_INFO_LINE3_LIGHT,SF_INFO_LINE4_LIGHT},
	.Translated_String_IDs = {STRING_INFO,STRING_INFO_LINE0,STRING_INFO_LINE1,STRING_INFO_LINE2,STRING_INFO_LINE3,STRING_INFO_LINE4},
	
	.A_Command = 0,
	.No_Of_Rings = 0,
	.No_Of_Buttons = 2,
	.No_Of_Gifs=0,
	.Gifs={},
	.Buttons = {
		{.button_id = BTN_TILE3,
			.image_id = 0,
			.press_image_id = 0,
			.top_layer = 0,
			.translated_text_id = 0,
			.button_text_id = 0,
		.action_id = BTN_ACTION_UNLOCK_DEV_MODE}

		,{.button_id = BTN_TILE4B,
			.action_id = BTN_ACTION_RETURN_FROM_INFO,
			.image_id = IMG_HALFTILE41_BLANK_LIGHT,
			.press_image_id = IMG_HALFTILE41_FRAME_LIGHT,
			.top_layer = IMG_HALFTILE41_HOME_ICON_LIGHT,
			.text_field_id = 0,
			
		}
	}
	
	
};


Page_t page_bt_connection = {
	.page_id = PAGE_ID_BT_CONNECTION,
	.Images = {IMG_BG_GRID_LIGHT,IMG_TILE0_NOHRD_LOGO,IMG_HR_INDICATOR_ICON_OFF},
	.Translated_String_Field_IDs = {SF_LABEL1_LIGHT,SF_LABEL2_LIGHT,SF_LABEL3_LIGHT,SF_LABEL4_LIGHT,SF_HALFTILE40_LINE1_LIGHT,SF_HALFTILE40_LINE2_LIGHT,SF_HALFTILE40_LINE3_LIGHT},
	#ifdef BIKE
	.Translated_String_IDs ={STRING_WATTS,STRING_RESISTANCE,STRING_HEARTRATE},
	#endif
	#ifdef SPRINTBOK
	.Translated_String_IDs ={STRING_PACE,STRING_SPEED,STRING_HEARTRATE},
	#endif
	.No_Of_Images = 3,
	.No_Of_Translated_Strings = 3,
	.A_Command = 0,
	#ifdef CICLOTTE
	.No_Of_Buttons = 3,
	#else
	.No_Of_Buttons = 1,
	#endif
	.No_Of_Common_Strings = 0,
	.Common_String_Field_IDs = {SF_INFO_LINE0_LIGHT,SF_INFO_LINE1_LIGHT},
	.Common_String_IDs = {0,0},
	.No_Of_Rings=0,
	.Rings= {},
	.No_Of_Gifs=0,
	.Gifs={},
	.Buttons={
		#ifdef CICLOTTE
		{.button_id = BTN_TILE0,
			.action_id = BTN_ACTION_STOP_SESSION,
			.image_id = 0,
			.press_image_id = 0,
			.text_field_id = 0,
			.translated_text_id = 0,
			.button_text_id = 0,
			.top_layer = 0
		},
		{.button_id = BTN_TILE4T,
			.action_id = BTN_ACTION_INCREASE_ANGLE,
			.image_id = IMG_HALFTILE40_BLANK_LIGHT,
			.press_image_id = IMG_HALFTILE40_FRAME_LIGHT,
			.text_field_id = 0,
			.translated_text_id = 0,
			.button_text_id = 0,
			.top_layer = IMG_HALFTILE40_PLUS_ICON_LIGHT
		},
		{.button_id = BTN_TILE4B,
			.action_id = BTN_ACTION_DECREASE_ANGLE,
			.image_id = IMG_HALFTILE41_BLANK_LIGHT,
			.press_image_id = IMG_HALFTILE41_FRAME_LIGHT,
			.text_field_id = 0,
			.translated_text_id = 0,
			.button_text_id = 0,
			.top_layer = IMG_HALFTILE41_MINUS_ICON_LIGHT
		}
		#else
		{.button_id = BTN_TILE4B,
			.action_id = BTN_ACTION_STOP_SESSION,
			.image_id = IMG_HALFTILE41_BLANK_LIGHT,
			.press_image_id = IMG_HALFTILE41_FRAME_LIGHT,
			.text_field_id = SF_HALFTILE41_LABEL_LIGHT,
			.translated_text_id = STRING_END,
			.button_text_id = 0,
			.top_layer = IMG_HALFTILE41_DISCONBT_LIGHT
		}
		#endif
	}
	
};

Page_t page_Connect_HR = {
	.page_id = PAGE_ID_CONNECT_HR,
	.Images = {IMG_BG_GRID_LIGHT,IMG_TILE0_NOHRD_LOGO,IMG_HR_INDICATOR_ICON_OFF},
	.Translated_String_Field_IDs = {SF_VALUE2_LIGHT,SF_LABEL2_LIGHT,SF_INFO_LINE0_LIGHT,SF_INFO_LINE1_LIGHT,SF_INFO_LINE2_LIGHT,SF_LABEL1_LIGHT},
	.Translated_String_IDs ={0,0,0,0,0,STRING_HEARTRATE},
	.No_Of_Images = 3,
	.No_Of_Translated_Strings = 6,
	.A_Command = 0,
	.No_Of_Buttons =4,
	.No_Of_Common_Strings = 2,
	.Common_String_Field_IDs = {SF_INFO_LINE0_LIGHT,SF_INFO_LINE1_LIGHT},
	.Common_String_IDs = {0,0},
	.No_Of_Rings=0,
	.Rings= {},
	.No_Of_Gifs=0,
	.Gifs={},
	.Buttons={
		{.button_id = BTN_TILE0,
			.action_id = BTN_ACTION_GOTO_MAIN_MENU,
			.image_id = 0,
			.press_image_id = 0,
			.text_field_id = 0,
			.translated_text_id = 0,
			.button_text_id = 0
		},
		{.button_id = BTN_TILE4B,
			.action_id = BTN_ACTION_GOTO_SETTINGS,
			.image_id = IMG_HALFTILE41_LEFT_ARROW_ICON_LIGHT,
			.press_image_id = IMG_HALFTILE41_FRAME_LIGHT,
			.text_field_id = 0,
			.translated_text_id = 0,
			.button_text_id = 0
		},
		{.button_id = BTN_TILE4T,
			.action_id = BTN_ACTION_REFRESH_HR,
			.image_id = IMG_HALFTILE40_BLANK_NEUTRAL,
			.press_image_id = IMG_HALFTILE40_FRAME_LIGHT,
			.top_layer = IMG_HALFTILE40_REFRESH_HR,
			.text_field_id = 0,
			.translated_text_id = 0,
			.button_text_id = 0
		},
		{.button_id = BTN_TILE3,
			.action_id = BTN_ACTION_CYCLE_CHANNEL_MODES,
			.image_id = IMG_TILE3_BLANK_LIGHT,
			.press_image_id = IMG_TILE3_FRAME_LIGHT,
			.top_layer = 0,
			.text_field_id = 0,
			.translated_text_id = 0,
			.button_text_id = 0
		}
	}
};

Page_t page_Time_Selection = {
	.page_id = PAGE_ID_TIME_SELECTION,
	.Images = {IMG_BG_HALFGRID_LIGHT,IMG_TILE0_TIME,IMG_TILE2_BLANK_LIGHT,IMG_HR_INDICATOR_ICON_OFF},
	.Translated_String_Field_IDs = {SF_LABEL0_DARK,SF_LABEL40_DARK,SF_LABEL2_LIGHT},
	.Translated_String_IDs ={STRING_TIME,STRING_START,STRING_MINUTES},
	.No_Of_Images =4,
	.No_Of_Translated_Strings = 3,
	.A_Command = 0,
	.No_Of_Buttons = 6,
	.No_Of_Common_Strings = 1,
	.Common_String_Field_IDs = {SF_VALUE2_LIGHT},
	.Common_String_IDs = {STRING_TIME_PRESELECT},
	.No_Of_Rings=0,
	.Rings= {},
	.No_Of_Gifs=0,
	.Gifs={},

	.Buttons={{.button_id = BTN_TILE1T,
		.action_id = BTN_ACTION_BIG_INCREASE_TIME,
		.image_id = IMG_HALFTILE10_BLANK_LIGHT,
		.press_image_id = IMG_HALFTILE10_FRAME_LIGHT,
		.text_field_id = SF_LABEL10_LIGHT,
		.translated_text_id = 0,
		.button_text_id = STRING_TIME_INCREMENT_BIG
	},
	{	.button_id = BTN_TILE1B,
		.action_id = BTN_ACTION_INCREASE_TIME,
		.image_id = IMG_HALFTILE11_BLANK_LIGHT,
		.press_image_id = IMG_HALFTILE11_FRAME_LIGHT,
		.top_layer = 0,
		.text_field_id = SF_LABEL11_LIGHT,
		.button_text_id = STRING_TIME_INCREMENT_SMALL,
	.translated_text_id = 0},
	{.button_id = BTN_TILE3T,
		.action_id = BTN_ACTION_DECREASE_TIME,
		.image_id = IMG_HALFTILE30_BLANK_LIGHT,
		.press_image_id = IMG_HALFTILE30_FRAME_LIGHT,
		.top_layer = 0,
		.text_field_id = SF_LABEL30_LIGHT,
		.translated_text_id = 0,
	.button_text_id = STRING_TIME_DECREMENT_SMALL},
	{.button_id = BTN_TILE3B,
		.action_id = BTN_ACTION_BIG_DECREASE_TIME,
		.image_id = IMG_HALFTILE31_BLANK_LIGHT,
		.press_image_id = IMG_HALFTILE31_FRAME_LIGHT,
		.text_field_id = SF_LABEL31_LIGHT,
		.translated_text_id = 0,
	.button_text_id = STRING_TIME_DECREMENT_BIG},
	{.button_id = BTN_TILE4B,
		.action_id = BTN_ACTION_GOTO_MAIN_MENU,
		.image_id = IMG_HALFTILE41_BLANK_LIGHT,
		.press_image_id = IMG_HALFTILE41_FRAME_LIGHT,
		.top_layer=IMG_HALFTILE41_HOME_ICON_LIGHT,
		.text_field_id = 0,
	.translated_text_id = 0x00},
	{.button_id = BTN_TILE4T,
		.action_id = BTN_ACTION_START_TIME_SESSION,
		.image_id = 0x00,
		.press_image_id = IMG_HALFTILE40_FRAME_DARK,
		.text_field_id = SF_LABEL40_DARK,
		.translated_text_id = STRING_START
	}
}
};

Page_t page_Distance_Selection = {
	.page_id = PAGE_ID_DISTANCE_SELECTION,
	.Images = {IMG_BG_HALFGRID_LIGHT,IMG_TILE0_DISTANCE,IMG_TILE2_BLANK_LIGHT,IMG_HR_INDICATOR_ICON_OFF},
	.Translated_String_Field_IDs = {SF_LABEL0_DARK,SF_LABEL40_DARK,SF_LABEL2_LIGHT},
	.Translated_String_IDs ={STRING_DISTANCE,STRING_START,STRING_METER},
	.No_Of_Images = 4,
	.No_Of_Translated_Strings = 3,
	.No_Of_Buttons = 6,
	.A_Command = 0,
	.No_Of_Gifs=0,
	.Gifs={},
	.No_Of_Common_Strings = 1,
	.Common_String_Field_IDs = {SF_VALUE2_LIGHT},
	.Common_String_IDs = {STRING_DISTANCE_PRESELECT},
	.No_Of_Rings=0,
	.Rings= {},

	.Buttons={{.button_id = BTN_TILE1T,
		.action_id = BTN_ACTION_BIG_INCREASE_DISTANCE,
		.image_id = IMG_HALFTILE10_BLANK_LIGHT,
		.press_image_id = IMG_HALFTILE10_FRAME_LIGHT,
		.text_field_id = SF_LABEL10_LIGHT,
		.button_text_id = STRING_DISTANCE_INCREMENT_BIG,
		.translated_text_id = 0
	},
	{.button_id = BTN_TILE1B,
		.action_id = BTN_ACTION_INCREASE_DISTANCE,
		.image_id = IMG_HALFTILE11_BLANK_LIGHT,
		.press_image_id = IMG_HALFTILE11_FRAME_LIGHT,
		.text_field_id = SF_LABEL11_LIGHT,
		.button_text_id = STRING_DISTANCE_INCREMENT_SMALL,
		.translated_text_id = 0
	},
	{.button_id = BTN_TILE3T,
		.action_id = BTN_ACTION_DECREASE_DISTANCE,
		.image_id = IMG_HALFTILE30_BLANK_LIGHT,
		.press_image_id = IMG_HALFTILE30_FRAME_LIGHT,
		.text_field_id = SF_LABEL30_LIGHT,
		.button_text_id = STRING_DISTANCE_DECREMENT_SMALL,
		.translated_text_id = 0
	},
	{.button_id = BTN_TILE3B,
		.action_id = BTN_ACTION_BIG_DECREASE_DISTANCE,
		.image_id = IMG_HALFTILE31_BLANK_LIGHT,
		.press_image_id = IMG_HALFTILE31_FRAME_LIGHT,
		.text_field_id = SF_LABEL31_LIGHT,
		.button_text_id = STRING_DISTANCE_DECREMENT_BIG,
		.translated_text_id = 0
	},
	{.button_id = BTN_TILE4B,
		.action_id = BTN_ACTION_GOTO_MAIN_MENU,
		.image_id = 0x00,
		.press_image_id = IMG_HALFTILE41_FRAME_LIGHT,
		.top_layer = IMG_HALFTILE41_HOME_ICON_LIGHT,
		.text_field_id = 0x00,
	.translated_text_id = 0},
	{.button_id = BTN_TILE4T,
		.action_id = BTN_ACTION_START_DISTANCE_SESSION,
		.image_id = 0x00,
		.press_image_id = IMG_HALFTILE40_FRAME_DARK,
		.text_field_id = SF_LABEL40_DARK,
		.translated_text_id = STRING_START
	},
}
};

Page_t page_calibrate = {
	.page_id = PAGE_ID_CALIBRATION,
	.Images = {IMG_BG_CALIBRATION,IMG_TILE0_NOHRD_LOGO,IMG_HR_INDICATOR_ICON_OFF},
	.No_Of_Images = 3,
	.Translated_String_IDs = {STRING_CALIBRATION,STRING_CALIBRATION_LINE0,STRING_CALIBRATION_LINE1,STRING_CALIBRATION_LINE2,STRING_CALIBRATION_LINE3_MAX,STRING_CALIBRATION_LINE4_MAX,STRING_STEP},
	.Translated_String_Field_IDs = {SF_LABEL0_DARK,SF_INFO_LINE0_LIGHT,SF_INFO_LINE1_LIGHT,SF_INFO_LINE2_LIGHT,SF_INFO_LINE3_LIGHT,SF_INFO_LINE4_LIGHT,SF_LABEL3_LIGHT},
	.No_Of_Translated_Strings =7,
	.No_Of_Common_Strings = 0,
	.No_Of_Rings=0,
	.No_Of_Gifs=1,
	.Gifs={GIF_CALIBRATE_UP},
	.No_Of_Buttons = 1,
	.A_Command = 0,
	.Buttons = {
		{
			.button_id = BTN_TILE4,
			.action_id = BTN_ACTION_GOTO_CALIBRATE_DOWN,
			.image_id = IMG_TILE4_BLANK_LIGHT,
			.press_image_id = IMG_TILE4_FRAME_LIGHT,
			.text_field_id = 0,
			.top_layer =IMG_TILE4_NEXT_ICON_LIGHT,
			.text_field_id = SF_LABEL4_LIGHT,
			.translated_text_id = STRING_NEXT
		},
	}
	
};


Page_t page_confirm_end = {
	.page_id = PAGE_ID_CONFIRM_END,
	.Images = {IMG_BG_LIGHT_HT40_NEUTRAL,IMG_TILE0_NOHRD_LOGO,IMG_HR_INDICATOR_ICON_OFF},
	.No_Of_Images = 3,
	.Translated_String_IDs = {STRING_QUIT},
	.Translated_String_Field_IDs = {SF_HALFTILE40_LINE2_DARK},
	.No_Of_Translated_Strings = 1,
	.No_Of_Common_Strings = 0,
	.No_Of_Rings=0,
	.No_Of_Buttons = 2,
	.A_Command = 0,
	.No_Of_Gifs=0,
	.Gifs={},
	.Buttons = {
		{
			.button_id = BTN_TILE1,
			.action_id = BTN_ACTION_CONFIRM_CONFIRM,
			.image_id = IMG_TILE1_BLANK_LIGHT,
			.press_image_id = IMG_TILE1_FRAME_LIGHT,
			.text_field_id = 0,
			.top_layer =IMG_TILE1_CHECK_ICON_LIGHT,
			.text_field_id = SF_LABEL1_LIGHT,
			.translated_text_id = STRING_YES
		},
		{
			.button_id = BTN_TILE2,
			.action_id = BTN_ACTION_CONFIRM_CANCEL,
			.image_id = IMG_TILE2_BLANK_LIGHT,
			.press_image_id = IMG_TILE2_FRAME_LIGHT,
			.text_field_id = 0,
			.top_layer =IMG_TILE2_CANCEL_ICON_LIGHT,
			.text_field_id = SF_LABEL2_LIGHT,
			.translated_text_id = STRING_NO2
		}
		
	}
	
};



Page_t page_Session = {
	.page_id = PAGE_ID_SESSION,
	.Images = {IMG_BG_GRID_LIGHT,IMG_TILE1_RING_1_9,IMG_TILE2_RING_2_9,IMG_TILE3_RING_3_9,IMG_HR_INDICATOR_ICON_OFF},
	.No_Of_Images = 5,
	.Translated_String_Field_IDs = {SF_LABEL0_DARK,SF_LABEL1_LIGHT,SF_LABEL2_LIGHT,SF_LABEL3_LIGHT},
	.Translated_String_IDs = {STRING_RESISTANCE,STRING_WATTS,STRING_CADENCE,STRING_RESISTANCE},
	.No_Of_Translated_Strings = 4,
	.Common_String_Field_IDs = {SF_VALUE0_DARK,SF_VALUE1_LIGHT,SF_VALUE2_LIGHT,SF_VALUE3_LIGHT},
	.Common_String_IDs = {STRING_VERSION_DISPLAY_APP,STRING_VALUE_WATTS,STRING_VALUE_CADENCE,STRING_VALUE_RESISTANCE},
	.A_Command = 0,
	.No_Of_Common_Strings = 4,
	.No_Of_Rings=3,
	.Rings= {0,0,0},
	.No_Of_Buttons = 9,
	.No_Of_Gifs=0,
	.Gifs={},
	.Buttons={
	#ifdef CICLOTTE
		{.button_id = BTN_TILE4T,
			.action_id = BTN_ACTION_INCREASE_ANGLE,
			.image_id = IMG_HALFTILE40_BLANK_NEUTRAL,
			.press_image_id = IMG_HALFTILE40_FRAME_LIGHT,
			.top_layer = IMG_HALFTILE40_PLUS_ICON_LIGHT,
			.text_field_id = 0,
		},
		{.button_id = BTN_TILE4B,
			.action_id = BTN_ACTION_DECREASE_DISTANCE,
			.image_id = IMG_HALFTILE41_BLANK_LIGHT,
			.press_image_id = IMG_HALFTILE41_FRAME_LIGHT,
			.top_layer = IMG_HALFTILE41_MINUS_ICON_LIGHT,
		},
	#else
	{.button_id = BTN_TILE4T,
		.action_id = BTN_ACTION_PAUSE_SESSION,
		.image_id = IMG_HALFTILE40_BLANK_NEUTRAL,
		.press_image_id = IMG_HALFTILE40_FRAME_LIGHT,
		.top_layer = IMG_HALFTILE40_PAUSE_ICON_DARK,
		.text_field_id = SF_LABEL4_LIGHT,
	},
	{.button_id = BTN_TILE4B,
		.action_id = BTN_ACTION_STOP_SESSION,
		.image_id = IMG_HALFTILE41_BLANK_LIGHT,
		.press_image_id = IMG_HALFTILE41_FRAME_LIGHT,
		.top_layer = IMG_HALFTILE41_STOP_ICON_LIGHT,
	},
	#endif
		#ifdef BIKE
		{.button_id = BTN_TILE1L,
			#else
			{.button_id = BTN_TILE1T,
				#endif
				.action_id = BTN_ACTION_LEFTCYCLE_SESSION_TILE1,
				.image_id = IMG_TILE1_BLANK_LIGHT,
				.press_image_id = IMG_RINGTILE10_FRAME_LIGHT,
				.text_field_id = 0x00,
				.translated_text_id = 0x00,
				.top_layer = 0,
			},
			#ifdef BIKE
			{.button_id = BTN_TILE1R,
				#else
				{.button_id = BTN_TILE1B,
					#endif
					.action_id = BTN_ACTION_RIGHTCYCLE_SESSION_TILE1,
					.image_id = IMG_TILE1_BLANK_LIGHT,
					.press_image_id = IMG_RINGTILE11_FRAME_LIGHT,
					.text_field_id = 0x00,
					.translated_text_id = 0x00,
					.top_layer = 0
				},
				#ifdef BIKE
				{.button_id = BTN_TILE2L,
					#else
					{.button_id = BTN_TILE2T,
						#endif
						.action_id = BTN_ACTION_LEFTCYCLE_SESSION_TILE2,
						.image_id = IMG_TILE2_BLANK_LIGHT,
						.press_image_id = IMG_RINGTILE20_FRAME_LIGHT,
						.text_field_id = 0x00,
						.translated_text_id = 0x00,
						.top_layer = 0,
					},
					#ifdef BIKE
					{.button_id = BTN_TILE2R,
						#else
						{.button_id = BTN_TILE2B,
							#endif
							.action_id = BTN_ACTION_RIGHTCYCLE_SESSION_TILE2,
							.image_id = IMG_TILE2_BLANK_LIGHT,
							.press_image_id = IMG_RINGTILE21_FRAME_LIGHT,
							.text_field_id = 0x00,
							.translated_text_id = 0x00,
							.top_layer = 0,
						},
						#ifdef BIKE
						{.button_id = BTN_TILE3L,
							#else
							{.button_id = BTN_TILE3T,
								#endif
								.action_id = BTN_ACTION_LEFTCYCLE_SESSION_TILE3,
								.image_id = IMG_TILE3_BLANK_LIGHT,
								.press_image_id = IMG_RINGTILE30_FRAME_LIGHT,
								.text_field_id = 0x00,
								.translated_text_id = 0x00,
								.top_layer = 0,
							},
							#ifdef BIKE
							{.button_id = BTN_TILE3R,
								#else
								{.button_id = BTN_TILE3B,
									#endif
									.action_id = BTN_ACTION_RIGHTCYCLE_SESSION_TILE3,
									.image_id = IMG_TILE3_BLANK_LIGHT,
									.press_image_id = IMG_RINGTILE31_FRAME_LIGHT,
									.text_field_id = 0x00,
									.translated_text_id = 0x00,
									.top_layer = 0,
								},
								#ifdef CICLOTTE
								{.button_id = BTN_TILE0,
									.action_id = BTN_ACTION_STOP_SESSION,
									.image_id = 0,
									.press_image_id = 0,
									.top_layer = 0,
								},
								#endif
							}
							
						};

						
						#else

						#endif

						Page_t *activePage;

						//////////////////// SESSION RINGS ////////////////////
						#ifdef NOHRD
						#ifdef BIKE

						Ring_t ring_Tile1SessionRing = {
							.no_of_targets = 3,
							.buffer = {{IMG_TILE1_RING_1_9,IMG_TILE1_RING_2_9,IMG_TILE1_RING_3_9,IMG_TILE1_RING_4_9,IMG_TILE1_RING_5_9,IMG_TILE1_RING_6_9,IMG_TILE1_RING_7_9,IMG_TILE1_RING_8_9,IMG_TILE1_RING_9_9},
							{STRING_VALUE_WATTS,STRING_VALUE_CADENCE,STRING_VALUE_RESISTANCE,STRING_VALUE_SPEED,STRING_VALUE_PACE,STRING_VALUE_TIME,STRING_VALUE_DISTANCE,STRING_VALUE_CALORIES,STRING_VALUE_HEARTRATE},
							{STRING_WATTS,STRING_CADENCE,STRING_RESISTANCE,STRING_SPEED,STRING_PACE,STRING_TIME,STRING_DISTANCE,STRING_CALORIES,STRING_HEARTRATE}
						},
						.max_idx = 8,
						.idx=0,
						.adresses = {&page_Session.Images[1],&page_Session.Common_String_IDs[1],&page_Session.Translated_String_IDs[1]},
						.function_pointer = {DisplayImage,DisplayCommonStringByStringId,DisplayTranslatedStringByStringId}
					};

					Ring_t ring_Tile2SessionRing = {
						.no_of_targets = 3,
						.buffer = {{IMG_TILE2_RING_1_9,IMG_TILE2_RING_2_9,IMG_TILE2_RING_3_9,IMG_TILE2_RING_4_9,IMG_TILE2_RING_5_9,IMG_TILE2_RING_6_9,IMG_TILE2_RING_7_9,IMG_TILE2_RING_8_9,IMG_TILE2_RING_9_9},
						{STRING_VALUE_WATTS,STRING_VALUE_CADENCE,STRING_VALUE_RESISTANCE,STRING_VALUE_SPEED,STRING_VALUE_PACE,STRING_VALUE_TIME,STRING_VALUE_DISTANCE,STRING_VALUE_CALORIES,STRING_VALUE_HEARTRATE},
						{STRING_WATTS,STRING_CADENCE,STRING_RESISTANCE,STRING_SPEED,STRING_PACE,STRING_TIME,STRING_DISTANCE,STRING_CALORIES,STRING_HEARTRATE}
					},
					.max_idx = 8,
					.idx=1,
					.adresses = {&page_Session.Images[2],&page_Session.Common_String_IDs[2],&page_Session.Translated_String_IDs[2]},
					.function_pointer = {DisplayImage,DisplayCommonStringByStringId,DisplayTranslatedStringByStringId}
				};

				Ring_t ring_Tile3SessionRing = {
					.no_of_targets = 3,
					.buffer = {{IMG_TILE3_RING_1_9,IMG_TILE3_RING_2_9,IMG_TILE3_RING_3_9,IMG_TILE3_RING_4_9,IMG_TILE3_RING_5_9,IMG_TILE3_RING_6_9,IMG_TILE3_RING_7_9,IMG_TILE3_RING_8_9,IMG_TILE3_RING_9_9},
					{STRING_VALUE_WATTS,STRING_VALUE_CADENCE,STRING_VALUE_RESISTANCE,STRING_VALUE_SPEED,STRING_VALUE_PACE,STRING_VALUE_TIME,STRING_VALUE_DISTANCE,STRING_VALUE_CALORIES,STRING_VALUE_HEARTRATE},
					{STRING_WATTS,STRING_CADENCE,STRING_RESISTANCE,STRING_SPEED,STRING_PACE,STRING_TIME,STRING_DISTANCE,STRING_CALORIES,STRING_HEARTRATE},
					
				},

				.max_idx = 8,
				.idx=2,
				.adresses = {&page_Session.Images[3],&page_Session.Common_String_IDs[3],&page_Session.Translated_String_IDs[3]},
				.function_pointer = {DisplayImage,DisplayCommonStringByStringId,DisplayTranslatedStringByStringId}
			};
			#else

			Ring_t ring_Tile1SessionRing = {
				.no_of_targets = 3,
				.buffer = {{IMG_TILE1_RING_1_9,IMG_TILE1_RING_2_9,IMG_TILE1_RING_3_9,IMG_TILE1_RING_4_9,IMG_TILE1_RING_5_9,IMG_TILE1_RING_6_9,IMG_TILE1_RING_7_9,IMG_TILE1_RING_8_9,IMG_TILE1_RING_9_9},
				{STRING_VALUE_SPEED,STRING_VALUE_AVG_SPEED,STRING_VALUE_PACE,STRING_VALUE_AVG_PACE,STRING_VALUE_DISTANCE,STRING_VALUE_TIME,STRING_VALUE_CALORIES,STRING_VALUE_PROJECTION,STRING_VALUE_HEARTRATE},
				{STRING_SPEED,STRING_AVG_SPEED,STRING_PACE,STRING_AVG_PACE,STRING_DISTANCE,STRING_TIME,STRING_CALORIES,STRING_XXK_PROJECTION,STRING_HEARTRATE}
			},
			.max_idx = 8,
			.idx=0,
			.adresses = {&page_Session.Images[1],&page_Session.Common_String_IDs[1],&page_Session.Translated_String_IDs[1]},
			.function_pointer = {DisplayImage,DisplayCommonStringByStringId,DisplayTranslatedStringByStringId}
		};

		Ring_t ring_Tile2SessionRing = {
			.no_of_targets = 3,
			.buffer = {{IMG_TILE2_RING_1_9,IMG_TILE2_RING_2_9,IMG_TILE2_RING_3_9,IMG_TILE2_RING_4_9,IMG_TILE2_RING_5_9,IMG_TILE2_RING_6_9,IMG_TILE2_RING_7_9,IMG_TILE2_RING_8_9,IMG_TILE2_RING_9_9},
			{STRING_VALUE_SPEED,STRING_VALUE_AVG_SPEED,STRING_VALUE_PACE,STRING_VALUE_AVG_PACE,STRING_VALUE_DISTANCE,STRING_VALUE_TIME,STRING_VALUE_CALORIES,STRING_VALUE_PROJECTION,STRING_VALUE_HEARTRATE},
			{STRING_SPEED,STRING_AVG_SPEED,STRING_PACE,STRING_AVG_PACE,STRING_DISTANCE,STRING_TIME,STRING_CALORIES,STRING_XXK_PROJECTION,STRING_HEARTRATE}
		},
		.max_idx = 8,
		.idx=1,
		.adresses = {&page_Session.Images[2],&page_Session.Common_String_IDs[2],&page_Session.Translated_String_IDs[2]},
		.function_pointer = {DisplayImage,DisplayCommonStringByStringId,DisplayTranslatedStringByStringId}
	};

	Ring_t ring_Tile3SessionRing = {
		.no_of_targets = 3,
		.buffer = {{IMG_TILE3_RING_1_9,IMG_TILE3_RING_2_9,IMG_TILE3_RING_3_9,IMG_TILE3_RING_4_9,IMG_TILE3_RING_5_9,IMG_TILE3_RING_6_9,IMG_TILE3_RING_7_9,IMG_TILE3_RING_8_9,IMG_TILE3_RING_9_9},
		{STRING_VALUE_SPEED,STRING_VALUE_AVG_SPEED,STRING_VALUE_PACE,STRING_VALUE_AVG_PACE,STRING_VALUE_DISTANCE,STRING_VALUE_TIME,STRING_VALUE_CALORIES,STRING_VALUE_PROJECTION,STRING_VALUE_HEARTRATE},
		{STRING_SPEED,STRING_AVG_SPEED,STRING_PACE,STRING_AVG_PACE,STRING_DISTANCE,STRING_TIME,STRING_CALORIES,STRING_XXK_PROJECTION,STRING_HEARTRATE}
		
	},

	.max_idx = 8,
	.idx=2,
	.adresses = {&page_Session.Images[3],&page_Session.Common_String_IDs[3],&page_Session.Translated_String_IDs[3]},
	.function_pointer = {DisplayImage,DisplayCommonStringByStringId,DisplayTranslatedStringByStringId}
};
#endif
#endif

#ifdef NOHRD



// Eine Funktion, die einen Ringbuffer mit einer bestimmten Größe initialisiert

void ring_buffer_init(ring_buffer_t *ring_buffer, int size) {
	// Setze die Größe, den Anfang und das Ende
	ring_buffer->size = size;
	ring_buffer->start = 0;
	ring_buffer->end = 0;
	// Reserviere Speicherplatz für den Puffer
	/*
	ring_buffer->buffer = (uint8_t *)malloc(size * sizeof(uint8_t)); // Speicher für size Elemente des Typs uint8_t reservieren

	// Prüfe, ob die Speicherzuweisung erfolgreich war
	if (ring_buffer->buffer == NULL) {
	// Speicherzuweisung fehlgeschlagen, setze die Größe auf 0 und buffer auf NULL
	ring_buffer->size = 0; // Optional, abhängig von der Handhabung in Ihrer Anwendung
	// Hier könnten Sie auch weitere Fehlerbehandlungen durchführen, z.B. eine Fehlermeldung ausgeben oder den Programmablauf ändern
	}
	*/
}

bool ring_buffer_is_full(ring_buffer_t *ring_buffer) {
	// Der Ringbuffer ist voll, wenn das Ende um eins erhöht gleich dem Anfang ist
	return (ring_buffer->end + 1) % ring_buffer->size == ring_buffer->start;
}

// Eine Funktion, die prüft, ob der Ringbuffer leer ist
bool ring_buffer_is_empty(ring_buffer_t *ring_buffer) {
	// Der Ringbuffer ist leer, wenn der Anfang gleich dem Ende ist
	return ring_buffer->start == ring_buffer->end;
}

// Eine Funktion, die ein Element in den Ringbuffer schreibt
void ring_buffer_write(ring_buffer_t *ring_buffer, uint8_t element) {
	// Prüfe, ob der Ringbuffer voll ist
	if (ring_buffer_is_full(ring_buffer)) {
		// Der Ringbuffer ist voll, überschreibe das älteste Element
		ring_buffer->start = (ring_buffer->start + 1) % ring_buffer->size;
	}
	// Schreibe das Element an das Ende des Ringbuffers
	ring_buffer->buffer[ring_buffer->end] = element;
	// Erhöhe das Ende um eins
	ring_buffer->end = (ring_buffer->end + 1) % ring_buffer->size;
}

uint8_t ring_buffer_read(ring_buffer_t *ring_buffer) {
	// Prüfe, ob der Ringbuffer leer ist
	if (ring_buffer_is_empty(ring_buffer)) {
		// Der Ringbuffer ist leer, gib 0 zurück
		return 0;
	}
	// Lese das Element am Anfang des Ringbuffers
	uint8_t element = ring_buffer->buffer[ring_buffer->start];
	// Erniedrige den Anfang um eins
	ring_buffer->start = (ring_buffer->start + 1) % ring_buffer->size;
	// Rückgabe des Elements
	return element;

}



void parser_init(parser_context_t *context) {
	context->state = PENDING;
	context->payload_index = 0;
	context->current_message.complete = false;
	context->current_message.expected_length=0;
}
bool notEmpty = false;



bool parse_Display(ring_buffer_t *ring_buffer, parser_context_t *context) {
	if (context->state==FINISHED){
		parser_init(context);
	}

	
	while (!ring_buffer_is_empty(ring_buffer)) {
		
		uint8_t byte = ring_buffer_read(ring_buffer);
		
		switch (context->state) {
			case PENDING:
			if (byte == 0xAA) { // Start of a new message
				context->current_message.prefix = byte;
				context->state = COLLECTING;
				context->payload_index = 0;
			}
			break;
			case COLLECTING:
			if (context->payload_index >= 12) {  // Message exceeds expected size, abort and reset
				context->state = ABORTED;
				break;
			}

			// Add byte to payload if not yet at suffix.
			context->current_message.payload[context->payload_index++] = byte;

			// Check if the last four bytes match the suffix pattern
			if (context->payload_index > 4 &&
			context->current_message.payload[context->payload_index - 4] == 0xE4 &&
			context->current_message.payload[context->payload_index - 3] == 0x1B &&
			context->current_message.payload[context->payload_index - 2] == 0x11 &&
			context->current_message.payload[context->payload_index - 1] == 0xEE) {
				context->state = FINISHED;
				context->current_message.payload_length = context->payload_index-6;
				context->current_message.complete = true;
				// Here, you should process the collected message before resetting.
				// Then reset for a new message.
				return true;
				break; // Exit the loop to process the collected message.
				
			}
			break;
			
			
			case ABORTED:
			// Message was aborted due to size limit, reset to start a new one.
			parser_init(context);
			return false;
		}
	}
	return false;
}

bool parse_GFIT(ring_buffer_t *ring_buffer, parser_context_t *context){
	
	if (context->state==FINISHED){
		parser_init(context);
	}
	
	
	while (!ring_buffer_is_empty(ring_buffer)) {
		
		uint8_t byte = ring_buffer_read(ring_buffer);
		
		//sprintf(String_buffer,"%u %u %u",ring_buffer->start,ring_buffer->end,ring_buffer->end-ring_buffer->start);
		//DisplayText(SF_INFO_LINE0_LIGHT,String_buffer,strlen(String_buffer));
		switch (context->state){
			case PENDING:
			if (byte==0xa4){
				context->current_message.prefix=byte;
				context->state=STARTED;
				context->payload_index=0;
			}
			break;
			case STARTED:
			context->current_message.expected_length=byte;
			context->state=COLLECTING;
			break;
			case COLLECTING:
			context->current_message.payload[context->payload_index++] = byte;
			if (context->payload_index>PAYLOAD_ABORT_LENGTH){
				context->state=ABORTED;
			}
			if (context->payload_index==context->current_message.expected_length){
				context->state=FINISHED;
				context->current_message.complete=true;
				context->current_message.payload_length=context->payload_index;
				
				return true;
			}
			break;
			case ABORTED:
			
			parser_init(context);
			return false;
		}
	}
	return false;
}

void actGFIT(void){
	
	#ifdef BIKE
	adc = 0;
	#endif
	if (devMode){
		sprintf(String_buffer,"%x %x %x %x",GFIT_Parser.current_message.payload[0],GFIT_Parser.current_message.payload[1],GFIT_Parser.current_message.payload[2],GFIT_Parser.current_message.payload[3]);
		DisplayText(SF_INFO_LINE0_LIGHT,String_buffer,strlen(String_buffer));
	
		sprintf(String_buffer,"%d",call++);
		DisplayText(SF_INFO_LINE1_LIGHT,String_buffer,strlen(String_buffer));
	}
	gfit_MSG_Stat=GFIT_MSG_STAT_BUSY;
	switch (GFIT_Parser.current_message.payload[0]){
		case 0x6f: // GFIT BOOT
			GFIT_boot_success = true;
			break;
		case 0xe0:  //  Event
			switch (GFIT_Parser.current_message.payload[1]){
				case 0xb1: // custom event
				switch (GFIT_Parser.current_message.payload[2]){
					case 0xfe: // custom Protocol
						switch (GFIT_Parser.current_message.payload[3]){
							case 0x01: // set value
								switch (GFIT_Parser.current_message.payload[4]){
									default:
									gfit_send_command(get_gfit_command(GFIT_COMMAND_RESPONSE_FAIL));
									break;
								}
							break;
							case 0x02: // handshake
								switch (GFIT_Parser.current_message.payload[4]){
									case 0x01: // nohrd bike app
						
										gfit_send_command(get_gfit_command(GFIT_COMMAND_RESPONSE_SUCCESS));
										break;
									case 0x02: // nohrd arena
										gfit_send_command(get_gfit_command(GFIT_COMMAND_RESPONSE_SUCCESS));
										break;
									default:
										gfit_send_command(get_gfit_command(GFIT_COMMAND_RESPONSE_FAIL));
										break;
								}
								break;

							case 0x04:
								gfit_send_command(get_gfit_command(GFIT_COMMAND_RESPONSE_SUCCESS));
								if (~readByteFromEEPROM(0x00f0)&(1<<4)){
									gfit_send_command(get_gfit_command(CALIBRATED_ACK));
									} else {
									gfit_send_command(get_gfit_command(CALIBRATED_NACK));
								}
								break;
							case 0xaf: // enter bootloader
								break;
							
							default:
								gfit_send_command(get_gfit_command(GFIT_COMMAND_RESPONSE_FAIL));
								break;
						}
						break;
					}
					break;
				case 0xb2: // heartrate event
			
				if (GFIT_Parser.current_message.payload[2]&1){
					if (!HRDevicePaired){
						HRDevicePaired = true;
						updateHRIndicator();
					}
						HRDoggy = 0;
						Heartrate_RSSI = GFIT_Parser.current_message.payload[4];
						Heartrate_BPM = GFIT_Parser.current_message.payload[3];

						if (Heartrate_BPM>max_Heartrate_BPM){
							max_Heartrate_BPM = Heartrate_BPM;
						}

						Heartrate_BLE = GFIT_Parser.current_message.payload[2]&(1<<1);
						
				}
			
				break;
				case 0xb5:
					switch(GFIT_Parser.current_message.payload[2]){
						case 0x11:   // target resistance
						gfit_send_command(get_gfit_command(GFIT_COMMAND_RESPONSE_SUCCESS));
						sprintf(String_buffer,"%x %d %d",GFIT_Parser.current_message.payload[2],GFIT_Parser.current_message.payload[3],GFIT_Parser.current_message.payload[4]);
						DisplayText(SF_LABEL1_LIGHT,String_buffer,strlen(String_buffer));
						gfit_send_command(get_gfit_command(GFIT_COMMAND_STATUS_UPDATE_RESISTANCE_CHANGED));
						break;
						
						case 0x12:
						gfit_send_command(get_gfit_command(GFIT_COMMAND_RESPONSE_SUCCESS));
						sprintf(String_buffer,"%x %d %d",GFIT_Parser.current_message.payload[2],GFIT_Parser.current_message.payload[3],GFIT_Parser.current_message.payload[4]);
						DisplayText(SF_LABEL2_LIGHT,String_buffer,strlen(String_buffer));
						gfit_send_command(get_gfit_command(GFIT_COMMAND_STATUS_UPDATE_RESISTANCE_CHANGED));
						break;
					}
					
				break;
				case 0xb9: // bootloader enter
				break;
				case 0xba: // ble periphearal event
				switch (gfit_message_buffer[2]){
					case 0x00: // connect
					BT_connected = true;
					IdleState = IDLE_STATE_ACTIVE;
					App_State = APP_STATE_GOTO_BT_CONNECTION;
					break;
					case 0x01: // disconnect
					BT_connected = false;
					App_State = APP_STATE_QUIT_PEER_CONNECTION;
				
					break;
				}
				//updateBTIndicator();
				break;
				case 0xfe: // gfit response
				break;
			}

		break;
		case 0xe2:	//
			break;
		case 0x40:  //
			break;
		case 0x4e:  //
			break;
		default:
		break;
	}
	gfit_MSG_Stat = GFIT_MSG_STAT_DONE;
}

void actDisplay(void){
	
	
	uint8_t command_id = Display_Parser.current_message.payload[0];
	uint8_t target_id = Display_Parser.current_message.payload[1];
	uint8_t response_code = Display_Parser.current_message.payload[2];
	
	switch (command_id){
		case (CMD_BUTTON):
		switch (response_code){
			case (BTN_PRESS):
			User_State=USER_PRESS;
			app_response_id = target_id;
			break;
			case (BTN_RELEASE):
			User_State = USER_RELEASE;
			app_response_id = target_id;
			break;
			default:
			break;
		}
		break;
		case (CMD_SWIPE):
		switch (response_code){
			case (SWIPE_UP):
			User_State = USER_SWIPE_UP;
			break;
			case (SWIPE_DOWN):
			User_State = USER_SWIPE_DOWN;
			break;
			case (SWIPE_LEFT):
			User_State = USER_SWIPE_LEFT;
			break;
			case (SWIPE_RIGHT):
			User_State = USER_SWIPE_RIGHT;
			break;
			default:
			break;
		}
		break;
		case (0x80):
			if (target_id==0xc4){
				
			}
		break;
		
		default:
		
		break;
	}

	DisplayIdx=0;


}

#endif

void bootSequence(void){
	
	sleep_1ms(1000);
	
	App_State = APP_STATE_MAIN_MENU;
	
	if (BT_on_startup){
		CallBackState = APP_STATE_MAIN_MENU;
		
		sleep_1ms(100);
		App_State = APP_STATE_TOGGLE_BLUETOOTH;
	}
	
	
}

void wdt_off(void) {
	cli(); // Interrupts deaktivieren
	wdt_reset();
	MCUSR &= ~(1<<WDRF); // Watchdog Reset Flag löschen
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR = 0x00; // Watchdog komplett deaktivieren
	sei(); // Interrupts wieder aktivieren
}

int main (void)
{
	
	
		
	wdt_off();
	EnableDisableDisplay(true);
	/*for (uint16_t a = 0; a<65000;a++){
		for (uint16_t c = 0; c<50;c++){
			b = c;
		}
	}
	*/	

	_delay_ms(5000);
	parser_init(&Display_Parser);
	
	parser_init(&GFIT_Parser);
	
	ring_buffer_init(&UsartRX_Buffer,DISPLAYRXBUFFERSIZE);
	ring_buffer_init(&GFITRX_Buffer,DISPLAYRXBUFFERSIZE);
	
	page_Session.Rings[0]=&ring_Tile1SessionRing;
	page_Session.Rings[1]=&ring_Tile2SessionRing;
	page_Session.Rings[2]=&ring_Tile3SessionRing;
	
	Setup_Registers();
	USART_Init();
	EnableDisableUsarts(true);
	sei();
	for (uint8_t k=0;k<3;k++){
	LED_PORT|=LED_PIN;
	sleep_1ms(500);
	LED_PORT&=~LED_PIN;
	sleep_1ms(500);
	}
	#ifdef CICLOTTE
	setServoAngle(setAngleIndex);
	#endif
	DisplayImage(IMG_BG_BOOT);

	sleep_1ms(100);

	#ifdef BIKE
	#ifndef CICLOTTE

	uint8_t CAL = readByteFromEEPROM(0x00f0)	;
	if ((~CAL)&(1<<4)) {
		Resistance_Low_value = (uint16_t) (readByteFromEEPROM(0x00f1)<<8) + readByteFromEEPROM(0x00f2);
		Resistance_High_value = (uint16_t) (readByteFromEEPROM(0x00f3)<<8) + readByteFromEEPROM(0x00f4);
		} else {

	}
	#endif
	#endif
	
	uint8_t storedvals = readByteFromEEPROM(0x00f5);
	if (storedvals==0xff){
		writeByteToEEPROM(0x00f5,0);
		writeByteToEEPROM(0x00f6,0);
		writeByteToEEPROM(0x00f7,1|1<<5);
		storedvals = readByteFromEEPROM(0x00f5);
	}
	App_Language = storedvals&0x0f;
	App_Darkmode = (storedvals>>4)&1;
	
	page_Session.Rings[0]->idx = readByteFromEEPROM(0x00f6);
	page_Session.Rings[1]->idx = readByteFromEEPROM(0x00f7)&0x0f;
	page_Session.Rings[2]->idx = readByteFromEEPROM(0x00f7)>>4;

	storedvals = readByteFromEEPROM(0x00f8);
	
	
	AppPowerSource = storedvals&(1);
	timeout = AppPowerSource? GRID_TIMEOUT : BATTERY_TIMEOUT;
	
	BT_on_startup = (storedvals>>1)&(1);
	brightness = ((storedvals>>3)&1)*2+((storedvals>>2)&(1));
		
	switch(brightness){
		case 0:
		App_Max_Brightness=CONTRAST_LOW;
		break;
		case 1:
		App_Max_Brightness=CONTRAST_MID;
		break;
		case 2:
		default:
		App_Max_Brightness=CONTRAST_HIGH;
		break;
	}
	
	#ifdef SPRINTBOK
	storedvals = readByteFromEEPROM(0x00f9);
	if (storedvals==0xff){
		writeByteToEEPROM(0x00f9,0x00);
		}
	storedvals = readByteFromEEPROM(0x00f9);
	if (storedvals&1<<7){
		inclineCorrStep=(storedvals&~(1<<7));
		
		inclineCorrFactor = 1.0 + ((double)inclineCorrStep)/20.0;
	}
	#endif
	
	
	storedvals = readByteFromEEPROM(0x00fa);
	if (storedvals==0xff){
		writeByteToEEPROM(0x00fa,0x00);
	}
	storedvals = readByteFromEEPROM(0x00fa);
	channel_mode=storedvals;
	
	
	bootSequence();
	
	setBrightness(App_Max_Brightness);
	
	uint8_t i = 0;
	while(1){
		
		///////////////////////////   APP-STATE HANDLER /////////////////////////////
		#ifdef NOHRD
		if (last_App_State!=App_State){
			last_App_State=App_State;
			switch(last_App_State){
				case (APP_STATE_TOGGLE_LANGUAGE):
				App_Language = (App_Language+1)%NO_OF_LANGUAGES;
				
				generic_Page.Buttons[0].top_layer = IMG_TILE1_ENGLISH_ICON+App_Language;
				Construct_Page(activePage);

				App_State=APP_STATE_IDLE;
				break;

				case (APP_STATE_MAIN_MENU):
				onoff = false;
				EnableDisableDetect(false); 
				EnableDisableUsarts(false);
				
				if (App_Language != (readByteFromEEPROM(0x00f5)&0x0f) || (App_Darkmode!=readByteFromEEPROM(0x00f5)<<4)){
					writeByteToEEPROM(0x00f5,(App_Darkmode<<4)|App_Language);
				}
				storedvals = readByteFromEEPROM(0x00f8);
				
				
				if (AppPowerSource!=storedvals&(1<<0) || brightness!=(storedvals&(1<<3))*2+(storedvals&(1<<2)) || bluetoothEngaged != storedvals&(1<<1))
				{
					writeByteToEEPROM(0x00f8,AppPowerSource | ((brightness&1)<<2) | ((brightness>>1)<<3) | bluetoothEngaged<<1);
				}
#ifdef SPRINTBOK
				storedvals = readByteFromEEPROM(0x00f9);
				if (inclineCorrStep!=(storedvals&(~(1<<7)))){
					writeByteToEEPROM(0x00f9,inclineCorrStep|(1<<7));
					inclineCorrFactor = 1.0 + ((double)inclineCorrStep)/20.0;
				}
#endif			
				storedvals = readByteFromEEPROM(0x00fa);
				if (channel_mode!=storedvals){
					writeByteToEEPROM(0x00fa,channel_mode);
					};
	
				EnableDisableUsarts(true);
				generic_Page.page_id = PAGE_ID_MAIN_MENU;
				generic_Page.Images[0] = IMG_BG_GRID_LIGHT;
				generic_Page.Images[1] = IMG_TILE0_NOHRD_LOGO;
				generic_Page.Translated_String_IDs[0]=0;

				generic_Page.Buttons[0].action_id = BTN_ACTION_GOTO_TIME_SELECTION;
				generic_Page.Buttons[0].translated_text_id = STRING_TIME;
				generic_Page.Buttons[0].top_layer = IMG_TILE1_TIME_ICON_LIGHT;

				generic_Page.Buttons[1].action_id = BTN_ACTION_GOTO_DISTANCE_SELECTION;
				generic_Page.Buttons[1].translated_text_id = STRING_DISTANCE;
				generic_Page.Buttons[1].top_layer = IMG_TILE2_DISTANCE_ICON_LIGHT;

				generic_Page.Buttons[2].action_id = BTN_ACTION_GOTO_FREE_SESSION;
				#ifdef BIKE
				generic_Page.Buttons[2].translated_text_id = STRING_CYCLE;
				generic_Page.Buttons[2].top_layer = IMG_TILE3_FREE_ICON_LIGHT;
				#else
				generic_Page.Buttons[2].translated_text_id = STRING_RUN;
				generic_Page.Buttons[2].top_layer = IMG_TILE3_FREE_ICON_LIGHT;
				#endif
				
				generic_Page.Buttons[3].action_id = BTN_ACTION_GOTO_SETTINGS;
				generic_Page.Buttons[3].translated_text_id = STRING_SETTINGS;
				generic_Page.Buttons[3].top_layer = IMG_TILE4_SETTINGS_ICON_LIGHT;

				


				Construct_Page(&generic_Page);
								
				break;
				case (APP_STATE_SETTINGS_SCREEN):

				generic_Page.page_id = PAGE_ID_SETTINGS;
				generic_Page.Translated_String_IDs[0]=STRING_SETTINGS;
				generic_Page.Images[0] = IMG_BG_GRID_LIGHT;
				generic_Page.Images[1] = IMG_TILE0_SETTINGS;

				generic_Page.Buttons[0].action_id = BTN_ACTION_GOTO_INFO;
				generic_Page.Buttons[0].translated_text_id = STRING_INFO;
				generic_Page.Buttons[0].top_layer = IMG_TILE1_INFO_LIGHT;

				generic_Page.Buttons[1].action_id = BTN_ACTION_CUSTOMIZE_SETTINGS;
				generic_Page.Buttons[1].translated_text_id = STRING_CUSTOMIZE;
				generic_Page.Buttons[1].top_layer = IMG_TILE2_CUSTOMIZE_ICON_LIGHT;
				
				generic_Page.Buttons[2].action_id = BTN_ACTION_SETUP_SETTINGS;
				generic_Page.Buttons[2].translated_text_id = STRING_SETUP;
				generic_Page.Buttons[2].top_layer = IMG_TILE3_SETUP_ICON_LIGHT;
				
				generic_Page.Buttons[3].action_id = BTN_ACTION_GOTO_MAIN_MENU;
				generic_Page.Buttons[3].translated_text_id = STRING_HOME;
				generic_Page.Buttons[3].top_layer = IMG_TILE4_HOME_LIGHT;

				Construct_Page(&generic_Page);

				break;
				case (APP_STATE_CUSTOMIZE_SCREEN):

				generic_Page.page_id = PAGE_ID_CUSTOM_SETTINGS;
				generic_Page.Translated_String_IDs[0]=STRING_SETTINGS;
				generic_Page.Images[0] = IMG_BG_GRID_LIGHT;
				generic_Page.Images[1] = IMG_TILE0_CUSTOM_LOGO;

				generic_Page.Buttons[0].action_id = BTN_ACTION_TOGGLE_LANGUAGE;
				generic_Page.Buttons[0].translated_text_id = STRING_LANGUAGE;
				generic_Page.Buttons[0].top_layer = IMG_TILE1_ENGLISH_ICON+App_Language;
				
				//#ifdef BIKE
				//generic_Page.Buttons[1].action_id = BTN_ACTION_GOTO_CALIBRATE_UP;
				//generic_Page.Buttons[1].translated_text_id = STRING_CALIBRATION;
				//generic_Page.Buttons[1].top_layer = IMG_TILE2_CALIBRATE_UP_LIGHT;
				//#else
				generic_Page.Buttons[1].action_id = BTN_ACTION_TOGGLE_LDMODE;
				generic_Page.Buttons[1].translated_text_id = STRING_LIGHTMODE+App_Darkmode;
				generic_Page.Buttons[1].top_layer = IMG_TILE2_SUN_ICON_LIGHT+App_Darkmode;
				//#endif

				generic_Page.Buttons[2].action_id = BTN_ACTION_GOTO_CONTRAST;
				generic_Page.Buttons[2].translated_text_id = STRING_CONTRAST;
				generic_Page.Buttons[2].top_layer = IMG_TILE3_LOW_BRIGHTNESS_ICON_LIGHT;

				generic_Page.Buttons[3].action_id = BTN_ACTION_GOTO_SETTINGS;
				generic_Page.Buttons[3].translated_text_id = STRING_BACK;
				generic_Page.Buttons[3].top_layer = IMG_TILE4_BACK_ICON_LIGHT;
				
				Construct_Page(&generic_Page);
				
				break;
				
				case (APP_STATE_SETUP_SCREEN):

				generic_Page.page_id = PAGE_ID_SETUP_SETTINGS;
				generic_Page.Translated_String_IDs[0]=STRING_SETTINGS;
				generic_Page.Images[0] = IMG_BG_GRID_LIGHT;
				generic_Page.Images[1] = IMG_TILE0_SETUP_LOGO;

				generic_Page.Buttons[0].action_id = BTN_ACTION_TOGGLE_POWER_SOURCE;
				generic_Page.Buttons[0].translated_text_id = STRING_BATTERY+AppPowerSource;
				generic_Page.Buttons[0].top_layer = IMG_TILE1_BATTERY_ICON_LIGHT+AppPowerSource*2;
				
				generic_Page.Buttons[1].action_id = BTN_ACTION_GOTO_HRCONNECT;
				generic_Page.Buttons[1].translated_text_id = STRING_HR_CONNECTION;
				generic_Page.Buttons[1].top_layer = IMG_TILE2_HR_ICON_LIGHT;
				
				generic_Page.Buttons[2].action_id = BTN_ACTION_TOGGLE_BLUETOOTH;
				generic_Page.Buttons[2].translated_text_id = STRING_BTOFF+bluetoothEngaged;
				generic_Page.Buttons[2].top_layer = IMG_TILE3_BT_ICON_OFF_LIGHT+bluetoothEngaged*2;
				
				generic_Page.Buttons[3].action_id = BTN_ACTION_GOTO_SETTINGS;
				generic_Page.Buttons[3].translated_text_id = STRING_BACK;
				generic_Page.Buttons[3].top_layer = IMG_TILE4_BACK_ICON_LIGHT;
				
				#ifdef SPRINTBOK
				generic_Page.Buttons[4].action_id = BTN_ACTION_GOTO_INCLINE_CORRECTION_SCREEN;
				generic_Page.Buttons[4].press_image_id = 0;
				#endif
				
				Construct_Page(&generic_Page);
				
				break;
				
				#ifdef SPRINTBOK
				case (APP_STATE_INCLINE_CORRECTION_SCREEN):
				
				inclineMenuUnlock = 0;
				
				generic_Page.page_id = PAGE_ID_INCLINE_CORRECTION_SCREEN;
				
				generic_Page.Buttons[0].action_id = BTN_ACTION_DECREASE_INCLINE_CORRECTION;
				generic_Page.Buttons[0].translated_text_id = 0;
				generic_Page.Buttons[0].top_layer = 0;
				
				generic_Page.Buttons[1].action_id = 0;
				generic_Page.Buttons[1].translated_text_id = 0;
				generic_Page.Buttons[1].top_layer = 0;
				
				generic_Page.Buttons[2].action_id = BTN_ACTION_INCREASE_INCLINE_CORRECTION;
				generic_Page.Buttons[2].translated_text_id = 0;
				generic_Page.Buttons[2].top_layer = 0;
				
				generic_Page.Buttons[3].action_id = BTN_ACTION_GOTO_MAIN_MENU;
				generic_Page.Buttons[3].translated_text_id = STRING_HOME;
				generic_Page.Buttons[3].top_layer = IMG_TILE4_HOME_LIGHT;
				
				generic_Page.Buttons[4].action_id = 0;
				Construct_Page(&generic_Page);
				App_State=APP_STATE_INCLINE_CORRECTION_SCREEN_UPDATE;
				
				break;
				case (APP_STATE_INCLINE_CORRECTION_SCREEN_UPDATE):
				sprintf(String_buffer,"incline Corr.");
				DisplayText(SF_LABEL0_DARK,String_buffer,strlen(String_buffer));
				sprintf(String_buffer,"+");
				DisplayText(SF_VALUE3_LIGHT,String_buffer,strlen(String_buffer));
				sprintf(String_buffer,"-");
				DisplayText(SF_VALUE1_LIGHT,String_buffer,strlen(String_buffer));
				sprintf(String_buffer,"%d%%",inclineCorrStep*5);
				DisplayText(SF_VALUE2_LIGHT,String_buffer,strlen(String_buffer));
				sprintf(String_buffer,"added Speed");
				DisplayText(SF_LABEL2_LIGHT,String_buffer,strlen(String_buffer));
				inclineCorrFactor = 1.0 + ((double)inclineCorrStep)/20.0;
				App_State=APP_STATE_IDLE;
				break;
				#endif
				case (APP_STATE_TOGGLE_DARKMODE):
				App_Darkmode = 1-App_Darkmode;
				
				generic_Page.Buttons[1].translated_text_id = STRING_LIGHTMODE+App_Darkmode;
				generic_Page.Buttons[1].top_layer = IMG_TILE2_SUN_ICON_LIGHT;
				Construct_Page(activePage);
				App_State=APP_STATE_IDLE;
				break;
				
				case (APP_STATE_TRY_GFIT):
				DisplayImage(IMG_TILE3_RING_1_9);
				
				RST_PORT |= (1<<RST_PIN);
				
				gfit_startup_step = 0;
				ticktocker(APP_STATE_TRY_AGAIN_GFIT,200);
				break;

				case (APP_STATE_CONFIGURE_GFIT):
				
				startup_bluetooth_new(gfit_startup_step++, true);
				ticktocker(APP_STATE_CONFIGURE_GFIT,100);
				if (gfit_startup_step==11){
					Showbutton(&generic_Page.Buttons[2],false);
					App_State=CallBackState;
				}
				break;
				
				case (APP_STATE_TRY_AGAIN_GFIT):
				if (GFIT_boot_success){
					App_State = APP_STATE_CONFIGURE_GFIT;
					
					} else {
					
					RST_PORT &= ~(1<<RST_PIN);
					sleep_1ms(10);
					ticktocker(APP_STATE_FAILED_TRY,200);
				}
				break;

				case (APP_STATE_FAILED_TRY):
				GFIT_boot_ctr++;

				if (GFIT_boot_ctr>10){
					App_State = CallBackState;
					GFIT_boot_ctr = 0;
					RemoveImage(IMG_TILE3_RING_1_9);
					} else {
					App_State = APP_STATE_TRY_GFIT;
				}
				break;

				case (APP_STATE_TOGGLE_BLUETOOTH):
				bluetoothEngaged = !bluetoothEngaged;
				EnableDisableGFIT(bluetoothEngaged);
				sleep_1ms(100);
				
				if(!bluetoothEngaged){
					HRDevicePaired = false;
					generic_Page.Buttons[2].top_layer=IMG_TILE3_BT_ICON_OFF_LIGHT;
					generic_Page.Buttons[2].translated_text_id=STRING_BTOFF;
					GFIT_boot_success = false;
					GFIT_boot_ctr = 0;
					BT_connected = false;
					HR_connected = false;
					updateHRIndicator();
					RST_PORT &= ~(1<<RST_PIN);
					Showbutton(&generic_Page.Buttons[2],false);
					App_State=APP_STATE_IDLE;
					}else{
					ticktocker(APP_STATE_TRY_GFIT,100);
					App_State = APP_STATE_TRY_GFIT;
				}
				break;

				case (APP_STATE_TIME_SELECTION_SCREEN):
				Construct_Page(&page_Time_Selection);
				App_State = APP_STATE_IDLE;
				break;

				case (APP_STATE_DISTANCE_SELECTION_SCREEN):
				Construct_Page(&page_Distance_Selection);
				App_State = APP_STATE_IDLE;
				break;
				
				
				case (APP_STATE_GOTO_CONTRAST):
				
				generic_Page.page_id = PAGE_ID_CONTRAST;
				generic_Page.Translated_String_IDs[0]=STRING_CONTRAST;
				generic_Page.Images[0] = IMG_BG_GRID_LIGHT;
				generic_Page.Images[1] = IMG_TILE0_CONTRAST_LOGO;

				generic_Page.Buttons[0].action_id = BTN_ACTION_CONTRAST_HIGH;
				generic_Page.Buttons[0].translated_text_id = STRING_HIGH;
				generic_Page.Buttons[0].top_layer = IMG_TILE1_FULL_BRIGHTNESS_ICON_LIGHT;
				
				generic_Page.Buttons[1].action_id = BTN_ACTION_CONTRAST_MID;
				generic_Page.Buttons[1].translated_text_id = STRING_MID;
				generic_Page.Buttons[1].top_layer = IMG_TILE2_HALF_BRIGHTNESS_ICON_LIGHT;
				
				generic_Page.Buttons[2].action_id = BTN_ACTION_CONTRAST_LOW;
				generic_Page.Buttons[2].translated_text_id = STRING_LOW;
				generic_Page.Buttons[2].top_layer = IMG_TILE3_LOW_BRIGHTNESS_ICON_LIGHT;
				
				generic_Page.Buttons[3].action_id = BTN_ACTION_CUSTOMIZE_SETTINGS;
				generic_Page.Buttons[3].translated_text_id = STRING_BACK;
				generic_Page.Buttons[3].top_layer = IMG_TILE4_BACK_ICON_LIGHT;
				
				Construct_Page(&generic_Page);
				break;
				
				
				case (APP_STATE_GOTO_SESSION_SCREEN):
				#ifdef CICLOTTE
				EnableDisablePWM(true);
				#endif
				saveSessionType = true;
				DisplayImage(IMG_BG_GRID_LIGHT);
				DisplayImage(IMG_TILE0_NOHRD_LOGO);
				gfit_send_command(get_gfit_command(GFIT_TRANSITION_TO_INUSE));
				
				#ifdef CICLOTTE
				page_Session.Buttons[0].button_id = BTN_TILE4T;
				page_Session.Buttons[0].action_id = BTN_ACTION_INCREASE_ANGLE;
				page_Session.Buttons[0].image_id = IMG_HALFTILE40_BLANK_LIGHT;
				page_Session.Buttons[0].press_image_id = IMG_HALFTILE40_FRAME_LIGHT;
				page_Session.Buttons[0].top_layer = IMG_HALFTILE40_PLUS_ICON_LIGHT;
				page_Session.Buttons[0].text_field_id = 0;
				page_Session.Buttons[0].translated_text_id = 0;

				page_Session.Buttons[1].action_id = BTN_ACTION_DECREASE_ANGLE;
				page_Session.Buttons[1].image_id = IMG_HALFTILE41_BLANK_LIGHT;
				page_Session.Buttons[1].press_image_id = IMG_HALFTILE41_FRAME_LIGHT;
				page_Session.Buttons[1].top_layer = IMG_HALFTILE41_MINUS_ICON_LIGHT;
				
				#else
				page_Session.Buttons[0].button_id = BTN_TILE4T;
				page_Session.Buttons[0].action_id = BTN_ACTION_PAUSE_SESSION;
				page_Session.Buttons[0].image_id = IMG_HALFTILE40_BLANK_NEUTRAL;
				page_Session.Buttons[0].press_image_id = IMG_HALFTILE40_FRAME_LIGHT;
				page_Session.Buttons[0].top_layer = IMG_HALFTILE40_PAUSE_ICON_DARK;
				page_Session.Buttons[0].translated_text_id = 0;

				page_Session.Buttons[1].action_id = BTN_ACTION_STOP_SESSION;
				page_Session.Buttons[1].image_id = IMG_HALFTILE41_BLANK_LIGHT;
				page_Session.Buttons[1].press_image_id = IMG_HALFTILE41_FRAME_LIGHT;
				page_Session.Buttons[1].top_layer = IMG_HALFTILE41_STOP_ICON_LIGHT;
				#endif

				Ring_t *ringPointerS[] = {&ring_Tile1SessionRing,&ring_Tile2SessionRing,&ring_Tile3SessionRing};
				#ifdef SPRINTBOK
				
				snprintf(Common_Strings[STRING_XXK_PROJECTION],STRING_LENGTH_XXK_PROJECTION,"1.0k Proj.");
				
				#endif
				for (uint8_t i = 0;i<3;i++){
					Ring_t *ptr = ringPointerS[i];
					for (uint8_t j = 0; j<9; j++){
						ptr->buffer[2][j]=SessStrings[j];
					}
					update_Ringbuffer(ringPointerS[i]);
				}

				switch (Session_type){
					case SESSION_ID_TIME:
					page_Session.Translated_String_IDs[0]=STRING_TIME;
					page_Session.Common_String_IDs[0]=STRING_VALUE_TIME_LEFT;
					break;
					case SESSION_ID_DISTANCE:
					page_Session.Translated_String_IDs[0]=STRING_DISTANCE;
					page_Session.Common_String_IDs[0]=STRING_VALUE_DISTANCE_LEFT;
					
					
					
					break;
					case SESSION_ID_FREE:
					#ifdef BIKE
					page_Session.Translated_String_IDs[0]=STRING_CYCLE;
					#else
					page_Session.Translated_String_IDs[0]=STRING_RUN;
					#endif
					page_Session.Common_String_IDs[0]=STRING_VALUE_TIME;
					break;
				}
				zeroValues();

				for (uint8_t i = 0; i<=3; i++){
					sprintf(String_buffer,"%d",3-i);
					if (i) {DisplayText(SF_VALUE4_LIGHT-4+i," ",1);}
					DisplayText(SF_VALUE4_LIGHT-3+i,String_buffer,strlen(String_buffer));
					
					sleep_1ms(1000);
				}

				
				Construct_Page(&page_Session);
				
				
				inSession=true;
				IdleState = IDLE_STATE_ACTIVE;
				StartStopTimer2(true);
				EnableDisableDetect(true);
				App_State = APP_STATE_SESSION_SCREEN_IDLE;
				
				break;
				case (APP_STATE_SESSION_SCREEN_UPDATE):
				///////////////////////////////////////  !!!!!!!!!!!!   App_State=APP_STATE_TIME_SESSION_SCREEN_IDLE;
				fillValues();

				

				switch (Session_type){
					case (SESSION_ID_TIME):
					
					rest = (Custom_Time-SecTimer);
					if (SecTimer>Custom_Time) rest = 0;
					if (Custom_Time-SecTimer>90*60){
						sprintf (Common_Strings[STRING_VALUE_TIME_LEFT],"%01d:%02d",rest/3600,(rest%3600)/60);
					}
					else if (Custom_Time-SecTimer>=60){
						sprintf (Common_Strings[STRING_VALUE_TIME_LEFT],"%02d:%02d",rest/60,rest%60);
						} else {
						sprintf (Common_Strings[STRING_VALUE_TIME_LEFT],"%02d",rest);
					}
					
					if (SecTimer>=Custom_Time){
						SecTimer = Custom_Time;
						App_State = APP_STATE_SESSION_FINISHED;
					}
					break;

					case (SESSION_ID_DISTANCE):
					
					
					rest = (Custom_Distance-(uint16_t)Distance_Meters);
					if (Distance_Meters>=Custom_Distance) rest = 0;
					if (Custom_Distance-Distance_Meters>1000){
						sprintf (Common_Strings[STRING_VALUE_DISTANCE_LEFT],"%d.%dk",rest/1000,(rest%1000)/100);
						} else if (rest<1000 && rest >= 100){
						sprintf (Common_Strings[STRING_VALUE_DISTANCE_LEFT],"%03d",rest);
						} else {
						sprintf (Common_Strings[STRING_VALUE_DISTANCE_LEFT],"%02d",rest);
					}
					
					if (Distance_Meters>=Custom_Distance){
						Distance_Meters = Custom_Distance;
						App_State = APP_STATE_SESSION_FINISHED;
					}
					break;
					case (SESSION_ID_FREE):
					break;
				}
				App_State = APP_STATE_SESSION_SCREEN_IDLE;
				
				if (lastSecMoved+180<SecTimer){
					App_State = APP_STATE_SESSION_FINISHED;
				}
				
				for (uint8_t i=0;i< activePage->No_Of_Common_Strings;i++){
					DisplayText(activePage->Common_String_Field_IDs[i],Common_Strings[activePage->Common_String_IDs[i]],strlen(Common_Strings[activePage->Common_String_IDs[i]]));
				}
				
				break;
				
				case (APP_STATE_UPDATE_PRESELECTION):
				if (activePage->page_id == PAGE_ID_DISTANCE_SELECTION){
					
					if (Custom_Distance>=DISTANCE_THRESH){
						DistanceGain=DISTANCE_GAIN_2;
						DistanceBigGain=DISTANCE_GAIN_3;
						Custom_Distance=(Custom_Distance/1000)*1000;
						
						sprintf(Common_Strings[STRING_DISTANCE_PRESELECT],"%dk",Custom_Distance/1000);
						
						} else {
						DistanceGain=DISTANCE_GAIN_0;
						DistanceBigGain=DISTANCE_GAIN_1;
						
						sprintf(Common_Strings[STRING_DISTANCE_PRESELECT],"%d",Custom_Distance);
						



					}
					DisplayText(SF_VALUE2_LIGHT,Common_Strings[STRING_DISTANCE_PRESELECT],strlen(Common_Strings[STRING_DISTANCE_PRESELECT]));
					
					sprintf(Button_Strings[STRING_DISTANCE_INCREMENT_SMALL],"+%d m",DistanceGain);
					sprintf(Button_Strings[STRING_DISTANCE_DECREMENT_SMALL],"-%d m",DistanceGain);
					sprintf(Button_Strings[STRING_DISTANCE_INCREMENT_BIG],"+%d m",DistanceBigGain);
					sprintf(Button_Strings[STRING_DISTANCE_DECREMENT_BIG],"-%d m",DistanceBigGain);
					
					} else {
					
					if (Custom_Time>=TIME_THRESH){
						TimeGain=TIME_GAIN_2;
						TimeBigGain=TIME_GAIN_3;
						
						Custom_Time=(Custom_Time/600)*600;
						sprintf(Common_Strings[STRING_TIME_PRESELECT],"%02d:%02d",Custom_Time/3600,(Custom_Time/60)%60);
						DisplayText(SF_VALUE2_LIGHT,Common_Strings[STRING_TIME_PRESELECT],strlen(Common_Strings[STRING_TIME_PRESELECT]));
						DisplayText(SF_LABEL2_LIGHT,get_translated_string(App_Language,STRING_HOURS),strlen(get_translated_string(App_Language,STRING_HOURS)));
						} else {
						TimeGain=TIME_GAIN_0;
						TimeBigGain=TIME_GAIN_1;
						sprintf(Common_Strings[STRING_TIME_PRESELECT],"%d",Custom_Time/60);
						DisplayText(SF_VALUE2_LIGHT,Common_Strings[STRING_TIME_PRESELECT],strlen(Common_Strings[STRING_TIME_PRESELECT]));
						DisplayText(SF_LABEL2_LIGHT,get_translated_string(App_Language,STRING_MINUTES),strlen(get_translated_string(App_Language,STRING_MINUTES)));
						
					}
					sprintf(Button_Strings[STRING_TIME_INCREMENT_SMALL],"+%d min",TimeGain/60);
					sprintf(Button_Strings[STRING_TIME_DECREMENT_SMALL],"-%d min",TimeGain/60);
					sprintf(Button_Strings[STRING_TIME_INCREMENT_BIG],"+%d min",TimeBigGain/60);
					sprintf(Button_Strings[STRING_TIME_DECREMENT_BIG],"-%d min",TimeBigGain/60);
					
				}
				Showbutton(&activePage->Buttons[3],false);
				Showbutton(&activePage->Buttons[0],false);
				Showbutton(&activePage->Buttons[1],false);
				Showbutton(&activePage->Buttons[2],false);
				
				
				
				App_State = APP_STATE_IDLE;
				break;

				case APP_STATE_INFO_SCREEN:
				Construct_Page(&page_Information);
				App_State = APP_STATE_IDLE;
				break;
				case (APP_STATE_TICK):
				if (tick_tock_timer>=tick_tock_timeout){
					App_State = APP_STATE_CALLBACK;
					} else {
					App_State = APP_STATE_TOCK;
				}
				break;


				case APP_STATE_QUIT_TIME_SESSION:

				case (APP_STATE_SESSION_FINISHED):
				
				inSession=false;
				StartStopTimer2(false);
				EnableDisableDetect(false);
				#ifdef CICLOTTE
				EnableDisablePWM(false);
				#endif
				UCSR1B &= ~(1<<RXCIE1);
				gfit_send_command(get_gfit_command(GFIT_TRANSITION_TO_FINISHED));
				gfit_send_command(get_gfit_command(GFIT_TRANSITION_TO_READY));
				
				if (page_Session.Rings[0]->idx != readByteFromEEPROM(0x00f6)){
					writeByteToEEPROM(0x00f6,page_Session.Rings[0]->idx);
				}
				if (page_Session.Rings[1]->idx != (readByteFromEEPROM(0x00f7)&0x0f)||page_Session.Rings[2]->idx!=readByteFromEEPROM(0x00f7)>>4){
					writeByteToEEPROM(0x00f7,(page_Session.Rings[1]->idx)|(page_Session.Rings[2]->idx)<<4);
				}
				
				if (SecTimer>0){
					
					sprintf(Common_Strings[STRING_VALUE_SPEED],"%d",(uint8_t)((Distance_Meters/SecTimer)*3.6));
					
					
					sprintf(Common_Strings[STRING_VALUE_WATTS],"%d",(uint8_t)((Watts_Accumulated/SecTimer)));
					
					if (SecsWithoutHR == SecTimer) {
						max_Heartrate_BPM=0;
						sprintf(Common_Strings[STRING_VALUE_RESISTANCE],"%d",0);
						#ifdef SPRINTBOK
						sprintf(Common_Strings[STRING_VALUE_SPEED],"%d",0);
						#endif
						
						} else {
						#ifdef BIKE
						sprintf(Common_Strings[STRING_VALUE_RESISTANCE],"%d",(uint8_t)((Heartrate_Accumulated/(SecTimer-SecsWithoutHR))));
						#endif
						#ifdef SPRINTBOK
						sprintf(Common_Strings[STRING_VALUE_SPEED],"%d",(uint8_t)((Heartrate_Accumulated/(SecTimer-SecsWithoutHR))));
						#endif
					}
					
					#ifdef BIKE
					sprintf(Common_Strings[STRING_VALUE_CADENCE],"%d",(uint8_t)(max_Heartrate_BPM));
					
					
					double avgms = (Distance_Meters/SecTimer);
					if (Speed_Meters_per_second>2.0){
						uint8_t mpkm,spkm;
						
							mpkm = (uint16_t)floor(1000.0/(avgms))/60;
							spkm = (uint16_t)floor(1000.0/(avgms))%60;
						
						sprintf(Common_Strings[STRING_VALUE_PACE],"%02d:%02d",mpkm,spkm);
						
						} else {
						sprintf(Common_Strings[STRING_VALUE_PACE],"--:--");
					}
					#endif
					#ifdef SPRINTBOK
					sprintf(Common_Strings[STRING_VALUE_PACE],"%d",(uint8_t)(max_Heartrate_BPM));
					
					#endif
					

					page_Session.Images[page_Session.No_Of_Images-2] = IMG_TILE0_FINISH;
					page_Session.Translated_String_IDs[0] = STRING_SUMMARY;
					page_Session.Common_String_IDs[0] = 0;
					
					page_Session.Buttons[0].button_id = BTN_TILE4	;
					page_Session.Buttons[0].action_id = BTN_ACTION_GOTO_MAIN_MENU	;
					page_Session.Buttons[0].image_id = IMG_TILE4_BLANK_LIGHT;
					page_Session.Buttons[0].press_image_id = IMG_TILE4_FRAME_LIGHT;
					page_Session.Buttons[0].top_layer = IMG_TILE4_HOME_LIGHT;
					page_Session.Buttons[0].text_field_id = SF_LABEL4_LIGHT;
					page_Session.Buttons[0].translated_text_id = STRING_HOME;
					
					page_Session.Buttons[1].action_id = BTN_ACTION_NONE;
					page_Session.Buttons[1].image_id = IMG_HALFTILE41_BLANK_LIGHT;
					page_Session.Buttons[1].press_image_id = IMG_HALFTILE41_FRAME_LIGHT;
					page_Session.Buttons[1].top_layer = IMG_HALFTILE41_STOP_ICON_LIGHT;
					
					
					
					Ring_t *ringPointer[] = {&ring_Tile1SessionRing,&ring_Tile2SessionRing,&ring_Tile3SessionRing};
					
					for (uint8_t i = 0;i<3;i++){
						Ring_t *ptr = ringPointer[i];
						for (uint8_t j = 0; j<8; j++){
							ptr->buffer[2][j]=SumStrings[j];
						}
						
						update_Ringbuffer(ringPointer[i]);
					}
					
					
					Construct_Page(&page_Session);
					
					UCSR1B |= (1<<RXCIE1);
					App_State = APP_STATE_UPDATE_HR;
					
					} else {
					App_State = APP_STATE_MAIN_MENU;
				}
				
				break;

				case APP_STATE_GOTO_CONNECT_HR:
				
				for (uint8_t i = 0; i<6;i++){
					page_Connect_HR.Translated_String_IDs[i]=0;
				}
				page_Connect_HR.Common_String_IDs[0] = 0;
				page_Connect_HR.Common_String_IDs[1] = 0;
				if (bluetoothEngaged){
					
					if (HRDevicePaired){
						if (Heartrate_BLE) {
							page_Connect_HR.Common_String_IDs[0] = STRING_BLUETOOTH;
							
							} else {
							page_Connect_HR.Common_String_IDs[0] = STRING_ANT;
							
						}
						page_Connect_HR.Common_String_IDs[1] = STRING_HRM_NAME;
						page_Connect_HR.Translated_String_IDs[4]=STRING_CONNECTED;
						page_Connect_HR.Translated_String_IDs[5]=STRING_HEARTRATE;
						App_State = APP_STATE_UPDATE_HR;
						}else {

						page_Connect_HR.Translated_String_IDs[2]=STRING_NO;
						page_Connect_HR.Translated_String_IDs[3]=STRING_SENSORS;
						page_Connect_HR.Translated_String_IDs[4]=STRING_FOUND;
						App_State=APP_STATE_IDLE;
					}
					
					} else {
					page_Connect_HR.Translated_String_IDs[0]=STRING_HR;
					page_Connect_HR.Translated_String_IDs[1]=STRING_DISABLED;
					App_State=APP_STATE_IDLE;
				}
				Construct_Page(&page_Connect_HR);
				if (channel_mode==CHANNELMODE_BOTH){
					sprintf(String_buffer,"ANT / BT");
					} else if (channel_mode==CHANNELMODE_ANTONLY){
					sprintf(String_buffer,"ANT");
					} else if (channel_mode==CHANNELMODE_BLEONLY){
					sprintf(String_buffer,"BT");
				}
				DisplayText(SF_LABEL3_LIGHT,String_buffer,strlen(String_buffer));
				
				break;

				case APP_STATE_START_REFRESH_HR:
				HR_Search_counter =0;
				gfit_send_command(get_gfit_command(GFIT_HRP_DISCONNECT_DEVICE));
				HRDevicePaired = false;
				for (uint8_t i = 0; i<6;i++){
					page_Connect_HR.Translated_String_IDs[i]=0;
				}
				page_Connect_HR.Common_String_IDs[0] = 0;
				page_Connect_HR.Common_String_IDs[1] = 0;
				updateHRIndicator();
				App_State = APP_STATE_REFRESH_HR;
				Construct_Page(&page_Connect_HR);
				break;

				case APP_STATE_REFRESH_HR:
				
				if (HR_Search_counter%250==0){
					DisplayImage(IMG_HALFTILE40_BLANK_NEUTRAL);
					DisplayImage(IMG_HALFTILE40_WAIT_HR_1_3+(HR_Search_counter/250)%3);
				}
				HR_Search_counter++;
				App_State = APP_STATE_REFRESH_HR_2;
				
				if (HRDevicePaired || HR_Search_counter>=25*750){
					App_State = APP_STATE_GOTO_CONNECT_HR;
				}
				
				break;

				case APP_STATE_REFRESH_HR_2:
				
				App_State = APP_STATE_REFRESH_HR;
				break;
				case APP_STATE_TOGGLE_POWER_SOURCE:
				AppPowerSource=!AppPowerSource;
				timeout = AppPowerSource? GRID_TIMEOUT : BATTERY_TIMEOUT;
				App_State = APP_STATE_SETUP_SCREEN;
				break;
				case APP_STATE_UPDATE_HR:
				if (HR_Update_Counter==0){
					sprintf(Common_Strings[STRING_VALUE_HEARTRATE],"%d",Heartrate_BPM);
					if (activePage->page_id==PAGE_ID_CONNECT_HR) {
						DisplayText(SF_VALUE1_LIGHT,Common_Strings[STRING_VALUE_HEARTRATE],strlen(Common_Strings[STRING_VALUE_HEARTRATE]));
						}	else {
						if (ring_Tile1SessionRing.idx==8){
							DisplayText(SF_VALUE1_LIGHT,Common_Strings[STRING_VALUE_HEARTRATE],strlen(Common_Strings[STRING_VALUE_HEARTRATE]));
						}
						if (ring_Tile2SessionRing.idx==8){
							DisplayText(SF_VALUE2_LIGHT,Common_Strings[STRING_VALUE_HEARTRATE],strlen(Common_Strings[STRING_VALUE_HEARTRATE]));
						}
						if (ring_Tile3SessionRing.idx==8){
							DisplayText(SF_VALUE3_LIGHT,Common_Strings[STRING_VALUE_HEARTRATE],strlen(Common_Strings[STRING_VALUE_HEARTRATE]));
						}
					}
				}
				App_State = APP_STATE_UPDATE_HR_2;
				break;
				case APP_STATE_UPDATE_HR_2:
				HR_Update_Counter++;
				App_State = APP_STATE_UPDATE_HR;
				break;

				case APP_STATE_GOTO_BT_CONNECTION:
				StartStopTimer2(true);
				inSession = true;
				EnableDisableDetect(true);
				#ifdef CICLOTTE
				EnableDisablePWM(true);
				#endif
				saveSessionType = false;
				zeroValues();
				gfit_send_command(get_gfit_command(GFIT_TRANSITION_TO_INUSE));
				App_State = APP_STATE_UPDATE_BT_CONNECTION;
				
				Construct_Page(&page_bt_connection);
				EnableDisableDetect(true);
				
				break;
				
				case APP_STATE_UPDATE_BT_CONNECTION:
				#ifdef BIKE
				if (BT_Update_Counter==0){
					Speed_Meters_per_second = SpeedFromWatts(Power_Watts);
					sprintf(String_buffer,"%d",Heartrate_BPM);
					DisplayText(SF_VALUE3_LIGHT,String_buffer,strlen(String_buffer));
					sprintf(String_buffer,"%d",(uint16_t)Power_Watts);
					DisplayText(SF_VALUE1_LIGHT,String_buffer,strlen(String_buffer));
					#ifdef CICLOTTE
					sprintf(String_buffer,"%d",Resistance_Percent/10);
					#else
					sprintf(String_buffer,"%d",Resistance_Percent);
					#endif
					
					DisplayText(SF_VALUE2_LIGHT,String_buffer,strlen(String_buffer));
				}
				
				#endif
				#ifdef SPRINTBOK
				if (BT_Update_Counter==0){
					actTickUpdate(lastSecMoved+5<SecTimer);
					sprintf(String_buffer,"%d",Heartrate_BPM);
					DisplayText(SF_VALUE3_LIGHT,String_buffer,strlen(String_buffer));
					
					if (Speed_Meters_per_second>0.2){
						uint8_t mpkm = (uint16_t)floor(1000.0/(Speed_Meters_per_second))/60;
						uint8_t spkm = (uint16_t)floor(1000.0/(Speed_Meters_per_second))%60;
						
						sprintf(String_buffer,"%02d:%02d",mpkm,spkm);
						} else {
						sprintf(String_buffer,"--:--");
					}
					
					DisplayText(SF_VALUE1_LIGHT,String_buffer,strlen(String_buffer));

					sprintf(String_buffer,"%d",(uint16_t)floor(Speed_Meters_per_second*3.6));
					DisplayText(SF_VALUE2_LIGHT,String_buffer,strlen(String_buffer));
				}
				#endif
				App_State = APP_STATE_UPDATE_BT_CONNECTION2;
				break;
				
				case APP_STATE_UPDATE_BT_CONNECTION2:
				BT_Update_Counter++;
				App_State = APP_STATE_UPDATE_BT_CONNECTION;
				break;
				case APP_STATE_QUIT_PEER_CONNECTION:
				BT_connected = false;
				inSession=false;
				StartStopTimer2(false);
				#ifdef CICLOTTE
				EnableDisablePWM(false);
				#endif
				gfit_send_command(get_gfit_command(GFIT_TRANSITION_TO_FINISHED));
				gfit_send_command(get_gfit_command(GFIT_TRANSITION_TO_READY));
				App_State = APP_STATE_MAIN_MENU;
				break;
				
				case APP_STATE_SESSION_PAUSED:
				page_Session.Buttons[0].action_id = BTN_ACTION_RESUME_SESSION;
				page_Session.Buttons[0].top_layer = IMG_HALFTILE40_PLAY_ICON_DARK;
				RemoveImage(App_Darkmode? IMG_HALFTILE41_STOP_ICON_DARK:IMG_HALFTILE41_STOP_ICON_LIGHT);
				DisplayImage(IMG_HALFTILE41_PAUSED_ICON_LIGHT);
				DisplayText(SF_HALFTILE41_LABEL_LIGHT,get_translated_string(App_Language,STRING_PAUSED),strlen(get_translated_string(App_Language,STRING_PAUSED)));
				
				//deactivateButton(BTN_TILE4B);
				Showbutton(&activePage->Buttons[0],false);

				App_State = APP_STATE_IDLE;
				StartStopTimer2(false);
				break;
				
				case APP_STATE_SESSION_RESUMED:
				page_Session.Buttons[0].action_id = BTN_ACTION_PAUSE_SESSION;
				page_Session.Buttons[0].top_layer = IMG_HALFTILE40_PAUSE_ICON_DARK;
				Showbutton(&activePage->Buttons[0],false);
				Showbutton(&activePage->Buttons[1],false);
				StartStopTimer2(true);
				App_State = APP_STATE_SESSION_SCREEN_IDLE;
				break;
				
				case APP_STATE_CONFIRM_SCREEN:
				StartStopTimer2(false);
				Construct_Page(&page_confirm_end);
				App_State = APP_STATE_IDLE;
				break;
				case APP_STATE_CONFIRM_CANCELED:
				if (saveSessionType){
					Construct_Page(&page_Session);
					App_State = APP_STATE_SESSION_SCREEN_IDLE;
					StartStopTimer2(true);
					} else {
					App_State = APP_STATE_UPDATE_BT_CONNECTION;
					Construct_Page(&page_bt_connection);
				}
				
				break;
				#ifdef BIKE
				#ifndef CICLOTTE

				case APP_STATE_GOTO_CALIBRATION_UP:
				page_calibrate.No_Of_Gifs=1;
				page_calibrate.Gifs[0]=GIF_CALIBRATE_UP;
				page_calibrate.Buttons[0].top_layer=IMG_TILE4_NEXT_ICON_LIGHT;
				page_calibrate.Buttons[0].translated_text_id = STRING_NEXT;
				page_calibrate.Buttons[0].action_id=BTN_ACTION_GOTO_CALIBRATE_DOWN;
				page_calibrate.Translated_String_IDs[1]=STRING_CALIBRATION_LINE0;
				page_calibrate.Translated_String_IDs[2]=STRING_CALIBRATION_LINE1;
				page_calibrate.Translated_String_IDs[3]=STRING_CALIBRATION_LINE2;
				page_calibrate.Translated_String_IDs[4]=STRING_CALIBRATION_LINE3_MAX;
				page_calibrate.Translated_String_IDs[5]=STRING_CALIBRATION_LINE4_MAX;
				page_calibrate.Translated_String_IDs[6]=STRING_STEP;
				
				Construct_Page(&page_calibrate);
				sprintf(String_buffer,"1/2");
				DisplayText(SF_VALUE3_LIGHT,String_buffer,strlen(String_buffer));
				break;
				case APP_STATE_GOTO_CALIBRATION_DOWN:
				readADC();
				adc = ADC_GetResult();
				
				writeByteToEEPROM(0x00f0,readByteFromEEPROM(0x00f0)&(~(1<<1)));
				writeByteToEEPROM(0x00f1,(uint8_t)(adc>>8));
				writeByteToEEPROM(0x00f2,(uint8_t)(adc&0xff));
				
				page_calibrate.Gifs[0]=GIF_CALIBRATE_DOWN;
				page_calibrate.Buttons[0].top_layer=IMG_TILE4_NEXT_ICON_LIGHT;
				page_calibrate.Buttons[0].translated_text_id = STRING_NEXT;
				page_calibrate.Buttons[0].action_id=BTN_ACTION_GOTO_CALIBRATE_END;
				page_calibrate.Translated_String_IDs[1]=STRING_CALIBRATION_LINE0;
				page_calibrate.Translated_String_IDs[2]=STRING_CALIBRATION_LINE1;
				page_calibrate.Translated_String_IDs[3]=STRING_CALIBRATION_LINE2;
				page_calibrate.Translated_String_IDs[4]=STRING_CALIBRATION_LINE3_MIN;
				page_calibrate.Translated_String_IDs[5]=STRING_CALIBRATION_LINE4_MIN;
				page_calibrate.Translated_String_IDs[6]=STRING_STEP;
				Construct_Page(&page_calibrate);
				sprintf(String_buffer,"2/2");
				DisplayText(SF_VALUE3_LIGHT,String_buffer,strlen(String_buffer));
				break;
				case APP_STATE_GOTO_CALIBRATION_END:
				readADC();
				adc = ADC_GetResult();
				
				writeByteToEEPROM(0x00f0,readByteFromEEPROM(0x00f0)&(~(1<<0)));
				
				writeByteToEEPROM(0x00f3,(uint8_t)(adc>>8));
				writeByteToEEPROM(0x00f4,(uint8_t)(adc&0xff));

				
				
				
				
				writeByteToEEPROM(0x00f0,readByteFromEEPROM(0x00f0)&(~(1<<4)));

				Resistance_Low_value = (uint16_t) (readByteFromEEPROM(0x00f1)<<8) + readByteFromEEPROM(0x00f2);
				Resistance_High_value = (uint16_t) (readByteFromEEPROM(0x00f3)<<8) + readByteFromEEPROM(0x00f4);

				page_calibrate.No_Of_Gifs=0;
				page_calibrate.Gifs[0]=GIF_CALIBRATE_UP;
				page_calibrate.Buttons[0].top_layer=IMG_TILE4_HOME_LIGHT;
				page_calibrate.Buttons[0].translated_text_id = STRING_HOME;
				page_calibrate.Buttons[0].action_id=BTN_ACTION_RETURN_FROM_CALIBRATE;
				page_calibrate.Translated_String_IDs[1]=STRING_CALIBRATION_LINE0_END;
				page_calibrate.Translated_String_IDs[2]=STRING_CALIBRATION_LINE1_END;
				page_calibrate.Translated_String_IDs[3]=0;
				page_calibrate.Translated_String_IDs[4]=0;
				page_calibrate.Translated_String_IDs[5]=0;
				page_calibrate.Translated_String_IDs[6]=0;
				Construct_Page(&page_calibrate);
				break;
				#endif				
				#endif
				
				case (APP_STATE_UNLOCK_DEVMODE):
					DisplayImage(IMG_TILE0_FINISH);
					App_State = APP_STATE_DEV_MODE_IDLE;
				break;
				case APP_STATE_DEV_MODE_IDLE:
					dev_mode_update_counter++;
					App_State = APP_STATE_DEV_MODE_IDLE2;
				break;
				case APP_STATE_DEV_MODE_IDLE2:
					if (dev_mode_update_counter==0){
				
				
					sprintf(String_buffer,"LO:%d",Resistance_Low_value);
					DisplayText(SF_INFO_LINE1_LIGHT,String_buffer,strlen(String_buffer));
					sprintf(String_buffer,"HI:%d",Resistance_High_value);
					DisplayText(SF_INFO_LINE2_LIGHT,String_buffer,strlen(String_buffer));
				
					if (~readByteFromEEPROM(0x00f0)&(1<<4)){
					sprintf(String_buffer,"CAL");
					} else {
					sprintf(String_buffer,"NOT CAL");
				
					}
					DisplayText(SF_INFO_LINE3_LIGHT,String_buffer,strlen(String_buffer));
				
					//double res = MIN(MAX(1.0-((double)(MAX(ADC_GetResult(),Resistance_Low_value)-Resistance_Low_value))/(Resistance_High_value-Resistance_Low_value),0),1);
					//sprintf(String_buffer,"RES:",(uint8_t)(res*100));
					DisplayText(SF_INFO_LINE4_LIGHT,String_buffer,strlen(String_buffer));
					}
				
					App_State=APP_STATE_DEV_MODE_IDLE;
					break;
				
				case (APP_STATE_TOCK):
				tick_tock_timer++;
				App_State = APP_STATE_TICK;
				break;

				default:
				break;
			}
		}
		#endif

		///////////////////////////   USER-ACTION HANDLER /////////////////////////////
		
		#ifdef NOHRD
		if (wakeUp){
			wakeUp=false;
			
			LED_PORT&=~LED_PIN;
			
			User_State=USER_PRESS;
			app_response_id=BTN_WAKEUP;
			IdleState=IDLE_STATE_DEEPSLEEP;
		}
		if (User_State!=last_User_State){
			IdleDoggy=0;
			
			//uint8_t cmd[] = {0xc0, 0x01, 0x30, 0x31};
			switch (User_State){
				case (USER_PRESS):
				buttonPressed = app_response_id;
				
				if (buttonPressed == BTN_WAKEUP){
					
					if (IdleState==IDLE_STATE_STANDBY || IdleState==IDLE_STATE_DEEPSLEEP){
						DisplayImage(IMG_BG_SS_1+ss_image);
						
						setBrightness(App_Max_Brightness);
						ss_image= (ss_image+1)%4;
						
						if (bluetoothEngaged) {
							EnableDisableGFIT(true);
							sleep_1ms(500);
							gfit_send_command(get_gfit_command(GFIT_TRANSITION_TO_READY));
							sleep_1ms(500);
						}
						
						
						sleep_1ms(3500);
						
					}
					if (IdleState==IDLE_STATE_DEEPSLEEP){
						last_App_State=0;
						App_State=APP_STATE_MAIN_MENU;
						}else{
					Construct_Page(activePage);
					
					}
					IdleState = IDLE_STATE_ACTIVE;	
					
					
					} else {
					for (uint8_t i=0;i<activePage->No_Of_Buttons;i++){
						if (app_response_id==activePage->Buttons[i].button_id){
							Showbutton(&activePage->Buttons[i],true);
						}
					}
				}
				break;
				case (USER_SWIPE_LEFT):
				case (USER_SWIPE_RIGHT):
				case (USER_SWIPE_UP):
				case (USER_SWIPE_DOWN):
				
				if (buttonPressed){
					for (uint8_t i = 0; i<activePage->No_Of_Buttons; i++){
						if (app_response_id==activePage->Buttons[i].button_id){
							Showbutton(&activePage->Buttons[i],false);
							if (app_response_id >= BTN_TILE1L && app_response_id <= BTN_TILE3R){
								switch(app_response_id-BTN_TILE1L){
									case 0:
									case 1:
									update_Ring(&ring_Tile1SessionRing);
									break;
									case 2:
									case 3:
									update_Ring(&ring_Tile2SessionRing);
									break;
									case 4:
									case 5:
									update_Ring(&ring_Tile3SessionRing);
									break;
								}
								
								} else {
								
							}
						}
					}
					buttonPressed = 0x00;
				}
				break;
				case (USER_RELEASE):
				if (buttonPressed){
					buttonPressed = 0x00;
					WatchDoggy = 0;
					for (uint8_t i = 0; i<activePage->No_Of_Buttons; i++){
						if (app_response_id==activePage->Buttons[i].button_id){
							Showbutton(&activePage->Buttons[i],false);
							
							switch (activePage->Buttons[i].action_id){
								case BTN_ACTION_GOTO_MAIN_MENU:
									App_State = APP_STATE_MAIN_MENU;
								break;
								case BTN_ACTION_GOTO_SETTINGS:
									App_State=APP_STATE_SETTINGS_SCREEN;
								
								break;
								#ifdef SPRINTBOK
								case BTN_ACTION_GOTO_INCLINE_CORRECTION_SCREEN:
									inclineMenuUnlock+=1;
									if (inclineMenuUnlock>=5){
										App_State=APP_STATE_INCLINE_CORRECTION_SCREEN;
									}
								break;
								
								case BTN_ACTION_DECREASE_INCLINE_CORRECTION:
									if (inclineCorrStep>0){
										inclineCorrStep-=1;
									}
									App_State=APP_STATE_INCLINE_CORRECTION_SCREEN_UPDATE;
								break;
								
								case BTN_ACTION_INCREASE_INCLINE_CORRECTION:
									if (inclineCorrStep<20){
										inclineCorrStep+=1;
									}
									App_State=APP_STATE_INCLINE_CORRECTION_SCREEN_UPDATE;
								break;
								#endif
								case BTN_ACTION_GOTO_SETTINGS2:
									App_State=APP_STATE_CUSTOMIZE_SCREEN;
								break;
								case BTN_ACTION_GOTO_HRCONNECT:
									App_State = APP_STATE_GOTO_CONNECT_HR;
								break;
								
								case BTN_ACTION_TOGGLE_POWER_SOURCE:
									
									App_State= APP_STATE_TOGGLE_POWER_SOURCE;
								break;
								
								
								//////////////////////////////////////////////////
								
								case BTN_ACTION_TOGGLE_LDMODE:
								App_State = APP_STATE_TOGGLE_DARKMODE;
								break;
								case BTN_ACTION_CUSTOMIZE_SETTINGS:
								App_State = APP_STATE_CUSTOMIZE_SCREEN;
								break;
								case BTN_ACTION_SETUP_SETTINGS:
								App_State = APP_STATE_SETUP_SCREEN;
								break;
								case BTN_ACTION_GOTO_INFO:
								saveLightMode = App_Darkmode;
								App_Darkmode = 0;
								
								App_State = APP_STATE_INFO_SCREEN;
								break;
								case BTN_ACTION_RETURN_FROM_INFO:
								App_Darkmode = saveLightMode;
								App_State = APP_STATE_MAIN_MENU;
								break;


								
								case BTN_ACTION_UNLOCK_DEV_MODE:
								if (!devMode && devModeUnlockCounter<15){
								devModeUnlockCounter++;
								} else {
								
								devMode = true;
								
								App_State = APP_STATE_UNLOCK_DEVMODE;
								
								}
								break;
								
								case BTN_ACTION_QUIT_PEER_CONNECTION:
								App_State = APP_STATE_QUIT_PEER_CONNECTION;
								break;
								
								case BTN_ACTION_GOTO_TIME_SELECTION:
								App_State = APP_STATE_TIME_SELECTION_SCREEN;
								break;
								case BTN_ACTION_GOTO_DISTANCE_SELECTION:
								App_State = APP_STATE_DISTANCE_SELECTION_SCREEN;
								break;
								case BTN_ACTION_GOTO_FREE_SESSION:
								Session_type = SESSION_ID_FREE;
								App_State=APP_STATE_GOTO_SESSION_SCREEN;
								break;
								
								case BTN_ACTION_TOGGLE_LANGUAGE:
								App_State= APP_STATE_TOGGLE_LANGUAGE;
								break;
								case BTN_ACTION_TOGGLE_BLUETOOTH:
								App_State= APP_STATE_TOGGLE_BLUETOOTH;
								break;
								case BTN_ACTION_BIG_INCREASE_TIME:
								Custom_Time=MIN(Custom_Time+TimeBigGain,MAX_TIME);
								App_State = APP_STATE_UPDATE_PRESELECTION;
								break;
								case BTN_ACTION_BIG_DECREASE_TIME:
								Custom_Time= Custom_Time <= TimeBigGain? MIN_TIME : Custom_Time-TimeBigGain;
								App_State = APP_STATE_UPDATE_PRESELECTION;
								break;
								case BTN_ACTION_INCREASE_TIME:
								Custom_Time=Custom_Time+60;
								//Custom_Time=MIN(Custom_Time+1,255);
								App_State = APP_STATE_UPDATE_PRESELECTION;
								break;
								case BTN_ACTION_DECREASE_TIME:
								//Custom_Time= MAX(1,Custom_Time -1);
								Custom_Time=Custom_Time-60;
								App_State = APP_STATE_UPDATE_PRESELECTION;
								break;
								case BTN_ACTION_BIG_INCREASE_DISTANCE:
								Custom_Distance = Custom_Distance>= 0xffff -DistanceBigGain? MAX_DISTANCE: Custom_Distance+DistanceBigGain;
								//Custom_Distance = MIN(Custom_Distance+DistanceBigGain,MAX_DISTANCE);
								
								App_State = APP_STATE_UPDATE_PRESELECTION;
								break;
								case BTN_ACTION_BIG_DECREASE_DISTANCE:
								Custom_Distance = Custom_Distance <= DistanceBigGain? MIN_DISTANCE : Custom_Distance-DistanceBigGain;
								App_State = APP_STATE_UPDATE_PRESELECTION;
								break;
								case BTN_ACTION_INCREASE_DISTANCE:
								Custom_Distance = Custom_Distance>= 0xffff -DistanceGain? MAX_DISTANCE: Custom_Distance+DistanceGain;
								//Custom_Distance = MIN(Custom_Distance+DistanceGain,MAX_DISTANCE);
								App_State = APP_STATE_UPDATE_PRESELECTION;
								break;
								case BTN_ACTION_DECREASE_DISTANCE:
								Custom_Distance = Custom_Distance <= DistanceGain? MIN_DISTANCE : Custom_Distance-DistanceGain;
								
								App_State = APP_STATE_UPDATE_PRESELECTION;
								break;
								
								case BTN_ACTION_GOTO_CONTRAST:
								App_State = APP_STATE_GOTO_CONTRAST;
								break;
								case BTN_ACTION_CONTRAST_HIGH:
								App_Max_Brightness = CONTRAST_HIGH;
								brightness=2;
								setBrightness(App_Max_Brightness);
								break;
								case BTN_ACTION_CONTRAST_MID:
								App_Max_Brightness = CONTRAST_MID;
								brightness=1;
								setBrightness(App_Max_Brightness);
								break;
								case BTN_ACTION_CONTRAST_LOW:
								App_Max_Brightness = CONTRAST_LOW;
								brightness=0;
								setBrightness(App_Max_Brightness);
								break;
								
								#ifdef BIKE
								case BTN_ACTION_GOTO_CALIBRATE_UP:
								saveLightMode = App_Darkmode;
								App_Darkmode = 0;
								App_State = APP_STATE_GOTO_CALIBRATION_UP;
								break;
								case BTN_ACTION_GOTO_CALIBRATE_DOWN:
								App_State = APP_STATE_GOTO_CALIBRATION_DOWN;
								break;
								case BTN_ACTION_GOTO_CALIBRATE_END:
								App_State = APP_STATE_GOTO_CALIBRATION_END;
								break;
								case BTN_ACTION_RETURN_FROM_CALIBRATE:
								App_Darkmode = saveLightMode;
								App_State = APP_STATE_MAIN_MENU;
								break;

								#endif
								case BTN_ACTION_REFRESH_HR:
								
								App_State = APP_STATE_START_REFRESH_HR;
								break;
								case BTN_ACTION_CYCLE_CHANNEL_MODES:
									channel_mode=(channel_mode+1)%3;
									if (channel_mode==CHANNELMODE_BOTH){
										sprintf(String_buffer,"ANT / BT");
									} else if (channel_mode==CHANNELMODE_ANTONLY){
										sprintf(String_buffer,"ANT only");
									} else if (channel_mode==CHANNELMODE_BLEONLY){
										sprintf(String_buffer,"BT only");
									} 
									DisplayText(SF_LABEL3_LIGHT,String_buffer,strlen(String_buffer));
									
								break;
								
								case BTN_ACTION_START_TIME_SESSION:
								Session_type = SESSION_ID_TIME;
								App_State=APP_STATE_GOTO_SESSION_SCREEN;
								break;
								case BTN_ACTION_START_DISTANCE_SESSION:
								Session_type = SESSION_ID_DISTANCE;
								App_State=APP_STATE_GOTO_SESSION_SCREEN;
								break;
								case BTN_ACTION_LEFTCYCLE_SESSION_TILE1:
								left_Cycle_Ring(&ring_Tile1SessionRing);
								update_Ring(&ring_Tile1SessionRing);
								break;
								case BTN_ACTION_RIGHTCYCLE_SESSION_TILE1:
								cycle_Ring(&ring_Tile1SessionRing);
								update_Ring(&ring_Tile1SessionRing);
								break;
								case BTN_ACTION_LEFTCYCLE_SESSION_TILE2:
								left_Cycle_Ring(&ring_Tile2SessionRing);
								update_Ring(&ring_Tile2SessionRing);
								break;
								case BTN_ACTION_RIGHTCYCLE_SESSION_TILE2:
								cycle_Ring(&ring_Tile2SessionRing);
								update_Ring(&ring_Tile2SessionRing);
								break;
								
								case BTN_ACTION_LEFTCYCLE_SESSION_TILE3:
								
								left_Cycle_Ring(&ring_Tile3SessionRing);
								update_Ring(&ring_Tile3SessionRing);
								break;
								case BTN_ACTION_RIGHTCYCLE_SESSION_TILE3:
								cycle_Ring(&ring_Tile3SessionRing);
								update_Ring(&ring_Tile3SessionRing);
								break;

								case BTN_ACTION_STOP_SESSION:
								App_State = APP_STATE_CONFIRM_SCREEN;
								break;
								case BTN_ACTION_PAUSE_SESSION:
								App_State = APP_STATE_SESSION_PAUSED;
								break;
								case BTN_ACTION_RESUME_SESSION:
								App_State = APP_STATE_SESSION_RESUMED;
								break;
								case BTN_ACTION_WAKEUP:
								App_State = APP_STATE_WAKEUP;
								break;
								case BTN_ACTION_INCREASE_ANGLE:
								setAngleIndex = MIN(setAngleIndex+1,9);
								setServoAngle(setAngleIndex);
								break;
								case BTN_ACTION_DECREASE_ANGLE:
								if (setAngleIndex>=1){
									setAngleIndex = (setAngleIndex-1);
									} else {
									setAngleIndex=0;
								}
								setServoAngle(setAngleIndex);
								break;
								case BTN_ACTION_CONFIRM_CONFIRM:
								if (saveSessionType) {
									App_State = APP_STATE_SESSION_FINISHED;
									} else {
									App_State = APP_STATE_QUIT_PEER_CONNECTION;
								}
								
								break;
								case BTN_ACTION_CONFIRM_CANCEL:
								App_State = APP_STATE_CONFIRM_CANCELED;
								break;
								
								//////////////////////////////////////////////////
								default:
								break;
							}
							break;
						}
						
					}
				}
				
				break;
				default:
				break;
			}
			last_User_State=User_State;
		}
		#endif
		///////////////////////////   ACT ON EVENTS //////////////////////

		if (buttonPressed) {
			WatchDoggy++;
			if (WatchDoggy > 200) {
				User_State = USER_RELEASE;
			}
		}
		
		if (inSession){
			IdleDoggy = 0;
		} else
		{
			if (IdleState!=IDLE_STATE_DEEPSLEEP){
				IdleDoggy++;
				if (IdleDoggy<timeout){
					setBrightness(App_Max_Brightness);
				}
				if (IdleDoggy==timeout) {
					EnableDisableDetect(true);
					for (uint8_t i = 0; i<activePage->No_Of_Buttons;i++){
						deactivateButton(activePage->Buttons[i].button_id);
					}

					setBrightness(App_Max_Brightness*2/3);
					DisplayActivateButton(BTN_WAKEUP);
					IdleState = IDLE_STATE_INACTIVE;
					
				}
				if (IdleDoggy==2*timeout) {
					if (bluetoothEngaged) gfit_send_command(get_gfit_command(GFIT_TRANSITION_TO_OFF));
					setBrightness(0);
					DisplayActivateButton(BTN_WAKEUP);
					IdleState = IDLE_STATE_STANDBY;
				}
				if (AppPowerSource == POWER_SOURCE_BATTERY && IdleDoggy==3*timeout) {
					deepsleep = true;
					IdleState = IDLE_STATE_DEEPSLEEP;
				}
			}
		}
		
		if (IdleState!=IDLE_STATE_ACTIVE && UpdateTick){
			//User_State = USER_PRESS;
			//app_response_id = BTN_WAKEUP;
			wakeUp=true;
		}
		if (deepsleep){
			MCUSR &= ~(1 << WDRF);
			WDTCSR |= (1 << WDCE)|(1 << WDE);
			WDTCSR = (1 << WDP3) | (1 << WDP0); // Setze Prescaler für 8 Sekunden
			WDTCSR |= (1 << WDIE); // Aktiviere Watchdog-Interrupt
			EnableDisableUsarts(false);
			#ifdef CICLOTTE
			EnableDisablePWM(false);
			#endif
			EnableDisableDisplay(false);
			EnableDisableGFIT(false);
			wasAsleep = true;
		}
		while (deepsleep){
			EnableDisableDetect(true)	;
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_enable();
			sleep_mode();
			sleep_disable();
			MCUSR &= ~(1 << WDRF);
			WDTCSR&=~(1<<WDE);
			WDTCSR |= (1 << WDIE); // Aktiviere Watchdog-Interrupt nach ISR
			
			//PORTA|=(1<<3);
			EnableDisablePBWake(true);
			sleep_1ms(PBWAKE_DURATION);	
			EnableDisablePBWake(false);
			
		}
		if (wasAsleep){
			EnableDisableLED(true);
			wdt_off();
			sleep_1ms(500);
			EnableDisableLED(false);
			wdt_enable(WDTO_15MS);
			while(1){}
				
			
		}		
		
		
		

		
			
			
		
		
		
		
		
		
		
		if (HRDevicePaired){
			if (HRDoggy>HRDoggyTimeOut)	{
				
				HRDevicePaired = false;
				Heartrate_BPM =0;
				updateHRIndicator();
				
				
				} else {
				HRDoggy++;
			}
		}
		
		

		if (Timer2Ticks_HalfSecond>1){
			
			SecTimer+=1;
			
			if (Heartrate_BPM==0) SecsWithoutHR+=1;
			
			Heartrate_Accumulated+=Heartrate_BPM;
			#ifdef BIKE
			Distance_Meters+=Speed_Meters_per_second;
			Calories_Accumulated+=(1.22904*Speed_Meters_per_second+1)*0.018148148148148;
			#endif
			#ifdef SPRINTBOK
			Calories_Accumulated+=(3.4368*Speed_Meters_per_second+1)*0.018148148148148;
			#endif
			
			Timer2Ticks_HalfSecond=0;
			Watts_Accumulated+=Power_Watts;
			
		}
		
		#ifdef BIKE
		if (UpdateRes) actResUpdate();
		#endif
		if (UpdateRes) {
			
		}

		if (UpdateTick)	actTickUpdate(lastSecMoved+5<SecTimer);
		
		if (parse_Display(&UsartRX_Buffer,&Display_Parser)) actDisplay();
		
		
		if (onoff){
		if (parse_GFIT(&GFITRX_Buffer,&GFIT_Parser)) actGFIT();
		} else {
		if (parse_GFIT(&GFITRX_Buffer,&GFIT_Parser)) actGFIT();
		//parse_GFIT(&GFITRX_Buffer,&GFIT_Parser);	
		}
		
		
		//////////// CLEANUP RECEIVE BUFFERS ///////////
		sleep_1ms(3);
		
		//PORTA ^= (1<<DDA3);
	}
	
}

void setServoAngle(uint8_t angleIndex) {
    uint16_t angle = setAngle[angleIndex];
	if (angle > 180) angle = 180;  // Limit angle to 0-180 degrees

    uint16_t minPulseWidth = 156;  // Timer count for minimum pulse width (0.5 ms)
    uint16_t maxPulseWidth = 781;  // Timer count for maximum pulse width (2.5 ms)

    // We use a multiplication first approach to avoid truncation problems
    uint16_t range = maxPulseWidth - minPulseWidth;
    uint32_t temp = (uint32_t)angle * range;  // Use a larger integer type to avoid overflow
    temp /= 180;                             // Now divide to scale the result down
    OCR1B = minPulseWidth + (uint16_t)temp;  // Add the base pulse width
}

void EnableDisableGFIT(bool stat){
	cli();
	if (stat){
		
		
		GFIT_PORT &=~GFIT_SLEEP_PIN;
		GFIT_PORT|=GFIT_ENABLE_PIN;
		GFIT_PORT|=GFIT_RST_PIN;
	}else{
		GFIT_PORT&=~GFIT_RST_PIN;
		GFIT_PORT|=GFIT_SLEEP_PIN;
		GFIT_PORT&=~GFIT_ENABLE_PIN;
	}
	sei();
}

void EnableDisableDisplay(bool stat){
	cli();
	if (stat){
		DISPLAY_PORT |= DISPLAY_PIN;
//		PORTD |= (1<<5);
	} else {
		DISPLAY_PORT &= ~DISPLAY_PIN;
	//	PORTD &= ~(1<<5);
	}
	sei();
}

void EnableDisablePBWake(bool stat){
	cli();
	if (stat){
		PBWAKE_PORT |= PBWAKE_PIN;
		} else {
		PBWAKE_PORT &=~PBWAKE_PIN;
	}
	sei();
}

void EnableDisableLED(bool stat){
	cli();
	if (stat){
		LED_PORT |= LED_PIN;
		} else {
		LED_PORT &=~LED_PIN;
	}
	sei();
}


void EnableDisableUsarts(bool stat){
	cli();
	if (stat){
		UCSR1B = (1 << RXEN1) | (1 << TXEN1)| (1 << RXCIE1);
		UCSR0B = (1 << RXEN0) | (1 << TXEN0)| (1 << RXCIE0);	
		DDRD |= (1 << 1);
		DDRD |= (1 << 3);
	} else {
		UCSR1B &= ~((1 << RXEN1) | (1 << TXEN1)| (1 << RXCIE1));
		UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0)| (1 << RXCIE0));	
		DDRD &= ~(1 << 1);
		DDRD &= ~(1 << 3);
	}
	sei();
}

void EnableDisableDetect(bool enable){
	cli();
	if (enable){
		EIMSK |= (1 << INT2);
		} else {
		EIMSK &= ~(1 << INT2);
	}
	sei();
}

void EnableDisablePWM(bool enable){
	cli();
	if (enable){
		ICR1 = 6249;  // Set the top value for a period of 20 ms
		TCCR1A = (1 << COM1B1) | (1 << WGM11);  // Set non-inverting mode for OC1B
		TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);  // Set prescaler to 64, Fast PWM
		DDRD |= (1 << DDD4);
	} else {
		TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));  // Compare Output Mode auf „Normal Port Operation“ setzen
		TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));  // Timer stoppen
		PORTD&=~(1<<4);
		DDRD &=~(1<<DDD4);
	}
	sei();
}

void initTimer0(void){
	cli();
	TCCR0A = (1<<WGM01);
	TCCR0B = (1<<CS00)|(1<<CS02);
	OCR0A = 19;
	TIMSK0 = (1<<OCIE1A);
	sei();
}

void Setup_Registers(void){
	
	cli();
	//////////// SETUP GPIO
	
	DISPLAY_DIR_REG |= DISPLAY_PIN;//DDRA |= (1<<DDA4); //DISPLAYPWR
	PBWAKE_DIR_REG |= PBWAKE_PIN;  //DDRA |= (1<<DDA5); //PBHEARTBEAT
	
	LED_DIR_REG |= LED_PIN;//DDRA |= (1<<DDA3); //LED
	GFIT_DIR_REG |= GFIT_SLEEP_PIN;//DDRB |= (1<<DDB1); //SLEEP
	GFIT_DIR_REG |= GFIT_ENABLE_PIN;//	DDRB |= (1<<DDB3); //LDO-EN and TUX-OE
	GFIT_DIR_REG|=GFIT_RST_PIN;//DDRB |= (1<<DDB0); //RST
	DISPLAY_RST_DIR_REG|=DISPLAY_RST_PIN;//DDRD |= (1<<5); // RSTDISPLAY
	DISPLAY_RST_PORT|=DISPLAY_RST_PIN;//PORTD|= (1<<5);
	//PORTB |= (1<<DDB3);// LDO AND TUX ON
	
	EnableDisableDisplay(true);
	EnableDisableGFIT(BT_on_startup);
	
	////////// TIMER 1 - PWM OUT ON PD4);


	
	#ifdef CICLOTTE
	EnableDisablePWM(true);
	#endif
	////////// TIMER 2 - 1 sec interrupt


    //TIMSK2 |= (1 << OCIE2A);  // Enable timer compare interrupt
    

	///////// TIMER 0 - for sleep_1ms

	initTimer0();
	
	//////////// SETUP ADC ON SLIDER PIN
	// Reference voltage: AVcc
	
	#ifdef BIKE
	#ifndef CICLOTTE
	ADMUX = (1 << REFS0); // Select AVcc as reference and ADC0 (PA0) as input

	// Enable ADC and set prescaler to 128 for 20MHz clock
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	#endif
	#endif
	
	PCICR |= (1 << PCIE0);

	 // Set baud rate
	 UBRR0 = (unsigned char)(129 >> 8)|(unsigned char)129;
	 //UBRR0H = (unsigned char)(129 >> 8);
	 //UBRR0L = (unsigned char)129;
	 // Enable receiver and transmitter
	 
	 // Set frame format: 8 data bits, 1 stop bit
	 UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	 // Set TX0 (PD1) as output
	 DDRD |= (1 << 1);
	 // Set double speed mode
	 UCSR1A = (1 << U2X1);

	 // Set baud rate
	 UBRR1H = (unsigned char)(21>> 8);
	 UBRR1L = (unsigned char)21;
	 // Enable receiver and transmitter
	 
	 // Set frame format: 8 data bits, 1 stop bit
	 UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
	 // Set TX1 (PD3) as output
	 DDRD |= (1 << 3);

	/////////// INTERRUPT FOR MAG PIN
	
	// Clear the external interrupt flag for INT2
	EIFR |= (1 << INTF2);
	// Set INT2 to trigger on rising edge
	EICRA |= (1 << ISC21) | (1 << ISC20);
	// Enable INT2
	
	// WATCHDOG
	
	
}


ISR(WDT_vect) {

}

// ISR for INT2
ISR(INT2_vect) {
	toggler = !toggler;
	
	// Interrupt service routine code here
	if (deepsleep){
		MCUSR &= ~(1 << WDRF);
		WDTCSR &= ~(1 << WDIE); // DeAktiviere Watchdog-Interrupt
		deepsleep = false;
	} else {

	Interval = (tick-tock);
	tock=tick;
	tickctr+=1;
	
	LED_PORT^=LED_PIN;
	//portd = ioport_get_port_level(IOPORT_PORTD,1);
	UpdateTick = 1;
	lastSecMoved = SecTimer;
	distanceTicks++;
	
	/*
	if (TCNT2-tock>20){
		tick = TCNT2;
		
		tickDiff = tick-tock;
		sprintf(String_buffer,"%u",tickDiff);
		DisplayText(SF_LABEL0_LIGHT,String_buffer,strlen(String_buffer));
		tock = tick;
		LED_PORT^=LED_PIN;
		//portd = ioport_get_port_level(IOPORT_PORTD,1);
		UpdateTick = 1;
		lastSecMoved = SecTimer;
		distanceTicks++;
		} else {
		tock = TCNT2;
	}
	*/
	}
}



ISR(PCINT0_vect) {
	PORTB ^= (1<<PORTB3);
}


uint8_t a = 0;
ISR(USART0_RX_vect) {
	a = UDR0;
	ring_buffer_write (&GFITRX_Buffer,a);
}

ISR(USART1_RX_vect) {
	// Handle received data for USART1
	
	ring_buffer_write (&UsartRX_Buffer,UDR1);
}


ISR(TIMER2_COMPA_vect) {
	tick+=1;
	Timer2_Overflow_Counter++;
	if (Timer2_Overflow_Counter >= 76){
		OCR2A = 252; // Adjust value for compensation
	} 
	if (Timer2_Overflow_Counter >= 76+237) { // Approximately 500 ms
	OCR2A = 238;
	Timer2_Overflow_Counter= 0;
	
	
		
	IdleDoggy=0;
	
	
	Timer2Ticks_HalfSecond += 1;

	UpdateRes = true;
	if (lastSecMoved+5<SecTimer){
		UpdateTick = 1;
	}
	
	if (inSession && !BT_connected){
		last_App_State=APP_STATE_IDLE;
		App_State=APP_STATE_SESSION_SCREEN_UPDATE;
	}
		}
}

ISR(TIMER0_COMPA_vect) {
	ms_counter++;
}

// Function to create a blocking delay of x milliseconds

/////////////////////    COMMON FUNCTIONS IMPLEMENTATION ////////////////////


void updateHRIndicator(void){
	uint8_t iconid = HRDevicePaired? IMG_HR_INDICATOR_ICON_ON : IMG_HR_INDICATOR_ICON_OFF;
	generic_Page.Images[generic_Page.No_Of_Images-1]=iconid;
	
	page_Information.Images[page_Information.No_Of_Images-1]=iconid;
	page_Session.Images[page_Session.No_Of_Images-1]=iconid;
	page_Distance_Selection.Images[page_Distance_Selection.No_Of_Images-1]=iconid;
	page_Time_Selection.Images[page_Time_Selection.No_Of_Images-1]=iconid;
	page_Connect_HR.Images[page_Connect_HR.No_Of_Images-1]=iconid;
	page_confirm_end.Images[page_confirm_end.No_Of_Images-1]=iconid;
	page_calibrate.Images[page_calibrate.No_Of_Images-1]=iconid;
	page_bt_connection.Images[page_bt_connection.No_Of_Images-1]=iconid;
	
	if (activePage->page_id==PAGE_ID_CONNECT_HR) {
		App_State = APP_STATE_GOTO_CONNECT_HR;
	}
	/*
	if (Heartrate_BLE) {
		for (uint8_t i = 0; i<MIN(9,gfit_message_buffer[13]);i++){
			Common_Strings[STRING_HRM_NAME][i] = gfit_message_buffer[14+i];
		}
		Common_Strings[STRING_HRM_NAME][MIN(9,gfit_message_buffer[13])]='\0';
		} else {
		sprintf(Common_Strings[STRING_HRM_NAME],"%u",(((uint16_t)gfit_message_buffer[8])<<8)+gfit_message_buffer[7]);
	}
*/
	
	DisplayImage(iconid);
}


void setBrightness(uint8_t br) {
	if (br!=App_Brightness){
		if (br==0){
			DisplayImage(IMG_BG_BLACK);
		}
		char buffer[] = {0xba,br};
		DisplayCMD(buffer,2);
		App_Brightness = br;
	}
}


void StartStopTimer2(bool start){
	cli();
	if (start) {
		
		TIMSK2 |= (1<<OCIE2A);
		TCCR2A |= (1 << WGM21);  
		OCR2A = 249;             
		TCCR2B |= (1 << CS22) | (1 << CS20);  
		TCNT2 = 0;                
		
		}	else {
		TIMSK2 = 0;
	}
	sei();
}

void writeByteToEEPROM(uint16_t address, uint8_t data) {
	eeprom_write_byte((uint8_t*)address, data);
}

uint8_t readByteFromEEPROM(uint16_t address) {
	return eeprom_read_byte((uint8_t*)address);
}

void sleep_1ms(uint16_t delay_ms) {
	if (TCCR0A==0){
		while (TCCR0A==0){
			initTimer0();
		}
	}
	uint16_t start_time = ms_counter;
	while ((ms_counter - start_time) < delay_ms) {
		// Wait until the required time has passed
	}
	ms_counter=0;
}

void deactivateButton(uint8_t button){
	uint8_t buffer[2]={0xa3,button};
	DisplayCMD(buffer,2);
}

void DisplayActivateButton(uint8_t button){
	uint8_t buffer[2]={0xa2,button};
	DisplayCMD(buffer,2);
}

void DisplayActivate9ACMD(uint8_t button){
	uint8_t buffer[2]={0x9a,button};
	DisplayCMD(buffer,2);
}

void RemoveImage(uint8_t image){
	uint8_t buffer[2] = {0x85,image};
	DisplayCMD(buffer,2);
}

uint8_t StringToHex(const char* string){

	uint8_t HexByte=0;
	uint8_t shift = 4;
	for (uint8_t i = 0;i<2;i++){
		if (string[i]>='0' && string[i]<='9'){
			HexByte|=((string[i]-'0')<<shift);
			} else if (string[i]>='A' && string[i]<='F'){
			HexByte|=((10+string[i]-'A')<<shift);
			} else if (string[i]>='a' && string[i]<='f'){
			HexByte|=((10+string[i]-'a')<<shift);
			} else {
		}
		shift=0;
	}
	return HexByte;
}


unsigned short Rx_CRC_CCITT(const uint8_t * puchMsg, unsigned int usDataLen)
{
	uint8_t i = 0;
	uint16_t wCRCin = 0x0000;
	uint16_t wCPoly = 0x1021;
	uint16_t wChar = 0;
	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		wCRCin ^= (wChar << 8);
		for (i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
			wCRCin = (wCRCin << 1) ^ wCPoly;
			else
			wCRCin = wCRCin << 1;
		}
	}
	return (wCRCin);
}

void DisplayCMD(const uint8_t* string, uint8_t length){
	uint8_t cdcBuffer[length+8];
	
	cdcBuffer[0]=0xaa;
	uint8_t i = 1;
	for (i;i<length+1;i++){
		cdcBuffer[i]=string[i-1];
	}
	
	cdcBuffer[i++]=(Rx_CRC_CCITT(string,length)>> 8 & 0xff);
	cdcBuffer[i++]=(Rx_CRC_CCITT(string,length) & 0xff);
	cdcBuffer[i++]=(0xe4);
	cdcBuffer[i++]=(0x1b);
	cdcBuffer[i++]=(0x11);
	cdcBuffer[i++]=(0xee);
	cdcBuffer[i]=(0x00);
	
	Display_send(cdcBuffer,i);
}


void USART_Init() {

}

void Serial_SendByte(uint8_t byte) {
	while (!(UCSR0A & (1<<UDRE0))); // Wait for empty transmit buffer
	
	UDR0 = byte;
}


void gfit_send_command(uint8_t cmd[]){
	uint8_t length = *cmd++;
	
	Serial_SendByte(0xa4);
	Serial_SendByte(length-1);
	uint8_t TXchksum = 0xa4 ^ (length-1);
	for (uint8_t i = 0; i< length;i++){
		TXchksum^=*cmd;
		Serial_SendByte(*cmd++);
	}
	Serial_SendByte(TXchksum);
}

char* get_gfit_command(uint8_t index){
	//const uint8_t (*ptr)[GFIT_MAX_LENGTH] = GFIT_commands;
	uint8_t idx = 0;
	
	const uint8_t* ptr = gfPointers[index];
	
	for (uint8_t i  = 0; i<=pgm_read_byte(&ptr[0]); i++){
		
		gfit_buffer[idx++]=pgm_read_byte(&ptr[i]);
		
	}
	
	
	return gfit_buffer;
}


void Display_send(unsigned char arr[], unsigned int len) {
	for (unsigned int i = 0; i < len; i++) {
		while (!(UCSR1A & (1<<UDRE1))); // Wait for empty transmit buffer
		UDR1 = arr[i]; // Send data
	}
}


void DisplayImage(uint8_t image){
	if (image<255){
		uint8_t App_LightDarkMode_Images [][2]= {
			
			#ifdef BIKE
			
			{IMG_BG_GRID_LIGHT,IMG_BG_GRID_DARK},
			{IMG_BG_HALFGRID_LIGHT,IMG_BG_HALFGRID_DARK},
			{IMG_BG_LIGHT,IMG_BG_DARK},
			#else
			{IMG_BG_GRID_LIGHT,IMG_BG_GRID_DARK},
			{IMG_BG_HALFGRID_LIGHT,IMG_BG_DARK_HT40_NEUTRAL},
			#endif
			{IMG_BG_LIGHT_HT40_NEUTRAL,IMG_BG_DARK_HT40_NEUTRAL},
			{IMG_TILE1_TIME_ICON_LIGHT,IMG_TILE1_TIME_ICON_DARK},
			{IMG_TILE2_DISTANCE_ICON_LIGHT,IMG_TILE2_DISTANCE_ICON_DARK},
			{IMG_TILE3_FREE_ICON_LIGHT, IMG_TILE3_FREE_ICON_DARK},
			{IMG_TILE4_SETTINGS_ICON_LIGHT,IMG_TILE4_SETTINGS_ICON_DARK},
			{IMG_HALFTILE41_MINUS_ICON_LIGHT,IMG_HALFTILE41_MINUS_ICON_DARK},
			{IMG_HALFTILE40_PLUS_ICON_LIGHT,IMG_HALFTILE40_PLUS_ICON_DARK},
			{IMG_HALFTILE40_BLANK_LIGHT,IMG_HALFTILE40_BLANK_DARK},
			{IMG_HALFTILE41_LEFT_ARROW_ICON_LIGHT,IMG_HALFTILE41_LEFT_ARROW_ICON_DARK},
			//{IMG_TILE0_BLANK_LIGHT,IMG_TILE0_BLANK_DARK},
			//{IMG_TILE0_FRAME_LIGHT,IMG_TILE0_FRAME_DARK},
			{IMG_TILE1_BLANK_LIGHT,IMG_TILE1_BLANK_DARK},
			{IMG_TILE2_BLANK_LIGHT,IMG_TILE2_BLANK_DARK},
			{IMG_TILE3_BLANK_LIGHT,IMG_TILE3_BLANK_DARK},
			{IMG_TILE4_BLANK_LIGHT,IMG_TILE4_BLANK_DARK},
			
			{IMG_TILE1_FRAME_LIGHT,IMG_TILE1_FRAME_DARK},
			{IMG_TILE2_FRAME_LIGHT,IMG_TILE2_FRAME_DARK},
			{IMG_TILE3_FRAME_LIGHT,IMG_TILE3_FRAME_DARK},
			{IMG_TILE4_FRAME_LIGHT,IMG_TILE4_FRAME_DARK},
			{IMG_HALFTILE10_FRAME_LIGHT,IMG_HALFTILE10_FRAME_DARK},
			{IMG_HALFTILE11_FRAME_LIGHT,IMG_HALFTILE11_FRAME_DARK},
			{IMG_HALFTILE20_FRAME_LIGHT,IMG_HALFTILE20_FRAME_DARK},
			{IMG_HALFTILE21_FRAME_LIGHT,IMG_HALFTILE21_FRAME_DARK},
			{IMG_HALFTILE30_FRAME_LIGHT,IMG_HALFTILE30_FRAME_DARK},
			{IMG_HALFTILE31_FRAME_LIGHT,IMG_HALFTILE31_FRAME_DARK},
			{IMG_HALFTILE40_FRAME_LIGHT,IMG_HALFTILE40_FRAME_DARK},
			{IMG_HALFTILE41_FRAME_LIGHT,IMG_HALFTILE41_FRAME_DARK},
			//{IMG_HALFTILE30_MINUS_ICON_LIGHT,IMG_HALFTILE30_MINUS_ICON_DARK},
			//{IMG_HALFTILE11_PLUS_ICON_LIGHT,IMG_HALFTILE11_PLUS_ICON_DARK},
			{IMG_HALFTILE41_LEFT_ARROW_ICON_LIGHT,IMG_HALFTILE41_LEFT_ARROW_ICON_DARK},
			{IMG_HALFTILE10_BLANK_LIGHT,IMG_HALFTILE10_BLANK_DARK},
			{IMG_HALFTILE11_BLANK_LIGHT,IMG_HALFTILE11_BLANK_DARK},
			{IMG_HALFTILE30_BLANK_LIGHT,IMG_HALFTILE30_BLANK_DARK},
			{IMG_HALFTILE31_BLANK_LIGHT,IMG_HALFTILE31_BLANK_DARK},
			{IMG_HALFTILE41_BLANK_LIGHT,IMG_HALFTILE41_BLANK_DARK},
			
			{IMG_TILE1_FRAME_LIGHT,IMG_TILE1_FRAME_DARK},
			{IMG_TILE2_FRAME_LIGHT,IMG_TILE2_FRAME_DARK},
			{IMG_TILE3_FRAME_LIGHT,IMG_TILE3_FRAME_DARK},
			{IMG_HALFTILE10_FRAME_LIGHT,IMG_HALFTILE10_FRAME_DARK},
			{IMG_HALFTILE11_FRAME_LIGHT,IMG_HALFTILE11_FRAME_DARK},
			{IMG_HALFTILE20_FRAME_LIGHT,IMG_HALFTILE20_FRAME_DARK},
			{IMG_HALFTILE21_FRAME_LIGHT,IMG_HALFTILE21_FRAME_DARK},
			{IMG_HALFTILE30_FRAME_LIGHT,IMG_HALFTILE30_FRAME_DARK},
			{IMG_HALFTILE31_FRAME_LIGHT,IMG_HALFTILE31_FRAME_DARK},
			{IMG_TILE2_SUN_ICON_LIGHT,IMG_TILE2_MOON_ICON_DARK},
			{IMG_TILE2_HR_ICON_LIGHT,IMG_TILE2_HR_ICON_DARK},
			{IMG_TILE4_NEXT_ICON_LIGHT,IMG_TILE4_NEXT_ICON_DARK},
			{IMG_TILE4_HOME_LIGHT,IMG_TILE4_HOME_DARK},
			{IMG_TILE1_INFO_LIGHT,IMG_TILE1_INFO_DARK},
			{IMG_TILE3_BT_ICON_OFF_LIGHT,IMG_TILE3_BT_ICON_OFF_DARK},
			{IMG_TILE2_CALIBRATE_UP_LIGHT,IMG_TILE2_CALIBRATE_UP_DARK},
			{IMG_HALFTILE41_STOP_ICON_LIGHT,IMG_HALFTILE41_STOP_ICON_DARK},
			{IMG_HALFTILE41_HOME_ICON_LIGHT,IMG_HALFTILE41_HOME_ICON_DARK},
			{IMG_HALFTILE41_DISCONBT_LIGHT,IMG_HALFTILE41_DISCONBT_DARK},
			{IMG_TILE2_CANCEL_ICON_LIGHT,IMG_TILE2_CANCEL_ICON_DARK},
			{IMG_HALFTILE41_PAUSED_ICON_LIGHT,IMG_HALFTILE41_PAUSED_ICON_DARK},
			{IMG_TILE1_CHECK_ICON_LIGHT,IMG_TILE1_CHECK_ICON_DARK},
			{IMG_TILE3_CONTRAST_ICON_LIGHT,IMG_TILE3_CONTRAST_ICON_DARK},
			{IMG_TILE2_CUSTOMIZE_ICON_LIGHT,IMG_TILE1_CUSTOMIZE_ICON_DARK},
			{IMG_TILE3_SETUP_ICON_LIGHT,IMG_TILE2_SETUP_ICON_DARK},
			{IMG_TILE1_FULL_BRIGHTNESS_ICON_LIGHT,IMG_TILE1_FULL_BRIGHTNESS_ICON_DARK},
			{IMG_TILE2_HALF_BRIGHTNESS_ICON_LIGHT,IMG_TILE2_HALF_BRIGHTNESS_ICON_DARK},
			{IMG_TILE3_LOW_BRIGHTNESS_ICON_LIGHT,IMG_TILE3_LOW_BRIGHTNESS_ICON_DARK},
			{IMG_TILE1_BATTERY_ICON_LIGHT,IMG_TILE1_BATTERY_ICON_DARK},
			{IMG_TILE1_GRID_ICON_LIGHT,IMG_TILE1_GRID_ICON_DARK},
			{IMG_TILE4_BACK_ICON_LIGHT,IMG_TILE4_BACK_ICON_DARK},
			{0,0}
		};

		uint8_t img = image;
		
		uint8_t i=0;
		if (App_Darkmode){
			do
			{
				if (App_LightDarkMode_Images[i][0]==image){
					img = App_LightDarkMode_Images[i][1];
					break;
				}
				i++;
			} while (App_LightDarkMode_Images[i][0]!=0);
		}
		
		uint8_t buffer[2] = {0x80,img};
		DisplayCMD(buffer,2);
	}
}


void DisplayText(uint8_t field, const char* text, uint8_t length){
	uint8_t App_LightDarkMode_Texts [][2]= {
		{SF_VALUE0_LIGHT,SF_VALUE0_DARK},
		{SF_LABEL1_LIGHT, SF_LABEL1_DARK},
		{SF_LABEL2_LIGHT,SF_LABEL2_DARK},
		{SF_LABEL3_LIGHT,SF_LABEL3_DARK},
		{SF_LABEL4_LIGHT,SF_LABEL4_DARK},
		//{SF_HALFTILE40_LINE1_LIGHT,SF_HALFTILE40_LINE1_DARK},
		//{SF_HALFTILE40_LINE2_LIGHT,SF_HALFTILE40_LINE2_DARK},
		//{SF_HALFTILE40_LINE3_LIGHT,SF_HALFTILE40_LINE3_DARK},
		{SF_VALUE1_LIGHT,SF_VALUE1_DARK},
		{SF_VALUE2_LIGHT,SF_VALUE2_DARK},
		{SF_VALUE3_LIGHT,SF_VALUE3_DARK},
		{SF_VALUE4_LIGHT,SF_VALUE4_DARK},
		{SF_LABEL10_LIGHT,SF_LABEL10_DARK},
		{SF_LABEL31_LIGHT,SF_LABEL31_DARK},
		{SF_LABEL11_LIGHT,SF_LABEL11_DARK},
		{SF_LABEL30_LIGHT,SF_LABEL30_DARK},
		{SF_INFO_LINE0_LIGHT,SF_INFO_LINE0_DARK},
		{SF_INFO_LINE1_LIGHT,SF_INFO_LINE1_DARK},
		{SF_INFO_LINE2_LIGHT,SF_INFO_LINE2_DARK},
		{SF_INFO_LINE3_LIGHT,SF_INFO_LINE3_DARK},
		{SF_INFO_LINE4_LIGHT,SF_INFO_LINE4_DARK},
		{SF_HALFTILE41_LABEL_LIGHT,SF_HALFTILE41_LABEL_DARK},

		{0,0}
	};
	
	uint8_t buffer[length+2];;
	buffer[0] = 0x9f;
	uint8_t fld = field;
	
	uint8_t i=0;
	if (App_Darkmode){
		
		do
		{
			if (App_LightDarkMode_Texts[i][0]==field){
				fld = App_LightDarkMode_Texts[i][1];
				break;
			}
			i++;
		} while (i>0 && App_LightDarkMode_Texts[i][0]!=0);
	}

	buffer[1] = fld;
	for (uint8_t i = 2; i<length+2;i++){
		buffer[i]=text[i-2];
	}
	
	DisplayCMD(buffer,length+2);
}
void DisplayCommonStringByStringId(uint8_t Fieldid){
	
	for (uint8_t i = 0; i<activePage->No_Of_Common_Strings;i++){
		if (activePage->Common_String_IDs[i]==Fieldid){
			if (Fieldid) DisplayText(activePage->Common_String_Field_IDs[i],Common_Strings[Fieldid],strlen(Common_Strings[activePage->Common_String_IDs[i]]));
		}
	}
}

void DisplayTranslatedStringByStringId(uint8_t Fieldid){
	
	for (uint8_t i = 0; i<activePage->No_Of_Translated_Strings;i++){
		if (activePage->Translated_String_IDs[i]==Fieldid){
			if (Fieldid ==STRING_PROJECTION){
				DisplayText(activePage->Translated_String_Field_IDs[i],Common_Strings[STRING_XXK_PROJECTION],strlen(Common_Strings[STRING_XXK_PROJECTION]));
				}else{
				DisplayText(activePage->Translated_String_Field_IDs[i],get_translated_string(App_Language,Fieldid),strlen(get_translated_string(App_Language,Fieldid)));
			}
		}
	}
}

void DisplayGif(uint8_t image){
	uint8_t buffer[2] = {0x88,image};
	DisplayCMD(buffer,2);
}

const char* get_translated_string(uint8_t lang, uint8_t index) {
	const char *Ptr = (const char *)pgm_read_ptr(&Translated_Strings_Pointers[lang][index]);
	strcpy_P(String_buffer,Ptr);
	
	return String_buffer;
}

void Construct_Page(Page_t *page){

	for (uint8_t i = 0; i<page->No_Of_Images;i++)	{
		DisplayImage(page->Images[i]);
	}

	if (page->No_Of_Gifs){
		for (uint8_t i = 0; i<page->No_Of_Gifs;i++){
			DisplayGif(page->Gifs[i]);
		}

	}
	//if (page->A_Command){
	//	DisplayActivate9ACMD(page->A_Command);
	//}

	for (uint8_t i = 0; i<page->No_Of_Translated_Strings;i++) {
		if (page->Translated_String_IDs[i])	DisplayText(page->Translated_String_Field_IDs[i],get_translated_string(App_Language,page->Translated_String_IDs[i]),strlen(get_translated_string(App_Language,page->Translated_String_IDs[i])));
	}

	for (uint8_t i = 0; i<page->No_Of_Common_Strings;i++) {
		if (page->Common_String_IDs[i])	DisplayText(page->Common_String_Field_IDs[i],Common_Strings[page->Common_String_IDs[i]],strlen(Common_Strings[page->Common_String_IDs[i]]));
	}

	if (page->No_Of_Buttons){
		for (uint8_t i = 0; i<page->No_Of_Buttons;i++){
			Showbutton(&page->Buttons[i],false);
		}
	}
	activePage = page;
	for (uint8_t i = 0; i<page->No_Of_Rings;i++) {
		update_Ring(page->Rings[i]);
	}
	activateSwipe();

	//sprintf(String_buffer,"%d",DEBUGCTR++);
	//DisplayText(SF_INFO_LINE0_LIGHT,String_buffer,strlen(String_buffer));


}
void cycle_Ring(Ring_t *ring){
	ring->idx = (ring->idx+1)%(ring->max_idx+1);
	update_Ringbuffer(ring);
};


void left_Cycle_Ring(Ring_t *ring){
	ring->idx = ring->idx == 0? ring->max_idx:(ring->idx-1);
	update_Ringbuffer(ring);
}


void update_Ringbuffer(Ring_t *ring){
	for (uint8_t i = 0; i<ring->no_of_targets; i++){
		*(ring->adresses[i])=ring->buffer[i][ring->idx];
	}
}


void update_Ring(Ring_t *ring){
	for (uint8_t i = 0; i<ring->no_of_targets;i++){
		(*ring->function_pointer[i])(*ring->adresses[i]);
	}
}

void set_Ring(Ring_t *ring, uint8_t set){
	ring->idx = set;
};

void activateSwipe(void){
	uint8_t cmd[] = {0xc9,0x01};
	DisplayCMD(cmd,2);
	//awaitDisplayResponse();
}
void Showbutton(Button_t *btn,bool pressed){
	if (btn->action_id){
		if (pressed) {
			if (btn->press_image_id) DisplayImage(btn->press_image_id);
			} else {
			if (btn->image_id) DisplayImage(btn->image_id);
		}
		
		if (btn->text_field_id) {
			if (btn->translated_text_id){
				DisplayText(btn->text_field_id,get_translated_string(App_Language,btn->translated_text_id),strlen(get_translated_string(App_Language,btn->translated_text_id)));
				} else if (btn->button_text_id){
				DisplayText(btn->text_field_id,Button_Strings[btn->button_text_id],strlen(Button_Strings[btn->button_text_id]));
			}
		}
		
		if (btn->top_layer){
			DisplayImage(btn->top_layer);
		}
		DisplayActivateButton(btn->button_id);
	}
}

void startup_bluetooth_new(uint8_t step, bool show){
	
	switch (step){
		case 0:
		gfit_send_command(get_gfit_command(GFIT_SET_EQUIPMENT));
		if (show) DisplayImage(IMG_TILE3_RING_2_9);
		break;
		case 1:
		gfit_send_command(get_gfit_command(GFIT_SET_METRICS));
		if (show)DisplayImage(IMG_TILE3_RING_3_9);
		break;
		case 2:
		gfit_send_command(get_gfit_command(GFIT_SET_PRODUCT_INFO));
		if (show) DisplayImage(IMG_TILE3_RING_4_9);
		break;
		case 3:
		gfit_send_command(get_gfit_command(GFIT_SET_MANUFACTURER_INFO));
		if (show) DisplayImage(IMG_TILE3_RING_5_9);
		break;
		case 4:
		gfit_send_command(get_gfit_command(GFIT_SET_HARDWARE_INFO));
		if (show) DisplayImage(IMG_TILE3_RING_6_9);
		break;
		case 5:
		gfit_send_command(get_gfit_command(GFIT_SET_FIRMWARE_INFO));
		break;
		case 6:
		gfit_send_command(get_gfit_command(GFIT_GET_VERSION_STRING));
		if (show) DisplayImage(IMG_TILE3_RING_7_9);
		break;
		case 7:
		gfit_send_command(get_gfit_command(GFIT_SET_INUSE_ADV_TIMEOUT_0));
		case 8:
		gfit_send_command(get_gfit_command(GFIT_SET_HRP_PAIRING_MODE_PROXIMITY));
		if (show) DisplayImage(IMG_TILE3_RING_8_9);
		break;
		case 9:
		gfit_send_command(get_gfit_command(GFIT_SET_HRP_PAIRING_PROXIMITY));
		if (channel_mode==CHANNELMODE_BOTH){
				gfit_send_command(get_gfit_command(GFIT_SET_CHANNEL_CONFIGURATION_BOTH));
			} else if (channel_mode==CHANNELMODE_ANTONLY){
				gfit_send_command(get_gfit_command(GFIT_SET_CHANNEL_CONFIGURATION_ANT));
			} else if (channel_mode==CHANNELMODE_BLEONLY){
				gfit_send_command(get_gfit_command(GFIT_SET_CHANNEL_CONFIGURATION_FTMS));
			}		
		break;
		case 10:
		gfit_send_command(get_gfit_command(GFIT_TRANSITION_TO_READY));
		if (generic_Page.page_id==PAGE_ID_SETUP_SETTINGS){
			generic_Page.Buttons[2].top_layer = IMG_TILE3_BT_ICON_ON;
			generic_Page.Buttons[2].translated_text_id=STRING_BTON;
		}
		bluetoothEngaged = true;

		break;
	}
}

void ticktocker(uint8_t callback, uint16_t timeout){
	
	tick_tock_timer = 0;
	tick_tock_timeout = timeout;
	APP_STATE_CALLBACK = callback;
	App_State = APP_STATE_TICK;
}

void zeroValues(void){

	lastSecMoved = 0;
	
	distanceTicks = 0;
	Calories_Accumulated = 0;
	Heartrate_Accumulated = 0;
	Watts_Accumulated  = 0;
	Distance_Meters = 0;
	Power_Watts = 0;
	Resistance_Percent = 0;
	Speed_Meters_per_second  = 0.0;
	Heartrate_BPM = 0;
	max_Heartrate_BPM=0;
	SecTimer = 0;
	Cadence_RPM =0;
	SecsWithoutHR=0;
	tick = 0;
	tock = 0;
	tickctr = 0;
	//tickDiff = 0.0;
	#ifdef SPRINTBOK
	for (uint8_t i = 0; i<10;i++){
		avgInterval[i]=0;
	}
	currproj = 10;
	#endif
	fillValues();
	sprintf (Common_Strings[STRING_VALUE_TIME_LEFT],"%01d:%02d",Custom_Time/3600,(Custom_Time%3600)/60);
	
	sprintf (Common_Strings[STRING_VALUE_DISTANCE_LEFT],"%d.%dk",Custom_Distance/1000,(Custom_Distance%1000)/100);
	
	sprintf(Common_Strings[STRING_VALUE_TIME],"00:00");
}


//////////////////////// UNIQUE FUNCTION IMPLEMENTATION

#ifdef BIKE


void actResUpdate(void){
	#ifndef CICLOTTE	
	readADC();
	
	if (ADC_IsReadingComplete()){
		ResistanceNom = MIN(MAX(1.0-((double)(MAX(ADC_GetResult(),Resistance_Low_value)-Resistance_Low_value))/(Resistance_High_value-Resistance_Low_value),0.0),1.0);
	}
	#else
		ResistanceNom = (setAngleIndex+1)/10.0;
	#endif
	
	Resistance_Percent = (uint8_t)floor(ResistanceNom*100);
	if (ResistanceNom<0.325){
		a1 = 76.356*ResistanceNom*ResistanceNom-4.4448*ResistanceNom+4.4206;
		a2 = -1.4699*ResistanceNom-2.1172;
		a3 = -34.093*ResistanceNom*ResistanceNom*ResistanceNom+10.033*ResistanceNom*ResistanceNom-0.3734*ResistanceNom+0.3677;
	}
	else
	{
		a1 = 98.857*ResistanceNom-22.315;
		a2 = -34.924*ResistanceNom+9.2642;
		a3 = 4.0951*ResistanceNom-1.174;
	}
	
}




void actTickUpdate(bool standstill){
	
	UpdateTick = 0;
	IdleDoggy =0;
	const double tau = 2*3.14159265358979323846;
	
	//double cadence = ((double)tickDiff)*8*4.19424/0xffff;
	//double cadence = ((double)tickDiff)*7.81*4.19424/0xffff;
	cli();
	//double cadence = ((double)tock/(double)tickctr)*7.81*0.01279;
	  //double cadence = ((double)tock/(double)tickctr)*0.024972475;
	  double cadence = ((double)tock/(double)tickctr)*0.012496;
	tick = tock%tickctr;
	tickctr=0;
	sei();
	//cadence = (long double)1/cadence;
	// TODO FACTOR 3 FOR TESTING ONLY !!!!!
	
	
	avgInterval[9] = (double)1/cadence;
	
	double cadenceSum = avgInterval[9];
	
	for (uint8_t i = 0; i<9; i++){
		avgInterval[i]=avgInterval[i+1];
		cadenceSum+=avgInterval[i];
	}
	cadence = cadenceSum/10;
	if (standstill) cadence = 0;
	//long double cadence = ((long double)diff)*8*4.19424/0xffff;
	
	
	double M;
	
	M = a3*cadence*cadence*cadence+a2*cadence*cadence+a1*cadence;
	
	Power_Watts = tau*M*cadence;
	
	Cadence_RPM = (uint16_t)floor(cadence*60*7.81/8.0);
	
	if (BT_connected){
		uint64_t conv = (uint64_t)Distance_Meters;
		uint8_t cmd[] = {21,0xe2,0xe1,0x22,(Cadence_RPM*2)&0xff,(Cadence_RPM*2)>>8,0x33,Resistance_Percent*2,0x00,0x2a,((uint16_t)Power_Watts)&0xff,((uint16_t)Power_Watts)>>8,0x2b,((uint16_t)Power_Watts)&0xff,((uint16_t)Power_Watts)>>8,0x21,((uint16_t)(Speed_Meters_per_second*1000))&0xff,((uint16_t)(Speed_Meters_per_second*1000))>>8,0x60,conv&0xff,(conv>>8)&0xff,(conv>>16)&0xff};
		gfit_send_command(cmd);
	}
	
	
	
}



double SpeedFromWatts(double P){
	const int riderWeightKilograms = 70;
	const int bikeWeightKilograms = 9;
	const double rollingResistance = 0.005;
	const double frontalArea = 0.388;
	const int temperatureCelsius = 25;
	const int elevationMeters = 100;
	const double transVelocityConstant = 0.95;
	
	double airDensity = (1.293 - 0.00426 * temperatureCelsius) * exp(-elevationMeters / 7000.0);
	double totalWeightNewtons = 9.8 * (riderWeightKilograms + bikeWeightKilograms);

	double airResistance = 0.5 * frontalArea * airDensity;
	double totalResistance = totalWeightNewtons * rollingResistance;

	return newton(airResistance, 0.0, totalResistance, transVelocityConstant, P);
}

double newton(double aero, double hw, double tr, double tran, double p) {
	double vel = 20.0; // Initial guess
	const double TOL = 0.05; // tolerance
	for (int i = 1; i <= 10; i++) {
		double tv = vel + hw;
		double aeroEff = (tv > 0.0) ? aero : -aero; // wind in face, must reverse effect
		double f = vel * (aeroEff * tv * tv + tr) - tran * p; // the function
		double fp = aeroEff * (3.0 * vel + hw) * tv + tr; // the derivative
		double vNew = (fp == 0.0) ? 0.0 : vel - f / fp;
		if (fabs(vNew - vel) < TOL) return vNew; // success
		vel = vNew;
	}
	return 0.0; // failed to converge
}


void fillValues(void){
	
	
	snprintf (Common_Strings[STRING_VALUE_CALORIES],STRING_LENGTH_CALORIES,"%d",(uint16_t)floor(Calories_Accumulated));
	
	if (Distance_Meters>=10000){
		snprintf (Common_Strings[STRING_VALUE_DISTANCE],STRING_LENGTH_DISTANCE,"%d.%dk",(uint16_t)(Distance_Meters/1000),(((uint16_t)(Distance_Meters))%1000)/100);
		} else {
		snprintf (Common_Strings[STRING_VALUE_DISTANCE],STRING_LENGTH_DISTANCE,"%d",(uint16_t)Distance_Meters);
	}
	//snprintf (Common_Strings[STRING_VALUE_DISTANCE],STRING_LENGTH_DISTANCE,"%d",(uint16_t)(distanceTicks));
	
	//sprintf (Common_Strings[STRING_VALUE_DISTANCE],"%d",(uint16_t)Distance_Meters);
	snprintf(Common_Strings[STRING_VALUE_WATTS],STRING_LENGTH_WATTS,"%d",(uint16_t)floor(Power_Watts));
	#ifdef CICLOTTE
	snprintf(Common_Strings[STRING_VALUE_RESISTANCE],STRING_LENGTH_RESISTANCE,"%d",setAngleIndex+1);
	
	#else
	snprintf(Common_Strings[STRING_VALUE_RESISTANCE],STRING_LENGTH_RESISTANCE,"%d",Resistance_Percent);
	#endif
	snprintf(Common_Strings[STRING_VALUE_CADENCE],STRING_LENGTH_CADENCE,"%d",Cadence_RPM);
	Speed_Meters_per_second = SpeedFromWatts(Power_Watts);
	#ifdef DEBUGGING 
	Speed_Meters_per_second = 10;
	#endif
	
	
	snprintf(Common_Strings[STRING_VALUE_SPEED],STRING_LENGTH_SPEED,"%d",(uint16_t)floor(Speed_Meters_per_second*3.6));
	
	
	snprintf(Common_Strings[STRING_VALUE_HEARTRATE],STRING_LENGTH_HEARTRATE,"%d",Heartrate_BPM);
	snprintf (Common_Strings[STRING_VALUE_TIME],STRING_LENGTH_TIME,"%02d:%02d",SecTimer/60,SecTimer%60);
	
	if (Speed_Meters_per_second>5){
		uint8_t mpkm,spkm;
		
		mpkm = (uint16_t)floor(1000.0/(Speed_Meters_per_second))/60;
		spkm = (uint16_t)floor(1000.0/(Speed_Meters_per_second))%60;
		
		snprintf(Common_Strings[STRING_VALUE_PACE],STRING_LENGTH_PACE,"%02d:%02d",mpkm,spkm);
		} else {
		sprintf(Common_Strings[STRING_VALUE_PACE],"--:--");
	}

}
#ifndef CICLOTTE

	void readADC() {
		uint16_t tim = TCNT2;
		ADC_StartReading(ADC_REFERENCE_AVCC|ADC_RIGHT_ADJUSTED|ADC_CHANNEL4);
		while (!ADC_IsReadingComplete() && TCNT2<tim+200){}
	}
#endif

#else

void fillValues(void){
	
	snprintf (Common_Strings[STRING_VALUE_CALORIES],STRING_LENGTH_CALORIES,"%d",(uint16_t)floor(Calories_Accumulated));
	
	if (Distance_Meters>10000){
		snprintf (Common_Strings[STRING_VALUE_DISTANCE],STRING_LENGTH_DISTANCE,"%d.%dk",(uint16_t)(Distance_Meters/1000),(((uint16_t)(Distance_Meters))%1000)/100);
		} else {
		snprintf (Common_Strings[STRING_VALUE_DISTANCE],STRING_LENGTH_DISTANCE,"%d",(uint16_t)Distance_Meters);

	}
	
	snprintf (Common_Strings[STRING_VALUE_TIME],STRING_LENGTH_TIME,"%02d:%02d",SecTimer/60,SecTimer%60);

	#ifdef DEBUGGING
	Speed_Meters_per_second = 10;
	#endif
	
	
	snprintf(Common_Strings[STRING_VALUE_SPEED],STRING_LENGTH_SPEED,"%d",(uint16_t)floor(Speed_Meters_per_second*3.6));
	
	snprintf(Common_Strings[STRING_VALUE_HEARTRATE],STRING_LENGTH_HEARTRATE,"%d",Heartrate_BPM);
	
	
	
	
	
	
	double avgspeed = 0;
	
	if (SecTimer>0){
		avgspeed = Distance_Meters/SecTimer;
		
		
		snprintf(Common_Strings[STRING_VALUE_AVG_SPEED],STRING_LENGTH_AVG_SPEED,"%d",(uint16_t)floor(avgspeed*3.6));
		
		
		} else {
		sprintf(Common_Strings[STRING_VALUE_AVG_SPEED],"---");
	}
	uint8_t mpkm =0;
	uint8_t spkm =0;

	if (avgspeed>0){
		
		mpkm = (uint16_t)floor(1000.0/(avgspeed))/60;
		spkm = (uint16_t)floor(1000.0/(avgspeed))%60;
		
		if (mpkm<30){
			snprintf(Common_Strings[STRING_VALUE_AVG_PACE],STRING_LENGTH_AVG_PACE,"%02d:%02d",mpkm,spkm);
			} else {
			sprintf(Common_Strings[STRING_VALUE_AVG_PACE],"--:--");
		}
		
		} else {
		sprintf(Common_Strings[STRING_VALUE_AVG_PACE],"--:--");
		
	}
	
	if (Speed_Meters_per_second>1.1){
		
		mpkm = (uint16_t)floor(1000.0/(Speed_Meters_per_second))/60;
		spkm = (uint16_t)floor(1000.0/(Speed_Meters_per_second))%60;
		
		snprintf(Common_Strings[STRING_VALUE_PACE],STRING_LENGTH_PACE,"%02d:%02d",mpkm,spkm);
		} else {
		sprintf(Common_Strings[STRING_VALUE_PACE],"--:--");
	}
	
	if (Distance_Meters<projections[5] && Speed_Meters_per_second>1.1){
		
		for (uint8_t i = 0;i<5;i++)	{
			if (Distance_Meters<projections[i]){
				projections_index = i;
				break;
				
				} else {
				projections_index = 5;
			}
		}
		
		
	
	
		switch (projections_index){
			case 5:
			sprintf(Common_Strings[STRING_XXK_PROJECTION],"40.2k Proj.");
			break;
			case 4:
			sprintf(Common_Strings[STRING_XXK_PROJECTION],"20.1k Proj.");
			break;
			case 3:
			sprintf(Common_Strings[STRING_XXK_PROJECTION],"15k Proj.");
			break;
			case 2:
			sprintf(Common_Strings[STRING_XXK_PROJECTION],"10k Proj.");
			break;
			case 1:
			sprintf(Common_Strings[STRING_XXK_PROJECTION],"5k Proj.");
			break;
			case 0:
			sprintf(Common_Strings[STRING_XXK_PROJECTION],"1k Proj.");
			break;
		}
		
		
		
		if (projections_index!=currproj)	{
			currproj=projections_index;
			
			
			
			
			if (ring_Tile1SessionRing.idx==7)update_Ring(&ring_Tile1SessionRing);
			if (ring_Tile2SessionRing.idx==7)update_Ring(&ring_Tile2SessionRing);
			if (ring_Tile3SessionRing.idx==7)update_Ring(&ring_Tile3SessionRing);
			
		}
		
		projected_difference = projections[projections_index]-(uint16_t)Distance_Meters;
		
		projected_time = SecTimer+(uint16_t)(((double)projected_difference)/Speed_Meters_per_second);
		
		snprintf(Common_Strings[STRING_VALUE_PROJECTION],STRING_LENGTH_PROJECTION,"%02d:%02d",projected_time/60,projected_time%60);

		} else {
		sprintf(Common_Strings[STRING_VALUE_PROJECTION],"---");
	
	}
	
	
}


void actTickUpdate(bool standstill){
	IdleDoggy =0;
	
	cli();
	
	if (tickctr==0){
			
			Interval = 1000.0;
		} else {
			Interval = Interval;
		}
		
//	sprintf(String_buffer,"%d %d %u",tock,tick,(uint16_t)(Interval*1000.0));
//	DisplayText(SF_INFO_LINE0_LIGHT,String_buffer,strlen(String_buffer));	
	sei();

	
	if (!standstill && Interval>0.0){
		
/*		avgInterval[9] = Interval ;
		
		double cadenceSum = 0;
		
		for (uint8_t i = 0; i<9; i++){
			avgInterval[i]=avgInterval[i+1];
			cadenceSum+=avgInterval[i];
		}
		Interval = (cadenceSum+Interval)/10;
*/
   
		Distance_Meters = (200*3.66/1396)*distanceTicks*inclineCorrFactor;
		//Speed_Meters_per_second = (0.4161849710/Interval);
		Speed_Meters_per_second = (260.115606875/Interval)*inclineCorrFactor;
		
	} else {
		Speed_Meters_per_second = 0;
	}
	if (standstill){
		Speed_Meters_per_second = 0;
	}
	
	if (BT_connected){
		uint64_t conv = (uint64_t)Distance_Meters;
		uint8_t cmd[] = {9,0xe2, 0xe1, 0x60,conv&0xff,(conv>>8)&0xff,(conv>>16)&0xff, 0x21, ((uint16_t)(1000*Speed_Meters_per_second))%0xff, ((uint16_t)(1000*Speed_Meters_per_second))/0xff};
		gfit_send_command(cmd);
	}

	
	UpdateTick = 0;
}

#endif

