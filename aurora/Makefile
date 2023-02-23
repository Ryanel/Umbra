.PHONY: clean ddev requirements test docker setup-env

clean:
	find . -type f -name '*.py[co]' -delete -o -type d -name __pycache__ -delete

dev:
	pip install -e .
	# Run these two commands
	export AURORA_CONFIG=./example/sysroot/etc/aurora/config.yaml
	export GMAKE=$(shell which gmake)
	@echo "aur and aurora_build are now available"

requirements:
	pip3 install -r requirements.txt

test:
	export AURORA_CONFIG=./example/sysroot/etc/aurora/config.yaml
	export GMAKE=$(shell which gmake)
	aurora_build boot/limine-2.0 --live-sources="./example/src/"

docker: clean
	./docker-build.sh

install-dev: 
	pip install -e .
	