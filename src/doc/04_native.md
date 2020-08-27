# Native Linux build {#native} #

As mentioned in the description of [How to use this software](@ref using), there are two basic ways in which the software can be run and built.  The easy method is to build and run the software in containers using Docker.  See [How to build the software](@ref building) for that version.  The alternative method described here is to build and run the software in multiple separate windows on a computer equipped with the Linux operating system.  As with the Docker version of the build instructions, these instructions are described in terms of running a *version 32 (or later) of a Fedora distribution of Linux*.

## Prerequisites ##
To build the software on a computer running Fedora version 32, a number of packages need to be installed first.  It's also recommended to update existing packages to the latest versions before beginneing.  To quickly install all packages necessary for building both the software and the documentation, execute the following command:


    sudo dnf update -y \
    && sudo dnf install -y gcc gcc-c++ make gsoap gsoap-devel git cmake unzip \
    doxygen graphviz texlive-latex texlive-tabu texlive-linegoal texlive-multirow \
    texlive-hanging texlive-adjustbox texlive-stackengine texlive-listofitems \
    texlive-ulem texlive-wasysym texlive-collection-fontsrecommended \
    texlive-sectsty texlive-fancyhdr texlive-natbib texlive-tocloft

Note that the `\` at the ends of the lines is a line-continuation character and is literally required to be typed.  It's also recommended to install the `docker-compose` and `docker` packages to assist in running the software, even with a native build as described on these pages.

### Build the software ###
The simplest way to build the software natively is to start with a compressed archive of the entire source software project in an otherwise empty directory.  This file will be named `dlms-cim-1.0.0-Source.tar.gz` with the `1.0.0` referring to the 1.0.0 version of the software.  Later versions will have different version numbers, of course.  

The next step is to extract the archive source the following command:

    tar -xvzf dlms-cim-1.0.0-Source.tar.gz 

This extracts all files from within the archive to a directory named `dlms-cim-1.0.0-Source`.  Next, create a build directory into which the software will be built.  We can call it `build` and navigate into it.

    mkdir build && cd build

Then run CMake to configure the build:

    cmake -Wno-dev ../dlms-cim-1.0.0-Source/

Finally, we're ready to build the software. 

    make

This will build all of the executable files and put them into the `src` directory.  Without changing directories, we can try running one:

    src/CISsim 

Since we haven't provided the required command line arguments, the software prints this and exits:

    Usage: src/CISsim HES_URL thisMachine_URL

See [How to use the software](@ref using) for how to run and use the software.

## Other build targets
There are other things one can build once the software has been built.  To get a complete list:

    make help

However, here are some of the most important targets:

    make pdf

This creates all of the documentation in the form of both a PDF file and a series of linked web pages.  

    make package_source

This is how to create the original `dlms-cim-1.0.0-Source.tar.gz` is created.  

    make clean

This simply removes all of the built targets and intermediate files to prepare for a fresh build.

## Further reading

[How to modify the software](@ref design)
