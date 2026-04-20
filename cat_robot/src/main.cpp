#include <Arduino.h>
#include <M5Unified.h>
#include <SCServo.h>
#include <WebServer.h>
#include <WiFi.h>

// ===== WiFi =====
const char *ssid = "iPhone (15)";
const char *password = "stfg15fe146br";

// ===== Web =====
WebServer server(80); // http通信をポート80で立ち上げ

// ===== Servo =====
SMS_STS st;
HardwareSerial servoSerial(1);

// ===== UART (UnitV2) =====
HardwareSerial camSerial(2);

// ===== モード =====
enum Mode { STOP, FORWARD, BACKWARD, TURNRIGHT, TURNLEFT };
Mode mode = STOP;

// ===== スピード =====
int speedVal = 50;
int bigspeed = 80;
int smallspeed = 30;

// ===== WiFiハンドラ =====
void handleForward() {
  mode = FORWARD;
  Serial.println("COMMAND: FORWARD"); // ←追加
  server.send(200, "text/plain", "forward");
}

void handleBackward() {
  mode = BACKWARD;
  Serial.println("COMMAND: BACKWARD"); // ←追加
  server.send(200, "text/plain", "backward");
}

void handleStop() {
  mode = STOP;
  Serial.println("COMMAND: STOP"); // ←追加
  server.send(200, "text/plain", "stop");
}

void handleTurnRight() {
  mode = TURNRIGHT;
  Serial.println("COMMAND: TURN RIGHT"); // ←追加
  server.send(200, "text/plain", "turnright");
}

void handleTurnLeft() {
  mode = TURNLEFT;
  Serial.println("COMMAND: TURN LEFT"); // ←追加
  server.send(200, "text/plain", "turnleft");
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
  auto cfg =
      M5.config(); // m5ライブラリが持ってるm5のデフォルト設定を取得してcfgに保存
  M5.begin(cfg);   // 取得した情報でM5を初期化し起動

  Serial.begin(115200);

  // ===== WiFi =====
  M5.Display.println("Connecting WiFi..."); // 追加分

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Display.print(".");
  }

  M5.Display.fillScreen(TFT_BLACK); // ここから
  M5.Display.setCursor(0, 0);
  M5.Display.println("WiFi Connected!");
  M5.Display.println(WiFi.localIP().toString()); // ここまで追加分

  //~/forwardみたいなurlが来たらhandleForward関数を呼ぶ
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/stop", handleStop);
  server.on("/turnright", handleTurnRight);
  server.on("/turnleft", handleTurnLeft);
  server.begin();

  // ===== Servo =====
  servoSerial.begin(1000000, SERIAL_8N1, 8, 7); // サーボ起動
  st.pSerial = &servoSerial;

  setWheelMode(1); // id1と2を車輪(無限に回転するモード)に設定
  setWheelMode(2);

  // ===== UnitV2 UART =====
  camSerial.begin(115200, SERIAL_8N1, 1, 2); // カメラ起動
  delay(1000);                               // カメラ起動待ち

  // ★ここ重要：物体認識モードに切り替え
  camSerial.println(
      "{\"function\":\"Object Recognition\",\"args\":[\"nanodet_80class\"]}");
  Serial.println("Camera: Object Recognition Mode ON");

  Serial.println("SYSTEM READY");
}

// ===== loop =====
void loop() {
  server.handleClient();

  // ===== モータ制御 =====
  if (mode == FORWARD) {
    st.WriteSpe(1, 1500, speedVal); //(2個目の変数が回転速度やんけ！！！！)
    st.WriteSpe(2, 1500, speedVal);
  } else if (mode == BACKWARD) {
    st.WriteSpe(1, -1500, speedVal);
    st.WriteSpe(2, -1500, speedVal);
  } else if (mode == TURNRIGHT) {
    st.WriteSpe(1, 300, speedVal);
    st.WriteSpe(2, 1500, speedVal);
  } else if (mode == TURNLEFT) {
    st.WriteSpe(1, 1500, speedVal);
    st.WriteSpe(2, 300, speedVal);
  } else {
    st.WriteSpe(1, 0, 0);
    st.WriteSpe(2, 0, 0);
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