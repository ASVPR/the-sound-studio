#!/usr/bin/env python3
"""Replace file headers in project-owned source files with standardized copyright header."""

import os
import re

SOURCE_DIR = os.path.join(os.path.dirname(__file__), '..', 'Codebase', 'TheSoundStudio', 'Source')

# Directories to exclude
EXCLUDE_DIRS = {'hiir', 'utilities'}

# Specific filenames to exclude
EXCLUDE_FILES = {'pluginconstants.h', 'synthfunctions.h', 'lookuptables.h', 'vConstants.h', 'LUTs.h'}

def make_header(filename):
    return f"""/*
  ==============================================================================

    {filename}

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/"""

# Pattern to match JUCE-style header blocks:
# /* followed by === line, content, === line, then */
HEADER_PATTERN = re.compile(
    r'/\*\s*\n\s*={10,}\s*\n.*?\n\s*={10,}\s*\n\s*\*/',
    re.DOTALL
)

def should_process(filepath):
    rel = os.path.relpath(filepath, SOURCE_DIR)
    parts = rel.split(os.sep)
    # Exclude specific directories
    if parts[0] in EXCLUDE_DIRS:
        return False
    basename = os.path.basename(filepath)
    if basename in EXCLUDE_FILES:
        return False
    return True

def process_file(filepath):
    basename = os.path.basename(filepath)
    with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
        content = f.read()

    new_header = make_header(basename)

    match = HEADER_PATTERN.match(content)
    if match:
        new_content = new_header + content[match.end():]
    else:
        # No existing header block — prepend
        new_content = new_header + '\n\n' + content

    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(new_content)
    return True

def main():
    count = 0
    source_dir = os.path.abspath(SOURCE_DIR)
    for root, dirs, files in os.walk(source_dir):
        # Skip excluded directories
        rel = os.path.relpath(root, source_dir)
        top_dir = rel.split(os.sep)[0]
        if top_dir in EXCLUDE_DIRS:
            continue

        for filename in sorted(files):
            if not (filename.endswith('.h') or filename.endswith('.cpp')):
                continue
            filepath = os.path.join(root, filename)
            if not should_process(filepath):
                continue
            if process_file(filepath):
                count += 1
                print(f"  Updated: {os.path.relpath(filepath, source_dir)}")

    print(f"\nTotal files updated: {count}")

if __name__ == '__main__':
    main()
