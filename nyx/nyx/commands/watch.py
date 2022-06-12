from time import time
from nyx.globals import *
from rich.table import Table
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

from nyx.commands.install import CommandInstall

was_processing = False
        
class CleanExit(object):
    def __enter__(self):
        return self
    def __exit__(self, exc_type, exc_value, exc_tb):
        if exc_type is KeyboardInterrupt:
            return True
        return exc_type is None

class FSEventHandler(FileSystemEventHandler):
    def __init__(self, args, engine, config) -> None:
        self.timeout = time() - 1
        self.timeoutPeriod = 1
        was_processing = False
        self.args = args
        self.engine = engine
        self.config = config
        super().__init__()


    def on_any_event(self, event):
        global was_processing
        if self.timeout <= time() and not was_processing:
            was_processing = True
        else:
            if not was_processing:
                nyx_log.debug(f"Ignoring: {event}")
            return

        nyx_log.debug(f"Event: {event}")

        CommandInstall(self.args, self.engine, self.config).run()
        self.timeout = time() + self.timeoutPeriod
        was_processing = False
        pass


class CommandWatch:
    """Watches for any changes in the src directory, then recompiles the packages..."""
    def __init__(self, args, engine, config) -> None:
        self.args = args
        self.engine = engine
        self.config = config

        self.args.yes = True

    def event_handler():
        nyx_log.debug("event")

    def run(self):

        should_exit = False

        while not should_exit:
            observer = Observer()
            event_handler = FSEventHandler(self.args, self.engine, self.config)
            observer.schedule(event_handler, "./src", recursive=True)
            observer.start()
            try:   
                with CleanExit():
                    while observer.is_alive():
                        observer.join(1)
                if not was_processing:
                    should_exit = True
                    nyx_log.info("Exiting...")
                else:
                    nyx_log.info("Process terminated, continuing watch...")
            finally:
                observer.stop()
                observer.join()
