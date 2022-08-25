from setuptools import setup
from Cython.Build import cythonize

setup(ext_modules=cythonize('./src/GUI/pychess.pyx'), options={'build_ext':{'build_lib':'./src/GUI'}})