"""
Firmware Versioning and Copy Script

This script:
1. Locates the latest .bin firmware file
2. Extracts version and device name from version.h
3. Creates a versioned filename
4. Copies the firmware with new naming convention

Usage: python3 generate_firmware.py
"""

import os
import re
import sys
import shutil
from datetime import datetime
from pathlib import Path

def find_bin_file(bin_dir: Path) -> Path:
    """
    Find the first .bin file in the specified directory.
    
    Args:
        bin_dir: Directory to search for .bin files
        
    Returns:
        Path to the found .bin file
        
    Raises:
        FileNotFoundError: If no .bin file is found
    """
    bin_files = list(bin_dir.glob("*.bin"))
    
    if not bin_files:
        raise FileNotFoundError(f"No .bin files found in {bin_dir}")
    
    # Return the first .bin file found
    return bin_files[0]

def extract_define_from_header(file_path: Path, define_name: str) -> str:
    """
    Extract value from #define in C header file.
    
    Args:
        file_path: Path to the header file
        define_name: Name of the #define to extract
        
    Returns:
        Extracted value (without quotes)
        
    Raises:
        FileNotFoundError: If header file doesn't exist
        ValueError: If #define is not found
    """
    if not file_path.exists():
        raise FileNotFoundError(f"Header file not found: {file_path}")
    
    # Pattern to match #define FIRMWARE_VERSION "x.y.z"
    pattern = rf'^#define\s+{define_name}\s+"([^"]+)"'
    
    with open(file_path, 'r', encoding='utf-8') as f:
        for line in f:
            match = re.search(pattern, line)
            if match:
                return match.group(1)
    
    raise ValueError(f"#define {define_name} not found in {file_path}")

def generate_filename(base_name: str, device_name: str, version: str) -> str:
    """
    Generate formatted firmware filename.
    
    Args:
        base_name: Base name for the firmware
        device_name: Device identifier
        version: Firmware version string
        
    Returns:
        Formatted filename: BaseName_Device_vX.Y.Z.bin
    """
    # Remove any invalid filename characters
    safe_device = re.sub(r'[^\w\-]', '_', device_name)
    
    return f"{base_name}_{safe_device}_v{version}.bin"

def main():
    """Main execution function."""
    try:
        # Get script directory and set up paths
        script_dir = Path(__file__).parent.absolute()
        
        # Project directory structure
        bin_dir = script_dir.parent / "STM32CubeIDE" / "Debug"
        version_dir = script_dir.parent / "STM32CubeIDE" / "Drivers" / "libs"
        version_file = version_dir / "version.h"
        
        # Check if directories exist
        if not bin_dir.exists():
            raise FileNotFoundError(f"Binary directory not found: {bin_dir}")
        
        if not version_dir.exists():
            raise FileNotFoundError(f"Version directory not found: {version_dir}")
        
        # 1. Find the firmware .bin file
        print(f"\nSearching for .bin files")
        bin_file = find_bin_file(bin_dir)
        print(f"Found: {bin_file.name}")
        
        # 2. Extract version information from version.h
        print(f"Reading version information")
        
        firmware_version = extract_define_from_header(version_file, "FIRMWARE_VERSION")
        device_name = extract_define_from_header(version_file, "BIN_DEST_NAME")
        
        print(f"   Firmware Version: {firmware_version}")
        print(f"   Device Name:      {device_name}")
        
        # 3. Generate new filename
        base_name = "Prod"  # You can make this configurable if needed
        new_filename = generate_filename(base_name, device_name, firmware_version)
        new_filepath = script_dir / new_filename
        
        # 4. Copy the file with new name
        print(f"Copying firmware file...")
        shutil.copy2(bin_file, new_filepath)
        
        # 5. Display summary
        print("\n   Firmware file successfully processed:")
        print(f"   Source:      {bin_file}")
        print(f"   Destination: {new_filepath}")
        print(f"   File size:   {os.path.getsize(new_filepath):,} bytes")
        print(f"   Timestamp:   {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        
        return 0
        
    except FileNotFoundError as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1
    except ValueError as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1
    except PermissionError as e:
        print(f"Permission error: {e}", file=sys.stderr)
        return 1
    except Exception as e:
        print(f"Unexpected error: {type(e).__name__}: {e}", file=sys.stderr)
        return 1

if __name__ == "__main__":
    sys.exit(main())