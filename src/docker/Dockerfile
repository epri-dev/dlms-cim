FROM fedora:30 AS builder

LABEL maintainer="Ed Beroset <beroset@ieee.org>"

RUN dnf update -y \
        && dnf install -y gcc gcc-c++ make gsoap gsoap-devel git cmake

WORKDIR /tmp/
RUN git clone https://github.com/epri-dev/dlms-cim.git && \
    cd dlms-cim && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make 

FROM fedora:30
WORKDIR /root/
COPY --from=builder /tmp/dlms-cim/build/src/disconnectserver 
COPY --from=builder /tmp/dlms-cim/build/src/disconnectclient 
ENTRYPOINT ["/bin/bash", "/root/disconnectclient", "8888"]
