#!/usr/bin/env python3
# The Nyx Package Manager
# File Purpose: Frontend and option selection
import sys
import argparse
import json

from nyx.engine import Engine
from nyx.globals import *
from nyx.json import nyx_read_json

# Commands
from nyx.commands.view import CommandView
from nyx.commands.install import CommandInstall
from nyx.commands.watch import CommandWatch

def main() -> int:
    parser = argparse.ArgumentParser(prog='npkg')
    parser.add_argument("module", help="What to do")
    parser.add_argument("packages", help="The packages to build", nargs="*")
    parser.add_argument("--temp", help="Where temporary files are stored", default="/build/build/")
    parser.add_argument("--config", help="Path to a repo configuration (config.json)", default="./")
    parser.add_argument("--only", help="Force only binary/source packages", default="")
    parser.add_argument("--prefer", help="Prefer either binary or source packages", default="binary")
    parser.add_argument('--verbose', '-v', action='count', default=0, help="How verbose output will be.")
    parser.add_argument("--rebuild", help="Rebuilds the selected packages", action="store_true")
    parser.add_argument("--rebuild-deps", help="Rebuilds the depedencies of the selected packages", action="store_true")
    parser.add_argument("--build-iso", help="Builds and runs an iso after the build command", action="store_true")
    parser.add_argument("--run-iso", help="Builds and runs an iso after the build command", action="store_true")
    parser.add_argument("--yes", help="Always agrees", action="store_true")
    parser.add_argument("--no-clean", help="Do not clean on rebuild", action="store_true")
    args = parser.parse_args()

    # Read the current config
    current_config = nyx_read_json("config.json")
    current_config['host_type']='host'
    repo_location = "./repo/" if current_config["repo_location"] is None else current_config["repo_location"]

    # Start the build engine...
    engine = Engine("docker")
    engine.load_packages(repo_location)
    engine.load_state(args.config + "state.json")
    engine.load_environment()

    # Run the needed command.
    if (args.module == "view"):
        CommandView(args, engine, current_config).run()
    elif (args.module == "run"):
        engine.run(current_config)
    elif (args.module == "watch"):
        CommandWatch(args, engine, current_config).run()
    elif (args.module == "install"):
        CommandInstall(args, engine, current_config).run()
    else:
        nyx_log.error(f"No module named {args.module} found.")

# Make this file executable...
if __name__ == '__main__':
    sys.exit(main())