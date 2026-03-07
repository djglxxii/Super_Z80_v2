find_package(SDL2)
include(TestBigEndian)

test_big_endian(SUPER_Z80_BIG_ENDIAN)

add_library(super_z80_z80ex
    vendor/z80ex/z80ex.c
)

if (SUPER_Z80_BIG_ENDIAN)
    target_compile_definitions(super_z80_z80ex
        PRIVATE
            WORDS_BIG_ENDIAN
    )
else ()
    target_compile_definitions(super_z80_z80ex
        PRIVATE
            WORDS_LITTLE_ENDIAN
    )
endif ()

target_compile_definitions(super_z80_z80ex
    PRIVATE
        Z80EX_API_REVISION=1
        Z80EX_VERSION_MAJOR=1
        Z80EX_VERSION_MINOR=19
        Z80EX_RELEASE_TYPE=""
        Z80EX_VERSION_STR=1.19
)

add_library(super_z80_crc32
    src/crc32.cpp
)

add_library(super_z80_memory
    src/memory.cpp
)

add_library(super_z80_io
    src/io.cpp
)

add_library(super_z80_irq_controller
    src/irq_controller.cpp
)

add_library(super_z80_vblank
    src/vblank.cpp
)

add_library(super_z80_vdp
    src/vdp.cpp
)

add_library(super_z80_bus
    src/bus.cpp
    src/dma.cpp
)

add_library(super_z80_cpu
    src/cpu.cpp
)

add_library(super_z80_scheduler
    src/scheduler.cpp
)

target_include_directories(super_z80_z80ex
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/z80ex/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/z80ex
)

target_include_directories(super_z80_crc32
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(super_z80_memory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(super_z80_io
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(super_z80_irq_controller
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(super_z80_vblank
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(super_z80_vdp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(super_z80_bus
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(super_z80_cpu
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(super_z80_scheduler
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(super_z80_bus
    PUBLIC
        super_z80_io
        super_z80_irq_controller
        super_z80_memory
        super_z80_vdp
        super_z80_vblank
)

target_link_libraries(super_z80_cpu
    PUBLIC
        super_z80_bus
    PRIVATE
        super_z80_z80ex
)

target_link_libraries(super_z80_scheduler
    PUBLIC
        super_z80_cpu
        super_z80_vdp
        super_z80_vblank
)

add_library(super_z80_core
    src/emulator_core.cpp
)

target_include_directories(super_z80_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(super_z80
    src/main.cpp
)

target_link_libraries(super_z80
    PRIVATE
        super_z80_core
)

add_executable(super_z80_test_crc32
    tests/unit/test_crc32.cpp
)

add_executable(super_z80_test_memory
    tests/unit/test_memory.cpp
)

add_executable(super_z80_test_io
    tests/unit/test_io.cpp
)

add_executable(super_z80_test_bus
    tests/unit/test_bus.cpp
)

add_executable(super_z80_test_irq
    tests/unit/test_irq.cpp
)

add_executable(super_z80_test_cpu
    tests/unit/test_cpu.cpp
)

add_executable(super_z80_test_scheduler
    tests/unit/test_scheduler.cpp
)

add_executable(super_z80_test_vblank
    tests/unit/test_vblank.cpp
)

add_executable(super_z80_test_dma
    tests/unit/test_dma.cpp
)

add_executable(super_z80_test_vdp
    tests/unit/test_vdp.cpp
)

add_executable(super_z80_test_tile_renderer
    tests/unit/test_tile_renderer.cpp
)

add_executable(super_z80_test_sprite_renderer
    tests/unit/test_sprite_renderer.cpp
)

add_executable(super_z80_test_palette
    tests/unit/test_palette.cpp
)

add_executable(super_z80_test_scrolling
    tests/unit/test_scrolling.cpp
)

add_executable(super_z80_test_foreground_plane
    tests/unit/test_foreground_plane.cpp
)

add_executable(super_z80_test_sprite_priority
    tests/unit/test_sprite_priority.cpp
)

add_executable(super_z80_test_sprite_scanline_limits
    tests/unit/test_sprite_scanline_limits.cpp
)

add_executable(super_z80_test_sprite_flipping
    tests/unit/test_sprite_flipping.cpp
)

add_executable(super_z80_test_sprite_pattern_banks
    tests/unit/test_sprite_pattern_banks.cpp
)

add_executable(super_z80_test_sprite_collision
    tests/unit/test_sprite_collision.cpp
)

add_executable(super_z80_test_tile_pattern_banks
    tests/unit/test_tile_pattern_banks.cpp
)

target_link_libraries(super_z80_test_crc32
    PRIVATE
        super_z80_crc32
)

target_link_libraries(super_z80_test_memory
    PRIVATE
        super_z80_memory
)

target_link_libraries(super_z80_test_io
    PUBLIC
        super_z80_io
)

target_link_libraries(super_z80_test_bus
    PRIVATE
        super_z80_bus
)

target_link_libraries(super_z80_test_irq
    PRIVATE
        super_z80_bus
        super_z80_cpu
        super_z80_irq_controller
)

target_link_libraries(super_z80_test_cpu
    PRIVATE
        super_z80_cpu
)

target_link_libraries(super_z80_test_scheduler
    PRIVATE
        super_z80_scheduler
)

target_link_libraries(super_z80_test_vblank
    PRIVATE
        super_z80_bus
        super_z80_vblank
)

target_link_libraries(super_z80_test_dma
    PRIVATE
        super_z80_scheduler
)

target_link_libraries(super_z80_test_vdp
    PRIVATE
        super_z80_bus
        super_z80_vdp
)

target_link_libraries(super_z80_test_tile_renderer
    PRIVATE
        super_z80_vdp
)

target_link_libraries(super_z80_test_sprite_renderer
    PRIVATE
        super_z80_vdp
)

target_link_libraries(super_z80_test_palette
    PRIVATE
        super_z80_vdp
)

target_link_libraries(super_z80_test_scrolling
    PRIVATE
        super_z80_bus
        super_z80_vdp
)

target_link_libraries(super_z80_test_foreground_plane
    PRIVATE
        super_z80_bus
        super_z80_vdp
)

target_link_libraries(super_z80_test_sprite_priority
    PRIVATE
        super_z80_vdp
)

target_link_libraries(super_z80_test_sprite_scanline_limits
    PRIVATE
        super_z80_vdp
)

target_link_libraries(super_z80_test_sprite_flipping
    PRIVATE
        super_z80_vdp
)

target_link_libraries(super_z80_test_sprite_pattern_banks
    PRIVATE
        super_z80_vdp
)

target_link_libraries(super_z80_test_sprite_collision
    PRIVATE
        super_z80_vdp
)

target_link_libraries(super_z80_test_tile_pattern_banks
    PRIVATE
        super_z80_vdp
)

enable_testing()
add_test(NAME super_z80_test_crc32 COMMAND super_z80_test_crc32)
add_test(NAME super_z80_test_memory COMMAND super_z80_test_memory)
add_test(NAME super_z80_test_io COMMAND super_z80_test_io)
add_test(NAME super_z80_test_bus COMMAND super_z80_test_bus)
add_test(NAME super_z80_test_irq COMMAND super_z80_test_irq)
add_test(NAME super_z80_test_cpu COMMAND super_z80_test_cpu)
add_test(NAME super_z80_test_scheduler COMMAND super_z80_test_scheduler)
add_test(NAME super_z80_test_vblank COMMAND super_z80_test_vblank)
add_test(NAME super_z80_test_dma COMMAND super_z80_test_dma)
add_test(NAME super_z80_test_vdp COMMAND super_z80_test_vdp)
add_test(NAME super_z80_test_tile_renderer COMMAND super_z80_test_tile_renderer)
add_test(NAME super_z80_test_sprite_renderer COMMAND super_z80_test_sprite_renderer)
add_test(NAME super_z80_test_palette COMMAND super_z80_test_palette)
add_test(NAME super_z80_test_scrolling COMMAND super_z80_test_scrolling)
add_test(NAME super_z80_test_foreground_plane COMMAND super_z80_test_foreground_plane)
add_test(NAME super_z80_test_sprite_priority COMMAND super_z80_test_sprite_priority)
add_test(NAME super_z80_test_sprite_scanline_limits COMMAND super_z80_test_sprite_scanline_limits)
add_test(NAME super_z80_test_sprite_flipping COMMAND super_z80_test_sprite_flipping)
add_test(NAME super_z80_test_sprite_pattern_banks COMMAND super_z80_test_sprite_pattern_banks)
add_test(NAME super_z80_test_sprite_collision COMMAND super_z80_test_sprite_collision)
add_test(NAME super_z80_test_tile_pattern_banks COMMAND super_z80_test_tile_pattern_banks)

# Keep runtime path simple for milestone verification commands.
set_target_properties(super_z80 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
