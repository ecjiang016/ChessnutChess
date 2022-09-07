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
        extra_compile_args=["/O2", "/std:c++17"],
    )
), options={'build_ext':{'build_lib':'./GUI'}})