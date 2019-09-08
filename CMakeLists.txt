cmake_minimum_required(VERSION 3.10)

project(klog)
set(CMAKE_CXX_STANDARD 17)

include(GoogleTest)
find_package(GTest MODULE REQUIRED)
find_library(GMOCK_LIBRARIES gmock)
enable_testing()


# dependencies:
find_package(benchmark CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Kthread CONFIG REQUIRED)


add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(examples)

# install rules
install(TARGETS klog
        EXPORT KlogTargets
        ARCHIVE DESTINATION lib COMPONENT lib
        PUBLIC_HEADER DESTINATION include/klog COMPONENT dev
)
install(FILES KlogConfig.cmake DESTINATION share/klog)

install(EXPORT KlogTargets DESTINATION share/klog NAMESPACE Klog::)
export(TARGETS klog NAMESPACE Klog:: FILE ${CMAKE_CURRENT_BINARY_DIR}/KlogTargets.cmake)
