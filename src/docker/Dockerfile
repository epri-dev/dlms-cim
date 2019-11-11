FROM fedora:30 AS builder

LABEL maintainer="Ed Beroset <beroset@ieee.org>"

# Only the first line is needed to build the software.  All packages starting
# with doxygen are only needed to generate the documentation.
RUN dnf update -y \
        && dnf install -y gcc gcc-c++ make gsoap gsoap-devel git cmake unzip \
        doxygen graphviz texlive-latex texlive-tabu texlive-linegoal texlive-multirow \
        texlive-hanging texlive-adjustbox texlive-stackengine texlive-listofitems \
        texlive-ulem texlive-wasysym texlive-collection-fontsrecommended \
        texlive-sectsty texlive-fancyhdr texlive-natbib texlive-tocloft

WORKDIR /tmp/
# When the software repository is no longer private, this will work 
# without requiring authentication:
# RUN git clone --recurse-submodules https://github.com/epri-dev/dlms-cim.git 
COPY dlms-cim-master.zip .
COPY DLMS-COSEM-master.zip .
RUN unzip dlms-cim-master.zip && \
    mv /tmp/dlms-cim-master /tmp/dlms-cim && \
    rmdir /tmp/dlms-cim/DLMS-COSEM && \
    unzip DLMS-COSEM-master.zip && \
    mv /tmp/DLMS-COSEM-master /tmp/dlms-cim/DLMS-COSEM 
RUN mkdir /tmp/dlms-cim/build && \
    cd /tmp/dlms-cim/build && \
    cmake .. && \
    make && \
    make pdf

FROM fedora:30
WORKDIR /root/
COPY --from=builder /tmp/dlms-cim/build/src/disconnectserver .
COPY --from=builder /tmp/dlms-cim/build/src/disconnectclient .
COPY --from=builder /tmp/dlms-cim/build/src/DLMS_sim .
COPY --from=builder /tmp/dlms-cim/build/src/HESSim .
COPY --from=builder /tmp/dlms-cim/build/src/MeterSim .
COPY --from=builder /tmp/dlms-cim/build/doc/latex/refman.pdf .
ENTRYPOINT ["/bin/bash", "/root/disconnectclient", "8888"]
