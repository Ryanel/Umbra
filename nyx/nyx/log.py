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
        self.isTerminal = False

    def process(self, str):
        return self.console.status(str, spinner='bouncingBar')

    def set_log_file(self, path):
        if path == "":
            return
        self.isTerminal = False

    def debug(self, string):
        if self.isTerminal:
            self.console.log(f"[gray]{string}[/gray]")
        else:
            print(string)

    def info(self, string):
        if self.isTerminal:
            self.console.log(f"[white]{string}[/white]")
        else:
            print(string)

    def error(self, string):
        if self.isTerminal:
            self.console.log(f"[red]{string}[/red]")
        else:
            print(string)

    def input(self, question):
        return self.console.input(question)