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
ADD dlms-cim-1.0.0-Source.tar.gz .
RUN mkdir dlms-cim-1.0.0-Source/build && \
    cd /tmp/dlms-cim-1.0.0-Source/build && \
    cmake -Wno-dev .. && \
    make && \
    make pdf

FROM nginx:alpine AS docserver
COPY --from=builder /tmp/dlms-cim-1.0.0-Source/build/doc/html/ /usr/share/nginx/html
COPY --from=builder /tmp/dlms-cim-1.0.0-Source/build/doc/latex/refman.pdf /usr/share/nginx/html/dlms-cim.pdf
COPY default.conf /etc/nginx/conf.d/default.conf

FROM alpine AS demo
WORKDIR /tmp/
COPY --from=builder /tmp/dlms-cim-1.0.0-Source/build/src/ .
