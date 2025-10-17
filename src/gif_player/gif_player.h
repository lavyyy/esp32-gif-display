#pragma once
#include <stdint.h>
#include <bb_spi_lcd.h>

void gifplayer_begin();
void gifplayer_stop();
void gifplayer_loop();
void gifplayer_reload();

bool is_gif_open();
int get_gif_width();
int get_gif_height();
int get_gif_x_offset();
int get_gif_y_offset();

void set_gif_orientation(int rotation);
int get_gif_orientation();
void save_orientation(int rotation);
int load_orientation();
