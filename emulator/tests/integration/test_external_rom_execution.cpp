#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

bool expect_true(const char* name, bool condition) {
    if (!condition) {
        std::cerr << "[FAIL] " << name << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << std::endl;
    return true;
}

bool write_file(const std::string& path, const std::vector<uint8_t>& bytes) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    return static_cast<bool>(file);
}

std::string shell_escape(const std::string& text) {
    std::string escaped = "'";
    for (char ch : text) {
        if (ch == '\'') {
            escaped += "'\\''";
        } else {
            escaped += ch;
        }
    }
    escaped += "'";
    return escaped;
}

bool run_command_capture(const std::string& command, std::string& output) {
    std::array<char, 256> buffer = {};
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe == nullptr) {
        return false;
    }

    output.clear();
    while (std::fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        output += buffer.data();
    }

    const int status = pclose(pipe);
    return status == 0;
}

} // namespace

int main() {
    bool ok = true;

    const std::string binary_path = SUPER_Z80_BINARY_PATH;
    const std::string temp_root = SUPER_Z80_TEST_TEMP_DIR;
    const std::string rom_path = temp_root + "/m29h_external_rom.bin";
    const std::string missing_path = temp_root + "/does_not_exist.bin";

    const std::vector<uint8_t> rom = {
        0x3AU, 0x00U, 0xC0U, // LD A,(0xC000)
        0x3CU,               // INC A
        0x32U, 0x00U, 0xC0U, // LD (0xC000),A
        0x3AU, 0x01U, 0xC0U, // LD A,(0xC001)
        0x3CU,               // INC A
        0x3CU,               // INC A
        0x32U, 0x01U, 0xC0U, // LD (0xC001),A
        0xC3U, 0x00U, 0x00U  // JP 0x0000
    };

    ok = expect_true("write-test-rom", write_file(rom_path, rom)) && ok;

    const std::string base_command =
        shell_escape(binary_path) + " --rom " + shell_escape(rom_path) + " --headless --frames 2 2>&1";
    std::string first_output;
    std::string second_output;
    ok = expect_true("first-headless-run-succeeds", run_command_capture(base_command, first_output)) && ok;
    ok = expect_true("second-headless-run-succeeds", run_command_capture(base_command, second_output)) && ok;
    ok = expect_true("headless-run-output-is-deterministic", first_output == second_output) && ok;
    ok = expect_true("headless-run-reports-summary",
                     first_output.find("HEADLESS_ROM_RESULT") != std::string::npos) && ok;

    const std::string missing_command =
        shell_escape(binary_path) + " --rom " + shell_escape(missing_path) + " --headless --frames 1 2>&1";
    std::string missing_output;
    const bool missing_ok = run_command_capture(missing_command, missing_output);
    ok = expect_true("missing-rom-fails", !missing_ok) && ok;
    ok = expect_true("missing-rom-error-is-reported",
                     missing_output.find("Unable to open ROM file") != std::string::npos) && ok;

    std::remove(rom_path.c_str());

    return ok ? 0 : 1;
}
