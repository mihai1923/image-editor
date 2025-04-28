A command-line image processing tool supporting PNM formats (PPM, PGM, PBM) with various manipulation features.

![C](https://img.shields.io/badge/C-100%25-blue?logo=c)

## Features

- **Supported Formats**: P1-P6 (ASCII & binary)
- **Image Manipulation**:
  - Rotation (90°, 180°, 270°)
  - Cropping
  - Region selection
- **Filters**:
  - Edge detection
  - Sharpen
  - Blur
  - Gaussian blur
- **Adjustments**:
  - Histogram equalization
  - Histogram visualization
- **Memory Management**: Robust allocation/freeing of image matrices

## Installation

1. Clone repository:
   ```bash
   git clone https://github.com/mihai1923/image_editor
   cd image_editor
   ```

2. Compile with GCC:
  ```bash
  gcc image_editor.c -o image_editor -lm
  ```
## Usage
  ```shell
  ./image_editor
  ```
## Commands
| Command     | Parameters                        | Description                      |
|------------|----------------------------------|----------------------------------|
| **LOAD**    | `filename`                        | Load image file                 |
| **SELECT**  | `ALL` or `x1 y1 x2 y2`           | Select region                   |
| **CROP**    | `-`                              | Crop to selected region         |
| **APPLY**   | `[EDGE\|SHARPEN\|BLUR\|GAUSSIAN_BLUR]` | Apply filter                    |
| **ROTATE**  | `[90\|180\|270]`                 | Rotate image/selection          |
| **SAVE**    | `filename [ascii]`               | Save image (optional ASCII format) |
| **HISTOGRAM** | `max_stars bins`               | Generate histogram              |
| **EQUALIZE** | `-`                              | Equalize image histogram        |
| **EXIT**    | `-`                              | Quit program                    |

## Dependencies

- Standard C Library (stdio, stdlib, string, math)
- No external dependencies

## Example Usage

1. Load and process an image:

```shell
LOAD input.ppm
SELECT 100 100 300 300
APPLY EDGE
ROTATE 90
SAVE output.ppm
```
2. Generate histogram:
```shell
LOAD image.pgm
HISTOGRAM 64 8
```
