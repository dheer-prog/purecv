#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image



def main() -> int:
    

    with Image.open("test_images/coins.png") as image:
        grayscale = image.convert("L")
        grayscale.save("test_images/coins_gray.png")

    print(f"Wrote grayscale image to")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
