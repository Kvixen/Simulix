# Simulix

[FMU](https://en.wikipedia.org/wiki/Functional_Mock-up_Interface) builder for [Simulink](https://mathworks.com/products/simulink.html) models using the C-API.

## Content
* [License](#license)
* [Usage](#usage)
  * [Download and Installation](#download-and-installation)
    * [Windows](#windows)
      * [Dependencies](#w-dependencies)
      * [Installation](#w-installation)
    * [Linux](#linux)
      * [Dependencies](#l-dependencies)
      * [Installation](#l-installation)
  * [Exporting your model from Simulink](#exporting-your-model-from-simulink)
  * [Running Simulix](#running-simulix)

## License

GNU GPL V.3

>Simulix generates an FMU from a simulink model source code.
> 
>Copyright (C) 2018 Scania CV AB and Simulix contributors
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

## Download and Installation

## Windows

<div id='w-dependencies'/>

### Dependencies

To use Simulix, you will need the following
* [Python 3](https://www.python.org/).
  * [lxml - Python package](https://lxml.de/)
* A [CMake](https://cmake.org/) installation, version 3.3 or above.
* A C compiler compatible with CMake. We recommend [GCC](https://gcc.gnu.org/), but also works with [Visual Studio](https://visualstudio.microsoft.com/vs/) 2010 or above.

<div id='w-installation'/>

### Windows Installation
In order to use Simulix you can either

1. **Clone the GitHub repository via git**:

    ```sh
    git clone https://github.com/Kvixen/Simulix.git
    cd Simulix
    pip install -r requirements.txt
    ```

<div id='linux'/>

## Linux

<div id='l-dependencies'/>

### Linux Dependencies

To use Simulix, you need the following
* Python 3
  * Lxml - Python package
* CMake

Optional for cross compiling to Windows
* mingw-w64

<div id='l-installation'/>

### Linux Installation

1. **Install the dependencies**:

    ```sh
    sudo apt-get --assume-yes python
    sudo apt-get --assume-yes cmake
    ```

2. **(OPTIONAL) If you want to be able to cross compile**:

    ```sh
    sudo apt-get --assume-yes mingw-w64
    ```

3. **Clone the github repository**:

    ```sh
    git clone https://github.com/Kvixen/Simulix.git
    cd Simulix
    pip install -r requirements.txt
    ```

## Exporting your model from Simulink

In order for Simulix to function correctly, the correct code generation options need to be selected in Simulink.

![Options](https://github.com/Kvixen/Simulix/blob/master/resources/Options.PNG)

In Simulink, the code generation options are located under *Code, C/C++ Code, Code Generation Options*.

![CodeG_list](https://github.com/Kvixen/Simulix/blob/master/resources/CodeG_list.PNG) ![CodeG](https://github.com/Kvixen/Simulix/blob/master/resources/CodeG.PNG)

In *Code Generation*, under *Target selection*, make sure that *System target file* is set to *grt.tlc* and that *Language* is set to *C*.

Under *Build process*, select *Package code and artifacts*. You can also fill in the name of the zip file that will be generated. If the field is left empty, the zip file will have the name of the Simulink model.

![Interface_list](https://github.com/Kvixen/Simulix/blob/master/resources/Interface_list.PNG) ![Interface](https://github.com/Kvixen/Simulix/blob/master/resources/Interface.PNG)

In the submenu *Interface* to *Code Generation*, make sure all boxes for C API generation are ticked.

To build your model, simply press *Ctrl+B* on your keyboard.

## Running Simulix

Simulix comes with certain flags

*Requires Arguments:*
* ` ZN:           Positional argument for name of zipfile`
* `-p :           Path for generating files and build folder`
* `-t :           Path to templates and includes folders (if not Simulix installation folder)`
* `-zp:           Path to zipfile`
* `-m :           Makefile program`
* `-f :           Build folder name`
* `-tf:           Name of temp folder`

*Just specify flag, not arguments:*
* `--DEBUG_BUILD: Do not build release configuration, but build debug configuration`
* `--ONLY_BUILD : Do not regenerate files, only build existing`
* `--NO-TEMP    : Do not delete the temp folder`
* `--NO-CMAKE   : Do not execute CMAKE`
* `--NO-MAKE    : Do not execute MAKE program`

Here are some examples of executing Simulix

The model zip file is called *example.zip* and exists in the current directory

    Simulix.py example

The model zip file exists in a subfolder called *zip* and I want to build in a subfolder called *generated*
    
    mkdir generated
    Simulix.py example -p generated -zp zip

