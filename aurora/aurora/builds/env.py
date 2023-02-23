""" Generates an environment for the package build"""
import os

from aurora.package import Package

def get_build_env(config: dict, package_str: dict, package: Package) -> dict:
    """ Get the environment variables """

    cross_compiling = config['cross_compile']
    arch       = 'x86'
    arch_board = 'x86_64'
    triple = 'x86_64-pc-linux-gnu' if not cross_compiling else f'{arch_board}-umbra-elf'

    package_info = Package.interpret_package_string(package_str)

    environment = {}
    environment['CFLAGS']        = '-O2'
    environment['LANG']          = 'C'
    environment['LC_CTYPE']      = 'C'
    environment['LANGUAGE']      = 'C'
    environment['ARCH']          = arch
    environment['ARCH_BOARD']    = arch_board
    environment['TARGET_TRIPLE'] = triple
    environment['SYSROOT']       = config['system_root']
    environment['FILESDIR']      = os.path.join(config['repo_path'], 'packages', package_info['name'])
    environment['WORKDIR']       = os.path.join(config['temp_path'],  package_str)
    environment['TMPDIR']        = os.path.join(config['temp_path'],  package_str, 'tmp')
    environment['D']             = os.path.join(config['temp_path'],  package_str, 'install')
    environment['S']             = os.path.join(config['temp_path'],  package_str, 'src')
    environment['P']             = package_info['name_no_category'] + '-' + package_info['version']
    environment['PN']            = package_info['name_no_category']
    environment['PV']            = package_info['version']
    environment['CATEGORY']      = package_info['category']
    environment['MAKE']          = config['gmake_path']
    environment['PATH']          = os.environ['PATH']
    if 'MAKE_JOBS' in os.environ:
        environment['MAKE_JOBS'] = os.environ['MAKE_JOBS']
    else:
        environment['MAKE_JOBS'] = str(os.cpu_count())

    if package.get_option('is_build_tool', False) is True:
        cross_compiling = False # Build tools are never cross compiled. These are intended to be run on the host system.
        environment['TOOL_INSTALL'] = config['toolchain_path']

    if cross_compiling:
        environment['CC']        = f"{triple}-gcc"
        environment["CXX"]       = f"{triple}-g++"
        environment["AR"]        = f"{triple}-ar"
        environment["AS"]        = f"{triple}-as"
        environment["LD"]        = f"{triple}-ld"
        environment["NM"]        = f"{triple}-nm"
        environment["RANLIB"]    = f"{triple}-ranlib"
        environment["STRIP"]     = f"{triple}-strip"
        environment["ASM"]       = "nasm"
        environment["ASM_FLAGS"] = "-f elf64 -F dwarf -g"
    
    if config['live_sources'] != '' and package.get_option('supports_live_sources', default='false') is True:
        live_source_path = package.get_option('live_source_path') or package_info['name']
        environment['LIVE_SOURCES'] = os.path.abspath(os.path.join(config['live_sources'], live_source_path))
    
    return environment
