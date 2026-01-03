# Quick Start Guide

Get up and running with Kindle DRM Converter in 5 minutes!

## Prerequisites

1. **Python 3.8+**
   ```bash
   python --version  # Should be 3.8 or higher
   ```

2. **Calibre** (required for conversion)
   ```bash
   # Linux
   sudo apt-get install calibre

   # macOS
   brew install calibre

   # Windows
   # Download from https://calibre-ebook.com/download
   ```

3. **Chrome Browser** (for Amazon login)

## Installation

### Step 1: Clone the Repository
```bash
git clone https://github.com/yourusername/Kindle-downloader-and-drm-removel-.git
cd Kindle-downloader-and-drm-removel-
```

### Step 2: Install Dependencies
```bash
pip install -r requirements.txt
```

### Step 3: Configure
```bash
# Copy the example config
cp config.yaml.example config.yaml

# Edit with your Amazon credentials
nano config.yaml  # or use your favorite editor
```

Edit these fields in `config.yaml`:
```yaml
amazon:
  email: "your-amazon-email@example.com"
  password: "your-amazon-password"
  region: "US"  # or UK, DE, FR, etc.
```

## Basic Usage

### Process All Your Kindle Books

Download all books, remove DRM, and convert to EPUB, PDF, and MOBI:

```bash
python main.py --all
```

That's it! Your books will be in the `output/` directory.

### Process Specific Book

```bash
python main.py --book "Harry Potter and the Philosopher's Stone"
```

### Download Only (No Conversion)

```bash
python main.py --download-only
```

### Process Existing Files

Already have Kindle files? Process them:

```bash
python main.py --input /path/to/kindle/files
```

### Custom Output Formats

Convert to specific formats:

```bash
python main.py --all --formats epub,pdf,txt
```

## Common Issues

### "Login failed"
- Check your credentials in `config.yaml`
- Try interactive mode: `python main.py --all --interactive`
- You may need to handle 2FA manually

### "Calibre not found"
- Ensure Calibre is installed: `ebook-convert --version`
- Add Calibre to your PATH

### "DRM removal failed"
- Add your Kindle serial number to `config.yaml`
- Try using Calibre DeDRM: `python main.py --all --use-calibre-drm`

### Chrome driver issues
```bash
# The script will auto-download chromedriver
# If issues persist, install webdriver-manager:
pip install webdriver-manager --upgrade
```

## Output Structure

```
output/
├── Book Title 1/
│   ├── Book Title 1.epub
│   ├── Book Title 1.pdf
│   ├── Book Title 1.mobi
│   └── Book Title 1.txt
├── Book Title 2/
│   └── ...
└── conversion_report.json
```

## Next Steps

- Read the full [README.md](README.md) for advanced options
- Check [CONTRIBUTING.md](CONTRIBUTING.md) if you want to contribute
- Report issues on GitHub

## Tips

1. **First Run**: Use `--interactive` flag for first login to handle 2FA
2. **Batch Size**: Adjust `--batch-size 5` for faster processing
3. **Formats**: Choose only formats you need to save time
4. **Keep Originals**: Use `--keep-drm` to preserve original files

## Example Workflows

**Download and convert to EPUB only:**
```bash
python main.py --all --formats epub
```

**Process existing files with custom output:**
```bash
python main.py --input ~/Documents/Kindle --output ~/Books --formats epub,pdf
```

**Verbose mode for debugging:**
```bash
python main.py --all --verbose
```

Happy reading! 📚
