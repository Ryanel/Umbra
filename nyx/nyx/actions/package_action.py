from nyx.package import NyxPackage

class PackageAction:
    def __init__(self, config: dict, final_env: dict, package: NyxPackage) -> None:
        self.config = config
        self.env = final_env
        self.package = package

    def execute(self) -> bool:
        return True
