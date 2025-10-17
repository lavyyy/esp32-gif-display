#include "config.h"
#include "src/display/display.h"
#include "src/gif_player/gif_player.h"
#include "src/webui/webui.h"
#include <LittleFS.h>

void setup() {
  Serial.begin(115200);
  delay(100);
  LittleFS.begin(true);
  display_init();

  // load and apply saved orientation before starting GIF player
  int saved_orientation = load_orientation();
  set_gif_orientation(saved_orientation);

  webui_begin();
  gifplayer_begin();
}

void loop() {
  webui_handle();

  static bool decoder_stopped = false;

  // stop the decoder on upload start
  if (g_request_stop && !decoder_stopped) {
    gifplayer_stop();
    decoder_stopped = true;
    g_request_stop = false;
  }

  // handle swap
  if (decoder_stopped && g_swap_ready) {
    // check if source file exists
    if (!LittleFS.exists(GIF_TMP_PATH)) {
      Serial.println("[swap] ERROR: source file does not exist");
      g_swap_ready = false;
      decoder_stopped = false;
      return;
    }

    // check source file size
    File srcFile = LittleFS.open(GIF_TMP_PATH, "r");
    if (srcFile) {
      srcFile.close();
    } else {
      Serial.println("[swap] ERROR: cannot open source file");
      g_swap_ready = false;
      decoder_stopped = false;
      return;
    }

    if (LittleFS.exists(GIF_ACTIVE_PATH)) {
      if (!LittleFS.remove(GIF_ACTIVE_PATH)) {
        Serial.println("[swap] WARNING: failed to remove old file");
      }
    }
    delay(50);

    if (LittleFS.rename(GIF_TMP_PATH, GIF_ACTIVE_PATH)) {
      // success
    } else {

      // try alternative approach: copy then delete
      File src = LittleFS.open(GIF_TMP_PATH, "r");
      File dst = LittleFS.open(GIF_ACTIVE_PATH, "w");

      if (src && dst) {
        uint8_t buffer[512];
        size_t bytesRead;
        size_t totalCopied = 0;

        while ((bytesRead = src.read(buffer, sizeof(buffer))) > 0) {
          dst.write(buffer, bytesRead);
          totalCopied += bytesRead;
        }

        src.close();
        dst.close();

        if (totalCopied > 0) {
          LittleFS.remove(GIF_TMP_PATH);
        } else {
          Serial.println("[swap] ERROR: copy failed");
        }
      } else {
        Serial.println("[swap] ERROR: cannot open files for copy");
        if (src) src.close();
        if (dst) dst.close();
      }
    }
    g_swap_ready = false;

    // reopen GIF and resume
    gifplayer_reload();
    decoder_stopped = false;
  }

  // drive the decoder when not uploading / not stopped
  if (!g_uploading && !decoder_stopped) {
    gifplayer_loop();
  }

  delay(1);
}
