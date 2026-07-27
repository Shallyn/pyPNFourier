# pyPNFourier

`pyPNFourier` 用于计算 PN-elliptic integrals。Python 接口通过 `ctypes`
调用项目内的 C 动态库。

## 平台与依赖

目前已在以下环境完成实际编译和运行验证：

- macOS (Apple Silicon)
- Python 3.9（`/opt/homebrew/bin/python3.9`）
- Apple Clang + CMake

Linux 使用相同的 CMake 构建流程，代码和构建配置已兼容 `.so` 动态库，
但发布前仍建议在 Linux CI 或容器中实际测试。Windows 目前不作为支持平台：
C 源码仍使用 `pthread.h`、`unistd.h` 和 C99 complex，不能保证用 MSVC 编译。

构建需要：

- Python >= 3.9
- CMake >= 3.15
- C 编译器（macOS 为 Apple Clang，Linux 推荐 GCC 或 Clang）
- GSL
- FFTW3
- OpenMP（可选；未找到时会自动构建串行版本）

Python 依赖 `numpy` 和 `pathos`，安装本模块时会由 `pip` 自动处理。

## 安装系统依赖

### macOS（Homebrew）

```bash
brew install cmake pkg-config gsl fftw libomp
```

`libomp` 是可选的；不需要 OpenMP 并行时可以不安装。

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install build-essential cmake pkg-config libgsl-dev libfftw3-dev libomp-dev python3-dev
```

其他 Linux 发行版请安装名称对应的 CMake、GSL、FFTW3、C 编译器和
Python 开发包。

## 从源码安装

在项目根目录运行：

```bash
/opt/homebrew/bin/python3.9 -m venv .venv
source .venv/bin/activate
python -m pip install --upgrade pip
python -m pip install .
```

`pip install .` 会自动调用 CMake 编译 `libbasic` 和 `libpnFourier`，
不需要手动运行 `make`。

开发时如需可编辑安装：

```bash
python -m pip install -e .
```

## 手动编译 C 动态库

一般安装无需执行本节。若要单独检查 C 构建：

```bash
cmake -S pyPNFourier -B build/cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build/cmake --config Release
```

macOS 生成 `.dylib`，Linux 生成 `.so`。若要明确关闭 OpenMP：

```bash
cmake -S pyPNFourier -B build/cmake -DCMAKE_BUILD_TYPE=Release -DUSE_OPENMP=OFF
cmake --build build/cmake --config Release
```

## 运行示例：`J_pqa0`

当前公开接口名称是 `J_pqa0(p, q, a, e)`；项目中没有名为
`J_pab0` 的 Python 函数。

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

也可以直接在终端验证：

```bash
python -c "from pyPNFourier import J_pqa0; print(J_pqa0(2, 1, 0, 0.1))"
```

## 构建发布包

由于动态库依赖系统安装的 GSL 和 FFTW3，当前最稳妥的发布形式是源码包
（sdist）：

```bash
python -m pip install --upgrade build twine
python -m build --sdist
python -m twine check dist/*
```

检查无误后再上传：

```bash
python -m twine upload dist/*
```

如果要发布 macOS/Linux 二进制 wheel，需要分别在目标平台和 CPU 架构上
构建，并使用平台工具修复或打包 GSL/FFTW3 等动态库依赖；不能把本机生成的
Apple Silicon `.dylib` 直接作为跨平台 wheel 发布。
