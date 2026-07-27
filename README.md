# pyPNFourier

`pyPNFourier` evaluates PN-elliptic integrals (arXiv:2604.25536). Its Python interface uses
`ctypes` to call the C shared libraries included in the project.

## Platforms and dependencies

The project has been compiled and tested in the following environment:

- macOS (Apple Silicon)
- Python 3.9 (`/opt/homebrew/bin/python3.9`)
- Apple Clang and CMake

Linux uses the same CMake build process, and the source and build configuration
support `.so` shared libraries. Testing in Linux CI or a Linux container is
still recommended before release.

Windows is not currently supported. The C source uses `pthread.h`, `unistd.h`,
and C99 complex numbers, so it is not guaranteed to compile with MSVC.

Build requirements:

- Python >= 3.9
- CMake >= 3.15
- A C compiler (Apple Clang on macOS; GCC or Clang recommended on Linux)
- GSL
- FFTW3
- OpenMP (optional; a serial version is built automatically when it is absent)

The Python dependencies are `numpy` and `pathos`. They are installed
automatically by `pip` when the package is installed.

## Installing system dependencies

### macOS (Homebrew)

```bash
brew install cmake pkg-config gsl fftw libomp
```

`libomp` is optional and may be omitted if OpenMP parallelism is not required.

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install build-essential cmake pkg-config libgsl-dev libfftw3-dev libomp-dev python3-dev
```

On other Linux distributions, install the equivalent packages for CMake, GSL,
FFTW3, a C compiler, and the Python development headers.

## Installing from source

Run the following commands from the project root:

```bash
/opt/homebrew/bin/python3.9 -m venv .venv
source .venv/bin/activate
python -m pip install --upgrade pip
python -m pip install .
```

`pip install .` invokes CMake automatically to compile `libbasic` and
`libpnFourier`; there is no need to run `make` manually.

For an editable development installation:

```bash
python -m pip install -e .
```

## Building the C shared libraries manually

This is not normally necessary during installation. To test the C build
separately, run:

```bash
cmake -S pyPNFourier -B build/cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build/cmake --config Release
```

The build produces `.dylib` files on macOS and `.so` files on Linux. To disable
OpenMP explicitly:

```bash
cmake -S pyPNFourier -B build/cmake -DCMAKE_BUILD_TYPE=Release -DUSE_OPENMP=OFF
cmake --build build/cmake --config Release
```

## Example: running `J_pqa0`

The current public function is named `J_pqa0(p, q, a, e)`. There is no Python
function named `J_pab0` in the project.

```python
from pyPNFourier import J_pqa0

value = J_pqa0(
    p=2,
    q=1,
    a=0,
    e=0.1,
)

print(value)
# 0.001248958658799919
```

The same example can be run directly from the terminal:

```bash
python -c "from pyPNFourier import J_pqa0; print(J_pqa0(2, 1, 0, 0.1))"
```

