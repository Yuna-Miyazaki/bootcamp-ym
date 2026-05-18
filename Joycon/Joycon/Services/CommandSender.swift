import Foundation

class CommandSender {
    let baseURL = "http://172.20.10.6"

    func sendCommand(_ command: String) {
        let urlString = "\(baseURL)/\(command)"

        guard let url = URL(string: urlString) else {
            print("Invalid URL:", urlString)
            return
        }

        URLSession.shared.dataTask(with: url) { _, _, error in
            if let error = error {
                print("error:", error)
            } else {
                print("sent:", command)
            }
        }.resume()
    }
}
