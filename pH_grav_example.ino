// to use the Atlas gravity circuits with 
// the gravity isolator board's pulse output 
// uncomment line 8: #define USE_PULSE_OUT
// you can use any pins instead of just the analog ones
// but it must be recalibrated
// note that the isolator's analog output also provides isolation

// #define USE_PULSE_OUT

// Which pin we are connected to.
#define PH_ANALOG_PIN A0


#ifdef USE_PULSE_OUT
  #include "ph_iso_grav.h"       
  Gravity_pH_Isolated pH = Gravity_pH_Isolated(PH_ANALOG_PIN);         
#else
  #include "ph_grav.h"             
  Gravity_pH pH = Gravity_pH(PH_ANALOG_PIN);   
#endif


/*
---------------------------------------------------------------------
Message handling
---------------------------------------------------------------------
*/
#define NEW_LINE 10
#define CARRIAGE_RETURN 13
#define EOL CARRIAGE_RETURN




uint8_t user_bytes_received = 0;                
const uint8_t bufferlen = 32;                   
char user_data[bufferlen];

uint8_t read_data=0;                     
uint8_t meter_status=0;

void parse_cmd(char* string) {                   
  strupr(string);                                
  if (strcmp(string, "PING") == 0) {
    Serial.println("PONG");
  }
  else if (strcmp(string, "CAL,7") == 0) {       
    pH.cal_mid();                                
    Serial.println("c:MID");
  }
  else if (strcmp(string, "CAL,4") == 0) {            
    pH.cal_low();                                
    Serial.println("c:LOW");
  }
  else if (strcmp(string, "CAL,10") == 0) {      
    pH.cal_high();                               
    Serial.println("c:HIGH");
  }
  else if (strcmp(string, "CAL,CLEAR") == 0) { 
    pH.cal_clear();                              
    Serial.println("c:CLEAR");
  }else if (strcmp(string, "READ_DATA") == 0) {
    read_data=1;
  }else if (strcmp(string, "STOP_DATA") ==0){
    read_data=0;
  }else if(strcmp(string, "STATUS") == 0){
    Serial.print("s:");Serial.print(meter_status);
    switch(meter_status){
      case 0: Serial.println(":Failed to load EEPROM!"); break;
      case 1: Serial.println(":EEPROM Loaded Successfully"); break;
      default: Serial.println(":Unkown Status Code");
    }
  }else if(strcmp(string,"GET_PH") == 0){
    Serial.println(pH.read_ph());
  }
}

void setup() {
  Serial.begin(9600);                            
  delay(200);
  Serial.println(F("Use commands \"CAL,7\", \"CAL,4\", and \"CAL,10\" to calibrate the circuit to those respective values"));
  Serial.println(F("Use command \"CAL,CLEAR\" to clear the calibration"));
  if (pH.begin()) {                                     
    Serial.println("Loaded EEPROM");
    meter_status=1;
  }else{
    Serial.println("Error loading EEPROM");
  }
}

void loop() {
  if (Serial.available() > 0) {                                                      
    user_bytes_received = Serial.readBytesUntil(EOL, user_data, sizeof(user_data));   
  }

  if (user_bytes_received) {                                                      
    parse_cmd(user_data);                                                          
    user_bytes_received = 0;                                                        
    memset(user_data, 0, sizeof(user_data));                                         
  }
  if(read_data == 1){
    Serial.println(pH.read_ph());  
  }
  delay(1000);
}
