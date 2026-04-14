#include <Arduino.h>
#include <M5Unified.h>
#include <SCServo.h>

// STS3215の制御インスタンスを作成
SMS_STS st;

// esp32のシリアル通信を使う用
HardwareSerial servoSerial(1);

// モーターを無限回転モード(Mode 1)に設定する関数
void setWheelMode(int id) {
  st.unLockEprom(id);      // EEPROMのロック解除
  st.writeByte(id, 33, 1); // アドレス33(Mode)を 1(Wheel/Speed Mode) に設定
  st.LockEprom(id);        // EEPROMをロック
  delay(50);               // 書き込み完了待ち
}

// モーターを通常の位置制御モード(Mode 0)に戻す関数
void setPositionMode(int id) {
  st.unLockEprom(id);      // EEPROMのロック解除
  st.writeByte(id, 33, 0); // アドレス33(Mode)を 0(Position Mode) に設定
  st.LockEprom(id);        // EEPROMをロック
  delay(50);
}

void setup() {
  Serial.begin(115200);

  // (通信速度, 通信形式, RXピン, TXピン)(ピン8で受信し、7で送信)
  servoSerial.begin(1000000, SERIAL_8N1, 8, 7);

  // SCServoにシリアルポートを割り当て
  st.pSerial = &servoSerial;

  delay(1000);

  // 最初はすべて位置制御モードにリセット（ID 1〜4）
  for (int id = 1; id <= 4; id++) {
    setPositionMode(id);
  }
}

void loop() {
  Serial.println("=== 位置制御モード (ID 1〜4) ===");
  for (int i = 0; i < 2; i++) {
    for (int id = 1; id <= 4; id++) {
      st.WritePosEx(id, 2048, 2000, 50); // センター
    }
    delay(1000);

    for (int id = 1; id <= 4; id++) {
      st.WritePosEx(id, 2300, 2000, 50); // 少し回転
    }
    delay(1000);
  }

  // 無限回転モード（ホイールモード）のテスト
  Serial.println("=== 無限回転モード (ID 1〜4) ===");
  for (int id = 1; id <= 4; id++) {
    setWheelMode(id); // ホイールモードに変更
  }

  // 5秒間、速度を変えて回転させる
  for (int id = 1; id <= 4; id++) {
    st.WriteSpe(id, 1500, 50); // WriteSpe(ID, 速度, 加速度) 速度が正で正転
  }
  delay(2500);

  for (int id = 1; id <= 4; id++) {
    st.WriteSpe(id, -1500, 50); // 速度が負で逆転
  }
  delay(2500);

  // 回転を停止し、元の位置制御モードに戻す
  for (int id = 1; id <= 4; id++) {
    st.WriteSpe(id, 0, 50); // 停止
    delay(100);
    setPositionMode(id);
  }
  delay(1000);
}
