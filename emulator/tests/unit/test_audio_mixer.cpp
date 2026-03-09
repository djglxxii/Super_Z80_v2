#include "audio_mixer.h"

#include <array>
#include <cstdint>
#include <iostream>

namespace {

bool expect_equal_i16(const char* name, int16_t actual, int16_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=" << expected
                  << " actual=" << actual << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=" << actual << std::endl;
    return true;
}

class TestSampleSource : public superz80::AudioSampleSource {
public:
    explicit TestSampleSource(int16_t sample) : sample_(sample) {}

    void set_sample(int16_t sample) {
        sample_ = sample;
    }

    int16_t current_sample() const override {
        return sample_;
    }

private:
    int16_t sample_;
};

} // namespace

int main() {
    bool ok = true;

    TestSampleSource psg(1000);
    TestSampleSource ym(2000);
    superz80::AudioMixer mixer(psg, ym);

    mixer.tick();
    ok = expect_equal_i16("simple-sum", mixer.current_sample(), 3000) && ok;

    psg.set_sample(-2000);
    ym.set_sample(500);
    mixer.tick();
    ok = expect_equal_i16("negative-mixing", mixer.current_sample(), -1500) && ok;

    psg.set_sample(30000);
    ym.set_sample(10000);
    mixer.tick();
    ok = expect_equal_i16("positive-clamp", mixer.current_sample(), 32767) && ok;

    psg.set_sample(-30000);
    ym.set_sample(-10000);
    mixer.tick();
    ok = expect_equal_i16("negative-clamp", mixer.current_sample(), -32768) && ok;

    std::array<int16_t, 4> repeat_a{};
    std::array<int16_t, 4> repeat_b{};
    psg.set_sample(2048);
    ym.set_sample(-512);
    for (std::size_t index = 0U; index < repeat_a.size(); ++index) {
        mixer.tick();
        repeat_a[index] = mixer.current_sample();
    }
    for (std::size_t index = 0U; index < repeat_b.size(); ++index) {
        mixer.tick();
        repeat_b[index] = mixer.current_sample();
    }
    ok = expect_equal_i16("deterministic-repeatability-first", repeat_a[0], repeat_b[0]) && ok;
    ok = expect_equal_i16("deterministic-repeatability-second", repeat_a[1], repeat_b[1]) && ok;
    ok = expect_equal_i16("deterministic-repeatability-third", repeat_a[2], repeat_b[2]) && ok;
    ok = expect_equal_i16("deterministic-repeatability-fourth", repeat_a[3], repeat_b[3]) && ok;

    return ok ? 0 : 1;
}
