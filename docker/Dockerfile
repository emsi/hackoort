ARG BASE
FROM $BASE as base

RUN apt-get update
#RUN apt-get upgrade -y
RUN apt-get install -y --no-install-recommends \
        git \
        build-essential cmake \
        libbluetooth-dev libreadline-dev \
        libglib2.0-dev bluez python3-pip python3-dev \
        libavahi-compat-libdnssd-dev

RUN apt-get clean && \
    rm -rf /var/lib/apt/lists/* \
    true

RUN mkdir /hackoort

RUN cd /usr/local/src && \
    git clone https://github.com/emsi/gattlib.git && \
    cd gattlib && git checkout docker && \
    mkdir build && cd build && \
    cmake .. && make && make install && \
    cp -al /usr/local/src/gattlib/gattlib-py/gattlib /hackoort
RUN cd /usr/local/src && \
    git clone https://github.com/emsi/hackoort && \
    cd hackoort && mkdir build && cd build && \
    cmake .. && make && make install && \
    cp -al /usr/local/src/hackoort/python/hackoort /hackoort && \
    cp -al /usr/local/src/hackoort/python/oorthap /hackoort && \
    ldconfig

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 10

RUN pip3 install --upgrade pip
RUN pip3 install --upgrade setuptools

COPY requirements.txt /hackoort

RUN pip3 install -r /hackoort/requirements.txt

ARG BASE
FROM $BASE

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        bluez python3 libavahi-compat-libdnssd-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 10

COPY --from=base /usr/local/lib /usr/local/lib
COPY --from=base /usr/local/bin /usr/local/bin
COPY --from=base /hackoort /hackoort
RUN ldconfig

# add ff unpriviledged user
RUN groupadd -g 1000 hackoort && \
    useradd -s /bin/bash -d /hackoort -u 1000 -g 1000 hackoort

RUN mkdir /var/run/oorthap && chown hackoort:hackoort /var/run/oorthap

# Project path
ENV PYTHONPATH="${PYTHONPATH}:/hackoort"

# Set shell to bash
ENV SHELL /bin/bash

# Set locale
ENV LANG "C.UTF-8"
ENV LC_COLLATE "C.UTF-8"