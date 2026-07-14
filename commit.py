#!/usr/bin/env python3

from pathlib import Path
import re
import subprocess
import sys


def run(cmd):
    print(">", " ".join(cmd))
    subprocess.run(cmd, check=True)


def main():
    if len(sys.argv) < 2:
        print("Usage:")
        print('  python update_progress.py "Your commit message"')
        sys.exit(1)

    commit_message = sys.argv[1]

    progress = Path("progress")

    files = sorted(
        progress.glob("d*.png"),
        key=lambda p: int(re.search(r"\d+", p.stem).group())
    )

    with open("README.md", "w", encoding="utf-8") as f:
        f.write("# Project Progress\n\n")

        for img in files:
            day = re.search(r"\d+", img.stem).group()

            f.write(f"## Day {day} Progress\n\n")
            f.write("---\n\n")
            f.write(f"![Day {day}]({img.as_posix()})\n\n")

    # Stage everything
    run(["git", "add", "."])

    # Check if anything changed
    diff = subprocess.run(
        ["git", "diff", "--cached", "--quiet"]
    )

    if diff.returncode == 0:
        print("Nothing to commit.")
        return

    # Commit
    run(["git", "commit", "-m", commit_message])

    # Push
    run(["git", "push"])

    print("\nREADME updated and changes pushed.")


if __name__ == "__main__":
    main()