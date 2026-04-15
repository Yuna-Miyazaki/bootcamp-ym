#include <Arduino.h>
#include <M5Unified.h>
#include <SCServo.h>
#include <WebServer.h>
#include <WiFi.h>

// ===== WiFi設定 =====
const char *ssid = "mm2020";
const char *password = "a3b7a2ede5bfb02fd5124b13f766f369";

// ===== Webサーバ =====
WebServer server(80);

// ===== サーボ =====
SMS_STS st;
HardwareSerial servoSerial(1);

// ===== モード設定関数 =====
void setWheelMode(int id) {
  st.unLockEprom(id);
  st.writeByte(id, 33, 1); // Wheel Mode
  st.LockEprom(id);
  delay(50);
}

// ===== フラグ受信時の処理 =====
void handleFlag() {
  Serial.println("にゃーん！");
  M5.Display.println("Nyaaan!");

  server.send(200, "text/plain", "OK");
}

// ===== セットアップ =====
void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  Serial.begin(115200);

  // ===== WiFi接続 =====
  M5.Display.println("Connecting WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Display.print(".");
  }

  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.println("Connected!");
  M5.Display.println(WiFi.localIP());

  Serial.println(WiFi.localIP());

  // ===== Webサーバ設定 =====
  server.on("/flag", handleFlag); // http://IP/flag で発火
  server.begin();

  // ===== サーボ初期化 =====
  servoSerial.begin(1000000, SERIAL_8N1, 8, 7);
  st.pSerial = &servoSerial;

  delay(1000);

  // ID1,2 をホイールモードに
  setWheelMode(1);
  setWheelMode(2);

  // 前進し続ける
  st.WriteSpe(1, 1500, 50);
  st.WriteSpe(2, 1500, 50);
}

// ===== ループ =====
void loop() {
  M5.update();
  server.handleClient(); // リクエスト待ち
}