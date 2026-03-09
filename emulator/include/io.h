#pragma once

#include <array>
#include <cstdint>

namespace superz80 {

class IO {
public:
    enum class Button : uint8_t {
        Up,
        Down,
        Left,
        Right,
        A,
        B,
        Start
    };

    static constexpr uint8_t kPad1Port = 0xDCU;
    static constexpr uint8_t kPad1SysPort = 0xDDU;
    static constexpr uint8_t kPad1IdleValue = 0xFFU;
    static constexpr uint8_t kPad1SysIdleValue = 0xFFU;

    IO();

    void reset();

    uint8_t read(uint8_t port) const;
    void write(uint8_t port, uint8_t value);
    void set_button(Button button, bool pressed);
    bool button(Button button) const;

private:
    struct ControllerState {
        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;
        bool a = false;
        bool b = false;
        bool start = false;
    };

    uint8_t read_controller_port(uint8_t port) const;

    std::array<uint8_t, 256> ports_;
    ControllerState pad1_;
};

} // namespace superz80
