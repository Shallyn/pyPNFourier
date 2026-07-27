#!/usr/bin/env python3
"""Build and package pyPNFourier."""

import os
import shutil
import subprocess
from pathlib import Path

from distutils.command.clean import clean
from setuptools import find_packages, setup
from setuptools.command.bdist_wheel import bdist_wheel
from setuptools.command.build_py import build_py


class BuildCMake(build_py):
    """Compile the C libraries with CMake before building the Python package."""

    def run(self):
        root_directory = Path(__file__).resolve().parent
        self.module_directory = root_directory / "pyPNFourier"
        build_command = self.get_finalized_command("build")
        self.cmake_build_directory = Path(build_command.build_temp) / "cmake"
        self.cmake_build_directory.mkdir(parents=True, exist_ok=True)

        config = "Release"
        subprocess.check_call([
            "cmake",
            "-S", str(self.module_directory),
            "-B", str(self.cmake_build_directory),
            f"-DCMAKE_BUILD_TYPE={config}",
        ])
        subprocess.check_call([
            "cmake",
            "--build", str(self.cmake_build_directory),
            "--config", config,
        ])

        super().run()
        self.copy_libraries()

    def copy_libraries(self):
        for library_directory in (
            self.cmake_build_directory / "libbasic",
            self.cmake_build_directory / "libpnfourier",
        ):
            library_files = [
                path
                for path in library_directory.iterdir()
                if path.suffix.lower() in {".so", ".dylib", ".dll"}
            ]
            if not library_files:
                raise FileNotFoundError(
                    f"No shared library was produced in {library_directory}"
                )

            target_directory = (
                Path(self.build_lib) / "pyPNFourier" / library_directory.name
            )
            target_directory.mkdir(parents=True, exist_ok=True)
            for library_file in library_files:
                shutil.copy2(library_file, target_directory / library_file.name)


class CleanCommand(clean):
    def run(self):
        for directory in ("build", "dist"):
            if os.path.exists(directory):
                shutil.rmtree(directory)
        super().run()


class PlatformWheel(bdist_wheel):
    """Mark ctypes wheels as platform-specific but Python-ABI independent."""

    def finalize_options(self):
        super().finalize_options()
        self.root_is_pure = False

    def get_tag(self):
        _, _, platform_tag = super().get_tag()
        return "py3", "none", platform_tag


setup(
    name="pyPNFourier",
    version="1.0.0",
    packages=find_packages(),
    cmdclass={
        "bdist_wheel": PlatformWheel,
        "build_py": BuildCMake,
        "clean": CleanCommand,
    },
    install_requires=["numpy>=1.21", "pathos>=0.2.8"],
    author="Xiaolin Liu",
    author_email="shallyn.liu@foxmail.com",
    description="Evaluate PN-elliptic integrals",
    long_description=Path("README.md").read_text(encoding="utf-8"),
    long_description_content_type="text/markdown",
    classifiers=[
        "Programming Language :: Python :: 3",
        "Operating System :: MacOS",
        "Operating System :: POSIX :: Linux",
    ],
    python_requires=">=3.9",
    include_package_data=True,
    package_data={
        "pyPNFourier": [
            "libbasic/*.so",
            "libbasic/*.dylib",
            "libbasic/*.dll",
            "libpnfourier/*.so",
            "libpnfourier/*.dylib",
            "libpnfourier/*.dll",
        ],
    },
    zip_safe=False,
)
