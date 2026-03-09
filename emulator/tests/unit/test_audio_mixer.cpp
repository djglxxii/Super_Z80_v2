#include "audio_mixer.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

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

bool expect_true(const char* name, bool value) {
    if (!value) {
        std::cerr << "[FAIL] " << name << " expected=true actual=false" << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=true" << std::endl;
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

std::vector<int16_t> collect_mixed_sequence(superz80::AudioMixer& mixer,
                                            TestSampleSource& psg,
                                            TestSampleSource& ym,
                                            const std::vector<int16_t>& psg_samples,
                                            const std::vector<int16_t>& ym_samples) {
    std::vector<int16_t> mixed;
    mixed.reserve(psg_samples.size());
    for (std::size_t index = 0U; index < psg_samples.size(); ++index) {
        psg.set_sample(psg_samples[index]);
        ym.set_sample(ym_samples[index]);
        mixer.tick();
        mixed.push_back(mixer.current_sample());
    }

    return mixed;
}

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

    const std::vector<int16_t> psg_sequence = {0, 1024, -4096, 30000, -30000, 777};
    const std::vector<int16_t> ym_sequence = {0, -512, 2048, 10000, -10000, -777};
    const std::vector<int16_t> mixed_a =
        collect_mixed_sequence(mixer, psg, ym, psg_sequence, ym_sequence);
    const std::vector<int16_t> mixed_b =
        collect_mixed_sequence(mixer, psg, ym, psg_sequence, ym_sequence);
    ok = expect_true("scripted-mixed-sequence-is-exactly-repeatable", mixed_a == mixed_b) && ok;
    ok = expect_true("scripted-mixed-sequence-preserves-silent-baseline", mixed_a.front() == 0) && ok;

    return ok ? 0 : 1;
}
