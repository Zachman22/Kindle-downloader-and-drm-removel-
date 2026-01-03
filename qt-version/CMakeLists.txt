cmake_minimum_required(VERSION 3.16)

project(KindleDRMConverter VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt packages
find_package(Qt6 COMPONENTS
    Core
    Network
    WebEngineCore
    WebEngineWidgets
    Concurrent
    REQUIRED
)

# If Qt6 is not found, try Qt5
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 COMPONENTS
        Core
        Network
        WebEngine
        WebEngineWidgets
        Concurrent
        REQUIRED
    )
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/ConfigManager.cpp
    src/KindleDownloader.cpp
    src/DRMRemover.cpp
    src/EbookConverter.cpp
)

set(HEADERS
    src/ConfigManager.h
    src/KindleDownloader.h
    src/DRMRemover.h
    src/EbookConverter.h
)

# Windows-specific configuration
if(WIN32)
    # Include Windows build config
    include(build-windows.cmake)
endif()

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Link Qt libraries
if(Qt6_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Network
        Qt6::WebEngineCore
        Qt6::WebEngineWidgets
        Qt6::Concurrent
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core
        Qt5::Network
        Qt5::WebEngine
        Qt5::WebEngineWidgets
        Qt5::Concurrent
    )
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Copy config example
install(FILES config.yaml.example
    DESTINATION share/${PROJECT_NAME}
)

# Create output directory structure
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/downloads)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/output)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/temp)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/logs)

# Print build configuration
message(STATUS "")
message(STATUS "======================================")
message(STATUS "Kindle DRM Converter Configuration")
message(STATUS "======================================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
if(Qt6_FOUND)
    message(STATUS "Qt Version: 6.x")
else()
    message(STATUS "Qt Version: 5.x")
endif()
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "======================================")
message(STATUS "")
