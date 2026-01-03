================================================================================
    Kindle DRM Converter - Windows Build Instructions
================================================================================

PROBLEM: deploy-windows.bat closes immediately?
SOLUTION: You need to BUILD FIRST, then DEPLOY.

================================================================================
    EASIEST METHOD - Just run this:
================================================================================

    1. Double-click: start-here.bat

       This script does EVERYTHING automatically:
       - Checks if Qt is installed
       - Builds the application
       - Creates standalone package
       - Guides you through each step

================================================================================
    STEP-BY-STEP MANUAL METHOD
================================================================================

STEP 1: Install Prerequisites
------------------------------

You need these installed BEFORE building:

1. Qt Framework 6.x (with WebEngine)
   Download: https://www.qt.io/download-qt-installer

   When installing, SELECT:
   ✓ Qt 6.x (latest version)
   ✓ Qt WebEngine
   ✓ MinGW 11.2.0 64-bit
   ✓ CMake

2. Calibre (for ebook conversion)
   Download: https://calibre-ebook.com/download


STEP 2: Check Your Installation
--------------------------------

Open Command Prompt and run:

    diagnose.bat

This will tell you if everything is installed correctly.


STEP 3: Build the Application
------------------------------

Run:

    build.bat

This will:
- Configure with CMake
- Compile the C++ code
- Create KindleDRMConverter.exe

Wait 5-10 minutes for first build.


STEP 4: Create Standalone Package
----------------------------------

NOW you can run:

    deploy-windows.bat

This will:
- Copy the EXE
- Bundle all Qt DLLs
- Create KindleDRMConverter-Windows folder

This folder can be zipped and distributed!


STEP 5 (Optional): Create Installer
------------------------------------

To create a professional installer:

1. Install NSIS: https://nsis.sourceforge.io/
2. Run: create-installer.bat
3. Get: KindleDRMConverter-Setup.exe

================================================================================
    WHY DOES deploy-windows.bat CLOSE?
================================================================================

The script closes because it can't find:
    build-Release\bin\KindleDRMConverter.exe

This means you need to run build.bat FIRST!

To see the actual error:
1. Open Command Prompt
2. Navigate to qt-version folder
3. Run: deploy-windows.bat
4. Read the error message

Or just run: diagnose.bat

================================================================================
    TROUBLESHOOTING
================================================================================

Problem: "Qt not found"
Fix: Install Qt and add to PATH:
     set PATH=%PATH%;C:\Qt\6.5.0\mingw_64\bin

Problem: "CMake not found"
Fix: Install CMake from https://cmake.org/download/

Problem: Build fails
Fix: Make sure Qt WebEngine is installed
     Run Qt Maintenance Tool and add it

Problem: EXE crashes
Fix: Run windeployqt or use deploy-windows.bat

Problem: Calibre not found
Fix: Install Calibre and add to PATH:
     set PATH=%PATH%;C:\Program Files\Calibre2

================================================================================
    FILE STRUCTURE AFTER BUILD
================================================================================

After successful build and deploy:

qt-version\
├── build-Release\
│   └── bin\
│       └── KindleDRMConverter.exe  ← Needs Qt DLLs
│
└── KindleDRMConverter-Windows\     ← DISTRIBUTE THIS!
    ├── KindleDRMConverter.exe      ← Ready to run
    ├── Qt6Core.dll                 ← All dependencies included
    ├── Qt6Network.dll
    ├── Qt6WebEngine*.dll
    ├── [many other DLLs]
    ├── config.yaml.example
    └── README.md

================================================================================
    QUICK REFERENCE
================================================================================

Command              Purpose
-----------------    --------------------------------------------------
start-here.bat       Automated build process (RECOMMENDED)
diagnose.bat         Check what's wrong
build.bat            Build the application
deploy-windows.bat   Create standalone package (AFTER building!)
create-installer.bat Create Windows installer (optional)

================================================================================
    WHAT TO DO NEXT
================================================================================

After building:

1. Go into: KindleDRMConverter-Windows\
2. Copy: config.yaml.example → config.yaml
3. Edit config.yaml with your Amazon credentials
4. Run: KindleDRMConverter.bat

Or from command line:
    KindleDRMConverter.exe --all

================================================================================
    NEED MORE HELP?
================================================================================

Full documentation:
- BUILD_WINDOWS_EXE.md - Complete build guide
- TROUBLESHOOTING_WINDOWS.md - Common problems and solutions
- WINDOWS_QUICKSTART.md - Quick reference

Online:
https://github.com/Zachman22/Kindle-downloader-and-drm-removel-

================================================================================

TL;DR: Run start-here.bat and it will do everything for you!

================================================================================
