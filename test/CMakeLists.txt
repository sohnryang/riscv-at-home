include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG v1.14.0)
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_library(test_utils INTERFACE test_utils.h)
target_link_libraries(test_utils INTERFACE GTest::gtest_main)

add_executable(alu_test alu.cc)
target_link_libraries(alu_test PRIVATE GTest::gtest_main)
gtest_discover_tests(alu_test)

add_executable(regfile_test regfile.cc)
target_link_libraries(regfile_test PRIVATE GTest::gtest_main)
gtest_discover_tests(regfile_test)

add_executable(imem_test imem.cc)
target_link_libraries(imem_test PRIVATE GTest::gtest_main)
gtest_discover_tests(imem_test)
