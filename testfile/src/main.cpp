#include <Arduino.h>
#include <M5Unified.h>
#include <SCServo.h>
#include <WebServer.h>
#include <WiFi.h>

// ===== WiFi =====
const char *ssid = "iPhone (15)";
const char *password = "stfg15fe146br";

// ===== Web =====
WebServer server(80);

// ===== Servo =====
SMS_STS st;
HardwareSerial servoSerial(1);

// ===== UART (UnitV2) =====
HardwareSerial camSerial(2);

// ===== モード =====
enum Mode { STOP, FORWARD, BACKWARD };
Mode mode = STOP;

// ===== スピード =====
int speedVal = 50;

// ===== WiFiハンドラ =====
void handleForward() {
  mode = FORWARD;
  server.send(200, "text/plain", "forward");
}

void handleBackward() {
  mode = BACKWARD;
  server.send(200, "text/plain", "backward");
}

void handleStop() {
  mode = STOP;
  server.send(200, "text/plain", "stop");
}

// ===== サーボ =====
void setWheelMode(int id) {
  st.unLockEprom(id);
  st.writeByte(id, 33, 1);
  st.LockEprom(id);
  delay(50);
}

// ===== setup =====
void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  Serial.begin(115200);

  // ===== WiFi =====
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/stop", handleStop);
  server.begin();

  // ===== Servo =====
  servoSerial.begin(1000000, SERIAL_8N1, 8, 7);
  st.pSerial = &servoSerial;

  setWheelMode(1);
  setWheelMode(2);

  // ===== UnitV2 UART =====
  camSerial.begin(115200, SERIAL_8N1, 1, 2);

  delay(1000);

  Serial.println("SYSTEM READY");
}

// ===== loop =====
void loop() {
  server.handleClient();

  // ===== モータ制御 =====
  if (mode == FORWARD) {
    st.WriteSpe(1, 1500, speedVal);
    st.WriteSpe(2, 1500, speedVal);
  } else if (mode == BACKWARD) {
    st.WriteSpe(1, 1500, -speedVal);
    st.WriteSpe(2, 1500, -speedVal);
  } else {
    st.WriteSpe(1, 0, 0);
    st.WriteSpe(2, 0, 0);
  }

  // ===== カメラ受信 =====
  if (camSerial.available()) {
    String line = camSerial.readStringUntil('\n');
    Serial.println(line);

    // ===== bottle検出 =====
    if (line.indexOf("\"type\":\"bottle\"") != -1) {
      Serial.println("🐱 にゃーん！");
    }
  }

  delay(20);
}