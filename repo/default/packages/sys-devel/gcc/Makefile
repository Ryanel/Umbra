all:

pkg_get_source:
	-git clone git://gcc.gnu.org/git/gcc.git --branch=releases/gcc-11.1.0 --depth=1 .
	# Apply patches.
	-patch -ruN -p1 -d . < $(FILESDIR)/gcc-11.1.0.patch

pkg_configure:
	@echo "Configuring $(CATEGORY)/$(P)"
	bash -c "cd libstdc++-v3 && autoconf"
	cd ${TMPDIR} && ${S}/configure --target=${TARGET_TRIPLE} --prefix=${TOOL_INSTALL}/ --with-sysroot=${SYSROOT} --enable-languages=c,c++ --disable-werror --disable-dnls

pkg_build:
	@echo "Building $(CATEGORY)/$(P)"
	cd ${TMPDIR} && make -j${MAKE_JOBS} all-gcc all-target-libgcc

pkg_install:
	@echo "Installing $(CATEGORY)/$(P)"
	cd ${TMPDIR} && make install-gcc install-target-libgcc