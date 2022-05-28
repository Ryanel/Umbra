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
            table = Table(title="Package States")
            table.add_column("Name", justify="left", style="green")
            table.add_column("Version", justify="right", style="magenta")
            table.add_column("Installed", justify="right", style="white")
            table.add_column("Dependencies", justify="left", style="dim white")

            for x in self.engine.packages:
                table.add_row(x,
                    self.engine.packages[x].version, 
                    "Yes" if self.engine.packages[x].state["installed"] else "No",
                    Pretty(self.engine.packages[x].dependencies)
                )
            nyx_log.console.print(table)