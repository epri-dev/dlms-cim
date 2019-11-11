# How to use this software # {#using}

Before the software is run, it will need to be built, which is described in [How to build this software](@ref building).

There are essentially two main ways to use this software.  The easy method is to run the software in containers using Docker.  The alternative method is to run the software in multiple separate windows on a computer equipped with the Linux operating system.  Each of these will be described below.  

## Docker images ##
[Docker](https://docs.docker.com/) is software that provides a way to run applications securely isolated in a container, packaged with all its dependencies and libraries.  There is also container orchestration software called `docker-compose` which is an optional part of Docker.  Using `docker-compose` we can very simply get everything running simply by using the command:

    docker-compose up

This will create two simulated meters, a simulated HES and a simulated CIS, each in its own separate Docker container and communicating with the other components over a simulated network.  As described earlier, the interaction among the software pieces is entirely automated and periodic.  Every ten seconds, the CIS sends a disconnect request, encoded as CIM, to the HES simulator.  The HES simulator, in turn, translates this into disconnect message in DLMS format and relays these commands to the two simulated meters.  After the simulated meters receive the commands, they respond to indicate that they have performed the disconnect.  The HES recieves these responses, encapsulates them into a single CIM response and finally sends this response message back to the CIS.  The CIS then delays for two seconds and sends service reconnect messages to the HES which performs the actions as before, except with a reconnect command instead of a disconnect command.

As these messages are being communicating among the simulated devices, there is little visual representation of what is happening except for some logging messages that appear in the console.  However, one can use the free and open source [Wireshark](https://www.wireshark.org/#download) network packet analyzer tool to examine the traffic among the simulated machines.  This procedure will not be described in detail here, but typically the Docker software will create virtual Ethernet interfaces (identifiable in Wireshark under Linux because the interface names begin with "virt") which Wireshark can then monitor.  It is useful and instructive to observe the formatting and timing of the various communications to gain insight into how a real system operates.

Stopping the software, if run in this way, is simply a matter of using the `Ctrl-C` key combination to shut down docker-compose.  The software will gracefully perform the shutdown steps before returning to the command line prompt.

### Running ###


```
#!/bin/bash
cp ../61968-9/target/61968-9.war .
cp ../61968-5/target/61968-5.war .
cp ../61968-6/target/61968-6.war .
cp ../sql/derms2full_10182018.mysql .
docker build -t epri/semantic-test-harness:v2 .
```

The last line of the file invokes the `docker build` command.  It copies the relevant files to the Docker daemon and creates an image file.  It will download some packages to install on the image (but *not* on your hard drive!)  and execute all of the steps to create the image.

## Running the image ##
If the previous steps were followed and worked correctly, the result should be a runnable Docker image file.  Running the image can be done in single command:

    docker run -p 8080:8080 epri/semantic-test-harness:v2

This will, in the Docker container, initialize the MySQL database and run the WSDL software.

One can verify that this is working by navigating in a browser window to http://localhost:8080/61968-9  This should show a table of available web services with their associated message names.  Clicking on one of the test names should then show the associated wsdl.

### Running the GUI ### 
The GUI may then be run from the local host as usual.  This is described under the section heading "install ruby/rails" on https://github.com/epri-dev/Semantic-Test-Harness/wiki/Installation-and-Setup
