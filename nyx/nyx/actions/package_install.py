import os
import tarfile
from nyx.globals import *

from nyx.actions.package_action import PackageAction

"""Installs a package to SYSROOT"""
class InstallAction(PackageAction):
    def execute(self) -> bool:
        if not self.package.has_package(self.config):
            nyx_log.error(f"Was not able to find a installable package file for {self.package.name}-{self.package.version}!")
            return False

        sysroot_dir = ""
        if (self.package.installType == 'tool'):
            sysroot_dir = os.path.abspath(f"{self.config['build_env']['tool_path']}/{self.package.install_root}")
        elif (self.package.installType == 'initrd'):
            sysroot_dir = os.path.abspath(f"{self.config['build_env']['initrd_root']}/{self.package.install_root}")
        else:
            sysroot_dir = self.env["SYSROOT"]

        self.package.util_createpath(sysroot_dir)
        with tarfile.open(self.package.pkg_path(self.config), mode="r") as zip_ref:
            zip_ref.extractall(sysroot_dir)

        self.package.state["installed"] = True
        return True
