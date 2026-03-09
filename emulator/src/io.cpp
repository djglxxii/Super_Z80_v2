#include "io.h"

namespace superz80 {

IO::IO() {
    reset();
}

void IO::reset() {
    ports_.fill(0x00U);
    pad1_ = ControllerState{};
}

uint8_t IO::read(uint8_t port) const {
    if (port == kPad1Port || port == kPad1SysPort) {
        return read_controller_port(port);
    }

    return ports_[port];
}

void IO::write(uint8_t port, uint8_t value) {
    if (port == kPad1Port || port == kPad1SysPort) {
        return;
    }

    ports_[port] = value;
}

void IO::set_button(Button button, bool pressed) {
    switch (button) {
    case Button::Up:
        pad1_.up = pressed;
        break;
    case Button::Down:
        pad1_.down = pressed;
        break;
    case Button::Left:
        pad1_.left = pressed;
        break;
    case Button::Right:
        pad1_.right = pressed;
        break;
    case Button::A:
        pad1_.a = pressed;
        break;
    case Button::B:
        pad1_.b = pressed;
        break;
    case Button::Start:
        pad1_.start = pressed;
        break;
    }
}

bool IO::button(Button button) const {
    switch (button) {
    case Button::Up:
        return pad1_.up;
    case Button::Down:
        return pad1_.down;
    case Button::Left:
        return pad1_.left;
    case Button::Right:
        return pad1_.right;
    case Button::A:
        return pad1_.a;
    case Button::B:
        return pad1_.b;
    case Button::Start:
        return pad1_.start;
    }

    return false;
}

uint8_t IO::read_controller_port(uint8_t port) const {
    if (port == kPad1Port) {
        uint8_t value = kPad1IdleValue;
        if (pad1_.up) {
            value &= static_cast<uint8_t>(~0x01U);
        }
        if (pad1_.down) {
            value &= static_cast<uint8_t>(~0x02U);
        }
        if (pad1_.left) {
            value &= static_cast<uint8_t>(~0x04U);
        }
        if (pad1_.right) {
            value &= static_cast<uint8_t>(~0x08U);
        }
        if (pad1_.a) {
            value &= static_cast<uint8_t>(~0x10U);
        }
        if (pad1_.b) {
            value &= static_cast<uint8_t>(~0x20U);
        }
        return value;
    }

    uint8_t value = kPad1SysIdleValue;
    if (pad1_.start) {
        value &= static_cast<uint8_t>(~0x01U);
    }
    return value;
}

} // namespace superz80
