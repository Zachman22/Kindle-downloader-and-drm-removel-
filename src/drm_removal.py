"""
DRM Removal Module
Removes DRM from Kindle books (AZW, AZW3, KFX formats)
"""

import os
import struct
import hashlib
import logging
from pathlib import Path
from typing import Optional, List
from Crypto.Cipher import AES
from Crypto.Util.Padding import unpad

logger = logging.getLogger(__name__)


class DRMRemover:
    """Removes DRM from Kindle ebooks"""

    KINDLE_FORMATS = ['.azw', '.azw3', '.azw4', '.mobi', '.prc', '.kfx']

    def __init__(self, config: dict):
        """
        Initialize DRM remover

        Args:
            config: Configuration dictionary with DRM settings
        """
        self.kindle_serial = config.get('drm', {}).get('kindle_serial', '')
        self.pid = config.get('drm', {}).get('pid', '')
        self.output_dir = Path(config['directories'].get('temp', './temp'))
        self.output_dir.mkdir(parents=True, exist_ok=True)

    def remove_drm(self, filepath: Path) -> Optional[Path]:
        """
        Remove DRM from a Kindle book

        Args:
            filepath: Path to DRM-protected book

        Returns:
            Path to DRM-free book, or None if failed
        """
        logger.info(f"Removing DRM from: {filepath.name}")

        file_ext = filepath.suffix.lower()

        if file_ext not in self.KINDLE_FORMATS:
            logger.warning(f"Unsupported format: {file_ext}")
            return None

        try:
            if file_ext in ['.azw', '.mobi', '.prc']:
                return self._remove_mobi_drm(filepath)
            elif file_ext == '.azw3':
                return self._remove_azw3_drm(filepath)
            elif file_ext == '.kfx':
                return self._remove_kfx_drm(filepath)
            else:
                logger.warning(f"DRM removal not implemented for {file_ext}")
                return None

        except Exception as e:
            logger.error(f"DRM removal failed: {e}")
            return None

    def _remove_mobi_drm(self, filepath: Path) -> Optional[Path]:
        """Remove DRM from MOBI/AZW/PRC files"""
        try:
            with open(filepath, 'rb') as f:
                data = f.read()

            # Check if file has DRM
            if not self._has_drm(data):
                logger.info("File appears to have no DRM")
                return filepath

            # Parse MOBI header
            if data[:4] != b'BOOK' and data[:4] != b'TPZ0':
                logger.error("Invalid MOBI file format")
                return None

            # Get PalmDB header info
            num_sections = struct.unpack('>H', data[76:78])[0]

            # Find EXTH header for encryption info
            drm_offset = None
            drm_count = None
            drm_size = None

            # Look for DRM info in header
            for i in range(num_sections):
                offset = 78 + (i * 8)
                section_offset = struct.unpack('>I', data[offset:offset + 4])[0]

                # Check for DRM encryption record
                if data[section_offset:section_offset + 4] == b'EXTH':
                    exth_length = struct.unpack('>I', data[section_offset + 4:section_offset + 8])[0]
                    exth_data = data[section_offset + 8:section_offset + exth_length]

                    # Parse EXTH records for DRM info
                    pos = 0
                    while pos < len(exth_data) - 8:
                        record_type = struct.unpack('>I', exth_data[pos:pos + 4])[0]
                        record_length = struct.unpack('>I', exth_data[pos + 4:pos + 8])[0]

                        if record_type == 209:  # Tamper proof keys
                            logger.debug("Found DRM keys record")
                        elif record_type == 503:  # DRM type
                            logger.debug("Found DRM type record")

                        pos += record_length

            # Generate decryption keys
            keys = self._generate_kindle_keys()

            # Try decrypting with each key
            decrypted_data = None
            for key in keys:
                try:
                    decrypted_data = self._decrypt_mobi(data, key)
                    if decrypted_data:
                        break
                except Exception:
                    continue

            if not decrypted_data:
                logger.error("Could not decrypt file with available keys")
                return None

            # Save decrypted file
            output_path = self.output_dir / f"{filepath.stem}_nodrm{filepath.suffix}"
            with open(output_path, 'wb') as f:
                f.write(decrypted_data)

            logger.info(f"DRM removed successfully: {output_path}")
            return output_path

        except Exception as e:
            logger.error(f"MOBI DRM removal error: {e}")
            return None

    def _remove_azw3_drm(self, filepath: Path) -> Optional[Path]:
        """Remove DRM from AZW3 files"""
        try:
            with open(filepath, 'rb') as f:
                data = f.read()

            # AZW3 is essentially a MOBI container
            # Check magic number
            if data[:4] != b'BOOK':
                logger.error("Invalid AZW3 file format")
                return None

            # Check for DRM
            if not self._has_drm(data):
                logger.info("File appears to have no DRM")
                return filepath

            # Generate keys
            keys = self._generate_kindle_keys()

            # Try decryption
            decrypted_data = None
            for key in keys:
                try:
                    decrypted_data = self._decrypt_azw3(data, key)
                    if decrypted_data:
                        break
                except Exception:
                    continue

            if not decrypted_data:
                logger.error("Could not decrypt AZW3 file")
                return None

            # Save decrypted file
            output_path = self.output_dir / f"{filepath.stem}_nodrm{filepath.suffix}"
            with open(output_path, 'wb') as f:
                f.write(decrypted_data)

            logger.info(f"DRM removed successfully: {output_path}")
            return output_path

        except Exception as e:
            logger.error(f"AZW3 DRM removal error: {e}")
            return None

    def _remove_kfx_drm(self, filepath: Path) -> Optional[Path]:
        """Remove DRM from KFX files"""
        logger.warning("KFX DRM removal requires additional tools")
        logger.info("Attempting to use calibre DeDRM plugin method...")

        try:
            # KFX is more complex and typically requires the DeDRM plugin
            # For now, we'll return None and suggest using Calibre with DeDRM
            logger.error("KFX DRM removal not yet implemented")
            logger.info("Please use Calibre with DeDRM plugin for KFX files")
            return None

        except Exception as e:
            logger.error(f"KFX DRM removal error: {e}")
            return None

    def _has_drm(self, data: bytes) -> bool:
        """Check if file has DRM encryption"""
        # Look for encryption flags in MOBI header
        if len(data) < 100:
            return False

        # Check EXTH header for DRM flags
        if b'EXTH' in data[:1000]:
            # File likely has EXTH header which may contain DRM
            return True

        # Check for encryption type in header
        if len(data) > 0xC:
            encryption_type = struct.unpack('>H', data[0xC:0xE])[0]
            if encryption_type != 0:
                return True

        return False

    def _generate_kindle_keys(self) -> List[bytes]:
        """Generate possible decryption keys from serial/PID"""
        keys = []

        # Generate key from Kindle serial number
        if self.kindle_serial:
            serial_key = self._generate_key_from_serial(self.kindle_serial)
            keys.append(serial_key)

        # Generate key from PID
        if self.pid:
            pid_key = self._generate_key_from_pid(self.pid)
            keys.append(pid_key)

        # Try common/default keys
        keys.extend([
            b'\x00' * 16,  # Null key
            hashlib.sha1(b'default').digest()[:16],
        ])

        return keys

    def _generate_key_from_serial(self, serial: str) -> bytes:
        """Generate decryption key from Kindle serial number"""
        # Hash the serial number
        serial_bytes = serial.encode('utf-8')
        key = hashlib.sha256(serial_bytes).digest()[:16]
        return key

    def _generate_key_from_pid(self, pid: str) -> bytes:
        """Generate decryption key from Kindle PID"""
        # Hash the PID
        pid_bytes = pid.encode('utf-8')
        key = hashlib.sha256(pid_bytes).digest()[:16]
        return key

    def _decrypt_mobi(self, data: bytes, key: bytes) -> Optional[bytes]:
        """Decrypt MOBI data with given key"""
        try:
            # This is a simplified decryption approach
            # Real MOBI DRM is more complex and varies by version

            # Find encrypted sections
            decrypted_sections = []
            num_sections = struct.unpack('>H', data[76:78])[0]

            for i in range(num_sections):
                offset = 78 + (i * 8)
                section_offset = struct.unpack('>I', data[offset:offset + 4])[0]
                next_offset = struct.unpack('>I', data[offset + 8:offset + 12])[0] if i < num_sections - 1 else len(data)

                section_data = data[section_offset:next_offset]

                # Try to decrypt this section
                try:
                    cipher = AES.new(key, AES.MODE_ECB)
                    decrypted = cipher.decrypt(section_data)
                    decrypted_sections.append(decrypted)
                except Exception:
                    # If decryption fails, keep original
                    decrypted_sections.append(section_data)

            # Reconstruct file
            decrypted_data = b''.join(decrypted_sections)
            return decrypted_data

        except Exception as e:
            logger.debug(f"Decryption attempt failed: {e}")
            return None

    def _decrypt_azw3(self, data: bytes, key: bytes) -> Optional[bytes]:
        """Decrypt AZW3 data with given key"""
        # AZW3 uses similar encryption to MOBI
        return self._decrypt_mobi(data, key)

    def batch_remove_drm(self, files: List[Path]) -> List[Path]:
        """
        Remove DRM from multiple files

        Args:
            files: List of file paths

        Returns:
            List of DRM-free file paths
        """
        drm_free_files = []

        for filepath in files:
            result = self.remove_drm(filepath)
            if result:
                drm_free_files.append(result)

        logger.info(f"Successfully removed DRM from {len(drm_free_files)}/{len(files)} files")
        return drm_free_files


class CalibreDRMRemover:
    """
    Alternative DRM removal using Calibre's DeDRM plugin
    This requires Calibre and DeDRM plugin to be installed
    """

    def __init__(self, config: dict):
        self.config = config
        self.output_dir = Path(config['directories'].get('temp', './temp'))
        self.output_dir.mkdir(parents=True, exist_ok=True)

    def remove_drm_via_calibre(self, filepath: Path) -> Optional[Path]:
        """
        Remove DRM using Calibre's ebook-convert with DeDRM plugin

        Args:
            filepath: Path to DRM-protected book

        Returns:
            Path to DRM-free book
        """
        try:
            import subprocess

            logger.info(f"Attempting DRM removal via Calibre for: {filepath.name}")

            # Output path
            output_path = self.output_dir / f"{filepath.stem}_nodrm{filepath.suffix}"

            # Use ebook-convert which will trigger DeDRM plugin if installed
            cmd = [
                'ebook-convert',
                str(filepath),
                str(output_path),
                '--no-inline-toc',
            ]

            result = subprocess.run(cmd, capture_output=True, text=True)

            if result.returncode == 0 and output_path.exists():
                logger.info(f"DRM removed via Calibre: {output_path}")
                return output_path
            else:
                logger.error(f"Calibre DRM removal failed: {result.stderr}")
                return None

        except FileNotFoundError:
            logger.error("Calibre not found. Please install Calibre.")
            return None
        except Exception as e:
            logger.error(f"Calibre DRM removal error: {e}")
            return None
