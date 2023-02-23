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
RUN mkdir -p /opt/aurora/tools/ && chown buildenv /opt/aurora/tools/
RUN mkdir -p /opt/aurora/tmp/ && chown buildenv /opt/aurora/tmp/
RUN mkdir -p /opt/aurora/image/ && chown buildenv /opt/aurora/image
RUN mkdir -p /opt/aurora/src/ && chown buildenv /opt/aurora/src
RUN mkdir -p /opt/aurora/media/ && chown buildenv /opt/aurora/media
RUN mkdir -p /opt/aurora/packages/ && chown buildenv /opt/aurora/packages
RUN mkdir -p /opt/aurora/repos/default && chown buildenv /opt/aurora/repos/default
RUN mkdir -p /opt/aurora/cfg/ && chown buildenv /opt/aurora/cfg
VOLUME /opt/aurora/tmp/ /opt/aurora/tools /opt/aurora/image/ /opt/aurora/src/ /opt/aurora/repos /opt/aurora/packages /opt/aurora/cfg

RUN chown -R buildenv /opt/aurora

USER buildenv
RUN mkdir -p /opt/aurora/aurora/ && chown buildenv /opt/aurora/aurora
# Setup User
WORKDIR /home/buildenv

RUN git config --global user.email "aurora-buildenv@localhost" \
	&& git config --global user.name "aurora-buildenv"

# Now, setup aurora
WORKDIR /opt/aurora/aurora

# Modify the bashrc to include aur and aurora_build in path, as well as differentiate the prompt
RUN echo "export PATH=$PATH:~/.local/bin:/opt/aurora/tools/bin" >> ~/buildenv-rc
RUN echo "export AURORA_CONFIG=/opt/aurora/cfg/build.yaml" >> ~/buildenv-rc
RUN echo "export GMAKE=$(which gmake)" >> ~/buildenv-rc
RUN echo "source $HOME/buildenv-rc" >> ~/.bashrc
RUN echo 'export PS1="\[\e[38;5;226m\]\u\[\e[38;5;220m\]@\[\e[38;5;214m\]\h \[\e[38;5;33m\]\w \[\033[0m\]$ "' >> ~/.bashrc

# Copy the aurora source
COPY --chown=buildenv:root requirements.txt .
RUN pip install --no-cache-dir --upgrade pip \
  && pip install --no-cache-dir -r requirements.txt
COPY --chown=buildenv:root . .
RUN pip install -e .

# Setup Runtime environment
WORKDIR /opt/aurora/
CMD ["bash"]
LABEL org.opencontainers.image.authors="corwinmcknight@gmail.com"
