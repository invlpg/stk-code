# IPC client for controlling a SuperTuxKart player / observing its environment
# Copyright (C) 2020 Carl Albrecht <carl.albrecht@griffithuni.edu.au>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.


import setuptools


with open("README.md", "r") as readme:
    long_description = readme.read()

setuptools.setup(
    name="stkremote",
    version="0.0.1",
    author="Carl Albrecht",
    author_email="invlpg@nocryp.to",
    description="IPC client for controlling a SuperTuxKart player / observing its environment",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/invlpg/stk-code",
    packages=setuptools.find_packages(),
    python_requires=">=3.6",
    install_requires=[],
    classifiers=[
        "Development Status :: 3 - Alpha"
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX"
    ]
)
