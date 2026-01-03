# Contributing to Kindle DRM Converter

Thank you for your interest in contributing to Kindle DRM Converter!

## Getting Started

1. Fork the repository
2. Clone your fork: `git clone https://github.com/yourusername/Kindle-downloader-and-drm-removel-.git`
3. Create a new branch: `git checkout -b feature/your-feature-name`
4. Make your changes
5. Test your changes
6. Commit your changes: `git commit -m "Add your feature"`
7. Push to your fork: `git push origin feature/your-feature-name`
8. Create a Pull Request

## Development Setup

```bash
# Create virtual environment
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install dependencies
pip install -r requirements.txt
pip install -e ".[dev]"

# Install Calibre (required for conversion)
# Linux: sudo apt-get install calibre
# macOS: brew install calibre
# Windows: Download from calibre-ebook.com
```

## Code Style

- Follow PEP 8 guidelines
- Use meaningful variable and function names
- Add docstrings to all functions and classes
- Keep functions focused and single-purpose
- Add type hints where appropriate

## Testing

Before submitting a PR, ensure:

1. Your code works with different Kindle formats (AZW, AZW3, MOBI)
2. All existing functionality still works
3. You've tested on your target platform (Linux/macOS/Windows)

## Pull Request Guidelines

- Provide a clear description of the changes
- Reference any related issues
- Include screenshots if UI changes are involved
- Update documentation if needed
- Ensure your code follows the existing style

## Reporting Issues

When reporting issues, please include:

- Operating system and version
- Python version
- Calibre version
- Complete error message and stack trace
- Steps to reproduce the issue
- Example file format (AZW, AZW3, etc.)

## Legal Notice

By contributing, you agree that:

- Your contributions will be licensed under the MIT License
- You have the right to submit the contributions
- This project is for educational purposes only
- Users are responsible for complying with applicable laws

## Feature Requests

We welcome feature requests! Please:

1. Check if the feature already exists or is planned
2. Describe the feature and its use case
3. Explain why it would be valuable
4. Consider submitting a PR if you can implement it

## Code of Conduct

- Be respectful and professional
- Welcome newcomers
- Focus on constructive feedback
- Respect different viewpoints and experiences

## Questions?

Feel free to open an issue for questions or discussions!

Thank you for contributing!
