import SwiftUI

struct ContentView: View {
    let sender = CommandSender()

    var body: some View {
        VStack(spacing: 30) {

            PressButton(
                title: "前",
                command: "forward",
                sender: sender
            )

            HStack(spacing: 40) {
                PressButton(
                    title: "左",
                    command: "turnleft",
                    sender: sender
                )

                PressButton(
                    title: "右",
                    command: "turnright",
                    sender: sender
                )
            }

            PressButton(
                title: "後ろ",
                command: "backward",
                sender: sender
            )
        }
        .padding()
    }
}

struct PressButton: View {
    let title: String
    let command: String
    let sender: CommandSender

    @State private var isPressed = false

    var body: some View {
        Text(title)
            .font(.title)
            .bold()
            .foregroundColor(.white)
            .frame(width: 130, height: 90)
            .background(isPressed ? Color.orange : Color.blue)
            .cornerRadius(20)
            .gesture(
                DragGesture(minimumDistance: 0)
                    .onChanged { _ in
                        if !isPressed {
                            isPressed = true
                            sender.sendCommand(command)
                        }
                    }
                    .onEnded { _ in
                        isPressed = false
                        sender.sendCommand("stop")
                    }
            )
    }
}
