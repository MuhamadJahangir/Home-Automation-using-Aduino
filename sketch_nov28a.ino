#include <dhtint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTPIN 5 // Adjust for your Arduino model
DHTINT sensor(DHTPIN);

const int pirPin = 6;
const int ldrPin = A0;
const int light = 8;
const int fan = 9;
const int ac = 10;

int motionReading = LOW;
int ldrReading;
int temperature; // Using lowercase 'temperature' for consistency

// Variables to track current state
int currentFanState = LOW; // Initial state: Fan off
int currentAcState = LOW;  // Initial state: AC off

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  pinMode(light, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(ac, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(ldrPin, INPUT);
  Serial.begin(9600);
}

void temperatureRead() {
  int chk = sensor.read();
  if (chk == DHTLIB_OK) {
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.setCursor(5, 0);
    lcd.print(temperature, 1); // Use 'temperature' variable
    lcd.setCursor(7, 0);
    lcd.print("C");


    Serial.print("Temp: ");
    temperature = sensor.getTemperature(); // Use 'temperature' variable
    Serial.print(temperature, 1);
    Serial.print("°C, Hum: ");
    Serial.print(sensor.getHumidity(), 1);
    Serial.println("%");
  } else {
    Serial.println("Error!");
  }
  delay(3000); // Wait 3 seconds before next reading
}

void motionDetect() {
  motionReading = digitalRead(pirPin);
  if (motionReading == HIGH) {
    Serial.println("Motion is Detected...");
  } else {
    Serial.println("Motion is not Detected....");
  }
}

void lightRead() {
  ldrReading = analogRead(ldrPin);
  Serial.print("The Light intensity is: ");
  Serial.println(ldrReading);

  lcd.setCursor(0, 1);
  lcd.print("Light: ");
  lcd.setCursor(6, 1);
  lcd.print(ldrReading);
  lcd.setCursor(8, 1);
  lcd.print("%");

  
}

void updateFanState(int newState) {
  if (currentFanState != newState) {
    digitalWrite(fan, newState);
    currentFanState = newState;
    if (newState == LOW) {
      Serial.println("Fan is LOW");
    } else {
      Serial.println("Fan is HIGH");
    }
  }
}

void updateAcState(int newState) {
  if (currentAcState != newState) {
    digitalWrite(ac, newState);
    currentAcState = newState;
    if (newState == LOW) {
      Serial.println("AC is LOW");
    } else {
      Serial.println("AC is HIGH");
    }
  }
}

void loop() 
{
  motionDetect();
  temperatureRead();
  lightRead();

  if (motionReading == HIGH) 
  {
    if (ldrReading < 20) 
    {
      digitalWrite(light, LOW); // Turn on light
      Serial.println("LIGHT is LOW.....");
    } 
    else if (ldrReading > 20) 
    {
      digitalWrite(light, HIGH); // Turn off light
    }

    // Temperature control with state updates
    if ((temperature >= 23) && (temperature <= 30)) 
    {
      updateFanState(LOW);  // Update fan state (turn off)
      updateAcState(HIGH); // Update AC state (turn on)
    } 
    else if (temperature >= 31) 
    {
      updateFanState(HIGH); 
      updateAcState(LOW);  
    } else if (temperature < 23) 
    {
      updateFanState(HIGH); // Update fan state
      updateAcState(HIGH); // Update fan state
    }
  }
  else
  {
    updateFanState(HIGH); 
    updateAcState(HIGH);
    digitalWrite(light, HIGH);
  }
}