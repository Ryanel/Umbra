#!/usr/bin/env python3
# NBuild: Nix Builder
# Build a single package. May not need to be called.
# Assumes a package has been prepared by nyx. Only handles actually building the package, nothing more.
# This is intended to be run in a container at the direction of Nyx, but it can be run on it's own. It loads the current configuration from Nyx
import sys
import argparse
from nyx.globals import *
from nyx.engine import Engine
from nyx.package import NyxPackage;
from nyx.json import nyx_read_json

current_config = {}
current_state = {}

def main() -> int:
    global current_config
    global current_state

    parser = argparse.ArgumentParser()
    parser.add_argument("command", help="The command to execute")
    parser.add_argument("package", help="The package to build")
    parser.add_argument("--dest", "--destination", help="The destination to store the package", default="out.npa")
    parser.add_argument("--temp", help="Where temporary files are stored", default="/build/build/")
    parser.add_argument("--config", help="Path to a repo configuration (config.json)", default="./")
    parser.add_argument("--log", help="Sets a file for log output to be directed to", default="")
    parser.add_argument("--no-color", help="Disables color printing", action="store_true")
    parser.add_argument("--clean", help="Cleans the package before running the command", action="store_true")
    parser.add_argument("--no-clean", help="Does not clean the package after installation", action="store_true")
    parser.add_argument("--rebuild", help="Forcibly rebuilds the package", action="store_true")
    
    args = parser.parse_args()

    nyx_log.set_no_color()
    nyx_log.set_log_file(args.log)

    # Read the current config and setup the world
    current_config = nyx_read_json(args.config + "config.json");
    repo_location = "./repo/" if current_config["repo_location"] is None else current_config["repo_location"]

    engine = Engine("local")
    engine.load_packages(repo_location);
    engine.load_state(args.config + "state.json")
    engine.load_environment()

    ## Find the package based on the query string
    packages_matching = engine.query_package(args.package)
    the_package: NyxPackage = None

    if len(packages_matching) > 1:
        nyx_log.error(f"Found multiple packages that refer to [bold green]\"{args.package}\"[/bold green]")
        return
    elif len(packages_matching) == 0:
        nyx_log.error(f"No packages were found matching [bold green]\"{args.package}\"[/bold green]")
        return
    else:
        the_package = packages_matching[0]

    if (args.clean):
        the_package.clean(current_config, engine.environment)

    # Force building, installing, and packaging...
    if (args.rebuild):
        the_package.state["built"] = False
        the_package.state["installed"] = False
        the_package.state["has_package"] = False

    if (args.command == "build"):
        with nyx_log.process(f"Building {args.package}"):
            the_package.build(current_config, args, engine.environment, shouldInstall=False)
    elif (args.command == "install"):
         with nyx_log.process(f"Building + Installing {args.package}"):
            the_package.build(current_config, args, engine.environment, shouldInstall=True)
    elif (args.command == "clean"):
        the_package.clean(current_config, engine.environment)
    else:
        nyx_log.error(f"No command {args.command}")
    engine.save_state(args.config + "state.json")

# Make this file executable... 
if __name__ == '__main__':
    sys.exit(main())