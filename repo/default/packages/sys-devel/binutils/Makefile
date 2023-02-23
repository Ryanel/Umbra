all:

pkg_get_source:
	-git clone git://sourceware.org/git/binutils-gdb.git --branch=binutils-2_35 --depth=1 .
	# Apply patches.
	-patch -ruN -p1 -d . < $(FILESDIR)/binutils-2.35.patch

pkg_configure:
	@echo "Configuring $(CATEGORY)/$(P)"
	bash -c "cd ld && aclocal"
	bash -c "cd ld && automake"
	cd ${TMPDIR} && ${S}/configure --target=${TARGET_TRIPLE} --prefix=${TOOL_INSTALL}/ --with-sysroot=${SYSROOT} --disable-werror --disable-dnls

pkg_build:
	@echo "Building $(CATEGORY)/$(P)"
	cd ${TMPDIR} && make -j${MAKE_JOBS}

pkg_install:
	@echo "Installing $(CATEGORY)/$(P)"
	cd ${TMPDIR} && make install