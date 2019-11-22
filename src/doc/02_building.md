# How to build this software {#building} #

As mentioned in the description of [How to use this software](@ref using), there are two basic ways in which the software can be run and built.  The easy method is to build and run the software in containers using Docker.  The alternative method is to build and run the software in multiple separate windows on a computer equipped with the Linux operating system.  See [Native Linux build](@ref native).  While other alternatives are possible (e.g. running Docker on a Windows 10 machine), these instructions are described in terms of running a *version 30 (or later) of a Fedora distribution of Linux*.  (Note however, that at the time of this writing, in November 2019, there is [a bug in Doxygen](https://github.com/doxygen/doxygen/issues/6837) which prevents Doxygen 1.8.15, which comes with Fedora 31, from successfully creating the PDF file version of this documentation.)

## Building with Docker ##
Docker provides a lightweight alternative to a full virtual machine.  One can create an *image* file which contains the software and all of the required infrastructure to be able to run it.  These are the steps required to be able to create and run the Docker image.  

### Get Docker installed and running ###
The following steps assume that you already have Docker running on a Linux machine of some kind.  See the [Docker "getting started"](https://docs.docker.com/get-started/) pages for how to do that.  It also assumes that you have `docker-compose` available, configured and running.  The same pages will be helpful with that as well.

### Build the software ###
The simplest way to build the software within a `Docker` container is to use start with a compressed archive of the entire source software project in an otherwise empty directory.  This file will be named `dlms-cim-1.0.0-Source.tar.gz` with the `1.0.0` referring to the 1.0.0 version of the software.  Later versions will have different version numbers, of course.  

The next step is to execute the following command:

    tar -xvzf dlms-cim-1.0.0-Source.tar.gz --strip=3 dlms-cim-1.0.0-Source/src/docker

This extracts some files from within the archive to the same directory.  Next is to make sure that you have a good connection to the internet and then execute:

    docker-compose up --build

This will download a base Docker image and install all required tools (to the image, and not to your host computer).  This may take many minutes depending on the speed of your internet connection and computer.  Eventually the build process will be complete and Docker will run five containers.  The last few lines of that should look something like this:

    Successfully tagged docker_dlms-cim-cis:latest
    Creating docker_dlms-cim-hes_1    ... done
    Creating docker_dlms-cim-meter1_1 ... done
    Creating docker_dlms-cim-docs_1   ... done
    Creating docker_dlms-cim-cis_1    ... done
    Creating docker_dlms-cim-meter2_1 ... done
        

### Alternative approach
One can also perform the build and run steps separately with `docker-compose`.  The effect is the same, but instead of this:

    docker-compose up --build

It's possible to separate those into two distinct steps:

    docker-compose build
    docker-compose up

The advantage is that if the container images have already been built, `docker-compose up` is faster than running `docker-compose up --build`.

## Further reading

[**Introduction**](index.html)

[How to use the software](@ref using)
