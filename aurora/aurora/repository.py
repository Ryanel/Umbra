from aurora.package import Package
import yaml

class Repository:
    """A repository is a collection of packages"""
    def __init__(self, path=None):
        self.packages = []
        self.path=""
        if path is not None:
            self.path = path
            self.load_from_file(path + "/packages.yml")

    def add_package(self, package):
        """Adds a package to the repository"""
        self.packages.append(package)

    def load_from_file(self, filename):
        """Loads a repository from a file"""
        with open(filename, 'r', encoding="utf8") as file:
            data = yaml.safe_load(file)
            for package in data:
                self.add_package(Package(package, self, data[package]))