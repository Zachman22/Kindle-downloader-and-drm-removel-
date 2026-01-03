"""
Utility functions for Kindle DRM Converter
"""

import os
import yaml
import logging
from pathlib import Path
from typing import Dict, Any


def setup_logging(log_level: str = 'INFO', log_file: Path = None) -> None:
    """
    Setup logging configuration

    Args:
        log_level: Logging level (DEBUG, INFO, WARNING, ERROR)
        log_file: Optional log file path
    """
    log_format = '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    date_format = '%Y-%m-%d %H:%M:%S'

    handlers = [logging.StreamHandler()]

    if log_file:
        log_file.parent.mkdir(parents=True, exist_ok=True)
        handlers.append(logging.FileHandler(log_file))

    logging.basicConfig(
        level=getattr(logging, log_level.upper()),
        format=log_format,
        datefmt=date_format,
        handlers=handlers
    )


def load_config(config_path: Path) -> Dict[str, Any]:
    """
    Load configuration from YAML file

    Args:
        config_path: Path to config file

    Returns:
        Configuration dictionary
    """
    if not config_path.exists():
        raise FileNotFoundError(f"Config file not found: {config_path}")

    with open(config_path, 'r') as f:
        config = yaml.safe_load(f)

    # Validate required fields
    required_fields = ['amazon', 'directories']
    for field in required_fields:
        if field not in config:
            raise ValueError(f"Missing required config field: {field}")

    # Set defaults
    if 'conversion' not in config:
        config['conversion'] = {
            'formats': ['epub', 'pdf', 'mobi'],
            'quality': 'high'
        }

    if 'drm' not in config:
        config['drm'] = {}

    return config


def sanitize_filename(filename: str) -> str:
    """
    Sanitize filename by removing invalid characters

    Args:
        filename: Original filename

    Returns:
        Sanitized filename
    """
    # Remove invalid characters
    invalid_chars = '<>:"/\\|?*'
    for char in invalid_chars:
        filename = filename.replace(char, '')

    # Remove leading/trailing spaces and dots
    filename = filename.strip(' .')

    # Limit length
    max_length = 200
    if len(filename) > max_length:
        name, ext = os.path.splitext(filename)
        filename = name[:max_length - len(ext)] + ext

    return filename


def get_file_size_mb(filepath: Path) -> float:
    """
    Get file size in megabytes

    Args:
        filepath: Path to file

    Returns:
        File size in MB
    """
    if not filepath.exists():
        return 0.0
    size_bytes = filepath.stat().st_size
    return size_bytes / (1024 * 1024)


def create_directory_structure(base_dir: Path, subdirs: list) -> None:
    """
    Create directory structure

    Args:
        base_dir: Base directory path
        subdirs: List of subdirectory names
    """
    base_dir.mkdir(parents=True, exist_ok=True)
    for subdir in subdirs:
        (base_dir / subdir).mkdir(parents=True, exist_ok=True)


def format_time(seconds: float) -> str:
    """
    Format seconds into human-readable time

    Args:
        seconds: Time in seconds

    Returns:
        Formatted time string
    """
    if seconds < 60:
        return f"{seconds:.1f}s"
    elif seconds < 3600:
        minutes = seconds / 60
        return f"{minutes:.1f}m"
    else:
        hours = seconds / 3600
        return f"{hours:.1f}h"


def validate_email(email: str) -> bool:
    """
    Basic email validation

    Args:
        email: Email address

    Returns:
        True if valid format
    """
    import re
    pattern = r'^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$'
    return re.match(pattern, email) is not None


def get_kindle_formats() -> list:
    """Get list of Kindle ebook formats"""
    return ['.azw', '.azw3', '.azw4', '.mobi', '.prc', '.kfx', '.kfx-zip']


def is_kindle_format(filepath: Path) -> bool:
    """
    Check if file is a Kindle format

    Args:
        filepath: Path to file

    Returns:
        True if Kindle format
    """
    return filepath.suffix.lower() in get_kindle_formats()


class ProgressTracker:
    """Simple progress tracker for batch operations"""

    def __init__(self, total: int, description: str = "Processing"):
        self.total = total
        self.current = 0
        self.description = description
        self.logger = logging.getLogger(__name__)

    def update(self, increment: int = 1):
        """Update progress"""
        self.current += increment
        percentage = (self.current / self.total) * 100 if self.total > 0 else 0
        self.logger.info(f"{self.description}: {self.current}/{self.total} ({percentage:.1f}%)")

    def finish(self):
        """Mark as finished"""
        self.logger.info(f"{self.description}: Complete ({self.total}/{self.total})")
