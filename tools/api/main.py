# Copyright 2025 The Lynx Authors. All rights reserved.
# Licensed under the Apache License Version 2.0 that can be found in the
# LICENSE file in the root directory of this source tree.

# /usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import os
import sys
from api_dump import update_api_metadata, diff_api_metadata
from env_setup import guarantee_generated_files


def main():
    """API Metadata Management CLI

    Handles command-line interface for:
    - Updating reference API metadata files
    - Comparing generated metadata with references

    Command Line Arguments:
        --update (-u): Update mode - overwrites reference files
        --diff (-d): Diff mode - shows file differences
        --platform (-p): Target platform(s) [both|ios|android]

    Usage Examples:
        python main.py -u -p both    # Update both platforms
        python main.py -d -p ios     # Show iOS differences
        python main.py -u -p android # Update Android only
    """
    # Initialize argument parser
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-u", "--update", action="store_true", default=False, help="Update API metadata"
    )
    parser.add_argument(
        "-d", "--diff", action="store_true", default=False, help="Diff API metadata"
    )
    parser.add_argument(
        "-p",
        "--platform",
        choices=["both", "ios", "android"],
        default="both",
        help="Specify the platform",
    )

    # Process command line arguments
    args = parser.parse_args()
    ios_result = True
    android_result = True

    # Handle update operation
    if args.update:
        # Ensure generated files are up-to-date
        guarantee_generated_files()
        # iOS platform update
        if args.platform == "both" or args.platform == "ios":
            ios_result = update_api_metadata(
                os.path.dirname(os.path.abspath(__file__)), "ios"
            )
        # Android platform update
        if args.platform == "both" or args.platform == "android":
            android_result = update_api_metadata(
                os.path.dirname(os.path.abspath(__file__)), "android"
            )
        sys.exit(0 if (ios_result and android_result) else 1)

    # Handle diff operation
    if args.diff:
        # Ensure generated files are up-to-date
        guarantee_generated_files()
        # iOS platform comparison
        if args.platform == "both" or args.platform == "ios":
            ios_result = diff_api_metadata(
                os.path.dirname(os.path.abspath(__file__)), "ios"
            )
        # Android platform comparison
        if args.platform == "both" or args.platform == "android":
            android_result = diff_api_metadata(
                os.path.dirname(os.path.abspath(__file__)), "android"
            )
        sys.exit(0 if (ios_result and android_result) else 1)


if __name__ == "__main__":
    main()
