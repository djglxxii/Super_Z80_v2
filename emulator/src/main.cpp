#include "emulator_core.h"

#include <iostream>
#include <string>

namespace {

void print_header() {
    std::cout << "Super_Z80_v2 Emulator" << std::endl;
}

void print_help() {
    print_header();
    std::cout << "Usage: super_z80 [--help] [--version]" << std::endl;
    std::cout << "  --help     Show this help message" << std::endl;
    std::cout << "  --version  Show version information" << std::endl;
}

void print_version() {
    print_header();
    std::cout << "Version: dev" << std::endl;
}

} // namespace

int main(int argc, char* argv[]) {
    EmulatorCore core;
    core.initialize();

    if (argc <= 1) {
        print_help();
        return 0;
    }

    const std::string arg = argv[1];
    if (arg == "--help") {
        print_help();
        return 0;
    }

    if (arg == "--version") {
        print_version();
        return 0;
    }

    std::cerr << "Unknown argument: " << arg << std::endl;
    print_help();
    return 1;
}
