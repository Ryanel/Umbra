import os
import shutil
from nyx.actions.package_action import PackageAction

""" Cleans all build traces of a package"""
class CleanAction(PackageAction):
    def execute(self) -> bool:
        install_dir = os.path.abspath(self.config["build_env"]["build_path"] + f"tmp/install/{self.package.name}/")
        src_dir = os.path.abspath(self.package.get_source_dir(self.config))
        
        if os.path.isdir(install_dir):
            shutil.rmtree(install_dir)
        if os.path.isdir(self.env['BUILD_DIR']):
            shutil.rmtree(self.env['BUILD_DIR'])
        if self.package.source["type"] != "local":
            if os.path.isdir(src_dir):
                shutil.rmtree(src_dir)

        # At this point, the package no longer has source code...
        self.package.state["built"] = False
        self.package.state["patched"] = False
        self.package.state["have_source"] = False
