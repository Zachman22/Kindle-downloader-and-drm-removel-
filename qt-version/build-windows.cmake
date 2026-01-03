# Windows-specific CMake configuration for creating standalone EXE

# Set Windows-specific options
if(WIN32)
    # Enable static linking for standalone EXE
    option(BUILD_STATIC "Build static executable" ON)

    if(BUILD_STATIC)
        set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a)
        set(BUILD_SHARED_LIBS OFF)

        # Static linking flags for MinGW
        if(MINGW)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
        endif()

        # For MSVC
        if(MSVC)
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        endif()
    endif()

    # Windows application (no console window for GUI)
    # For console app, use: set(CMAKE_WIN32_EXECUTABLE OFF)
    set(CMAKE_WIN32_EXECUTABLE OFF)  # Keep console for CLI app

    # Windows resource file (icon, version info)
    set(WINDOWS_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources/windows/app.rc")
    if(EXISTS ${WINDOWS_RESOURCES})
        target_sources(${PROJECT_NAME} PRIVATE ${WINDOWS_RESOURCES})
    endif()

    # Set executable icon
    set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/windows/app.ico")

    # Version information
    set(VERSION_MAJOR 1)
    set(VERSION_MINOR 0)
    set(VERSION_PATCH 0)
    set(VERSION_BUILD 0)

    # Company information
    set(COMPANY_NAME "Kindle DRM Converter")
    set(FILE_DESCRIPTION "Kindle DRM Converter - Download, Remove DRM, Convert")
    set(PRODUCT_NAME "Kindle DRM Converter")
    set(COPYRIGHT "Copyright (C) 2024")
endif()

# Qt deployment configuration
if(WIN32)
    # Find windeployqt
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin ${Qt5_DIR}/../../../bin)

    if(WINDEPLOYQT_EXECUTABLE)
        message(STATUS "Found windeployqt: ${WINDEPLOYQT_EXECUTABLE}")

        # Add custom target to run windeployqt after build
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE}
                --no-translations
                --no-system-d3d-compiler
                --no-opengl-sw
                --no-compiler-runtime
                $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Running windeployqt..."
        )
    else()
        message(WARNING "windeployqt not found - manual deployment required")
    endif()
endif()
