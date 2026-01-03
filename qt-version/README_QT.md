# Kindle DRM Converter - Qt C++ Version

A high-performance C++ implementation using the Qt Framework to download Kindle books, remove DRM protection, and convert them to all popular ebook formats.

## Features

- **Qt-Powered Performance**: Native C++ implementation with Qt's powerful libraries
- **Cross-Platform**: Works on Linux, macOS, and Windows
- **Qt WebEngine**: Automated Amazon login and book downloading
- **DRM Removal**: Remove DRM from AZW, AZW3, MOBI, KFX formats
- **Multi-Format Conversion**: Convert to EPUB, PDF, MOBI, AZW3, TXT, HTML, RTF, ODT, DOCX, and more
- **Qt Cryptography**: Built-in encryption/decryption support
- **Asynchronous Operations**: Non-blocking I/O with Qt's signal/slot mechanism

## Prerequisites

### Required Software

1. **Qt Framework 5.15+ or Qt 6.x**
   ```bash
   # Linux (Ubuntu/Debian)
   sudo apt-get install qt6-base-dev qt6-webengine-dev

   # macOS
   brew install qt@6

   # Windows
   # Download Qt from https://www.qt.io/download-qt-installer
   ```

2. **CMake 3.16+**
   ```bash
   # Linux
   sudo apt-get install cmake

   # macOS
   brew install cmake

   # Windows
   # Download from https://cmake.org/download/
   ```

3. **C++ Compiler**
   - GCC 7+ or Clang 6+ (Linux/macOS)
   - MSVC 2019+ (Windows)

4. **Calibre** (for ebook conversion)
   ```bash
   # Linux
   sudo apt-get install calibre

   # macOS
   brew install calibre

   # Windows
   # Download from https://calibre-ebook.com/download
   ```

## Build Instructions

### Linux / macOS

```bash
# Clone the repository
git clone <repository-url>
cd Kindle-downloader-and-drm-removel-/qt-version

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
make -j$(nproc)

# Install (optional)
sudo make install

# Run
./KindleDRMConverter --help
```

### Windows

```bash
# Open Qt Command Prompt or use CMake GUI

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -G "Visual Studio 16 2019"

# Build
cmake --build . --config Release

# Run
Release\KindleDRMConverter.exe --help
```

### Alternative: Qt Creator

1. Open `CMakeLists.txt` in Qt Creator
2. Configure the project with your Qt kit
3. Build (Ctrl+B / Cmd+B)
4. Run (Ctrl+R / Cmd+R)

## Configuration

1. Copy the example config:
```bash
cp config.yaml.example config.yaml
```

2. Edit `config.yaml` with your Amazon credentials:
```yaml
amazon:
  email: "your-email@example.com"
  password: "your-password"
  region: "US"

directories:
  download: "./downloads"
  output: "./output"
  temp: "./temp"

drm:
  kindle_serial: ""  # Optional but recommended
  pid: ""

conversion:
  formats:
    - epub
    - pdf
    - mobi
  quality: high
```

## Usage

### Basic Usage

```bash
# Process all your Kindle books
./KindleDRMConverter --all

# Download only (no DRM removal or conversion)
./KindleDRMConverter --download-only

# Process existing Kindle files
./KindleDRMConverter --input /path/to/kindle/files

# Convert to specific formats
./KindleDRMConverter --all --formats epub,pdf,mobi

# Process specific book
./KindleDRMConverter --book "Book Title"
```

### Advanced Options

```bash
./KindleDRMConverter --help

Options:
  -a, --all                  Process all Kindle books
  -d, --download-only        Only download books
  -i, --input <path>         Process existing files
  -o, --output <path>        Output directory
  -f, --formats <formats>    Output formats (comma-separated)
  -b, --book <title>         Process specific book
  --batch-size <size>        Batch processing size (default: 3)
  -c, --config <path>        Config file path (default: config.yaml)
  --keep-drm                 Keep original DRM files
  --interactive              Interactive mode (for 2FA)
  -v, --verbose             Verbose logging
  --use-calibre-drm         Use Calibre for DRM removal
```

## Architecture

### Qt Modules Used

- **Qt Core**: Foundation classes, file I/O, event loop
- **Qt Network**: HTTP requests, network access
- **Qt WebEngine**: Web browser for Amazon authentication
- **Qt Concurrent**: Parallel processing
- **Qt Cryptography**: Encryption/decryption (via QCA if available)

### Class Structure

```
ConfigManager     - YAML config parser and settings manager
KindleDownloader  - Amazon authentication and book downloading
                   (Qt WebEngine, Qt Network)
DRMRemover        - DRM decryption and removal
                   (Qt Cryptography, custom algorithms)
EbookConverter    - Multi-format conversion
                   (Qt Process, Calibre integration)
```

### Signal/Slot Communication

The application uses Qt's signal/slot mechanism for asynchronous operations:

```cpp
// Progress updates
emit progressUpdate("Downloading book...");

// Login events
emit loginSuccess();
emit loginFailed("Error message");

// Conversion events
emit conversionComplete(inputFile, format, outputFile);
```

## Performance

Qt C++ version offers significant performance improvements over Python:

- **Faster startup time**: Native binary vs interpreted Python
- **Lower memory usage**: Efficient memory management
- **Better concurrency**: Qt's thread pool and event loop
- **Native GUI potential**: Can easily add Qt Widgets GUI later

## Development

### Adding Features

1. **Add new conversion format**: Extend `EbookConverter::getFormatOptions()`
2. **Custom DRM algorithm**: Implement in `DRMRemover::decryptMobi()`
3. **Add GUI**: Create Qt Widgets or QML interface

### Building in Debug Mode

```bash
mkdir build-debug && cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Running Tests (Future)

```bash
# Tests can be added using Qt Test framework
ctest
```

## Troubleshooting

### Qt WebEngine Issues

If Qt WebEngine is not found:
```bash
# Linux
sudo apt-get install qt6-webengine-dev libqt6webenginewidgets6

# macOS
brew install qt6 --with-webengine
```

### Missing Libraries

```bash
# Check Qt installation
qmake --version

# Find Qt libraries
cmake --find-package -DNAME=Qt6 -DCOMPILER_ID=GNU
```

### Calibre Not Found

Ensure Calibre binaries are in your PATH:
```bash
export PATH=$PATH:/opt/calibre/bin  # Linux
# or
export PATH=$PATH:/Applications/calibre.app/Contents/MacOS  # macOS
```

## Comparison with Python Version

| Feature | Qt C++ | Python |
|---------|---------|--------|
| Performance | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |
| Memory Usage | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |
| Startup Time | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |
| Cross-Platform | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| Development Speed | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| GUI Support | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |

## Future Enhancements

- [ ] Qt Widgets GUI application
- [ ] QML/Qt Quick modern UI
- [ ] Qt Test framework integration
- [ ] OpenSSL integration for advanced DRM
- [ ] Qt Charts for statistics
- [ ] Multi-language support (Qt Linguist)
- [ ] System tray integration
- [ ] Drag & drop file support

## Contributing

See [CONTRIBUTING.md](../CONTRIBUTING.md) for guidelines.

## License

MIT License - See [LICENSE](../LICENSE) for details

## Acknowledgments

- Qt Framework for excellent C++ libraries
- Calibre for ebook conversion
- DeDRM community for documentation
- Original Python implementation

---

**Note**: This is the Qt C++ version of the Kindle DRM Converter. For the Python version, see the main [README.md](../README.md).
