# Windows EXE - Quick Start Guide

## For Users (Pre-built EXE)

If someone gave you the EXE or you downloaded a release:

### Step 1: Extract
```
KindleDRMConverter-Windows.zip
    └── Extract to a folder
```

### Step 2: Install Calibre
Download and install from: **https://calibre-ebook.com/download**

### Step 3: Configure
1. Open `config.yaml.example`
2. Save as `config.yaml`
3. Edit with your Amazon credentials:
   ```yaml
   amazon:
     email: "your-email@example.com"
     password: "your-password"
   ```

### Step 4: Run
Double-click: **`KindleDRMConverter.bat`**

Or from Command Prompt:
```batch
KindleDRMConverter.exe --all
```

✅ Done! Your books will be in the `output` folder.

---

## For Developers (Building EXE)

### Prerequisites
- Windows 10/11
- Qt 6.x with WebEngine
- CMake
- MinGW or Visual Studio

### Quick Build
```batch
# 1. Clone repository
git clone <repo-url>
cd qt-version

# 2. Build
build.bat

# 3. Create standalone package
deploy-windows.bat

# 4. (Optional) Create installer
create-installer.bat
```

### Result
- **build-Release\bin\KindleDRMConverter.exe** - Executable (needs Qt DLLs)
- **KindleDRMConverter-Windows\** - Standalone folder (distribute this!)
- **KindleDRMConverter-Setup.exe** - Windows installer

---

## Common Issues

### ❌ "VCRUNTIME140.dll not found"
**Fix**: Install Visual C++ Redistributable
- Download: https://aka.ms/vs/17/release/vc_redist.x64.exe

### ❌ "Qt6Core.dll not found"
**Fix**: Use the deployed version in `KindleDRMConverter-Windows` folder

### ❌ "Calibre not found"
**Fix**: Install Calibre and make sure it's in PATH
```batch
set PATH=%PATH%;C:\Program Files\Calibre2
```

### ❌ Login fails
**Fix**: Use interactive mode for 2FA:
```batch
KindleDRMConverter.exe --all --interactive
```

---

## Usage Examples

```batch
# Download all books and convert to all formats
KindleDRMConverter.exe --all

# Download only (no conversion)
KindleDRMConverter.exe --download-only

# Process existing Kindle files
KindleDRMConverter.exe --input "C:\Users\YourName\Documents\My Kindle Content"

# Convert to specific formats only
KindleDRMConverter.exe --all --formats epub,pdf,mobi

# Process one specific book
KindleDRMConverter.exe --book "Harry Potter and the Sorcerer's Stone"

# Show help
KindleDRMConverter.exe --help
```

---

## Distribution Checklist

If you're distributing the EXE to others:

- [ ] Run `deploy-windows.bat` to create standalone package
- [ ] Test on a clean Windows machine (without Qt installed)
- [ ] Include `config.yaml.example`
- [ ] Include README or QUICKSTART.txt
- [ ] Mention Calibre requirement
- [ ] Zip the entire folder
- [ ] (Optional) Create installer with `create-installer.bat`

---

## File Structure

```
KindleDRMConverter-Windows/
├── KindleDRMConverter.exe      ← Main executable
├── KindleDRMConverter.bat      ← Easy launcher
├── config.yaml.example         ← Configuration template
├── README.md                   ← Documentation
├── QUICKSTART.txt              ← Quick instructions
├── Qt6Core.dll                 ← Qt libraries
├── Qt6Network.dll              │
├── Qt6WebEngine*.dll           │
├── [other Qt DLLs]            ├─ Auto-deployed
├── plugins/                    │
│   ├── platforms/             │
│   └── [other plugins]        │
├── downloads/                  ← Downloaded books
├── output/                     ← Converted books
├── temp/                       ← Temporary files
└── logs/                       ← Log files
```

---

## Advanced: Code Signing (Optional)

To avoid Windows SmartScreen warnings:

1. Get a code signing certificate
2. Sign the EXE:
   ```batch
   signtool sign /f certificate.pfx /p password /t http://timestamp.digicert.com KindleDRMConverter.exe
   ```

---

## Support

- 📖 Full documentation: `BUILD_WINDOWS_EXE.md`
- 🐛 Report issues: GitHub Issues
- 💬 Questions: GitHub Discussions

---

**Made with Qt Framework ❤️**
