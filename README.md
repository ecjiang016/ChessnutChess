# Overview
A bitboard based move generator for Chess.
Currently at 30 million nps.

Compile by running `make` in command line.

## Usage:
ChessnutChess only supports these UCI commands at this time:

Angle brackets (`<>`) indicates an argument to be placed there by the user.

### Displaying the current board:
```
d
```

### Loading a fen:
```
position fen <fen>
```

### Running a perft from the current position:
```
go perft <depth>
```

### Quitting:
```
quit
```

# GUI
The GUI is coded in Python with Cython bindings to the C++ move gen.

## Prerequisites
NumPy, Pygame, and Cython are needed to run the GUI. Install the packages through command line using:
```
python3 -m pip install numpy pygame Cython
```

## Compliation

To compile run `make cython` in command line. If that doesn't work, run `python3 GUI/setup.py build_ext` instead.

You may need to change the compiler flags in `setup.py` depending on which compiler Cython decides to use. <br>
If Cython is using g++ or clang, change the contents of `setup.py` to:
```python
from setuptools import setup, Extension
from Cython.Build import cythonize

setup(ext_modules=cythonize(
    Extension(
        "chess",
        ["./GUI/chess.pyx",
        "./src/masks.cpp",
        "./src/bits.cpp",
        "./src/piece.cpp",
        "./src/game.cpp"
        ],
        language="c++",
        extra_compile_args=["-O3", "std=c++17"],
    )
), options={'build_ext':{'build_lib':'./GUI'}})
```

## Run
Run `main.py` to play Chess!

# Details
Moves are generated using pre-computed masks generated by a Python script, `masks.py`.

Sliding piece moves are generated on the spot using the pre-computed masks and hyperbola quintessence algorithm.

Takes inspiration from nkarve's [surge](https://github.com/nkarve/surge)