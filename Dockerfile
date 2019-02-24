FROM ubuntu:18.04
LABEL maintainer="zwimer@gmail.com"

# Dependencies
RUN    apt-get update \
	&& apt-get install -yq git make cmake

# Clone
RUN git clone https://github.com/zwimer/C-bind

# Build
RUN    mkdir ./C-bind/src/build/ \
    && cd ./C-bind/src/build/ \
    && cmake .. \
    && make

# Execute
ENTRYPOINT cd ./C-bind/src/build/ && /bin/bash
