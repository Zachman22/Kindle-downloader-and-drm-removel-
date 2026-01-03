#!/usr/bin/env python3
"""
Setup script for Kindle DRM Converter
"""

from setuptools import setup, find_packages
from pathlib import Path

# Read README for long description
readme_file = Path(__file__).parent / 'README.md'
long_description = readme_file.read_text(encoding='utf-8') if readme_file.exists() else ''

setup(
    name='kindle-drm-converter',
    version='1.0.0',
    description='Download Kindle books, remove DRM, and convert to all ebook formats',
    long_description=long_description,
    long_description_content_type='text/markdown',
    author='Kindle DRM Converter Team',
    author_email='',
    url='https://github.com/yourusername/Kindle-downloader-and-drm-removel-',
    packages=find_packages(),
    include_package_data=True,
    install_requires=[
        'requests>=2.31.0',
        'beautifulsoup4>=4.12.0',
        'selenium>=4.15.0',
        'PyYAML>=6.0.1',
        'cryptography>=41.0.0',
        'pycryptodome>=3.19.0',
        'ebooklib>=0.18',
        'lxml>=4.9.0',
        'Pillow>=10.1.0',
        'webdriver-manager>=4.0.0',
        'tqdm>=4.66.0',
        'colorama>=0.4.6',
    ],
    extras_require={
        'dev': [
            'pytest>=7.4.0',
            'pytest-cov>=4.1.0',
            'black>=23.0.0',
            'flake8>=6.1.0',
            'mypy>=1.5.0',
        ],
    },
    entry_points={
        'console_scripts': [
            'kindle-converter=main:main',
        ],
    },
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: End Users/Desktop',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
    ],
    python_requires='>=3.8',
    keywords='kindle ebook drm converter epub pdf mobi',
)
