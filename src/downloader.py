"""
Kindle Book Downloader Module
Downloads Kindle books from Amazon account
"""

import os
import json
import time
import logging
from pathlib import Path
from typing import List, Dict, Optional
import requests
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.chrome.options import Options
from selenium.common.exceptions import TimeoutException, NoSuchElementException

logger = logging.getLogger(__name__)


class KindleDownloader:
    """Downloads Kindle books from Amazon"""

    AMAZON_DOMAINS = {
        'US': 'amazon.com',
        'UK': 'amazon.co.uk',
        'DE': 'amazon.de',
        'FR': 'amazon.fr',
        'ES': 'amazon.es',
        'IT': 'amazon.it',
        'JP': 'amazon.co.jp',
        'CA': 'amazon.ca',
        'AU': 'amazon.com.au',
    }

    def __init__(self, config: Dict):
        """
        Initialize the Kindle downloader

        Args:
            config: Configuration dictionary with Amazon credentials and settings
        """
        self.email = config['amazon']['email']
        self.password = config['amazon']['password']
        self.region = config['amazon'].get('region', 'US')
        self.download_dir = Path(config['directories']['download'])
        self.download_dir.mkdir(parents=True, exist_ok=True)

        self.domain = self.AMAZON_DOMAINS.get(self.region, 'amazon.com')
        self.base_url = f"https://www.{self.domain}"
        self.session = requests.Session()
        self.driver = None

    def setup_driver(self, headless: bool = True) -> webdriver.Chrome:
        """Setup Chrome WebDriver for Amazon login"""
        chrome_options = Options()
        if headless:
            chrome_options.add_argument('--headless')
        chrome_options.add_argument('--no-sandbox')
        chrome_options.add_argument('--disable-dev-shm-usage')
        chrome_options.add_argument('--disable-blink-features=AutomationControlled')
        chrome_options.add_experimental_option("excludeSwitches", ["enable-automation"])
        chrome_options.add_experimental_option('useAutomationExtension', False)

        # Set download directory
        prefs = {
            "download.default_directory": str(self.download_dir.absolute()),
            "download.prompt_for_download": False,
            "download.directory_upgrade": True,
            "safebrowsing.enabled": True
        }
        chrome_options.add_experimental_option("prefs", prefs)

        driver = webdriver.Chrome(options=chrome_options)
        driver.execute_script("Object.defineProperty(navigator, 'webdriver', {get: () => undefined})")

        return driver

    def login(self, interactive: bool = False) -> bool:
        """
        Login to Amazon account

        Args:
            interactive: If True, don't use headless mode (for CAPTCHA)

        Returns:
            bool: True if login successful
        """
        logger.info(f"Logging in to Amazon {self.region}...")

        self.driver = self.setup_driver(headless=not interactive)

        try:
            # Navigate to Kindle library
            self.driver.get(f"{self.base_url}/hz/mycd/digital-console/contentlist/booksAll/dateDsc/")
            time.sleep(2)

            # Check if already logged in
            if "sign" in self.driver.current_url.lower() or "login" in self.driver.current_url.lower():
                # Find and fill email
                try:
                    email_field = WebDriverWait(self.driver, 10).until(
                        EC.presence_of_element_located((By.ID, "ap_email"))
                    )
                    email_field.clear()
                    email_field.send_keys(self.email)

                    # Click continue
                    continue_btn = self.driver.find_element(By.ID, "continue")
                    continue_btn.click()
                    time.sleep(2)
                except (TimeoutException, NoSuchElementException):
                    # Try alternative email field
                    email_field = self.driver.find_element(By.NAME, "email")
                    email_field.clear()
                    email_field.send_keys(self.email)

                # Find and fill password
                try:
                    password_field = WebDriverWait(self.driver, 10).until(
                        EC.presence_of_element_located((By.ID, "ap_password"))
                    )
                    password_field.clear()
                    password_field.send_keys(self.password)

                    # Click sign in
                    signin_btn = self.driver.find_element(By.ID, "signInSubmit")
                    signin_btn.click()
                    time.sleep(3)
                except (TimeoutException, NoSuchElementException) as e:
                    logger.error(f"Could not find password field: {e}")
                    return False

                # Handle 2FA or CAPTCHA if needed
                if interactive:
                    logger.info("If 2FA or CAPTCHA is required, please complete it manually...")
                    logger.info("Waiting 60 seconds for manual intervention...")
                    time.sleep(60)

            # Verify login
            if "digital-console" in self.driver.current_url or "myk" in self.driver.current_url:
                logger.info("Login successful!")

                # Store cookies for requests session
                for cookie in self.driver.get_cookies():
                    self.session.cookies.set(cookie['name'], cookie['value'])

                return True
            else:
                logger.error(f"Login failed. Current URL: {self.driver.current_url}")
                return False

        except Exception as e:
            logger.error(f"Login error: {e}")
            return False

    def get_kindle_library(self) -> List[Dict]:
        """
        Get list of all Kindle books in library

        Returns:
            List of book dictionaries with metadata
        """
        logger.info("Fetching Kindle library...")
        books = []

        try:
            # Navigate to Kindle library
            self.driver.get(f"{self.base_url}/hz/mycd/digital-console/contentlist/booksAll/dateDsc/")
            time.sleep(3)

            # Scroll to load all books (lazy loading)
            last_height = self.driver.execute_script("return document.body.scrollHeight")
            while True:
                self.driver.execute_script("window.scrollTo(0, document.body.scrollHeight);")
                time.sleep(2)
                new_height = self.driver.execute_script("return document.body.scrollHeight")
                if new_height == last_height:
                    break
                last_height = new_height

            # Parse book elements
            book_elements = self.driver.find_elements(By.CSS_SELECTOR, "[data-asin]")

            for element in book_elements:
                try:
                    asin = element.get_attribute("data-asin")
                    if not asin:
                        continue

                    # Extract book details
                    title_elem = element.find_element(By.CSS_SELECTOR, ".library-item-title, h2")
                    title = title_elem.text.strip()

                    try:
                        author_elem = element.find_element(By.CSS_SELECTOR, ".library-item-author, .author")
                        author = author_elem.text.strip()
                    except NoSuchElementException:
                        author = "Unknown"

                    book_info = {
                        'asin': asin,
                        'title': title,
                        'author': author,
                        'url': f"{self.base_url}/dp/{asin}"
                    }

                    books.append(book_info)
                    logger.debug(f"Found book: {title} by {author}")

                except Exception as e:
                    logger.debug(f"Error parsing book element: {e}")
                    continue

            logger.info(f"Found {len(books)} books in library")
            return books

        except Exception as e:
            logger.error(f"Error fetching library: {e}")
            return []

    def download_book(self, book: Dict) -> Optional[Path]:
        """
        Download a single Kindle book

        Args:
            book: Book dictionary with metadata

        Returns:
            Path to downloaded file, or None if failed
        """
        logger.info(f"Downloading: {book['title']}")

        try:
            # Try to download via "Download & transfer via USB" option
            download_url = f"{self.base_url}/hz/mycd/ajax/downloadDeviceReaderBook?asin={book['asin']}"

            # Use the authenticated session
            headers = {
                'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'
            }

            response = self.session.get(download_url, headers=headers, stream=True)

            if response.status_code == 200:
                # Determine file extension from content-type or default to .azw
                content_type = response.headers.get('content-type', '')
                if 'azw3' in content_type:
                    ext = '.azw3'
                elif 'mobi' in content_type:
                    ext = '.mobi'
                else:
                    ext = '.azw'

                # Sanitize filename
                safe_title = "".join(c for c in book['title'] if c.isalnum() or c in (' ', '-', '_')).strip()
                filename = f"{safe_title}{ext}"
                filepath = self.download_dir / filename

                # Save file
                with open(filepath, 'wb') as f:
                    for chunk in response.iter_content(chunk_size=8192):
                        if chunk:
                            f.write(chunk)

                logger.info(f"Downloaded: {filepath}")

                # Save metadata
                metadata_file = filepath.with_suffix('.json')
                with open(metadata_file, 'w') as f:
                    json.dump(book, f, indent=2)

                return filepath
            else:
                logger.warning(f"Download failed with status {response.status_code}")

                # Alternative: Try using Selenium to click download button
                return self._download_via_selenium(book)

        except Exception as e:
            logger.error(f"Error downloading {book['title']}: {e}")
            return None

    def _download_via_selenium(self, book: Dict) -> Optional[Path]:
        """Fallback method to download using Selenium"""
        try:
            logger.info("Trying Selenium download method...")

            # Navigate to book details page
            self.driver.get(book['url'])
            time.sleep(2)

            # Look for download button
            download_button = None
            selectors = [
                "//a[contains(text(), 'Download')]",
                "//button[contains(text(), 'Download')]",
                "//a[contains(@class, 'download')]",
                "//*[contains(text(), 'Download & transfer via USB')]"
            ]

            for selector in selectors:
                try:
                    download_button = self.driver.find_element(By.XPATH, selector)
                    break
                except NoSuchElementException:
                    continue

            if download_button:
                download_button.click()
                time.sleep(5)

                # Find downloaded file
                downloads = list(self.download_dir.glob("*"))
                if downloads:
                    latest_file = max(downloads, key=os.path.getctime)
                    logger.info(f"Downloaded via Selenium: {latest_file}")
                    return latest_file

            logger.warning(f"Could not download {book['title']} via Selenium")
            return None

        except Exception as e:
            logger.error(f"Selenium download error: {e}")
            return None

    def download_all(self, book_titles: Optional[List[str]] = None) -> List[Path]:
        """
        Download all books or specific books from library

        Args:
            book_titles: Optional list of specific book titles to download

        Returns:
            List of paths to downloaded files
        """
        books = self.get_kindle_library()
        downloaded_files = []

        if book_titles:
            # Filter for specific titles
            books = [b for b in books if b['title'] in book_titles]
            logger.info(f"Downloading {len(books)} requested books...")
        else:
            logger.info(f"Downloading all {len(books)} books...")

        for book in books:
            filepath = self.download_book(book)
            if filepath:
                downloaded_files.append(filepath)
            time.sleep(2)  # Be nice to Amazon's servers

        logger.info(f"Successfully downloaded {len(downloaded_files)} books")
        return downloaded_files

    def close(self):
        """Clean up resources"""
        if self.driver:
            self.driver.quit()
        self.session.close()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
