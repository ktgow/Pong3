#include "Adafruit_NeoPixel.h"

#include "Pong3Table.h"
#include "KeyFrameColorInterpolator.h"
#include "OverheadLights.h"
#include "TableController.h"
#include "Util.h"

// TODO ideas:
// - More animations...
// - Jellyfish with 6 different colors.
// - Colors stepping from one triangle to another.

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, 6, NEO_GRB + NEO_KHZ800);
Pong3Table table(&strip);
TableController* controller = NULL;
ColorSet overhead_colors;
OverheadLights* overhead_lights = NULL;


int rf_state[4] = {};

void SetupOverheadLights() {
  Serial.println("Setting up overhead lights...");
  const Color red(255, 0, 0);
  const Color yellow(230, 230, 0);
  const Color green(0, 230, 0);
  const Color light_blue(0, 230, 255);
  const Color blue(0, 0, 255);
  const Color pink(255, 0, 255);
  const Color purple(128, 0, 255);
  const Color white(255, 255, 255);
  const Color black(0, 0, 0);

  ColorSet::ColorKeyframeInfo color_keys[] = {
    { red, 0.0 }, // Rainbow
    { yellow, 1.0 / 6.0 },
    { green, 2.0 / 6.0 },
    { light_blue, 3.0 / 6.0},
    { blue, 4.0 / 6.0},
    { purple, 5.0 / 6.0},
    { red, 1.0 },

    { white, 0.0 },  // White Yellow Green
    { yellow, 0.333 },
    { green, 0.667 },
    { white, 1.0 },

    { red, 0.0 },  // Red Blue
    { blue, 0.5 },
    { red, 1.0 },

    { black, 0.0 },  // Black.
    { black, 1.0 },

    { yellow, 0.0 },  // Yellow Purple Green
    { purple, 0.333 },
    { green, 0.667 },
    { yellow, 1.0 },
    
    { yellow, 0.0 },  // Yellow Red
    { red, 0.5 },
    { yellow, 1.0 },
  };

  overhead_colors.AddColors(color_keys,
			    sizeof(color_keys) / sizeof(color_keys[0]));
  for (int i = 0; i < overhead_colors.GetNumColors(); ++i) {
    KeyFrameColorInterpolator* colors =
      (KeyFrameColorInterpolator*)overhead_colors.GetColor(i);
    colors->SetCacheEnabled(false);
  }

  overhead_lights = new OverheadLights(overhead_colors.GetColor(0), &table,
				       table.GetBaseLayer());
  Serial.println("Overhead lights done.");
}

void setup() {
  Serial.begin(9600);
  Serial.print("Available mem on startup:");
  Serial.println(availableMemory());

  // Setup the overhead lights.
  SetupOverheadLights();

  controller = new TableController(&table);

  randomSeed(2);
  strip.begin();



  // Setup keyfob receiver.
  pinMode(52, OUTPUT);
  digitalWrite(52, HIGH);
  pinMode(50, INPUT);
  pinMode(48, INPUT);
  pinMode(46, INPUT);
  pinMode(44, INPUT);


  table.Show();
}

//#define PROFILING

#define MIN_LOOP_MS 33
unsigned long last_millis = 0;

void loop() {
  unsigned long cur_millis = millis();
  if (cur_millis - last_millis < MIN_LOOP_MS) {
    delay(MIN_LOOP_MS - (cur_millis - last_millis));
  }
#ifdef PROFILING
  Serial.print("ms since last loop: ");
  Serial.println(cur_millis - last_millis);
#endif
  last_millis = cur_millis;

  // Handle RF receiver.
  int new_rf_state[4] = {};
  new_rf_state[0] = digitalRead(50);
  new_rf_state[1] = digitalRead(48);
  new_rf_state[2] = digitalRead(46);
  new_rf_state[3] = digitalRead(44);
  for (int i = 0; i < 4; ++i) {
    if (new_rf_state[i] != rf_state[i]) {
      rf_state[i] = new_rf_state[i];

      switch(i) {
        case 0:
          Serial.println("Next Setting.");
          controller->NextSetting();
          break;
        case 1:
          Serial.println("Next Color.");
          controller->NextColorSet();
          break;
        case 2:
          Serial.println("Next Pattern.");
          controller->NextPattern();
          break;
        case 3:
          Serial.println("Back.");
          controller->Back();
          break;
      }
    }
  }

  controller->Step();
  overhead_lights->Step();
  table.Show();

  static bool mem_after_loop = true;
  if (mem_after_loop) {
    mem_after_loop = false;
    Serial.print("Available mem after loop:");
    Serial.println(availableMemory());
  }
}
