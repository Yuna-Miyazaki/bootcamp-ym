import Foundation

class CommandSender {
    let baseURL = "http://172.20.10.4"

    func sendJoy(forward: Double, turn: Double) {

        let urlString = "\(baseURL)/joy?f=\(forward)&t=\(turn)"

        guard let url = URL(string: urlString) else { return }

        URLSession.shared.dataTask(with: url) { _, _, err in
            if let err = err {
                print("error:", err)
            }
        }.resume()
    }
}
