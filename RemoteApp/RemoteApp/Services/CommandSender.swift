import Foundation

class CommandSender {
    let baseURL = "http://172.20.10.4" // ←ATOM S3が取得したIPに変更

    func send(_ command: Command) {
        let path: String //string型変数定義, letはなに

        switch command {
        case .forward:
            path = "/forward" //pathに/forwardを代入
        case .stop:
            path = "/stop"
        }

        guard let url = URL(string: baseURL + path) else { return } //urlを正しく作れたらおけ

        URLSession.shared.dataTask(with: url) { data, res, err in
            if let err = err {
                print("error:", err)
                return
            }
            print("sent:", path)
        }.resume() //URLSession.shared.dataTask(with: url) { ... }.resume()で、指定したurlにアクセスして通信する
    }
}
