from nyx.actions.package_action import PackageAction

"""Uninstalls a package from SYSROOT"""
class UninstallAction(PackageAction):
    def execute(self) -> bool:
        self.package.state["installed"] = False
