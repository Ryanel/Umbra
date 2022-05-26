import time
import readline
import rich
from rich.console import Console

class Log:
    def __init__(self):
        self.isTerminal = True
        self.logLevel = 0
        self.console = Console()

    def set_no_color(self):
        self.console = Console(color_system=None)

    def process(self, str):
        return self.console.status(str, spinner='bouncingBar')

    def set_log_file(self, path):
        if path == "":
            return

    def debug(self, string):
        self.console.log(f"[gray]{string}[/gray]")

    def info(self, string):
        self.console.log(f"[white]{string}[/white]")

    def error(self, string):
        self.console.log(f"[red]{string}[/red]")

    def input(self, question):
        return self.console.input(question)