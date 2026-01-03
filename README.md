# Kindle DRM Converter

A comprehensive tool to download Kindle books, remove DRM protection, and convert them to all popular ebook formats.

## Features

- **Kindle Book Downloader**: Download your Kindle books from Amazon
- **DRM Removal**: Remove DRM protection from Kindle books (AZW, AZW3, KFX formats)
- **Multi-Format Conversion**: Convert books to all major ebook formats:
  - EPUB
  - PDF
  - MOBI
  - AZW3
  - TXT
  - HTML
  - RTF
  - ODT
  - DOCX
- **Batch Processing**: Process multiple books at once
- **Automatic Processing**: Download → Remove DRM → Convert in one command

## Legal Notice

**IMPORTANT**: This tool is for educational purposes only. You should only use it with books you have legally purchased and own. DRM removal may violate the Terms of Service of ebook retailers and may be illegal in some jurisdictions. Use at your own risk.

## Prerequisites

- Python 3.8 or higher
- Calibre (for ebook conversion)
- Valid Amazon Kindle account

## Installation

1. Clone this repository:
```bash
git clone https://github.com/yourusername/Kindle-downloader-and-drm-removel-.git
cd Kindle-downloader-and-drm-removel-
```

2. Install Python dependencies:
```bash
pip install -r requirements.txt
```

3. Install Calibre:
   - **Linux**: `sudo apt-get install calibre`
   - **macOS**: `brew install calibre`
   - **Windows**: Download from [calibre-ebook.com](https://calibre-ebook.com/download)

4. Copy the example config and edit with your credentials:
```bash
cp config.yaml.example config.yaml
# Edit config.yaml with your Amazon credentials and preferences
```

## Configuration

Edit `config.yaml` with your settings:

```yaml
amazon:
  email: "your-email@example.com"
  password: "your-password"
  region: "US"  # US, UK, DE, FR, etc.

directories:
  download: "./downloads"
  output: "./output"
  temp: "./temp"

conversion:
  formats:
    - epub
    - pdf
    - mobi
    - txt
  quality: high

drm:
  kindle_serial: ""  # Your Kindle device serial number (optional)
  pid: ""  # Your Kindle PID (optional)
```

## Usage

### Basic Usage

Process all your Kindle books (download, remove DRM, convert to all formats):
```bash
python main.py --all
```

### Download Only

Download your Kindle books without DRM removal or conversion:
```bash
python main.py --download-only
```

### Process Existing Files

Remove DRM and convert existing Kindle files:
```bash
python main.py --input /path/to/kindle/files
```

### Specify Output Formats

Convert to specific formats only:
```bash
python main.py --formats epub,pdf,mobi
```

### Process Single Book

Process a specific book by title:
```bash
python main.py --book "Book Title"
```

### Advanced Options

```bash
python main.py --help

Options:
  --all                 Download, remove DRM, and convert all Kindle books
  --download-only       Only download books without processing
  --input PATH          Process existing Kindle files from PATH
  --output PATH         Output directory for converted books
  --formats FORMATS     Comma-separated list of output formats (default: all)
  --book TITLE          Process specific book by title
  --batch-size N        Number of books to process simultaneously (default: 3)
  --keep-drm            Keep original DRM-protected files
  --verbose            Enable verbose logging
```

## How It Works

1. **Download**: Authenticates with Amazon and downloads your Kindle books
2. **DRM Removal**: Uses DeDRM plugin methods to remove DRM protection
3. **Conversion**: Uses Calibre's ebook-convert to transform books into multiple formats

## Output Structure

```
output/
├── Book Title 1/
│   ├── Book Title 1.epub
│   ├── Book Title 1.pdf
│   ├── Book Title 1.mobi
│   ├── Book Title 1.txt
│   └── metadata.json
├── Book Title 2/
│   ├── Book Title 2.epub
│   ├── Book Title 2.pdf
│   └── ...
```

## Troubleshooting

### DRM Removal Fails

- Make sure you're using the same Amazon account that purchased the books
- Try providing your Kindle serial number or PID in `config.yaml`
- Some newer KFX formats may require additional steps

### Conversion Errors

- Ensure Calibre is properly installed and in your PATH
- Try converting to one format at a time
- Check the logs in `logs/` directory

### Download Issues

- Verify your Amazon credentials in `config.yaml`
- Check your internet connection
- Amazon may require CAPTCHA - run with `--interactive` flag

## Dependencies

- `requests` - HTTP library
- `beautifulsoup4` - HTML parsing
- `selenium` - Browser automation for Amazon login
- `cryptography` - Encryption/decryption for DRM
- `pycryptodome` - Additional crypto support
- `lxml` - XML parsing
- `Pillow` - Image processing
- `ebooklib` - EPUB handling
- Calibre CLI tools

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

MIT License - See LICENSE file for details

## Disclaimer

This software is provided for educational purposes only. The authors are not responsible for any misuse or legal consequences resulting from the use of this software. Always respect copyright laws and terms of service.

## Acknowledgments

- Based on DeDRM tools concepts
- Uses Calibre for ebook conversion
- Inspired by the ebook preservation community
