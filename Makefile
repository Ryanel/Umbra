UMBRA_SHELL ?= bash

setup-dev-initial:
	# Setup dev environment...
	cd aurora && make requirements
	cd aurora && make docker
	cd aurora && make install-dev

setup-dev:
	@cd aurora && make install-dev

shell-local: setup-dev
	@ AURORA_CONFIG=$(shell pwd)/repo/cfg/config.yaml GMAKE=$(shell which gmake) ${UMBRA_SHELL}