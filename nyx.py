#!/usr/bin/env python

# Nyx build coordinator
# By Ryanel

import os
import sys
import subprocess
import argparse
import json
import shutil

from nyx.package import *
from nyx.engine import *

config = dict()
repo_json = None
def read_repo():
    repo_file = open('repo.json')
    dat = json.load(repo_file)
    repo_file.close()
    return dat

def main() -> int:
    global config
    global repo_json
    # Arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("module", help="The module to execute. Can be clean, build, run")
    parser.add_argument("-r","--run", help="Run an image, if possible", action="store_true")
    parser.add_argument("--uncache", help="Marks the specified packages as uncached, forcing them to be rebuilt. Comma seperated list")
    parser.add_argument("--full", help="Clean: Performs a full clean", action="store_true")
    args = parser.parse_args()
    config['run_on_completion'] = args.run or False

    repo_json = read_repo()

    # Read configuration from the repo.json file
    common_environment = dict()
    common_environment['PATH'] = os.environ.get("PATH")
    common_environment |= repo_json['general']['env']
    config["source_root"] = repo_json['general']['source_root']
    config["sysroot"] = repo_json['general']['sysroot']
    config["build_root"] = repo_json['general']['build_root']
    config["package_root"] = repo_json['general']['package_root']
    config["initrd_root"] = repo_json['general']['initrd_root']

    # Read in a list of packages...
    engine = BuildEngine(repo_json, config, args)
    engine.enviroment = common_environment
    engine.load_packages()

    if args.uncache:
        marked_as_dirty = args.uncache.split(',')
        for pkg in marked_as_dirty:
            engine.uncache(pkg)

    if (args.module == 'build'):
        if (engine.build() == 0) and config['run_on_completion']:
            engine.run()
    elif (args.module == 'debug'):
        engine.debug()
    elif (args.module == 'clean'):
        if (args.full == True):
            engine.clean()
        else:
            engine.clean_build_root()
    return 0

# Make this file executable...
if __name__ == '__main__':
    sys.exit(main())