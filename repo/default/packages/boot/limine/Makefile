all:

pkg_get_source:
	-git clone https://github.com/limine-bootloader/limine.git --branch=v2.0-branch-binary --depth=1 .

pkg_configure:
	@echo "Configuring $(CATEGORY)/$(P)"

pkg_build:
	@echo "Building $(CATEGORY)/$(P)"
	@echo "${CC} ${CFLAGS} ${LDFLAGS} -o limine-install limine-install.c"
	make

pkg_install:
	@echo "Installing $(CATEGORY)/$(P)"
	mkdir -p $(TOOL_INSTALL)/limine/
	cp limine-install ${TOOL_INSTALL}/limine/limine-install
	cp *.sys ${TOOL_INSTALL}/limine
	cp *.bin ${TOOL_INSTALL}/limine