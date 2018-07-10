# Simulix

[FMU](https://en.wikipedia.org/wiki/Functional_Mock-up_Interface) builder for [simulink](https://mathworks.com/products/simulink.html) models.

## Content
* [License](#license)
* [Usage](#usage)
  * [Download and Installation](#download-and-installation)
  * [Running Simulix](#running-simulix)
* [Currently Work In Progress](#currently-work-in-progress)

## License

GNU GPL V.3

>Simulix generates an FMU from a simulink model source code.
> 
>Copyright (C) 2018 Scania and Simulix contributors
>
>This program is free software: you can redistribute it and/or modify
>it under the terms of the GNU General Public License as published by
>the Free Software Foundation, either version 3 of the License, or
>(at your option) any later version.
>
>This program is distributed in the hope that it will be useful,
>but WITHOUT ANY WARRANTY; without even the implied warranty of
>MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>GNU General Public License for more details.
>
>You should have received a copy of the GNU General Public License
>along with this program.  If not, see <http://www.gnu.org/licenses/>.

## Usage

### Download and Installation

In order to use Simulix you can either

1. **Clone the GitHub repository via git**:

    ```sh
    git clone https://github.com/Kvixen/Simulix.git
    cd Simulix
    ```

2. **Download the GitHub repository zip**:

    ```sh
    Click the green 'Clone or Download' button on the repo
    Click 'Download ZIP'
    ```


To use Simulix, you will need the following
* [Python](https://www.python.org/).
* A [CMake](https://cmake.org/) installation, version 3 or above.
* A C compiler compatible with CMake. We recommend [GCC](https://gcc.gnu.org/)

Note that you have to add the installation directory to your PATH variable if you don't want to specify the installation path every time you want to run Simulix.

### Running Simulix

Simulix comes with certain flags
* `z:             Positional argument for name of zipfile`
* `-p:            Path for building`
* `-t:            Path to template files`
* `-zp:           Path to zipfile`
* `-g:            Makefile generator`
* `-m:            Makefile program`
* `--ONLY_BUILD:  Not generate new files, only build existing`

Here are some examples of executing Simulix

The model zip file is called *example.zip* and exists in the current directory

    Simulix.py example

The model zip file exists in a subfolder called *zip* and I want to build in a subfolder called *generated*
    
    mkdir generated
    Simulix.py example -p generated -zp zip
    

# Currently Work In Progress

Almost everything in this project is currently work in progress, even the project name. Feel free to contribute all you want. 
