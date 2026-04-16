#include <Arduino.h>
#include <M5Unified.h>
#include <SCServo.h>
#include <WebServer.h>
#include <WiFi.h>

// ===== WiFi設定 =====
const char *ssid = "iPhone (15)";
const char *password = "stfg15fe146br";

// ===== Webサーバ =====
WebServer server(80);

// ===== サーボ =====
SMS_STS st;
HardwareSerial servoSerial(1);

// ===== モード管理 =====
enum Mode { STOP, FORWARD, BACKWARD };
Mode mode = STOP;

// ===== スピード =====
int speed = 50; // 必要なら変更

// ===== サーボモード設定 =====
void setWheelMode(int id) {
  st.unLockEprom(id);
  st.writeByte(id, 33, 1); // Wheel Mode
  st.LockEprom(id);
  delay(50);
}

// ===== HTTPハンドラ =====
void handleForward() {
  Serial.println("HTTP: FORWARD");
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.println("FORWARD");

  mode = FORWARD;

  server.send(200, "text/plain", "forward ok");
}

void handleBackward() {
  Serial.println("HTTP: BACKWARD");
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.println("BACKWARD");

  mode = BACKWARD;

  server.send(200, "text/plain", "backward ok");
}

void handleStop() {
  Serial.println("HTTP: STOP");
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.println("STOP");

  mode = STOP;

  server.send(200, "text/plain", "stop ok");
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

  // ===== Webサーバ =====
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/stop", handleStop);
  server.begin();

  // ===== サーボ初期化 =====
  servoSerial.begin(1000000, SERIAL_8N1, 8, 7);
  st.pSerial = &servoSerial;

  delay(1000);

  // Wheel Mode設定
  setWheelMode(1);
  setWheelMode(2);

  // 初期停止
  mode = STOP;
}

// ===== ループ =====
void loop() {
  M5.update();
  server.handleClient();

  // ===== ここが無限回転の本質 =====
  if (mode == FORWARD) {
    st.WriteSpe(1, 1500, speed);
    st.WriteSpe(2, 1500, speed);
  } else if (mode == BACKWARD) {
    st.WriteSpe(1, 1500, -speed);
    st.WriteSpe(2, 1500, -speed);
  } else {
    st.WriteSpe(1, 0, 0);
    st.WriteSpe(2, 0, 0);
  }

  delay(20); // 安定用（超重要）
}