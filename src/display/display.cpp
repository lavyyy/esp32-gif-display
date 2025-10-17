#include "../../config.h"
#include "display.h"
#include "../gif_player/gif_player.h"
#include <bb_spi_lcd.h>
#include <Arduino.h>

BB_SPI_LCD lcd;
bool upload_indicator_active = false;

bool display_init()
{
  lcd.begin(LCD_ST7789, FLAGS_NONE, 40000000, LCD_CS, LCD_DC, LCD_RST, -1, LCD_MISO, LCD_MOSI, LCD_SCLK);
  lcd.setRotation(load_orientation());
  display_backlight_on(true);
  emptyScreen();
  return true;
}

int display_width()  { return LCD_W; }
int display_height() { return LCD_H; }

void display_backlight_on(bool on)
{
  if (LCD_BL >= 0) {
    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, on ? HIGH : LOW);
  }
}

void emptyScreen() {
  lcd.fillScreen(TFT_BLACK);
}

void show_upload_indicator() {
  if (!upload_indicator_active) {
    upload_indicator_active = true;

    // draw border
    lcd.drawRect(0, 0, LCD_W, LCD_H, TFT_YELLOW);
    lcd.drawRect(1, 1, LCD_W-2, LCD_H-2, TFT_YELLOW);
    lcd.drawRect(2, 2, LCD_W-4, LCD_H-4, TFT_YELLOW);

    lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
    lcd.setTextSize(2);
    // calculate text position manually (approximate center)
    int textWidth = 12 * 6;
    int textHeight = 8;
    int x = (LCD_W - textWidth) / 2;
    int y = (LCD_H - textHeight) / 2;
    lcd.setCursor(x, y);
    lcd.println("UPLOADING...");
  }
}

void hide_upload_indicator() {
  if (upload_indicator_active) {
    upload_indicator_active = false;

    emptyScreen();
  }
}
