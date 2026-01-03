"""
Ebook Format Converter Module
Converts ebooks to multiple formats using Calibre
"""

import os
import subprocess
import logging
from pathlib import Path
from typing import List, Optional, Dict
import json
from concurrent.futures import ThreadPoolExecutor, as_completed

logger = logging.getLogger(__name__)


class EbookConverter:
    """Converts ebooks to multiple formats"""

    SUPPORTED_FORMATS = [
        'epub',
        'pdf',
        'mobi',
        'azw3',
        'txt',
        'html',
        'rtf',
        'odt',
        'docx',
        'fb2',
        'lit',
        'lrf',
        'pdb',
        'pml',
        'rb',
        'snb',
        'tcr',
    ]

    def __init__(self, config: Dict):
        """
        Initialize converter

        Args:
            config: Configuration dictionary
        """
        self.output_dir = Path(config['directories']['output'])
        self.output_dir.mkdir(parents=True, exist_ok=True)

        self.formats = config.get('conversion', {}).get('formats', ['epub', 'pdf', 'mobi'])
        self.quality = config.get('conversion', {}).get('quality', 'high')

        # Verify Calibre is installed
        if not self._check_calibre():
            raise RuntimeError("Calibre is not installed or not in PATH")

    def _check_calibre(self) -> bool:
        """Check if Calibre is installed"""
        try:
            result = subprocess.run(['ebook-convert', '--version'],
                                    capture_output=True, text=True)
            if result.returncode == 0:
                logger.info(f"Calibre found: {result.stdout.strip()}")
                return True
            return False
        except FileNotFoundError:
            logger.error("Calibre not found. Please install Calibre.")
            return False

    def convert(self, input_file: Path, output_formats: Optional[List[str]] = None) -> Dict[str, Path]:
        """
        Convert ebook to specified formats

        Args:
            input_file: Path to input ebook file
            output_formats: List of output formats (default: from config)

        Returns:
            Dictionary mapping format to output file path
        """
        if not input_file.exists():
            logger.error(f"Input file not found: {input_file}")
            return {}

        formats = output_formats or self.formats
        logger.info(f"Converting {input_file.name} to formats: {', '.join(formats)}")

        # Create output directory for this book
        book_dir = self.output_dir / input_file.stem
        book_dir.mkdir(parents=True, exist_ok=True)

        # Convert to each format
        converted_files = {}
        for fmt in formats:
            output_file = self._convert_to_format(input_file, book_dir, fmt)
            if output_file:
                converted_files[fmt] = output_file

        logger.info(f"Successfully converted to {len(converted_files)}/{len(formats)} formats")
        return converted_files

    def _convert_to_format(self, input_file: Path, output_dir: Path, output_format: str) -> Optional[Path]:
        """
        Convert single file to specific format

        Args:
            input_file: Input file path
            output_dir: Output directory
            output_format: Target format

        Returns:
            Path to converted file, or None if failed
        """
        output_format = output_format.lower().strip('.')

        if output_format not in self.SUPPORTED_FORMATS:
            logger.warning(f"Unsupported format: {output_format}")
            return None

        output_file = output_dir / f"{input_file.stem}.{output_format}"

        logger.info(f"Converting to {output_format.upper()}...")

        # Build ebook-convert command
        cmd = [
            'ebook-convert',
            str(input_file),
            str(output_file),
        ]

        # Add format-specific options
        cmd.extend(self._get_format_options(output_format))

        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=300  # 5 minute timeout per conversion
            )

            if result.returncode == 0 and output_file.exists():
                logger.info(f"Successfully converted to {output_format.upper()}: {output_file}")
                return output_file
            else:
                logger.error(f"Conversion to {output_format} failed: {result.stderr}")
                return None

        except subprocess.TimeoutExpired:
            logger.error(f"Conversion to {output_format} timed out")
            return None
        except Exception as e:
            logger.error(f"Conversion error: {e}")
            return None

    def _get_format_options(self, output_format: str) -> List[str]:
        """Get format-specific conversion options"""
        options = []

        # Common options
        options.extend([
            '--chapter', '/',
            '--page-breaks-before', '/',
            '--preserve-cover-aspect-ratio',
        ])

        # Format-specific options
        if output_format == 'epub':
            options.extend([
                '--epub-version', '3',
                '--flow-size', '0',
                '--no-default-epub-cover',
            ])

        elif output_format == 'pdf':
            if self.quality == 'high':
                options.extend([
                    '--paper-size', 'letter',
                    '--pdf-page-numbers',
                    '--pdf-sans-family', 'Arial',
                    '--pdf-serif-family', 'Times New Roman',
                    '--pdf-mono-family', 'Courier New',
                ])
            options.extend([
                '--preserve-cover-aspect-ratio',
            ])

        elif output_format == 'mobi':
            options.extend([
                '--mobi-file-type', 'both',
                '--no-inline-toc',
            ])

        elif output_format == 'azw3':
            options.extend([
                '--mobi-file-type', 'new',
            ])

        elif output_format == 'txt':
            options.extend([
                '--txt-output-formatting', 'markdown',
                '--max-line-length', '0',
                '--txt-output-encoding', 'utf-8',
            ])

        elif output_format == 'docx':
            options.extend([
                '--docx-page-size', 'letter',
            ])

        return options

    def batch_convert(self, input_files: List[Path],
                      output_formats: Optional[List[str]] = None,
                      max_workers: int = 3) -> Dict[Path, Dict[str, Path]]:
        """
        Convert multiple files in parallel

        Args:
            input_files: List of input file paths
            output_formats: List of output formats
            max_workers: Number of parallel conversion workers

        Returns:
            Dictionary mapping input file to dict of format->output path
        """
        logger.info(f"Batch converting {len(input_files)} files...")

        results = {}

        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            # Submit all conversion jobs
            future_to_file = {
                executor.submit(self.convert, file, output_formats): file
                for file in input_files
            }

            # Collect results as they complete
            for future in as_completed(future_to_file):
                input_file = future_to_file[future]
                try:
                    converted_files = future.result()
                    results[input_file] = converted_files
                except Exception as e:
                    logger.error(f"Batch conversion error for {input_file}: {e}")
                    results[input_file] = {}

        successful = sum(1 for r in results.values() if r)
        logger.info(f"Batch conversion complete: {successful}/{len(input_files)} successful")

        return results

    def convert_with_metadata(self, input_file: Path,
                             metadata: Optional[Dict] = None,
                             output_formats: Optional[List[str]] = None) -> Dict[str, Path]:
        """
        Convert ebook and embed metadata

        Args:
            input_file: Input file path
            metadata: Metadata dictionary (title, author, etc.)
            output_formats: Output formats

        Returns:
            Dictionary of converted files
        """
        # First convert the file
        converted_files = self.convert(input_file, output_formats)

        # If metadata provided, embed it
        if metadata:
            self._embed_metadata(converted_files, metadata)

        return converted_files

    def _embed_metadata(self, files: Dict[str, Path], metadata: Dict):
        """Embed metadata into converted files using Calibre"""
        for fmt, filepath in files.items():
            try:
                logger.debug(f"Embedding metadata in {filepath.name}")

                cmd = ['ebook-meta', str(filepath)]

                # Add metadata options
                if 'title' in metadata:
                    cmd.extend(['--title', metadata['title']])
                if 'author' in metadata:
                    cmd.extend(['--authors', metadata['author']])
                if 'publisher' in metadata:
                    cmd.extend(['--publisher', metadata['publisher']])
                if 'published' in metadata:
                    cmd.extend(['--date', str(metadata['published'])])
                if 'language' in metadata:
                    cmd.extend(['--language', metadata['language']])
                if 'tags' in metadata:
                    cmd.extend(['--tags', ','.join(metadata['tags'])])

                subprocess.run(cmd, capture_output=True, check=True)

            except Exception as e:
                logger.warning(f"Could not embed metadata in {filepath.name}: {e}")

    def get_metadata(self, filepath: Path) -> Dict:
        """
        Extract metadata from ebook file

        Args:
            filepath: Path to ebook file

        Returns:
            Dictionary with metadata
        """
        try:
            result = subprocess.run(
                ['ebook-meta', str(filepath)],
                capture_output=True,
                text=True,
                check=True
            )

            # Parse metadata from output
            metadata = {}
            for line in result.stdout.split('\n'):
                if ':' in line:
                    key, value = line.split(':', 1)
                    metadata[key.strip().lower()] = value.strip()

            return metadata

        except Exception as e:
            logger.error(f"Could not extract metadata: {e}")
            return {}

    def create_conversion_report(self, results: Dict[Path, Dict[str, Path]],
                                 output_file: Optional[Path] = None) -> Path:
        """
        Create a JSON report of conversion results

        Args:
            results: Conversion results dictionary
            output_file: Optional output path for report

        Returns:
            Path to report file
        """
        if output_file is None:
            output_file = self.output_dir / 'conversion_report.json'

        report = {
            'total_files': len(results),
            'successful': sum(1 for r in results.values() if r),
            'failed': sum(1 for r in results.values() if not r),
            'conversions': []
        }

        for input_file, converted in results.items():
            entry = {
                'input': str(input_file),
                'formats': {fmt: str(path) for fmt, path in converted.items()},
                'success': len(converted) > 0
            }
            report['conversions'].append(entry)

        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)

        logger.info(f"Conversion report saved: {output_file}")
        return output_file
