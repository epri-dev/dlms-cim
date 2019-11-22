# How to use this software # {#using}

Before the software is run, it will need to be built, which is described in [How to build this software](@ref building) and in [Native Linux build](@ref native).

There are essentially two main ways to use this software.  The easy method is to run the software in containers using Docker.  The alternative method is to run the software in multiple separate windows on a computer equipped with the Linux operating system.  Each of these will be described below.  

## Docker images ##
[Docker](https://docs.docker.com/) is software that provides a way to run applications securely isolated in a container, packaged with all its dependencies and libraries.  There is also container orchestration software called `docker-compose` which is an optional part of Docker.  Using `docker-compose` we can very simply get everything running simply by using the command:

    docker-compose up --build

The process to do this is described in [How to build this software](@ref building).  For this description, it is assumed that the reader has followed the instructions there and that `docker-compose` is successfully running. 

At this point we have five containers with two simulated meters, a simulated HES and a simulated CIS, each in its own separate Docker container and communicating with the other components over a simulated network.  There is also a separate web server serving the documentation you are now reading.  As described earlier, the interaction among the software pieces is entirely automated and periodic.  Every ten seconds, the CIS sends a disconnect request, encoded as CIM, to the HES simulator.  The HES simulator, in turn, translates this into disconnect message in DLMS format and relays these commands to the two simulated meters.  After the simulated meters receive the commands, they respond to indicate that they have performed the disconnect.  The HES recieves these responses, encapsulates them into a single CIM response and finally sends this response message back to the CIS.  The CIS then delays for two seconds and sends service reconnect messages to the HES which performs the actions as before, except with a reconnect command instead of a disconnect command.

### Using the documentation server
The documentation server maps to port 8080 on the host, so viewing the documentation is as simple as using any web browser and going to http://localhost:8080/ to see the main page of this documentation.

### Examining simulator communications with Wireshark
As these messages are being communicating among the simulated devices, there is little visual representation of what is happening except for some logging messages that appear in the console.  However, one can use the free and open source [Wireshark](https://www.wireshark.org/#download) network packet analyzer tool to examine the traffic among the simulated machines.  

First, let's look at the networks that `docker-compose` has created for us:

    docker network ls

This lists the networks.  The output is something like this:

    NETWORK ID          NAME                DRIVER              SCOPE
    16e623d9c484        bridge              bridge              local
    7645e1d21c80        docker_docnet       bridge              local
    3b8a0b25952e        docker_hesnet       bridge              local
    6690d3b8e9ac        docker_meternet     bridge              local
    40b82e199c11        host                host                local
    87a1d2962ff9        none                null                local

The `bridge`, `host` and `none` networks are defaults created by Docker and we can ignore these for now.  The important ones are `docker_hesnet` and `docker_meternet`.  The `docker_docnet` does not interact with the others and is solely used as a convenient way to separate the web server that provides this documentation.

These network ID numbers are also used by Wireshark.  If we open Wireshark and examine the available capture interfaces, we should see a list that includes devices name `br-3b8a0b25952e` which is the virtual bridge that is associated with the `docker_hesnet` network and `br-6690d3b8e9ac` which corresponds with the `docker_meternet` network.  If we select both of these capture interfaces and start a capture, we might see something like that shown in the picture below.

@image html Wireshark1.png "Wireshark screen shot"
@image latex Wireshark1.png "Wireshark screen shot" width=\textwidth

We can see that the CIM request in packet 7 (encapsulated in HTTP/XML message) was sent by 192.168.192.2 (the simulated CIS) to 192.168.192.3 (simulated HES).  We can also see that the HES starts to send out a dlms-cosem message in response in packet 24 from 192.168.160.4 (the HES on the `meternet` side) to 192.168.160.3 (this is one of the simulated meters).  We can see that they are different networks, as would be the case in the real world, and that each device has its own IP address.  

There are, however, some unrealistic aspects to the simulation.  In particular, the order that packets are delivered to Wireshark is not always the same order that they would actually appear on a real network.  The result is that the packets can appear to be out of order or duplicated.  This is because the simulated network has unrealistically low latency and Wireshark doesn't actually necessarily get the packets in the order they are actually sent.  For example, in this particular trace, the first 31 packets all arrive within one millisecond.  To mostly correct this, we can sort on the time column by clicking on the column header in Wireshark.  There still we be some apparent duplicate packets, but the ordering should be mostly correct.

### Stopping the software
Stopping the software, if run in this way, is simply a matter of using the `Ctrl-C` key combination to shut down docker-compose.  The software will gracefully perform the shutdown steps before returning to the command line prompt.  To also remove the virtual networks, one can run `docker-compose down` afterwards.


## Further reading

[Adapting the software](@ref design)

[Native Linux build](@ref native)
