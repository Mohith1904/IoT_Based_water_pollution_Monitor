// Blynk Credentials
#define BLYNK_TEMPLATE_ID "TMPL3A_jHyROq"
#define BLYNK_TEMPLATE_NAME "Pollution Monitor"
#define BLYNK_AUTH_TOKEN "rXK01MSzBH8hHda2m_2q-PaDbwjQPilp"

//You can comment the below line if you don't want to print anything and save some space
//However for now understanding, i have kept it and using the print
//Don't forget that you SHOULDN'T use semicolon after the below statement
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Servo.h>
BlynkTimer timer;


Servo ESC;
//variables for motor control
int ESC_pin_number; //i still did not assign the pin number i have to do it later while connecting to the pin 
int ESC_power_value;


Servo rudderServo;
int current_angle;
int desired_angle;
int angle_increment = 1;
int delay_time = 10;
int rudder_pin_value=33;// don't forget to set it while connecting the rudder
void move_rudder(){
   if (current_angle < desired_angle) {
    for (int angle = current_angle; angle <= desired_angle; angle += angle_increment) {
      rudderServo.write(angle);
      delay(delay_time);
    }
  } else if (current_angle > desired_angle) {
    for (int angle = current_angle; angle >= desired_angle; angle -= angle_increment) {
      rudderServo.write(angle);
      delay(delay_time);
    }
  }
  current_angle=desired_angle;
}
//receiving the data from the blynk platform for servo angle
BLYNK_WRITE(V1)
{
  desired_angle = param.asInt(); // Update the global variable with the new value
}
void show_rudder_angle(){
  Serial.print("Rudder angle is: ");
  Serial.println(desired_angle);
}


int turbidity_sensor_pin=34;
float turbidity;
void read_turbidity(){
  turbidity = ((float)analogRead(turbidity_sensor_pin)/2792.00);
}
void upload_turbidity(){
  Blynk.virtualWrite(V3, turbidity);
}
void show_turbidity(){
  Serial.print("turbidity is: " );
  Serial.println(turbidity);
  //Serial.println(analogRead(turbidity_sensor_pin));
  
}



#include <OneWire.h>
#include <DallasTemperature.h>
//variables for temperature sensor
int temp_sensor_pin_value =16; // you have to set the value of this pin later while connecting temperature sensor
float tempinC;
OneWire oneWire(temp_sensor_pin_value);
DallasTemperature temp_sensor(&oneWire);
void get_temperature(){
  temp_sensor.requestTemperatures();
  tempinC = temp_sensor.getTempCByIndex(0);
}
void show_temperature(){
  Serial.print("Temperature = ");
  Serial.print(tempinC);
  Serial.println("ºC");
}
void upload_temperature(){
  Blynk.virtualWrite(V2, tempinC);
}



// My WiFi credentials.
char ssid[] = "realmeGT2";
char pass[] = "dummy420";

//BLDC motor code
int value = 0; // Global variable to store the value of V0(Blynk input)
// This function is called whenever the value of virtual pin V0 changes
BLYNK_WRITE(V0)
{
  value = param.asInt(); // Update the global variable with the new value
}
// This function is called every second to print the value of virtual pin V0
void printVirtualPinValue()
{
  Serial.print("The value of V0 is: ");
  Serial.println(value);
}
void run_motor(){
  int ESC_power_value = map(value, 0, 1023, 0, 180); //check the range of the esc whether it is from 0-180 or something else
  ESC.write(ESC_power_value);
}


void setup()
{
  // Debug console
  Serial.begin(115200);

  //connecting to wifi
  Serial.println("connecting...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("connected ");

  //Connecting ESC to NodeMCU
  ESC.attach(ESC_pin_number,1000,2000);

  //connecting rudder
   rudderServo.attach(rudder_pin_value);

  //starting the temperature sensor
  temp_sensor.begin();

  //setting the time interval frequency for every function
  timer.setInterval(4000L, printVirtualPinValue);
  timer.setInterval(1000L,get_temperature); 
  timer.setInterval(2000L,show_temperature);
  timer.setInterval(2000L,show_rudder_angle); 
  timer.setInterval(2000L,upload_temperature); 
  timer.setInterval(1000L,read_turbidity);
  timer.setInterval(2000L,upload_turbidity);
  timer.setInterval(2000L,show_turbidity);
}

void loop()
{
  run_motor();
  move_rudder();
  Blynk.run();
  timer.run();
}




