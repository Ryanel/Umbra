from nyx.globals import *
from rich.table import Table

class CommandInstall:
    def __init__(self, args, engine, config) -> None:
        self.args = args
        self.engine = engine
        self.config = config

    def display_and_ask(self, deps_in_install_order: list, skip = False) -> bool:
        if skip: 
            return True
        # Display table and ask for permission
        table = Table(title="Install order")
        table.add_column("Name", justify="left", style="green")
        table.add_column("Version", justify="right", style="magenta")
        table.add_column("Action", justify="right", style="white")

        reversed_list = deps_in_install_order[::-1]

        for x in reversed_list:
            if not x.state["installed"]:
                table.add_row(x.name, x.version, "Build")
            elif x.name in self.args.packages and self.args.rebuild:
                table.add_row(x.name, x.version, "Rebuild")
            elif x.state["installed"] and self.args.rebuild_deps and not x.installType == "tool":
                table.add_row(x.name, x.version, "Rebuild Dependency")
        nyx_log.console.print(table)

        if (not self.args.yes):
            input = nyx_log.input("Proceed? (y/n): ")
            if input != "y":
                return

    def run(self):
        result = 0
        if (len(self.args.packages) > 0):
            nyx_log.debug(f"Calculating dependencies for {self.args.packages}")
            total_deps = set()

            for x in self.args.packages:
                pkg = self.engine.query_package(x, latest_if_no_version=True)
                if len(pkg) == 1:
                    total_deps.add(pkg[0])
                    total_deps.update(self.engine.get_dependencies(pkg[0]))
                elif len(pkg) == 0:
                    nyx_log.error(f"{x} matches no packages")
                    return
                else:
                    nyx_log.error(f"{x} matches multiple packages.")

            deps_in_install_order = self.engine.sort_install_order(total_deps)

            self.display_and_ask(deps_in_install_order, self.args.module == 'watch' or self.args.yes)

            for x in deps_in_install_order:
                hasBinary = x.has_package(self.config)
                installed = x.state["installed"]
                rebuildInstallList = self.args.rebuild and not x.installType == "tool"
                rebuildDeps = self.args.rebuild_deps and not x.installType == "tool"
                isInstallList = x.name in self.args.packages

                if installed and isInstallList and not rebuildInstallList:
                    continue
                elif installed and not isInstallList and not rebuildDeps:   
                    continue

                # Determine if we want binary or source building. Override for rebuilds
                typePreferred = 'binary' if self.args.prefer=='binary' else 'source'
                if rebuildInstallList and isInstallList:
                    typePreferred = 'source'
                if rebuildDeps and not isInstallList:
                    typePreferred = 'source'

                # Are we potentially rebuilding?
                if hasBinary and typePreferred == 'binary':
                    result = self.engine.coordinator_run_command(self.config, f'./nyx/nbuild.py --no-color install-pkg {x.name}-{x.version}')
                else:
                    result = self.engine.coordinator_build_package(self.config, self.args, x, rebuildInstallList if isInstallList else rebuildDeps)

            if result == 0:
                if (self.args.build_iso or self.args.run_iso):
                    self.engine.build_iso(self.config)
                if (self.args.run_iso):
                    self.engine.run(self.config)
        else:
            nyx_log.error(f"No package specified, unable to build.")
