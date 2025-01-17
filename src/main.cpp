#include <NMEA2000_CAN.h>
#include <N2KMessages.h>

#include <LiquidCrystal_I2C.h>
#include <direction_indicator.h>
#include "persistent_data.h"
#include <ctime>

LiquidCrystal_I2C lcd(0x20, 20, 4, &Wire1);  // set the LCD address to 0x27 for a 16 chars and 2 line display
DirectionIndicator indicator;

PersistentData persistent_data;

bool switch_direction()
{
  switch (indicator.get_direction())
  {
  case Up:
    indicator.set_direction(Down);
    break;
  
  case Down:
  default:
    indicator.set_direction(Up);
    break;
  }

  return true;
}


bool pin10state = LOW;

bool tick_seconds()
{
  auto secs = persistent_data.get_total_seconds_elapsed();
  char msg[20];
  sprintf(msg, "secs elapsed = %lu", secs);
  lcd.setCursor(0, 1);
  lcd.print(msg);
  persistent_data.set_total_seconds_elapsed(secs + 1);
  
  pin10state = !pin10state;
  digitalWrite(10, pin10state);

  return true;
}

// List here messages your device will transmit.
const unsigned long TransmitMessages[] PROGMEM={128776L,128777L,128778L,0};

// Define schedulers for messages. Define schedulers here disabled. Schedulers will be enabled
// on OnN2kOpen so they will be synchronized with system.
// We use own scheduler for each message so that each can have different offset and period.
// Setup periods according PGN definition (see comments on IsDefaultSingleFrameMessage and
// IsDefaultFastPacketMessage) and message first start offsets. Use a bit different offset for
// each message so they will not be sent at same time.
tN2kSyncScheduler WindlassScheduler(false,10000,500);
tN2kSyncScheduler ScrollScheduler(false,20000,0);
tN2kSyncScheduler SecondsScheduler(false,1000,0);

void OnN2kOpen() {
  // Start schedulers now.
  WindlassScheduler.UpdateNextTime();
  ScrollScheduler.UpdateNextTime();
  SecondsScheduler.UpdateNextTime();
}

void HandleNMEA2000Msg(const tN2kMsg &N2kMsg) {
  if (N2kMsg.PGN == 126992L)
  {
    unsigned char SID;
    uint16_t SystemDate;
    double SystemTime;
    tN2kTimeSource TimeSource;
    
    if (ParseN2kSystemTime(N2kMsg,SID,SystemDate,SystemTime,TimeSource) ) {
      int systemTimeInt = (int)SystemTime;
      int hours = systemTimeInt / 3600;
      int mins = (systemTimeInt - (hours * 3600)) / 60;
      int secs = systemTimeInt - (hours * 3600) - (mins * 60);

      char timeStr[18];
      sprintf(timeStr, "%02d:%02d:%02d", hours, mins, secs);
      lcd.home();
      lcd.print(timeStr);
    }
  }
}  

void handle_buttons() {
  if (digitalRead(14) == LOW) {
    lcd.setCursor(2,2);
    lcd.print("D");
  } else {
    lcd.setCursor(2,2);
    lcd.print(" ");
  }
  if (digitalRead(15) == LOW)  {
   lcd.setCursor(0,2);
    lcd.print("U");
  } else {
   lcd.setCursor(0,2);
    lcd.print(" ");
  }
  if (digitalRead(16) == LOW)  {
    lcd.setCursor(6,2);
    lcd.print("R");
  } else {
    lcd.setCursor(6,2);
    lcd.print(" ");
  }
  if (digitalRead(17) == LOW)  {
    lcd.setCursor(4,2);
    lcd.print("L");
  } else {
    lcd.setCursor(4,2);
    lcd.print(" ");
  }

  if (digitalRead(2) == HIGH) {
    lcd.setCursor(0,3);
    lcd.print("2");
  } else {
    lcd.setCursor(0,3);
    lcd.print(" ");
  }
  if (digitalRead(3) == HIGH) {
    lcd.setCursor(2,3);
    lcd.print("3");
  } else {
    lcd.setCursor(2,3);
    lcd.print(" ");
  }
  if (digitalRead(4) == HIGH) {
    lcd.setCursor(4,3);
    lcd.print("4");
  } else {
    lcd.setCursor(4,3);
    lcd.print(" ");
  }
  if (digitalRead(5) == HIGH) {
    lcd.setCursor(6,3);
    lcd.print("5");
  } else {
    lcd.setCursor(6,3);
    lcd.print(" ");
  }

}

void setup() {
  lcd.init();                      // initialize the lcd 
  indicator.begin(&lcd);
  persistent_data.begin(0x50, &Wire1);
  
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);
  
  lcd.backlight();
  
  switch_direction();

  // Set Product information
  NMEA2000.SetProductInformation("00000001", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                 "Chain Counter",  // Manufacturer's Model ID
                                 "1.0.0.0 (2024-12-19)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2024-12-19)" // Manufacturer's Model version
                                 );
  // Set device information
  NMEA2000.SetDeviceInformation(112233, // Unique number. Use e.g. Serial number.
                                130, // Device function=Diagnostic. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                10, // Device class=System Tools. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2040 // Just choosen free from code list on https://web.archive.org/web/20190529161431/http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                               );
  // Uncomment 2 rows below to see, what device will send to bus. Use e.g. OpenSkipper or Actisense NMEA Reader                           
  Serial.begin(115200);
  NMEA2000.SetForwardStream(&Serial);
  // If you want to use simple ascii monitor like Arduino Serial Monitor, uncomment next line
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode,22);
  //NMEA2000.SetDebugMode(tNMEA2000::dm_Actisense); // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
  NMEA2000.EnableForward(false); // Disable all msg forwarding to USB (=Serial)
  // Here we tell library, which PGNs we transmit
  NMEA2000.ExtendTransmitMessages(TransmitMessages);
  // Define OnOpen call back. This will be called, when CAN is open and system starts address claiming.
  NMEA2000.SetOnOpen(OnN2kOpen);

  NMEA2000.SetMsgHandler(HandleNMEA2000Msg);

  NMEA2000.Open();

  Serial.write("Starting...\r\n");
}

void SendN2kWindlass();

void loop() {
  indicator.tick();
  if (ScrollScheduler.IsTime()) {
    ScrollScheduler.UpdateNextTime();
    switch_direction();
  }
  if (SecondsScheduler.IsTime()) {
    SecondsScheduler.UpdateNextTime();
    tick_seconds();
  }
  handle_buttons();
  SendN2kWindlass();
  NMEA2000.ParseMessages();
}

uint8_t next_SID(uint8_t old_SID)
{
  return (old_SID >= 252) ? 0 : old_SID + 1;
}

uint8_t windlass_SID = 0xFF;

void SendN2kWindlass()
{
  tN2kMsg N2kMsg;

  if (WindlassScheduler.IsTime()) {
    WindlassScheduler.UpdateNextTime();

    windlass_SID = next_SID(windlass_SID);

    SetN2kPGN128776(
      N2kMsg,
      windlass_SID,           // SID        
      0,                      // WindlassIdentifier
      N2kDD484_Off,           // WindlassDirectionControl
      0,                      // SpeedControl
      N2kDD488_SingleSpeed    // SpeedControlType
    );
    NMEA2000.SendMsg(N2kMsg);

    SetN2kPGN128777(
      N2kMsg,
      windlass_SID,           // SID        
      0,                      // WindlassIdentifier
      0.0                     // RodeCounterValue
    );
    NMEA2000.SendMsg(N2kMsg);

    SetN2kPGN128778(
      N2kMsg,
      windlass_SID,           // SID        
      0,                      // WindlassIdentifier
      0.0                     // TotalMotorTime
    );
    NMEA2000.SendMsg(N2kMsg);
  }
}