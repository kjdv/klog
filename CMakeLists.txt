cmake_minimum_required(VERSION 3.10)

project(klog)
set(CMAKE_CXX_STANDARD 17)

include(GoogleTest)
find_package(GTest MODULE REQUIRED)
find_library(GMOCK_LIBRARIES gmock)
enable_testing()


# dependencies:

find_package(fmt CONFIG REQUIRED)
find_package(Results CONFIG REQUIRED)
find_package(Kthread CONFIG REQUIRED)


add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(examples)

# install rules
install(TARGETS klog
        EXPORT KlogConfig
        ARCHIVE DESTINATION lib COMPONENT lib
        PUBLIC_HEADER DESTINATION include/klog COMPONENT dev
)

export(TARGETS klog NAMESPACE Klog:: FILE ${CMAKE_CURRENT_BINARY_DIR}/KlogConfig.cmake)
install(EXPORT KlogConfig DESTINATION share/klog NAMESPACE Klog::)
