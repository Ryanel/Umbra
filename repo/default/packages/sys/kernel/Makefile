all:

pkg_get_source:
	@exit 0
pkg_configure:
	@exit 0
pkg_build:
	@echo "Building $(CATEGORY)/$(P)"
	@cd $(LIVE_SOURCES) && make all
pkg_install:
	@echo "Installing $(CATEGORY)/$(P)"
	@cd $(LIVE_SOURCES) && make symbols install
