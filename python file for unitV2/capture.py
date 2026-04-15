import subprocess
import numpy as np
import cv2
import time

# 現時点では雑
WIDTH = 640
HEIGHT = 480

cmd = [
    "ffmpeg",
    "-f", "video4linux2",
    "-i", "/dev/video0",
    "-f", "image2pipe",
    "-vcodec", "rawvideo",
    "-pix_fmt", "bgr24",
    "-"
]

pipe = subprocess.Popen(cmd, stdout=subprocess.PIPE)

#　ctrl+c押すまで画像取得し続ける(保存はしない)
print("start capturing... (Ctrl+Cで停止)")

try:
    while True:
        # ffmpegから来る画像データ取得
        raw = pipe.stdout.read(WIDTH * HEIGHT * 3)
        # 取得できんとき
        if not raw:
            print("no frame")
            break
        # ただのバイト列をnumpy配列に
        frame = np.frombuffer(raw, dtype=np.uint8).reshape((HEIGHT, WIDTH, 3))

        # --- ここに将来の処理を書く ---
        
        # --- 負荷軽減（重要） ---
        time.sleep(0.1)

# ctrl+cで止める
except KeyboardInterrupt:
    print("\nstopped by user")

# 終了するときにffmpegを止める
finally:
    pipe.terminate()
    print("camera stopped")