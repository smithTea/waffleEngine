#!/usr/bin/env python3

import argparse
import glob
import os
import re
from PIL import Image, ImageDraw


def natural_sort_key(path):
    """Sort image1, image2, image10 naturally."""
    name = os.path.basename(path)
    return [
        int(part) if part.isdigit() else part.lower()
        for part in re.split(r"(\d+)", name)
    ]


def contain_image(image, width, height, padding=0):
    """
    Resize without cropping or stretching.

    The complete source image remains visible. Empty space is filled black.
    """
    usable_width = max(1, width - padding * 2)
    usable_height = max(1, height - padding * 2)

    scale = min(
        usable_width / image.width,
        usable_height / image.height,
        )

    resized_width = max(1, round(image.width * scale))
    resized_height = max(1, round(image.height * scale))

    resized = image.resize(
        (resized_width, resized_height),
        Image.Resampling.LANCZOS,
    )

    panel = Image.new("RGB", (width, height), "black")

    x = (width - resized_width) // 2
    y = (height - resized_height) // 2

    panel.paste(resized, (x, y))
    return panel


def gradient_color(x, y, width, height):
    """
    Blue near the lower-left, magenta near the upper-right.
    """
    horizontal = x / max(1, width - 1)
    vertical = 1.0 - y / max(1, height - 1)

    amount = max(0.0, min(1.0, (horizontal + vertical) / 2.0))

    blue = (0, 105, 255)
    magenta = (255, 0, 145)

    return tuple(
        round(blue[i] * (1.0 - amount) + magenta[i] * amount)
        for i in range(3)
    )


def draw_gradient_rectangle(image, box, thickness):
    """Draw a gradient rectangular border."""
    left, top, right, bottom = box
    pixels = image.load()
    canvas_width, canvas_height = image.size

    for offset in range(thickness):
        x1 = left + offset
        y1 = top + offset
        x2 = right - offset
        y2 = bottom - offset

        if x1 > x2 or y1 > y2:
            break

        for x in range(x1, x2 + 1):
            pixels[x, y1] = gradient_color(
                x, y1, canvas_width, canvas_height
            )
            pixels[x, y2] = gradient_color(
                x, y2, canvas_width, canvas_height
            )

        for y in range(y1, y2 + 1):
            pixels[x1, y] = gradient_color(
                x1, y, canvas_width, canvas_height
            )
            pixels[x2, y] = gradient_color(
                x2, y, canvas_width, canvas_height
            )


def draw_gradient_vertical(image, x, y1, y2, thickness):
    pixels = image.load()
    canvas_width, canvas_height = image.size

    start_x = x - thickness // 2

    for px in range(start_x, start_x + thickness):
        if not 0 <= px < canvas_width:
            continue

        for y in range(y1, y2 + 1):
            if 0 <= y < canvas_height:
                pixels[px, y] = gradient_color(
                    px, y, canvas_width, canvas_height
                )


def draw_gradient_horizontal(image, y, x1, x2, thickness):
    pixels = image.load()
    canvas_width, canvas_height = image.size

    start_y = y - thickness // 2

    for py in range(start_y, start_y + thickness):
        if not 0 <= py < canvas_height:
            continue

        for x in range(x1, x2 + 1):
            if 0 <= x < canvas_width:
                pixels[x, py] = gradient_color(
                    x, py, canvas_width, canvas_height
                )


def main():
    parser = argparse.ArgumentParser(
        description="Combine three images into a bordered 3-panel JPEG."
    )

    parser.add_argument(
        "pattern",
        help='Input wildcard, for example "*.png" or "renders/*.png"',
    )
    parser.add_argument(
        "output",
        help="Output JPEG name, for example combined.jpg",
    )
    parser.add_argument(
        "--width",
        type=int,
        default=1408,
        help="Output width; default: 1408",
    )
    parser.add_argument(
        "--height",
        type=int,
        default=1056,
        help="Output height; default: 1056",
    )
    parser.add_argument(
        "--left-ratio",
        type=float,
        default=0.46,
        help="Width occupied by the left panel; default: 0.46",
    )
    parser.add_argument(
        "--top-ratio",
        type=float,
        default=0.50,
        help="Height occupied by the top-right panel; default: 0.50",
    )
    parser.add_argument(
        "--border",
        type=int,
        default=7,
        help="Gradient border thickness; default: 7",
    )
    parser.add_argument(
        "--padding",
        type=int,
        default=8,
        help="Black padding inside each panel; default: 8",
    )
    parser.add_argument(
        "--quality",
        type=int,
        default=95,
        help="JPEG quality; default: 95",
    )

    args = parser.parse_args()

    files = sorted(glob.glob(args.pattern), key=natural_sort_key)

    if len(files) != 3:
        raise SystemExit(
            f"Expected exactly 3 images matching {args.pattern!r}, "
            f"but found {len(files)}:\n"
            + "\n".join(files)
        )

    images = []

    for path in files:
        try:
            with Image.open(path) as source:
                images.append(source.convert("RGB"))
        except Exception as error:
            raise SystemExit(f"Could not open {path}: {error}") from error

    width = args.width
    height = args.height
    border = args.border

    if width <= border * 4 or height <= border * 4:
        raise SystemExit("Canvas dimensions are too small for the border.")

    canvas = Image.new("RGB", (width, height), "black")

    inner_left = border
    inner_top = border
    inner_right = width - border
    inner_bottom = height - border

    inner_width = inner_right - inner_left
    inner_height = inner_bottom - inner_top

    split_x = inner_left + round(inner_width * args.left_ratio)
    split_y = inner_top + round(inner_height * args.top_ratio)

    divider_half = border // 2

    left_box = (
        inner_left,
        inner_top,
        split_x - divider_half,
        inner_bottom,
    )

    top_right_box = (
        split_x + divider_half,
        inner_top,
        inner_right,
        split_y - divider_half,
    )

    bottom_right_box = (
        split_x + divider_half,
        split_y + divider_half,
        inner_right,
        inner_bottom,
    )

    boxes = [left_box, top_right_box, bottom_right_box]

    for source, box in zip(images, boxes):
        x1, y1, x2, y2 = box
        panel_width = x2 - x1
        panel_height = y2 - y1

        panel = contain_image(
            source,
            panel_width,
            panel_height,
            padding=args.padding,
        )

        canvas.paste(panel, (x1, y1))

    # Outer gradient border.
    draw_gradient_rectangle(
        canvas,
        (0, 0, width - 1, height - 1),
        border,
    )

    # Vertical divider.
    draw_gradient_vertical(
        canvas,
        split_x,
        border,
        height - border - 1,
        border,
        )

    # Horizontal divider only on the right.
    draw_gradient_horizontal(
        canvas,
        split_y,
        split_x,
        width - border - 1,
        border,
        )

    output = args.output

    if not output.lower().endswith((".jpg", ".jpeg")):
        output += ".jpg"

    canvas.save(
        output,
        format="JPEG",
        quality=args.quality,
        subsampling=0,
        optimize=True,
    )

    print("Input order:")
    print(f"  Left:         {files[0]}")
    print(f"  Top-right:    {files[1]}")
    print(f"  Bottom-right: {files[2]}")
    print(f"Saved: {output}")


if __name__ == "__main__":
    main()