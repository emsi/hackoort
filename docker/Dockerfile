FROM ubuntu:16.04

RUN apt-get update
RUN apt-get install -y --no-install-recommends \
        git \
        wget \
        vim \
        inetutils-ping net-tools \
        openssh-server \
        sudo \
        build-essential cmake \
        ca-certificates \
        libbluetooth-dev libreadline-dev
RUN apt-get install -y --no-install-recommends \
    pkg-config libglib2.0-dev dbus bluez python3-pip python3-dev \
    libavahi-compat-libdnssd-dev


RUN apt-get clean && \
    rm -rf /var/lib/apt/lists/* \
    true

RUN mkdir /hackoort

RUN cd /usr/local/src && \
    git clone https://github.com/labapart/gattlib.git && \
    cd gattlib && git checkout initial-python-support && \
    mkdir build && cd build && \
    cmake .. && make && make install && \
    cp -al /usr/local/src/gattlib/gattlib-py/gattlib /hackoort
RUN cd /usr/local/src && \
    git clone https://github.com/emsi/hackoort && \
    cd hackoort && mkdir build && cd build && \
    cmake .. && make && make install && \
    cp -al /usr/local/src/hackoort/python/hackoort /hackoort && \
    ldconfig

RUN pip3 install --upgrade pip
RUN pip3 install --upgrade setuptools

COPY requirements.txt /hackoort

RUN pip3 install -r /hackoort/requirements.txt

# add ff unpriviledged user
RUN groupadd -g 1000 hackoort && \
    useradd -s /bin/bash -d /hackoort -u 1000 -g 1000 hackoort

# Set shell to bash
ENV SHELL /bin/bash

# Set locale
ENV LANG "C.UTF-8"
ENV LC_COLLATE "C.UTF-8"

# Project path
ENV PYTHONPATH="${PYTHONPATH}:/hackoort"


# For development we use remote ssh
RUN sed -i \
    -e 's/^Port 22/Port 1122/' \
    /etc/ssh/sshd_config && \
	mkdir /root/.ssh && chmod 700 /root/.ssh && mkdir /var/run/sshd
COPY authorized_keys /root/.ssh
EXPOSE 22
RUN echo "export LANG=C.UTF-8" >> /etc/profile &&\
    echo "export LC_COLLATE=C.UTF-8" >> /etc/profile && \
    echo "export PYTHONPATH=${PYTHONPATH}:/hackoort" >> /etc/profile && \
    true

WORKDIR /hackoort
CMD ["/usr/sbin/sshd", "-D", "-e"]