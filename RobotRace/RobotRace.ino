//Libraries
#include <Pololu3pi.h>
#include <PololuQTRSensors.h>
#include <OrangutanMotors.h>
#include <OrangutanAnalog.h>
#include <OrangutanLEDs.h>
#include <OrangutanLCD.h>
#include <OrangutanPushbuttons.h>
#include <OrangutanBuzzer.h>

Pololu3pi robot;
unsigned int sensors[5]; // an array to hold sensor values
unsigned int last_proportional = 0;
long integral = 0;

//DO NOT CHANGE
//Data storage file
#include <avr/pgmspace.h>

// Introductory messages.  The "PROGMEM" identifier causes the data to
// go into program space.

// Yes. We did name the robot Squirt.
const char welcome_line1[] PROGMEM = " Hello,";
const char demo_name_line1[] PROGMEM = "Killer";
const char demo_name_line2[] PROGMEM = "Queen";

// A couple of simple tunes, stored in program space.
const char welcome[] PROGMEM = ">g32>>c32";
const char go[] PROGMEM = "L16 cdegreg4";

// Data for generating the characters used in load_custom_characters
// and display_readings.  By reading levels[] starting at various
// offsets, we can generate all of the 7 extra characters needed for a
// bargraph.  This is also stored in program space.

// Loading custom characters and display readings constants
// Generates the bar graph that stores the light readings.
const char levels[] PROGMEM = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

//###################################################################
// This function loads custom characters into the LCD.  Up to 8
// characters can be loaded; we use them for 7 levels of a bar graph.
//###################################################################
void load_custom_characters()
{
  OrangutanLCD::loadCustomCharacter(levels + 0, 0); // no offset, e.g. one bar
  OrangutanLCD::loadCustomCharacter(levels + 1, 1); // two bars
  OrangutanLCD::loadCustomCharacter(levels + 2, 2); // etc...
  OrangutanLCD::loadCustomCharacter(levels + 3, 3);
  OrangutanLCD::loadCustomCharacter(levels + 4, 4);
  OrangutanLCD::loadCustomCharacter(levels + 5, 10);
  OrangutanLCD::loadCustomCharacter(levels + 6, 6);
  OrangutanLCD::clear(); // the LCD must be cleared for the characters to take effect
}

//###################################################################
//                    Plays The Jurassic Park Theme
//###################################################################
void playJurasicParkTheme(){
  OrangutanBuzzer::playFrequency(523, 500, 10);   //C5
  delay(250);
  OrangutanBuzzer::playFrequency(247, 250, 10);   //B4
  delay(250);
  OrangutanBuzzer::playFrequency(523, 500, 10);   //C5
  delay(500);
  OrangutanBuzzer::playFrequency(392, 500, 10);   //G4
  delay(500);
  OrangutanBuzzer::playFrequency(349, 500, 10);   //F4
  delay(500);

  OrangutanBuzzer::playFrequency(523, 500, 10);   //C5
  delay(250);
  OrangutanBuzzer::playFrequency(247, 250, 10);   //B4
  delay(250);
  OrangutanBuzzer::playFrequency(523, 500, 10);   //C5
  delay(500);
  OrangutanBuzzer::playFrequency(392, 500, 10);   //G4
  delay(500);
  OrangutanBuzzer::playFrequency(349, 500, 10);   //F4
  delay(500);

  OrangutanBuzzer::playFrequency(523, 500, 10);   //C5
  delay(500);
  OrangutanBuzzer::playFrequency(247, 250, 10);   //B4
  delay(250);
  OrangutanBuzzer::playFrequency(247, 250, 10);   //B4
  delay(250);
  OrangutanBuzzer::playFrequency(523, 500, 10);   //C5
  delay(500);
  OrangutanBuzzer::playFrequency(523, 1000, 10);   //C5
  delay(500);
  }

//###################################################################
//                    Plays Another One "Bytes" The Dust
//###################################################################
void playAnotherOneBytesTheDust(){
  OrangutanBuzzer::play("!T110 V10 L16 RRag L8 eReReR L16 MS RRRe ML eReR g. MS Re ML aRRRR RRag L8 eReReR L16 MS RRRe ML eReR g. MS Re ML aRRRR");
}

//###################################################################
//                    Plays We are the Champions
//###################################################################
void playWeAreTheChampions(){
}



//###################################################################
//                    Calibration
//###################################################################
void calibration(){

  unsigned int counter;

  // Auto-calibration: turn right and left while calibrating the
  // sensors.
  for (counter=0; counter<80; counter++)
  {
    if (counter < 20 || counter >= 60)
      OrangutanMotors::setSpeeds(40, -40);
    else
      OrangutanMotors::setSpeeds(-40, 40);

    robot.calibrateLineSensors(IR_EMITTERS_ON);

    // Since our counter runs to 80, the total delay will be
    // 80*20 = 1600 ms.
    delay(20);
  }
  OrangutanMotors::setSpeeds(0, 0);

}
//###################################################################
//              Displays the sensor readings
//###################################################################
void display_readings(const unsigned int *calibrated_values)
{
  unsigned char i;

  for (i=0;i<5;i++) {
    // Initialize the array of characters that we will use for the
    // graph.  Using the space, an extra copy of the one-bar
    // character, and character 255 (a full black box), we get 10
    // characters in the array.
    const char display_characters[10] = { ' ', 0, 0, 1, 2, 3, 4, 5, 6, 255 };

    // The variable c will have values from 0 to 9, since
    // calibrated values are in the range of 0 to 500, and
    // 1000/101 is 9 with integer math.
    char c = display_characters[calibrated_values[i] / 101];

    // Display the bar graph character.
    OrangutanLCD::print(c);
  }
}

//###################################################################
//    Initializes the 3pi, displays a welcome message, and calibrates
//###################################################################
void setup()
{
  //THIS MUST BE CALLED FIRST - the 2000 is a timeout
  robot.init(2000);

  load_custom_characters(); // load the custom characters

  //Prints Welcome Messages When Turned On
  OrangutanLCD::printFromProgramSpace(welcome_line1);
  OrangutanLCD::gotoXY(0, 1);
  delay(1000);

  OrangutanLCD::clear();
  OrangutanLCD::printFromProgramSpace(demo_name_line1);
  OrangutanLCD::gotoXY(0, 1);
  OrangutanLCD::printFromProgramSpace(demo_name_line2);
  delay(1000);

  /*
   *  Display battery voltage and wait for button press.
   *  The button press shows the battery amount then begins
   *  the calibration process.
  */
  while (!OrangutanPushbuttons::isPressed(BUTTON_B))
  {
    int bat = OrangutanAnalog::readBatteryMillivolts();

    OrangutanLCD::clear();
    OrangutanLCD::print(bat);
    OrangutanLCD::print("mV");
    OrangutanLCD::gotoXY(0, 1);
    OrangutanLCD::print("Press B");

    delay(100);
  }

  //The delay gives you time to move your hand away.
  OrangutanPushbuttons::waitForRelease(BUTTON_B);
  delay(1000);

  calibration();
 
  // Display calibrated values as a bar graph.
  while (!OrangutanPushbuttons::isPressed(BUTTON_B))
  {
    // Read the sensor values and get the position measurement.
    unsigned int position = robot.readLine(sensors, IR_EMITTERS_ON);

    /* Display the position measurement, which will go from 0
    * (when the leftmost sensor is over the line) to 4000 (when
    * the rightmost sensor is over the line)
    */
    OrangutanLCD::clear();
    OrangutanLCD::print(position);
    OrangutanLCD::gotoXY(0, 1);
    display_readings(sensors);

    delay(100);
  }
  OrangutanPushbuttons::waitForRelease(BUTTON_B);

  OrangutanLCD::clear();

  OrangutanLCD::print("Go!");

  // Play music and start driving.
//  playAnotherOneBytesTheDust();

  delay(1000);
}

  void leftTurn(){
      unsigned int position = robot.readLine(sensors, IR_EMITTERS_ON);
      while (position<1800){
        position = robot.readLine(sensors, IR_EMITTERS_ON);
        OrangutanMotors::setSpeeds(-50, 120);
      }
  }

  void rightTurn(){
      unsigned int position = robot.readLine(sensors, IR_EMITTERS_ON);
      while (position>2200){
        position = robot.readLine(sensors, IR_EMITTERS_ON);
        OrangutanMotors::setSpeeds(120, -50);
      }
  }

// The main function.  This function is repeatedly called by
// the Arduino framework.
void loop()
{
  // Get the position of the line.  Note that we *must* provide
  // the "sensors" argument to read_line() here, even though we
  // are not interested in the individual sensor readings.
  unsigned int position = robot.readLine(sensors, IR_EMITTERS_ON);

  // The "proportional" term should be 0 when we are on the line.
  int proportional = (int)position - 2000;

  // Compute the derivative (change) and integral (sum) of the
  // position.
  int derivative = proportional - last_proportional;
  integral += proportional;

  // Remember the last position.
  last_proportional = proportional;

  // Compute the difference between the two motor power settings,
  // m1 - m2.  If this is a positive number the robot will turn
  // to the right.  If it is a negative number, the robot will
  // turn to the left, and the magnitude of the number determines
  // the sharpness of the turn.  You can adjust the constants by which
  // the proportional, integral, and derivative terms are multiplied to
  // improve performance.
  int power_difference = proportional/40 + integral/10000 + derivative*6/2;

  // Compute the actual motor settings.  We never set either motor to a negative value.
  // This is so the car can move on a curved line
  // It will keep the car moving forward.
  const int maximum = 80;
  const int multiplier = 1;
  if (power_difference > maximum)
    power_difference = maximum;
  if (power_difference < -maximum)
    power_difference = -maximum;

  // This logic forces the hard 90 degree turns.
  // If the track is what we see on confluence
  // we may want to change this.
  if (position <= 1000){
    //left turn
    leftTurn();
  }
  else if (position >= 1000 && position <=3000){
    //straight line
    if (power_difference < 0)
//    OrangutanMotors::setSpeeds(0,0);
    OrangutanMotors::setSpeeds((maximum + power_difference)*multiplier, (maximum)*multiplier);
  else
//      OrangutanMotors::setSpeeds(0,0);
    OrangutanMotors::setSpeeds(maximum*multiplier, (maximum - power_difference)*multiplier);
  }
  else{
//    //right turn
    rightTurn();
  }

  OrangutanLCD::clear();
  OrangutanLCD::print(position);
  OrangutanLCD::gotoXY(0, 1);
}
