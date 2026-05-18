// #include "M5Capsule.h"
#include "cat_audio.h"
#include <Arduino.h>
#include <M5Unified.h>
#include <SCServo.h>
#include <WebServer.h>
#include <WiFi.h>

// ===== WiFi =====
const char *ssid = "***";
const char *password = "***";

// ===== Web =====
WebServer server(80);

// ===== Servo =====
SMS_STS st;
HardwareSerial servoSerial(1);

// ===== UART UnitV2 =====
HardwareSerial camSerial(2);

// ===== モード =====
enum Mode { STOP, FORWARD, BACKWARD, TURNRIGHT, TURNLEFT };
Mode mode = STOP;

// ===== スピード =====
int speedVal = 50;

// ===== 尻尾サーボ =====
constexpr uint8_t SERVO_ID = 4;
constexpr int SERVO_SPEED = 800;
constexpr int SERVO_ACC = 50;

constexpr int CENTER_POS = 2048;
constexpr int LEFT_90_POS = CENTER_POS - 1024;
constexpr int RIGHT_90_POS = CENTER_POS + 1024;

// ===== 尻尾タイマー =====
unsigned long lastTailMoveTime = 0;
bool tailLeft = true;
constexpr unsigned long TAIL_INTERVAL = 200;

// ===== 音声 =====
void meow() {
  M5.Speaker.setVolume(255);
  M5.Speaker.playRaw(cat_raw, cat_raw_len, 16000, false, 1, -1, true);

  while (M5.Speaker.isPlaying()) {
    server.handleClient(); // 鳴いてる間もスマホ操作を受ける
    delay(1);
  }

  M5.Speaker.stop();
}

// ===== 尻尾動作 =====
void moveServoCenter() {
  st.WritePosEx(SERVO_ID, CENTER_POS, SERVO_SPEED, SERVO_ACC);
}

void moveServoleft() {
  st.WritePosEx(SERVO_ID, LEFT_90_POS, SERVO_SPEED, SERVO_ACC);
}

void moveServoright() {
  st.WritePosEx(SERVO_ID, RIGHT_90_POS, SERVO_SPEED, SERVO_ACC);
}

// ===== WiFi接続確認 =====
void connectWiFi() {
  Serial.println("Connecting WiFi...");

  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);

  WiFi.begin(ssid, password);

  int retry = 0;

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    retry++;

    if (retry > 40) {
      Serial.println("\nWiFi FAILED");

      while (true) {
        delay(1000);
      }
    }
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ===== WiFiハンドラ =====
void handleForward() {
  mode = FORWARD;
  Serial.println("COMMAND: FORWARD");
  server.send(200, "text/plain", "forward");
}

void handleBackward() {
  mode = BACKWARD;
  Serial.println("COMMAND: BACKWARD");
  server.send(200, "text/plain", "backward");
}

void handleStop() {
  mode = STOP;
  Serial.println("COMMAND: STOP");
  server.send(200, "text/plain", "stop");
}

void handleTurnRight() {
  mode = TURNRIGHT;
  Serial.println("COMMAND: TURN RIGHT");
  server.send(200, "text/plain", "turnright");
}

void handleTurnLeft() {
  mode = TURNLEFT;
  Serial.println("COMMAND: TURN LEFT");
  server.send(200, "text/plain", "turnleft");
}

// ===== サーボを車輪モードにする =====
void setWheelMode(int id) {
  st.unLockEprom(id);
  st.writeByte(id, 33, 1);
  st.LockEprom(id);
  delay(50);
}

// ===== モータ制御 =====
void updateMotors() {
  if (mode == FORWARD) {
    st.WriteSpe(1, -1500, speedVal);
    st.WriteSpe(2, 1500, speedVal);
  } else if (mode == BACKWARD) {
    st.WriteSpe(1, 1500, speedVal);
    st.WriteSpe(2, -1500, speedVal);
  } else if (mode == TURNRIGHT) {
    st.WriteSpe(1, -1500, speedVal);
    st.WriteSpe(2, 300, speedVal);
  } else if (mode == TURNLEFT) {
    st.WriteSpe(1, -300, speedVal);
    st.WriteSpe(2, 1500, speedVal);
  } else {
    st.WriteSpe(1, 0, 0);
    st.WriteSpe(2, 0, 0);
  }
}

// ===== 尻尾制御 =====
void updateTail() {
  if (millis() - lastTailMoveTime >= TAIL_INTERVAL) {
    lastTailMoveTime = millis();

    if (tailLeft) {
      moveServoleft();
    } else {
      moveServoright();
    }

    tailLeft = !tailLeft;
  }
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  // シリアルポートの初期化
  Serial.begin(115200);
  // アクセスポイントに接続
  connectWiFi();
  // コールバック関数の定義
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/stop", handleStop);
  server.on("/turnright", handleTurnRight);
  server.on("/turnleft", handleTurnLeft);
  server.begin();

  // ===== Servo =====
  servoSerial.begin(1000000, SERIAL_8N1, 1, 3);
  st.pSerial = &servoSerial;

  setWheelMode(1);
  setWheelMode(2);

  // ===== UnitV2 UART =====
  camSerial.begin(115200, SERIAL_8N1, 15, 13);
  delay(1000);

  camSerial.println(
      "{\"function\":\"Object Recognition\",\"args\":[\"nanodet_80class\"]}");
  Serial.println("Camera: Object Recognition Mode ON");

  moveServoCenter();

  // Serial.println("SYSTEM READY");
  meow();
}

void loop() {
  server.handleClient();

  updateMotors();

  // ===== カメラ受信 =====
  if (camSerial.available()) {
    String line = camSerial.readStringUntil('\n');
    // Serial.println("[CAM] " + line);

    if (line.indexOf("\"type\":\"bottle\"") != -1) {
      meow();
    }
  }

  updateTail();

  delay(10);
}