#include <Arduino.h>
#include <M5Unified.h>
#include <SCServo.h>
#include <WebServer.h>
#include <WiFi.h>

// =========================
// WiFi
// =========================
const char *ssid = "iPhone (15)";
const char *password = "stfg15fe146br";

// =========================
// WebServer
// =========================
WebServer server(80);

// =========================
// Servo
// =========================
SMS_STS st;
HardwareSerial servoSerial(1);

// =========================
// Camera (UnitV2)
// =========================
HardwareSerial camSerial(2);

// =========================
// 状態（ジョイコン入力）
// =========================
float forwardVal = 0.0;
float turnVal = 0.0;

int speedVal = 50;

// 安全停止用
unsigned long lastCmdTime = 0;

// =========================
// モーター初期化
// =========================
void setWheelMode(int id) {
  st.unLockEprom(id);
  st.writeByte(id, 33, 1);
  st.LockEprom(id);
  delay(50);
}

// =========================
// ジョイスティック受信
// =========================
void handleJoy() {
  if (server.hasArg("f"))
    forwardVal = server.arg("f").toFloat();
  if (server.hasArg("t"))
    turnVal = server.arg("t").toFloat();

  lastCmdTime = millis();

  Serial.print("f: ");
  Serial.print(forwardVal);
  Serial.print(" t: ");
  Serial.println(turnVal);

  server.send(200, "text/plain", "ok");
}

// =========================
// WiFi接続
// =========================
void connectWiFi() {
  WiFi.begin(ssid, password);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());
}

// =========================
// setup
// =========================
void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  Serial.begin(115200);

  // 初期化（暴走防止）
  forwardVal = 0;
  turnVal = 0;

  // WiFi
  connectWiFi();

  // Web
  server.on("/joy", handleJoy);
  server.begin();

  // Servo
  servoSerial.begin(1000000, SERIAL_8N1, 8, 7);
  st.pSerial = &servoSerial;

  setWheelMode(1);
  setWheelMode(2);

  // Camera
  camSerial.begin(115200, SERIAL_8N1, 1, 2);

  delay(1000); // カメラ起動待ち

  // ★ここ重要：物体認識モードに切り替え
  camSerial.println(
      "{\"function\":\"Object Recognition\",\"args\":[\"nanodet_80class\"]}");
  Serial.println("Camera: Object Recognition Mode ON");

  Serial.println("SYSTEM READY");
}

// =========================
// loop
// =========================
void loop() {
  server.handleClient();

  // ===== 安全停止 =====
  if (millis() - lastCmdTime > 500) {
    forwardVal = 0;
    turnVal = 0;
  }

  // ===== デッドゾーン =====
  if (abs(forwardVal) < 0.05)
    forwardVal = 0;
  if (abs(turnVal) < 0.05)
    turnVal = 0;

  // ===== 差動計算 =====
  int base = forwardVal * speedVal;
  int turn = turnVal * speedVal;

  int leftMotor = base + turn;
  int rightMotor = base - turn;

  // ===== モーター出力（方向対応版）=====
  // 左
  if (leftMotor >= 0) {
    st.WriteSpe(1, 1500, leftMotor);
  } else {
    st.WriteSpe(1, 1500, (-leftMotor) | 1024);
  }

  // 右
  if (rightMotor >= 0) {
    st.WriteSpe(2, 1500, rightMotor);
  } else {
    st.WriteSpe(2, 1500, (-rightMotor) | 1024);
  }

  // ===== カメラ受信 =====
  if (camSerial.available()) {
    String line = camSerial.readStringUntil('\n');
    Serial.println("[CAM] " + line);

    if (line.indexOf("\"type\":\"bottle\"") != -1) {
      Serial.println("🐱 にゃーん！");
    }
  }

  delay(20);
}