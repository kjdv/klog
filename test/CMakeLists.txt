file(GLOB sources *.c *.cc *.cpp *.h *.hh)

add_executable(klog_test ${sources})
target_link_libraries(klog_test klog ${GMOCK_LIBRARIES} GTest::GTest GTest::Main)

gtest_discover_tests(klog_test)
