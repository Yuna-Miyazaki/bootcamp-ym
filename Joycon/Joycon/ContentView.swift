import SwiftUI

struct ContentView: View {
    let sender = CommandSender()

    @State private var dragOffset = CGSize.zero

    var body: some View {
        VStack {

            Text("Joystick Control")
                .font(.title)

            ZStack {
                Circle()
                    .frame(width: 200, height: 200)
                    .opacity(0.2)

                Circle()
                    .frame(width: 60, height: 60)
                    .offset(dragOffset)
                    .gesture(
                        DragGesture()
                            .onChanged { value in
                                let dx = value.translation.width / 100
                                let dy = value.translation.height / 100

                                let x = max(min(dx, 1.0), -1.0)
                                let y = max(min(-dy, 1.0), -1.0)

                                dragOffset = value.translation

                                sender.sendJoy(forward: y, turn: x)
                            }
                            .onEnded { _ in
                                dragOffset = .zero
                                sender.sendJoy(forward: 0, turn: 0)
                            }
                    )
            }
        }
        .padding()
    }
}
