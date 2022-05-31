from nyx.globals import *

from rich import inspect
from rich.table import Table
from rich.pretty import Pretty

class CommandView:
    def __init__(self, args, engine, config) -> None:
        self.args = args
        self.engine = engine
        self.config = config


    def run(self):
        if (len(self.args.packages) > 0):
            nyx_log.info(f"Package string: {self.args.packages}")
        else:
            # Print info about all loaded packages...
            table = Table(title="All Packages")
            table.add_column("Name", justify="left", style="green")
            table.add_column("Version", justify="right", style="magenta")
            table.add_column("Ins", justify="right", style="white")
            table.add_column("Description", justify="left", style="white")
            table.add_column("Dependencies", justify="left", style="dim white")

            for x in self.engine.packages:
                pkg = self.engine.packages[x]
                table.add_row(pkg.name,
                    pkg.version, 
                    "Yes" if pkg.state["installed"] else "No",
                    pkg.description,
                    Pretty(pkg.dependencies)
                )
            nyx_log.console.print(table)