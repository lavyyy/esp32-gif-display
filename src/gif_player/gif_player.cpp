#include "../../config.h"
#include "gif_player.h"
#include "../display/display.h"
#include <bb_spi_lcd.h>
#include <Arduino.h>
#include <GIFPlayer.h>
#include <LittleFS.h>

GIFPlayer gp;

bool gif_open = false;
int gw = 0, gh = 0;
int xOff = 0, yOff = 0;
int current_rotation = LCD_ROTATION_PORTRAIT;

static void compute_offsets() {
  const int W = LCD_W;
  const int H = LCD_H;
  xOff = (W > gw) ? (W - gw) / 2 : 0;
  yOff = (H > gh) ? (H - gh) / 2 : 0;
}

static void clear_state() {
  gif_open = false;
  gw = gh = 0;
  xOff = yOff = 0;
}

void gifplayer_begin()
{
  if (!LittleFS.exists(GIF_ACTIVE_PATH)) {
    gif_open = false;
    return;
  }

  File testFile = LittleFS.open(GIF_ACTIVE_PATH, "r");
  if (testFile) {
    testFile.close();
  } else {
    gif_open = false;
    return;
  }

  int result = gp.openLFS(&lcd, GIF_ACTIVE_PATH);

  if (result == GIF_SUCCESS) {
    gp.getInfo(&gw, &gh);
    compute_offsets();
    gif_open = true;
  } else {
    gif_open = false;
  }
}

void gifplayer_stop() {
  if (gif_open) {
    clear_state();
  }
}

void gifplayer_reload()
{
  gifplayer_stop();
  lcd.fillScreen(TFT_BLACK);

  delay(100);

  gifplayer_begin();
}

void gifplayer_loop() {
  if (!gif_open || gw <= 0 || gh <= 0) return;

  const int rc = gp.play(GIF_CENTER, GIF_CENTER, /*bDelay=*/true);
  if (rc < 0) {
    gifplayer_stop();
  }
}

bool is_gif_open() {
  return gif_open;
}

int get_gif_width() {
  return gw;
}

int get_gif_height() {
  return gh;
}

int get_gif_x_offset() {
  return xOff;
}

int get_gif_y_offset() {
  return yOff;
}

void set_gif_orientation(int rotation) {
  if (rotation != current_rotation) {
    current_rotation = rotation;
    lcd.setRotation(rotation);
    save_orientation(rotation);
    if (gif_open) {
      gifplayer_reload();
    }
  }
}

int get_gif_orientation() {
  return current_rotation;
}

void save_orientation(int rotation) {
  File f = LittleFS.open(ORIENTATION_FILE, "w");

  if (f) {
    f.print(rotation);
    f.close();
  }
}

int load_orientation() {
  if (!LittleFS.exists(ORIENTATION_FILE)) {
    return LCD_ROTATION_PORTRAIT; // default
  }

  File f = LittleFS.open(ORIENTATION_FILE, "r");
  if (f) {
    int rotation = f.parseInt();
    f.close();
    return rotation;
  }

  return LCD_ROTATION_PORTRAIT;
}
