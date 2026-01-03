#!/usr/bin/env python3
"""
Kindle DRM Converter - Main Script
Download, remove DRM, and convert Kindle books to all ebook formats
"""

import argparse
import sys
import time
from pathlib import Path
from typing import List, Optional

from src.downloader import KindleDownloader
from src.drm_removal import DRMRemover, CalibreDRMRemover
from src.converter import EbookConverter
from src.utils import (
    setup_logging,
    load_config,
    format_time,
    ProgressTracker,
    is_kindle_format
)


def parse_arguments():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(
        description='Kindle DRM Converter - Download, remove DRM, and convert Kindle books',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Process all Kindle books
  python main.py --all

  # Download only
  python main.py --download-only

  # Process existing files
  python main.py --input /path/to/kindle/files

  # Convert to specific formats
  python main.py --all --formats epub,pdf,mobi

  # Process specific book
  python main.py --book "Book Title"
        """
    )

    parser.add_argument(
        '--all',
        action='store_true',
        help='Download, remove DRM, and convert all Kindle books'
    )

    parser.add_argument(
        '--download-only',
        action='store_true',
        help='Only download books without DRM removal or conversion'
    )

    parser.add_argument(
        '--input',
        type=Path,
        help='Process existing Kindle files from this directory or file'
    )

    parser.add_argument(
        '--output',
        type=Path,
        help='Output directory for converted books (default: from config)'
    )

    parser.add_argument(
        '--formats',
        type=str,
        help='Comma-separated list of output formats (e.g., epub,pdf,mobi)'
    )

    parser.add_argument(
        '--book',
        type=str,
        help='Process specific book by title'
    )

    parser.add_argument(
        '--batch-size',
        type=int,
        default=3,
        help='Number of books to process simultaneously (default: 3)'
    )

    parser.add_argument(
        '--config',
        type=Path,
        default=Path('config.yaml'),
        help='Path to configuration file (default: config.yaml)'
    )

    parser.add_argument(
        '--keep-drm',
        action='store_true',
        help='Keep original DRM-protected files'
    )

    parser.add_argument(
        '--interactive',
        action='store_true',
        help='Run in interactive mode (for CAPTCHA/2FA)'
    )

    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Enable verbose logging'
    )

    parser.add_argument(
        '--use-calibre-drm',
        action='store_true',
        help='Use Calibre DeDRM plugin for DRM removal'
    )

    return parser.parse_args()


def process_files(files: List[Path],
                 config: dict,
                 output_formats: Optional[List[str]] = None,
                 batch_size: int = 3,
                 use_calibre_drm: bool = False) -> dict:
    """
    Process files: Remove DRM and convert to formats

    Args:
        files: List of file paths to process
        config: Configuration dictionary
        output_formats: List of output formats
        batch_size: Batch size for parallel processing
        use_calibre_drm: Use Calibre for DRM removal

    Returns:
        Dictionary with processing results
    """
    results = {
        'total': len(files),
        'drm_removed': 0,
        'converted': 0,
        'failed': 0,
        'converted_files': {}
    }

    # Initialize processors
    if use_calibre_drm:
        drm_remover = CalibreDRMRemover(config)
    else:
        drm_remover = DRMRemover(config)

    converter = EbookConverter(config)

    # Process each file
    tracker = ProgressTracker(len(files), "Processing books")

    for filepath in files:
        try:
            # Step 1: Remove DRM
            print(f"\n{'=' * 60}")
            print(f"Processing: {filepath.name}")
            print(f"{'=' * 60}")

            if use_calibre_drm:
                drm_free_file = drm_remover.remove_drm_via_calibre(filepath)
            else:
                drm_free_file = drm_remover.remove_drm(filepath)

            if not drm_free_file:
                print(f"⚠ DRM removal failed for {filepath.name}")
                results['failed'] += 1
                tracker.update()
                continue

            results['drm_removed'] += 1
            print(f"✓ DRM removed successfully")

            # Step 2: Convert to formats
            converted_files = converter.convert(drm_free_file, output_formats)

            if converted_files:
                results['converted'] += 1
                results['converted_files'][filepath] = converted_files
                print(f"✓ Converted to {len(converted_files)} format(s)")

                for fmt, path in converted_files.items():
                    print(f"  - {fmt.upper()}: {path}")
            else:
                print(f"⚠ Conversion failed for {filepath.name}")
                results['failed'] += 1

            tracker.update()

        except Exception as e:
            print(f"✗ Error processing {filepath.name}: {e}")
            results['failed'] += 1
            tracker.update()
            continue

    tracker.finish()
    return results


def main():
    """Main entry point"""
    args = parse_arguments()

    # Setup logging
    log_level = 'DEBUG' if args.verbose else 'INFO'
    log_file = Path('logs') / 'kindle_converter.log'
    setup_logging(log_level, log_file)

    print("=" * 60)
    print("Kindle DRM Converter")
    print("=" * 60)
    print()

    # Load configuration
    try:
        config = load_config(args.config)
    except Exception as e:
        print(f"✗ Error loading config: {e}")
        print(f"Please create a config.yaml file (see config.yaml.example)")
        return 1

    # Override config with command line args
    if args.output:
        config['directories']['output'] = str(args.output)

    if args.formats:
        formats = [f.strip() for f in args.formats.split(',')]
        config['conversion']['formats'] = formats

    start_time = time.time()

    try:
        files_to_process = []

        # Mode 1: Download from Kindle library
        if args.all or args.download_only or args.book:
            print("Connecting to Amazon Kindle...")

            with KindleDownloader(config) as downloader:
                # Login
                if not downloader.login(interactive=args.interactive):
                    print("✗ Login failed")
                    return 1

                print("✓ Login successful")
                print()

                # Download books
                if args.book:
                    # Download specific book
                    downloaded = downloader.download_all(book_titles=[args.book])
                else:
                    # Download all books
                    downloaded = downloader.download_all()

                print(f"\n✓ Downloaded {len(downloaded)} book(s)")
                files_to_process = downloaded

            # If download-only mode, exit here
            if args.download_only:
                print("\n✓ Download complete (--download-only mode)")
                return 0

        # Mode 2: Process existing files
        elif args.input:
            input_path = args.input

            if not input_path.exists():
                print(f"✗ Input path not found: {input_path}")
                return 1

            if input_path.is_file():
                if is_kindle_format(input_path):
                    files_to_process = [input_path]
                else:
                    print(f"✗ Not a Kindle format: {input_path}")
                    return 1
            else:
                # Directory - find all Kindle files
                for ext in ['.azw', '.azw3', '.mobi', '.kfx']:
                    files_to_process.extend(input_path.glob(f"*{ext}"))
                    files_to_process.extend(input_path.glob(f"**/*{ext}"))

                files_to_process = list(set(files_to_process))  # Remove duplicates

            print(f"Found {len(files_to_process)} Kindle file(s) to process")

        else:
            print("✗ Please specify --all, --download-only, --input, or --book")
            print("Run with --help for usage information")
            return 1

        # Process files (DRM removal + conversion)
        if files_to_process:
            print("\nStarting DRM removal and conversion...")
            print(f"Output formats: {', '.join(config['conversion']['formats'])}")
            print()

            results = process_files(
                files_to_process,
                config,
                output_formats=config['conversion']['formats'],
                batch_size=args.batch_size,
                use_calibre_drm=args.use_calibre_drm
            )

            # Print summary
            print("\n" + "=" * 60)
            print("SUMMARY")
            print("=" * 60)
            print(f"Total files processed: {results['total']}")
            print(f"DRM removed: {results['drm_removed']}")
            print(f"Successfully converted: {results['converted']}")
            print(f"Failed: {results['failed']}")
            print()

            if results['converted_files']:
                print(f"Output directory: {config['directories']['output']}")
                print()

            elapsed = time.time() - start_time
            print(f"Total time: {format_time(elapsed)}")
            print("=" * 60)

            # Create conversion report
            converter = EbookConverter(config)
            report_file = converter.create_conversion_report(results['converted_files'])
            print(f"\nDetailed report saved to: {report_file}")

        return 0

    except KeyboardInterrupt:
        print("\n\n✗ Interrupted by user")
        return 130

    except Exception as e:
        print(f"\n✗ Unexpected error: {e}")
        if args.verbose:
            import traceback
            traceback.print_exc()
        return 1


if __name__ == '__main__':
    sys.exit(main())
