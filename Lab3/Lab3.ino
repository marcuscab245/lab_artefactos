
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3C

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1




Adafruit_SH1106G display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);


void setup() {

  Serial.begin(9600);

 
  Wire.begin();

  delay(250);
 

}

void loop() {
 
  int lectura = analogRead(A0);
  float temperatura = (lectura*(5.0/1023.0))*100;
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(15, 10);
  display.println("Temperatura:");
  display.setTextSize(1);
  display.setCursor(15, 40);
  display.print(temperatura, 2);
  display.println(" C");
  Serial.println(temperatura);
 
  display.display();
  delay(1000);
 
}
