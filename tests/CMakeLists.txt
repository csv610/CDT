# No project() call here, it's a subdirectory

# Add all test source files
file(GLOB TEST_SOURCES "*.cpp")

# Create the test executable
add_executable(run_tests 
    ${TEST_SOURCES}
    ../src/Plc.cpp
    ../src/GiftWrap.cpp
    ../src/Delaunay.cpp
    ../src/Cdt.cpp
)

# Inherit includes and settings
target_include_directories(run_tests PRIVATE
    ../src
    ../include
    .
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(run_tests PUBLIC "/fp:strict" "/Oi" "/D _CRT_SECURE_NO_WARNINGS")
else()
    target_compile_options(run_tests PUBLIC -frounding-math)
    if(APPLE)
        target_link_options(run_tests PUBLIC "-Wl,-stack_size,0x808000")
    else()
        target_compile_options(run_tests PUBLIC "-Wl,-z,stacksize=8421376")
    endif()
endif()

# Enable testing
add_test(NAME AllTests COMMAND run_tests)
