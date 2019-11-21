FROM fedora:30 AS builder
ARG projectsourcename

LABEL maintainer="Ed Beroset <beroset@ieee.org>"

# Only the first line is needed to build the software.  All packages starting
# with doxygen are only needed to generate the documentation.
RUN dnf update -y \
        && dnf install -y gcc gcc-c++ make gsoap gsoap-devel git cmake unzip \
        doxygen graphviz texlive-latex texlive-tabu texlive-linegoal texlive-multirow \
        texlive-hanging texlive-adjustbox texlive-stackengine texlive-listofitems \
        texlive-ulem texlive-wasysym texlive-collection-fontsrecommended \
        texlive-sectsty texlive-fancyhdr texlive-natbib texlive-tocloft \
        texlive-newunicodechar texlive-etoc

WORKDIR /tmp/
# When the software repository is no longer private, this will work 
# without requiring authentication:
# RUN git clone --recurse-submodules https://github.com/epri-dev/dlms-cim.git 
ADD $projectsourcename.tar.gz .
RUN mv $projectsourcename dlms-cim && \
    mkdir dlms-cim/build && \
    cd /tmp/dlms-cim/build && \
    cmake -Wno-dev .. && \
    make && \
    make pdf

FROM nginx:alpine AS docserver
COPY --from=builder /tmp/dlms-cim/build/doc/html/ /usr/share/nginx/html
COPY --from=builder /tmp/dlms-cim/build/doc/latex/refman.pdf /usr/share/nginx/html/dlms-cim.pdf
COPY default.conf /etc/nginx/conf.d/default.conf

FROM fedora:30 AS demo
RUN dnf update -y \
        && dnf install -y gsoap
WORKDIR /tmp/
COPY --from=builder /tmp/dlms-cim/build/src/disconnectserver .
COPY --from=builder /tmp/dlms-cim/build/src/disconnectclient .
COPY --from=builder /tmp/dlms-cim/build/src/CISsim .
COPY --from=builder /tmp/dlms-cim/build/src/DLMS_sim .
COPY --from=builder /tmp/dlms-cim/build/src/HESsim .
COPY --from=builder /tmp/dlms-cim/build/src/Metersim .
