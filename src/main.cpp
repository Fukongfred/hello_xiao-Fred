#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>


#define LED_PIN     A1 // Example pin
#define NUM_LEDS    4 // Number of LEDs in your strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Initialize the BME280 using the I2C interface
Adafruit_BME280 bme; 

// For an SSD1306 display, the width and height can change according to your screen's specification
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Initialize the OLED display using the I2C interface
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

#include <Wire.h>
#include "Adafruit_VEML7700.h"

Adafruit_VEML7700 veml = Adafruit_VEML7700();

void setup() {
  Serial.begin(9600);
  
  strip.begin(); // Initialize the LED strip
  strip.show(); // Initialize all pixels to 'off'

  if (!veml.begin()) {
    Serial.println("Sensor not found");
    while (1);
  }
  veml.setGain(VEML7700_GAIN_1);
  veml.setIntegrationTime(VEML7700_IT_800MS);

  if (!bme.begin(0x76)) { // The I2C address can be 0x77 or 0x76
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  
  // Initialize the OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // The I2C address can be 0x3D or 0x3C
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.display();

  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(1); // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
}

void animateLEDs() {
  for (int cycle = 0; cycle < 4; cycle++) { // Run the animation twice
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color(255, 255, 255)); // Set the pixel to white
      strip.show();
      delay(150); // Speed of the animation
      strip.setPixelColor(i, strip.Color(0, 0, 0)); // Turn off the pixel
    }
  }
}

void loop() {
  float lux = veml.readLux();
  Serial.print("Lux: "); Serial.println(lux);
  if (lux < 170.0) { // Assumes darkness below 10 lux, adjust as necessary
    Serial.println("DARK");
    animateLEDs(); // Call the animation function
  } else {
    Serial.println("LIGHT");
  }

  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;
  
  // Display the results on the screen
  display.clearDisplay(); // Clear the display buffer

  // Set the cursor to the top-left position
  display.setCursor(0, 0); 

  // Print the values to the display buffer
  display.print("Temp: "); display.print(temperature); display.println(" C");
  display.print("Humidity: "); display.print(humidity); display.println(" %");
  display.print("Pressure: "); display.print(pressure); display.println(" hPa");
  display.print("Lux: ");display.println(lux);

  // Update the display with all of the above graphics
  display.display(); // Wait for 0.5 seconds
}