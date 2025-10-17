#include "../../config.h"
#include "webui.h"
#include "../display/display.h"
#include "../gif_player/gif_player.h"

#include <WiFi.h>
#include <LittleFS.h>

WebServer server(80);
volatile bool g_need_reload = false;
volatile bool g_uploading   = false;
volatile bool g_request_stop = false;
volatile bool g_swap_ready   = false;

static void sendIndex() {
  File file = LittleFS.open("/index.html", "r");
  if (file) {
    String html = file.readString();
    file.close();

    // replace template placeholders with actual config values
    html.replace("{{AP_SSID}}", String(AP_SSID));
    html.replace("{{AP_PASSWORD}}", String(AP_PASS));
    html.replace("{{AP_IP}}", WiFi.softAPIP().toString());

    server.send(200, "text/html", html);
  } else {
    server.send(500, "text/plain", "Failed to load index.html");
  }
}

static void sendStatus() {
  String json = "{";
  json += "\"uploading\":" + String(g_uploading ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

static void sendOrientation() {
  int lcd_rotation = get_gif_orientation();
  int web_orientation = (lcd_rotation == LCD_ROTATION_PORTRAIT) ? 0 : 1;
  server.send(200, "text/plain", String(web_orientation));
}


static void handleOrientation() {
  if (server.hasArg("orientation")) {
    int orientation = server.arg("orientation").toInt();
    if (orientation == 0 || orientation == 1) {
      int lcd_rotation = (orientation == 0) ? LCD_ROTATION_PORTRAIT : LCD_ROTATION_LANDSCAPE;
      set_gif_orientation(lcd_rotation);
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Invalid orientation");
    }
  } else {
    server.send(400, "text/plain", "Missing orientation parameter");
  }
}

static File s_upload;
static size_t up_written = 0;
static bool   up_failed  = false;

static bool hasGifMagic(const char* path) {
  File f = LittleFS.open(path, "r");
  if (!f) return false;
  char hdr[6]; int n = f.read((uint8_t*)hdr, 6);
  f.close();
  return n==6 && (memcmp(hdr,"GIF89a",6)==0 || memcmp(hdr,"GIF87a",6)==0);
}

static void handleUpload() {
  HTTPUpload &up = server.upload();

  if (up.status == UPLOAD_FILE_START) {
    g_uploading   = true;
    up_written    = 0;
    up_failed     = false;

    show_upload_indicator();

    g_request_stop = true;

    LittleFS.mkdir("/gifs");
    LittleFS.remove(GIF_TMP_PATH);
    s_upload = LittleFS.open(GIF_TMP_PATH, FILE_WRITE);
    if (!s_upload) { up_failed = true; }
    return;
  }

  if (up.status == UPLOAD_FILE_WRITE) {
    if (!up_failed && s_upload) {
      size_t w = s_upload.write(up.buf, up.currentSize);
      if (w != up.currentSize) {
        up_failed = true;
      } else {
        up_written += w;
      }
    }
    return;
  }

  if (up.status == UPLOAD_FILE_END) {
    if (s_upload) { s_upload.flush(); s_upload.close(); }

    size_t finalSize = 0;
    { File f = LittleFS.open(GIF_TMP_PATH, "r"); if (f) { finalSize = f.size(); f.close(); } }

    if (!up_failed && finalSize == up.totalSize && finalSize > 0 && hasGifMagic(GIF_TMP_PATH)) {

      g_swap_ready = true;
    } else {
      LittleFS.remove(GIF_TMP_PATH);
    }

    g_uploading = false;

    hide_upload_indicator();

    return;
  }

  if (up.status == UPLOAD_FILE_ABORTED) {
    if (s_upload) s_upload.close();
    LittleFS.remove(GIF_TMP_PATH);
    g_uploading = false;
  }
}

void webui_begin() {
  WiFi.mode(WIFI_AP);
  WiFi.setSleep(false);
  WiFi.softAP(AP_SSID, AP_PASS);

  server.on("/", HTTP_GET, sendIndex);
  server.on("/status", HTTP_GET, sendStatus);
  server.on("/orientation", HTTP_GET, sendOrientation);
  server.on("/orientation", HTTP_POST, handleOrientation);
  server.on("/upload", HTTP_POST,
    [](){ server.sendHeader("Location","/",true); server.send(303); },
    handleUpload);
  server.onNotFound([](){ server.send(404, "text/plain", "Not found"); });
  server.begin();
}

void webui_handle() {
  server.handleClient();
}
