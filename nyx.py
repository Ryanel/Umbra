#!/usr/bin/env python

import os
import sys
import subprocess
import argparse
import json
import shutil

config = {
    'build_directory': "build",
    'sysroot': "sysroot",
    'target': 'i686',
    'build_loader': False,
    'cancel_on_fail': True,
    'buildtool': 'make',
    'debugger': 'ddd'
}

def main() -> int:
    global build_directory

    # Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("module", help="The module to execute")
    parser.add_argument("-v","--verbose", help="Increase output verbosity", action="store_true")
    parser.add_argument("--build-directory", help="Sets the build directory for this invocation")
    parser.add_argument("-d","--debug", help="Turn on debugging mode", action="store_true")
    parser.add_argument("-r","--run", help="Run an image, if possible", action="store_true")
    parser.add_argument("--sysroot", help="Sets the sysroot directory")
    parser.add_argument("--target", help="Sets the target processor to build for")
    args = parser.parse_args()

    # Create the build environment
    if args.build_directory:
        config['build_directory'] = args.build_directory

    create_build_directory()

    # Load the saved build enviroment, if we already have one
    config_load()
    config_modify(args)
    config_save()

    # This is not preserved on load
    config['debugging'] = args.debug or False
    config['run_on_completion'] = args.run or False

    if args.module == 'config':
        mod_configure()
    elif args.module == 'build':
        mod_build()
    elif args.module == 'run':
        mod_run()
    elif args.module == 'debug':
        mod_debug()
    elif args.module == 'clean':
        mod_clean()
    elif args.module == 'distclean':
        mod_distclean()
    elif args.module == 'all':
        mod_distclean()
        create_build_directory()
        config_save() # Persist configuration even after distclean
        mod_configure()
        mod_build()  

    return 0

def create_build_directory():
    global config
    dir = config['build_directory']
    try:
        os.mkdir(dir)
        print('[configure] Created build directory: ' + dir)
    except FileExistsError as error:
        return

def helper_make_directory(dir):
    try:
        os.makedirs(dir)
    except FileExistsError as error:
        return

# Modules

def config_load():
    global config
    build_dir = config['build_directory']

    try:
        with open(build_dir + '/nyx.json', 'r') as convert_file:
            data = json.load(convert_file)
            config = data
    except FileNotFoundError as error:
        return

def config_modify(args):
    global config
    if args.sysroot:
        config['sysroot'] = args.sysroot
    if args.target:
        config['target'] = args.target

def config_save():
    global config
    build_dir = config['build_directory']
    with open(build_dir + '/nyx.json', 'w') as convert_file:
        convert_file.write(json.dumps(config))

def mod_configure():
    global config
    toolchain_file = 'nyx/targets/' + config['target'] + '.cmake'
    toolchain_abs_path = os.path.abspath(toolchain_file)
    system_root = os.path.abspath(config['build_directory'] +  '/' + config['sysroot'])
    cmake_config = ['cmake', f'-DCMAKE_TOOLCHAIN_FILE={toolchain_abs_path}', f'-DCMAKE_SYSROOT={system_root}', f'-DCMAKE_STAGING_PREFIX={system_root}']

    print('[configure] Configuring the build directory for target "' + config['target'] + '"')

    if config['build_loader']:
        helper_make_directory(config['build_directory'] + '/temp/boot/')
        subprocess.run(cmake_config + [  '-S', './src/boot/', '-B', f"{config['build_directory']}/temp/boot"], stdout=sys.stdout)
    
    helper_make_directory(config['build_directory'] + '/temp/')
    subprocess.run(cmake_config + ['-S', './src/', '-B', f"{config['build_directory']}/temp"], stdout=sys.stdout)
    
def mod_build():
    global config
    if config['build_loader']:
        boot_dir = os.path.abspath(config['build_directory'] + '/temp/boot')
        subprocess.run([config['buildtool']], stdout=sys.stdout, cwd=boot_dir)
        subprocess.run([config['buildtool'], 'install'], stdout=sys.stdout, cwd=boot_dir)

    src_dir = os.path.abspath(config['build_directory'] + '/temp/')
    compile_results = subprocess.run([config['buildtool']], stdout=sys.stdout, cwd=src_dir)

    if config['cancel_on_fail'] and compile_results.returncode > 0:
        print("Build failed, cancelling future build steps.")
        exit(1)

    subprocess.run([config['buildtool'], 'install'], stdout=sys.stdout, cwd=src_dir)

    if config['target'] == 'i686':
        subprocess.run(['./nyx/scripts/x86-create-iso.sh'], shell=True, stdout=sys.stdout)
        if config['run_on_completion']:
            mod_run()
    
def mod_run():
    if config['debugging']:
        mod_debug()
    else:
        subprocess.run(['./nyx/scripts/x86-run.sh'], shell=True, stdout=sys.stdout)

def mod_debug():
    global config
    subprocess.Popen([config['debugger']])
    subprocess.run(['./nyx/scripts/x86-run-debug.sh'], shell=True, stdout=sys.stdout)

def mod_clean():
    global config
    if config['build_loader']:
        boot_dir = os.path.abspath(config['build_directory'] + '/temp/boot')
        subprocess.run([config['buildtool'], 'clean'], stdout=sys.stdout, cwd=boot_dir)

    src_dir = os.path.abspath(config['build_directory'] + '/temp/')
    subprocess.run([config['buildtool'], 'clean'], stdout=sys.stdout, cwd=src_dir)

    system_root = os.path.abspath(config['build_directory'] +  '/' + config['sysroot'])
    shutil.rmtree(system_root, ignore_errors=False)
 
def mod_distclean():
    global config
    build_dir = os.path.abspath(config['build_directory'])
    shutil.rmtree(build_dir, ignore_errors=False)
 
if __name__ == '__main__':
    sys.exit(main())