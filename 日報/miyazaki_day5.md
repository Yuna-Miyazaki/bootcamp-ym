# 日報　(宮崎)

## 本日の目標
1. unitV2からATOM S3にゴミ落ちてたよフラグ送れるか確認する
2. 画像取得からゴミ認識までできるやつがあるかUnitV2の情報を調べる

## 進捗
1. スマホでモーター制御できた
   - iphoneのテザリング繋げてatom s3のipアドレス取得した
   - swiftでhttpリクエストおくるコード作った
   - 前進、停止のボタンは作ったので、次は前後左右にボタンを押している間動き続けるようにする

2. UnitV2から物体認識時のメッセージを受け取れた
3. ゴミ認識できるものに関しては

## バカくそ詰まってたこと
1. UnitV2の使い方
   - そもそも何をどうやったら認識結果が受け取れるのかとかがわからない
   - とりあえずドライバ入れようとしたら安全が保証できないからって却下された▶︎プライバシーとセキュリティのとこ触ったら入った
   - 物体認識をやらせようとした時、unitV2さんはシリアル出力かなんかで認識結果だけ送ってくれるみたいだから、それをATOM S3で受け取ろうとした
   - なんか色々触ってたら一生再起動っぽいことしてるのが表示される
   - カメラから何もこない。何をしなければならないのかもわからない
         entry 0x403c98d0
         ESP-ROM:esp32s3-20210327
         Build:Mar 27 2021
         rst:0x8 (TG1WDT_SYS_RST),boot:0x2b (SPI_FAST_FLASH_BOOT)
         Saved PC:0x403743c5
         SPIWP:0xee
         mode:DIO, clock div:1
         load:0x3fce3808,len:0x4bc
         load:0x403c9700,len:0xbd8
         load:0x403cc700,len:0x2a0c
         entry 0x403c98d0
         ESP-ROM:esp32s3-20210327
         Build:Mar 27 2021
         rst:0x8 (TG1WDT_SYS_RST),boot:0x2b (SPI_FAST_FLASH_BOOT)
         Saved PC:0x40379008
         SPIWP:0xee
         mode:DIO, clock div:1
         load:0x3fce3808,len:0x4bc
         load:0x403c9700,len:0xbd8
         load:0x403cc700,len:0x2a0c
         .......て感じ
   - RxピンとTxピンが間違ってたからかな？そこ直したら出なくなった
   -   // JSONコマンド送信
        Serial2.println("{\"function\":\"Object Recognition\",\"args\":[\"nanodet_80class\"]}"); で物体認識モードに切り替えて、
        if (Serial2.available()) {
            Serial.println(Serial2.readStringUntil('\n'));
        }でカメラからのメッセージを受け取れた
2. 


## 明日やること
1. 車輪動かすコードとカメラから出力を受け取るコードを合体させる
2. 


## やることリスト
### atom s3
+ [x] モータ動かす
+ [x] スマホからwifi経由で命令を受け取る
+ [ ] 鳴き声を出す
+ [x] カメラからゴミ判定を受け取る
+ [ ] ゴミを分別する(bottleだったらフラグonみたいにする)

### unitV2
+ [x] 画像を取得する
+ [x] ゴミ判定をatomに送る
+ [x] ゴミ判定をする()

### 外装
+ [x] cadでタイヤ作る
+ [ ] cadで猫作る
+ [ ] cadでしっぽ作る
+ [ ] cadでキャスター作る
+ [x] タイヤ印刷する
+ [ ] 猫印刷する
+ [ ] 尻尾印刷する
+ [ ] キャスター印刷する

### スマホ
+ [x] リモコンアプリの試作を作る
+ [x] リモコンアプリからモーターを前進させる
+ [ ] モーターを押している間動くようにする
+ [ ] 前後左右に動くようにする
+ [ ] ジョイコンにする