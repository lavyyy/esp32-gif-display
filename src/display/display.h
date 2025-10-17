#pragma once
#include <stdint.h>
#include <bb_spi_lcd.h>

extern BB_SPI_LCD lcd;

bool display_init();
int  display_width();
int  display_height();
void display_backlight_on(bool on);
void emptyScreen();

// Upload indicators
void show_upload_indicator();
void hide_upload_indicator();
