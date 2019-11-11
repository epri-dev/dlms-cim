# How to build this software {#building} #

As mentioned in the description of [How to use this software](@ref using), there are two basic ways in which the software can be run and built.  The easy method is to build and run the software in containers using Docker.  The alternative method is to build and run the software in multiple separate windows on a computer equipped with the Linux operating system.  Each of these will be described below.  

## Building with Docker ##

Docker provides a lightweight alternative to a full virtual machine.  One can create an *image* file which contains the software and all of the required infrastructure to be able to run it.  These are the steps required to be able to create and run the Docker image.  

### Get Docker installed and running ###
The following steps assume that you already have `docker` running on a Linux machine of some kind.  See the [Docker "getting started"](https://docs.docker.com/get-started/) pages for how to do that.  It also assumes that you have `docker-compose` available, configured and running.  The same pages will be helpful with that as well.

### Build the software ###
Build the `dlms-cim` software by navigating to the root directory of the project and using the command `mvn package`.  This should run `Maven` and build the files required.

### Run the script ### 
Navigate to the `docker` directory under the `Semantic-Test-Harness` project and execute `create.sh` by typing `./create.sh`.  This will copy the `61968-5.war`, `61968-6.war` and `61968-9.war` files (which should have been built by the previous step) into the appropriate directory, and then copies the database file.  Finally, it launches the docker build process.  The contents of the `create.sh` file are:
