#include <NMEA2000_CAN.h>
#include <N2KMessages.h>
#include <LiquidCrystal_I2C.h>
#include "menu.h"

LiquidCrystal_I2C lcd(0x20, 20, 4, &Wire1);  // set the LCD address to 0x27 for a 16 chars and 2 line display

auto time_menu_item = new TimeMenuItem();
void set_time(int hrs, int mins, int secs) { time_menu_item->set_time(hrs, mins, secs); }

Menu menu({
  time_menu_item,
  new NumericMenuItem("Gypsy circ", "cm", 32),
  new NumericMenuItem("Rode length", "m", 60),
  new TextMenuItem("Item 4")
}, &lcd);

class TimeMsgHandler : public tNMEA2000::tMsgHandler {
public:
  TimeMsgHandler(void (*set_time_fn)(int, int, int)) :
    tNMEA2000::tMsgHandler(126992L) {
      set_time = set_time_fn;
    }

  virtual void HandleMsg(const tN2kMsg &N2kMsg) {
    unsigned char SID;
    uint16_t SystemDate;
    double SystemTime;
    tN2kTimeSource TimeSource;

    if (ParseN2kSystemTime(N2kMsg,SID,SystemDate,SystemTime,TimeSource)) {
      int systemTimeInt = (int)SystemTime;
      int hours = systemTimeInt / 3600;
      int mins = (systemTimeInt - (hours * 3600)) / 60;
      int secs = systemTimeInt - (hours * 3600) - (mins * 60);
      set_time(hours, mins, secs);
    }
  }

private:
  void (*set_time)(int, int, int);
};

void N2K_setup()
{
  const unsigned long TransmitMessages[] PROGMEM={128776L,128777L,128778L,0};

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
  NMEA2000.SetForwardStream(&Serial);
  // If you want to use simple ascii monitor like Arduino Serial Monitor, uncomment next line
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  //NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode,22);
  //NMEA2000.SetDebugMode(tNMEA2000::dm_Actisense); // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
  NMEA2000.EnableForward(false); // Disable all msg forwarding to USB (=Serial)
  // Here we tell library, which PGNs we transmit
  NMEA2000.ExtendTransmitMessages(TransmitMessages);
  // Define OnOpen call back. This will be called, when CAN is open and system starts address claiming.
  //NMEA2000.SetOnOpen(OnN2kOpen);

  NMEA2000.AttachMsgHandler(new TimeMsgHandler(set_time));

  NMEA2000.Open(); 
}

void setup() {
  lcd.init();                      // initialize the lcd 
  lcd.backlight();

  N2K_setup();

  Serial.begin(115200);
  Serial.write("Starting...\r\n");
  lcd.setCursor(0,0);
}


void calculate_loop_time() {
  static uint32_t last_loop_millis = 0;
  auto loop_millis = millis();
  auto loop_time = loop_millis - last_loop_millis;
  last_loop_millis = loop_millis;
  if (loop_time > 1) {
    char loop_time_str[20];
    sprintf(loop_time_str, "%08lu\r\n", (unsigned long)loop_time);
    Serial.write(loop_time_str);
  }
}

void loop() {
  menu.display();
  NMEA2000.ParseMessages();
  calculate_loop_time();
}
