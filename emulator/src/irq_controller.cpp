#include "irq_controller.h"

namespace superz80 {

IRQController::IRQController()
    : irq_status_(0x00U),
      irq_enable_(0x00U) {}

void IRQController::reset() {
    irq_status_ = 0x00U;
    irq_enable_ = 0x00U;
}

void IRQController::request(uint8_t irq_bit) {
    irq_status_ = static_cast<uint8_t>(irq_status_ | irq_bit);
}

uint8_t IRQController::status() const {
    return irq_status_;
}

uint8_t IRQController::enable() const {
    return irq_enable_;
}

void IRQController::set_enable(uint8_t mask) {
    irq_enable_ = mask;
}

void IRQController::acknowledge(uint8_t mask) {
    irq_status_ = static_cast<uint8_t>(irq_status_ & static_cast<uint8_t>(~mask));
}

bool IRQController::irq_line() const {
    return (irq_status_ & irq_enable_) != 0U;
}

IRQController::Snapshot IRQController::snapshot() const {
    return {irq_status_, irq_enable_};
}

void IRQController::restore(const Snapshot& snapshot) {
    irq_status_ = snapshot.status;
    irq_enable_ = snapshot.enable;
}

} // namespace superz80
