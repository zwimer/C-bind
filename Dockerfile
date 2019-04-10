FROM ubuntu:18.04
LABEL maintainer="zwimer@gmail.com"

# Dependencies
RUN    DEBIAN_FRONTEND=noninteractive apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -yq git make cmake

# Clone
RUN git clone --depth 1 --single-branch \
	https://github.com/zwimer/C-bind

# Build
RUN    mkdir ./C-bind/src/build/ \
    && cd ./C-bind/src/build/ \
    && cmake .. \
    && make

# Execute
ENTRYPOINT cd ./C-bind/src/build/ && /bin/bash
