//values
#define TRUE 1
#define FALSE 0
#define TIME_MSG_LEN 11
#define TIME_HEADER 'T'

constexpr unsigned long printInterval{ 1000 };
unsigned long printNow{};

int ScalesReady = FALSE;
int ButtonReady = FALSE;
int UserPromptNeeded = TRUE;
String UserQuestion1 = "Is your pet a dog or cat?";
String UserQuestion2 = "How much (in pounds) does your pet weigh? Please round to the nearest whole number.";

//sensor (analog???) pins
const int FoodPin = 9; //analog GPIO pin 9
const int WaterPin = 10; //analog GPIO pin 10

//motor pins
const int directionPin = 12; //Digital GPIO D6
const int pwmPin = 3; //AREF for PWM
const int brakePin = 9; //Digital GPIO D9

//other pins
const int buttonPin = 2;

//function defines
void ScaleValue(void);
void food(void);
void RunButton(void);
void WaterValue(void);
void PictureProcess(void);
void RTCset(void);
