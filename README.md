# Simulix

[FMU](https://en.wikipedia.org/wiki/Functional_Mock-up_Interface) builder for [Simulink](https://mathworks.com/products/simulink.html) models using the C-API.

Also provides the ability to compile existing FMUs when the FMU is missing binaries for the current OS.

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
  * [Extensions](#extensions)
* [WIKI](#wiki)
* [Currently Work In Progress](#currently-work-in-progress)
* [Support](#support)

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
    python install -r requirements.txt
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
* `-e :           Path to extension`
* `-tf:           Name of temp folder`

*Just specify flag, not arguments:*
* `--ONLY_BUILD:  Not generate new files, only build existing`
* `--NO-TEMP   :  Doesn't delete the temp folder`
* `--NO-CMAKE  :  Doesn't execute CMAKE`
* `--NO-MAKE   :  Doesn't execute MAKE program`

Here are some examples of executing Simulix

The model zip file is called *example.zip* and exists in the current directory

    Simulix.py example

The model zip file exists in a subfolder called *zip* and I want to build in a subfolder called *generated*
    
    mkdir generated
    Simulix.py example -p generated -zp zip
    

## Extensions

With Simulink, there's a lot of toolboxes. Instead of waiting around for support of a special toolbox you can temporary do your own.

With our program, you can supply a path to your extension with the model when building. This extension can provide additional information and replace our template files.

In order to use it, make a new folder containing a extension.py file. This is the only required step. If this file exists, even if it's empty, the extension will be recognized.

With an extension, you can use certain functions inside extension.py that will be called.

    get_template_info(replace_dict):
        return replace_dict

    If this function exist, the dict Simulix uses to generate template files will be passed so you either can log the variables, change them or remove them.
    In order for this to take effect, you have to return the replace_dict after your changes.

With an extension, you can also modify Simulix template files. If a template file is found inside the extension in a templates folder, Simulix will default to that template file. An example of an extension can look something like this


`extension.py`

    get_template_info(replace_dict):
        replace_dict['TEXT_STRING'] = "Hello World"
        return replace_dict

`templates/CMakeLists_template.txt`

    message(STATUS {TEXT_STRING})


Because we are using pythons [format](https://docs.python.org/3.4/library/string.html#string.Formatter.format) function on the entire file, to use brackets without replacing anything and without raising a KeyError error, use double brackets. An example can look like this

`templates/CMakeLists_template.txt`

    set(STRING_VARIABLE {TEXT_STRING})
    message(STATUS ${{STRING_VARIABLE}})

This will produce the CMakeLists file like so

`ModelDir/CMakeLists.txt`

    set(STRING_VARIABLE "Hello World")
    message(STATUS ${STRING_VARIABLE})


# WIKI

Wiki pages are coming soon, so we can have less clutter in the read-me. This is not top-priority, but I promise, soon™.


# Currently Work In Progress

Almost everything in this project is currently work in progress, even the project name. Feel free to contribute all you want.

# Support

While we are in Alpha, we will list what we are currently supporting.

✓: We currently support this software/OS

✕: We currently do not support this software/OS

?: We are currently not able to test Simulix on this software and are unable to determine if we support it or not.


* Windows
  * 32-bit: ✕
  * 64-bit: ✓
* Linux: ✓
* macOS: ?


