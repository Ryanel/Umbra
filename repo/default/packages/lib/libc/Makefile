all:

pkg_get_source:
	@exit 0
pkg_configure:
	@exit 0
pkg_build:
	cd $(LIVE_SOURCES) && make libc.a
pkg_install:
	@echo "Installing $(CATEGORY)/$(P)"
	cd $(LIVE_SOURCES) && make install
