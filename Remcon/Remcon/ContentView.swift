//ここで画面レイアウトを変更
import SwiftUI

struct ContentView: View { //viewプロトコルに準拠した型の構造体ってこと
    let sender = CommandSender()//コマンド送信係の定義

    var body: some View {//some: パフォーマンス面のロスを減らしてくれる　▶︎無理のないパフォーマンスのview型
        VStack(spacing: 20) { //ボタンを縦に並べる

            Button("Forward") { //画面にforwardボタン表示
                sender.send(.forward) //ボタンが押されたらコマンド送信
            }

            Button("Stop") {
                sender.send(.stop)
            }
            Button("Backward"){   //add
                sender.send(.backward)
            }
            Button("Turn Right"){
                sender.send(.turnright)
            }
            Button("Turn Left"){   //add
                sender.send(.turnleft)
            }
        }
        .padding()
    }
}
