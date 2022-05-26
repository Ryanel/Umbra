#!/usr/bin/env python3
# The Nyx Package Manager
# File Purpose: Frontend and option selection

import os
import sys
import subprocess
import argparse
import json
import shutil
from rich import inspect
from rich.table import Table
from rich.pretty import Pretty
from nyx.engine import Engine
from nyx.globals import *
from nyx import json

current_config = {}

def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("module", help="What to do")
    parser.add_argument("packages", help="The packages to build", nargs="*")
    parser.add_argument("--temp", help="Where temporary files are stored", default="/build/build/")
    parser.add_argument("--config", help="Path to a repo configuration (config.json)", default="./")
    parser.add_argument("--repo", help="Path to a repo.json file", default="./")
    parser.add_argument("--only", help="Force only binary/source packages", default="")
    parser.add_argument("--prefer", help="Prefer either binary or source packages", default="source")
    parser.add_argument('--verbose', '-v', action='count', default=0, help="How verbose output will be.")
    parser.add_argument("--rebuild", help="Rebuilds the selected packages", action="store_true")
    parser.add_argument("--rebuild-deps", help="Rebuilds the depedencies of the selected packages", action="store_true")
    args = parser.parse_args()

    # Read the current config
    current_config = json.read_json("config.json")
    # Start the build engine...
    engine = Engine("docker")
    engine.load_packages(args.repo)
    engine.load_state(args.config + "state.json")
    engine.load_environment()

    # Seperate modules into different files
    # Module: build, view, clean, install/uninstall, rebuild-sysroot, pkgbuild, 
    # Search, Info, config

    if (args.module == "view"):
        if (len(args.packages) > 0):
            nyx_log.info(f"Package string: {args.packages}")
        else:
            # Print info about all loaded packages...
            table = Table(title="Package States")
            table.add_column("Name", justify="left", style="green")
            table.add_column("Version", justify="right", style="magenta")
            table.add_column("Installed", justify="right", style="white")
            table.add_column("Dependencies", justify="left", style="dim white")

            for x in engine.packages:
                table.add_row(x,
                    engine.packages[x].version, 
                    "Yes" if engine.packages[x].state["installed"] else "No",
                    Pretty(engine.packages[x].dependencies)
                )
            nyx_log.console.print(table)
            pass
    elif (args.module == "install"):
        if (len(args.packages) > 0):
            nyx_log.debug(f"Calculating dependencies for {args.packages}")
            total_deps = set()
            
            for x in args.packages:
                pkg = engine.query_package(x, latest_if_no_version=True)
                if len(pkg) == 1:
                    total_deps.add(pkg[0])
                    total_deps.update(engine.get_dependencies(pkg[0]))
                elif len(pkg) == 0:
                    nyx_log.error(f"{x} matches no packages")
                    return
                else:
                    nyx_log.error(f"{x} matches multiple packages.")

            deps_in_install_order = engine.sort_install_order(total_deps)

            table = Table(title="Install order")
            table.add_column("Name", justify="left", style="green")
            table.add_column("Version", justify="right", style="magenta")
            table.add_column("Action", justify="right", style="white")

            reversed_list = deps_in_install_order[::-1]

            for x in reversed_list:
                if not x.state["installed"]:
                    table.add_row(x.name,
                        x.version,
                        "Build"
                    )
                elif x.name in args.packages and args.rebuild:
                    table.add_row(x.name,
                        x.version,
                        "Rebuild"
                    )
                elif x.state["installed"] and args.rebuild_deps and not x.isTool:
                    table.add_row(x.name,
                        x.version,
                        "Rebuild Dependency"
                    )
            nyx_log.console.print(table)
            nyx_log.input("Proceed? (y/n)")

            for x in deps_in_install_order:
                if not x.state["installed"] or x.name in args.packages and args.rebuild or x.state["installed"] and args.rebuild_deps and not x.isTool:
                    rb = x.name in args.packages and args.rebuild or x.state["installed"] and args.rebuild_deps and not x.isTool
                    engine.coordinator_build_package(current_config, x, rb)

        else:
            nyx_log.error(f"No package specified, unable to build.")
    else:
        nyx_log.error(f"No module named {args.module} found.")

# Make this file executable...
if __name__ == '__main__':
    sys.exit(main())