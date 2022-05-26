# syntax=docker/dockerfile:1

FROM debian:bullseye-slim AS base
RUN apt-get update \
	&& apt-get -y install \
		# Don't install recommended stuff
		--no-install-recommends \
		# Used by gnutls
		autogen \
        automake \
        autotools-dev \
		# Used by host-bison
		autopoint \
		# Used by binutils in the build process
		bison \
		# Contains gcc and g++
		build-essential \
		# Used by several packages
		cmake \
		# Used by protobuf
		curl \
		# Used by binutils in the build process
		flex \
		# Required by flex but not a hard dependency in Debian.
		libfl2 \
		# Used by many tools in the build process
		gawk \
		# Used by libidn2
		gengetopt \
		# Used by host-gettext
		gettext \
		# Git is used extensively in the build process
		git \
		# Used by eudev
		gperf \
		# Used by libiconv
		groff \
		# Used by libtool
		help2man \
		# Used by shared-mime-info
		itstool \
		# Used by wayland-scanner
		libexpat1-dev \
		# Used by gtk+-2
		libgdk-pixbuf2.0-bin \
		# Used by gdk-pixbuf
		libglib2.0-dev-bin \
		# Used by host-qt6
		libgl-dev \
		# GMP, MPFR and MPC are used by gcc
		libgmp-dev \
		libmpc-dev \
		libmpfr-dev \
		# Used by cmake
		libssl-dev \
		# Used by ace
		libpng-dev \
        libtool \
		# Used by many gnu build steps		
		m4 \
		# Required to build images for Android's fastboot protocol.
		mkbootimg \
		# Used by limine
		mtools \
		# Used by libjpeg-turbo for performance reasons
		nasm \
		# Used by groff
		netpbm \
		# Used by Umbra
		ninja-build \
		# To build the certificates, we need trust, provided by p11-kit
		p11-kit \
		# Various build systems invoke pkg-config
		pkg-config \
		# Used by itstool
		python3-libxml2 \
		# Used by mesa
		python3-mako \
		# We need pip, setuptools and wheel for installing python packages
		python3-pip \
		python3-setuptools \
		python3-wheel \
		# Several programs use rsync
		rsync \
		# makeinfo is used in gnu build systems
		texinfo \
        xorriso \
		# Used by protobuf
		unzip \
		# We use wget to fetch some stuff further down
		wget \
		# Used by various programs, for example, it is used by file
		zlib1g-dev \
		# Used by the developers
        htop \
		# Cleanup time
		&& apt-get clean \
        && rm -rf /var/lib/apt/lists/*

RUN pip install cxxfilt rich docker

# Create user...
FROM base AS user
ARG USER=1000

RUN useradd -ms /bin/bash buildenv -g root -G sudo -u $USER

# Create volumes
RUN mkdir -p /opt/umbra-buildenv/src/ && chown buildenv /opt/umbra-buildenv/src
RUN mkdir -p /opt/umbra-buildenv/build/ && chown buildenv /opt/umbra-buildenv/build
RUN mkdir -p /opt/umbra-buildenv/artifacts/ && chown buildenv /opt/umbra-buildenv/artifacts

VOLUME /opt/umbra-buildenv/src/ /opt/umbra-buildenv/build/ /opt/umbra-buildenv/artifacts/

# Setup User
USER buildenv
WORKDIR /home/buildenv

RUN git config --global user.email "umbra-buildenv@localhost" \
	&& git config --global user.name "umbra-buildenv"

# Setup Runtime environment
WORKDIR /opt/umbra-buildenv/src/

CMD ["bash"]
LABEL org.opencontainers.image.authors="corwinmcknight.gmail.com"
