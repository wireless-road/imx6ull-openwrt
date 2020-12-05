FROM debian:latest

RUN apt-get update -qq &&\
    apt-get install -y \
        build-essential \
        curl \
        file \
        gawk \
        gettext \
        git \
        libncurses5-dev \
        libssl-dev \
        python2.7 \
        python3 \
        rsync \
        subversion \
        sudo \
        swig \
        unzip \
        wget \
        zlib1g-dev \
        && apt-get -y autoremove \
        && apt-get clean \
        && rm -rf /var/lib/apt/lists/*

RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
RUN useradd -c "OpenWrt Builder" -m -d /home/build -G sudo -s /bin/bash build

COPY --chown=build:build . /home/build/openwrt/
RUN chown build:build /home/build/openwrt

USER build
ENV HOME /home/build

WORKDIR /home/build/openwrt/

# RUN mkdir -p ./bin/targets/imx6ull/cortexa7/
# RUN touch ./bin/targets/imx6ull/cortexa7/hello.txt
# RUN touch ./bin/targets/imx6ull/cortexa7/hello2.txt
# RUN ./compile.sh lorawan_gateway_ethernet
# RUN cp -R ./bin/targets/imx6ull/cortexa7/ /home/build/bin
