#!/usr/bin/env python3
"""Validate the TSS codebase for professional release readiness."""

import os
import re
import sys

BASE_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
SOURCE_DIR = os.path.join(BASE_DIR, 'Codebase', 'TheSoundStudio', 'Source')

# Directories to exclude from checks (third-party / generated)
EXCLUDE_DIRS = {'hiir', 'utilities'}

# Files to exclude from header checks (third-party)
EXCLUDE_FILES = {'pluginconstants.h', 'synthfunctions.h', 'lookuptables.h', 'vConstants.h', 'LUTs.h'}

EXPECTED_COPYRIGHT = 'Copyright (c) 2026 Ziv Elovitch. All rights reserved.'

errors = []

def is_project_file(filepath):
    """Check if a file is project-owned (not third-party/excluded)."""
    rel = os.path.relpath(filepath, SOURCE_DIR)
    parts = rel.split(os.sep)
    if parts[0] in EXCLUDE_DIRS:
        return False
    if os.path.basename(filepath) in EXCLUDE_FILES:
        return False
    return True

def get_project_files():
    """Get all project-owned .h and .cpp files."""
    files = []
    for root, dirs, filenames in os.walk(SOURCE_DIR):
        rel = os.path.relpath(root, SOURCE_DIR)
        top_dir = rel.split(os.sep)[0]
        if top_dir in EXCLUDE_DIRS:
            continue
        for f in sorted(filenames):
            if f.endswith('.h') or f.endswith('.cpp'):
                path = os.path.join(root, f)
                if is_project_file(path):
                    files.append(path)
    return files

def check_pragma_once():
    """Check 1: All project .h files have #pragma once."""
    print("Check 1: #pragma once in all project .h files...")
    count = 0
    for filepath in get_project_files():
        if not filepath.endswith('.h'):
            continue
        with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
            content = f.read()
        if '#pragma once' not in content:
            rel = os.path.relpath(filepath, BASE_DIR)
            errors.append(f"  MISSING #pragma once: {rel}")
            count += 1
    if count == 0:
        print("  PASS")
    else:
        print(f"  FAIL ({count} files missing #pragma once)")

def check_no_hardcoded_paths():
    """Check 2: No hardcoded /Users/ paths in source files."""
    print("Check 2: No hardcoded /Users/ paths...")
    count = 0
    for filepath in get_project_files():
        with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
            for i, line in enumerate(f, 1):
                if '/Users/' in line:
                    rel = os.path.relpath(filepath, BASE_DIR)
                    errors.append(f"  HARDCODED PATH: {rel}:{i}: {line.strip()}")
                    count += 1
    if count == 0:
        print("  PASS")
    else:
        print(f"  FAIL ({count} hardcoded paths found)")

def check_copyright_headers():
    """Check 3: All project files have correct copyright header."""
    print("Check 3: Correct copyright header on all project files...")
    count = 0
    for filepath in get_project_files():
        with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
            content = f.read(500)  # Only need to check the top
        if EXPECTED_COPYRIGHT not in content:
            rel = os.path.relpath(filepath, BASE_DIR)
            errors.append(f"  MISSING COPYRIGHT: {rel}")
            count += 1
    if count == 0:
        print("  PASS")
    else:
        print(f"  FAIL ({count} files missing correct copyright)")

def check_no_bak_files():
    """Check 4: No .bak files remain in the project."""
    print("Check 4: No .bak files...")
    count = 0
    for root, dirs, files in os.walk(BASE_DIR):
        if '.git' in root:
            continue
        for f in files:
            if '.bak' in f:
                rel = os.path.relpath(os.path.join(root, f), BASE_DIR)
                errors.append(f"  BAK FILE: {rel}")
                count += 1
    if count == 0:
        print("  PASS")
    else:
        print(f"  FAIL ({count} .bak files found)")

def check_notes_deleted():
    """Check 5: notes.h has been deleted."""
    print("Check 5: notes.h deleted...")
    notes_path = os.path.join(SOURCE_DIR, 'notes.h')
    if os.path.exists(notes_path):
        errors.append(f"  notes.h still exists: {notes_path}")
        print("  FAIL")
    else:
        print("  PASS")

def check_no_old_authors():
    """Check 6: No 'Author: Gary Jones' or 'Author: Akash Murthy' in project files."""
    print("Check 6: No old author attributions in project files...")
    count = 0
    pattern = re.compile(r'Author:\s*(Gary Jones|Akash Murthy)', re.IGNORECASE)
    for filepath in get_project_files():
        with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
            for i, line in enumerate(f, 1):
                if pattern.search(line):
                    rel = os.path.relpath(filepath, BASE_DIR)
                    errors.append(f"  OLD AUTHOR: {rel}:{i}: {line.strip()}")
                    count += 1
    if count == 0:
        print("  PASS")
    else:
        print(f"  FAIL ({count} old author references found)")

def main():
    print("=" * 60)
    print("TSS Codebase Validation")
    print("=" * 60)
    print()

    check_pragma_once()
    check_no_hardcoded_paths()
    check_copyright_headers()
    check_no_bak_files()
    check_notes_deleted()
    check_no_old_authors()

    print()
    print("=" * 60)
    if errors:
        print(f"VALIDATION FAILED — {len(errors)} issue(s) found:")
        print()
        for e in errors:
            print(e)
        sys.exit(1)
    else:
        print("ALL CHECKS PASSED")
        sys.exit(0)

if __name__ == '__main__':
    main()
