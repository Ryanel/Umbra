import os
from posixpath import curdir, split
import shutil
import subprocess
import zipfile

class NyxPackage:
    def __init__(self, name):
        self.name = name
        self.version = 'dev'
        self.acquisition = "local" # Local, git, or http...
        self.cached = True
        self.src_path = "lib/test/"
        self.architecture = "all"
        self.tools = []
        self.requirements = []
        self.patches = []
        self.configure_steps = []
        self.build_steps = ['make']
        self.package_steps = ['make install']
        self.installroot = '/lib/'
        self.enviroment = dict()
        self.isTool = False

    def build_dir(self, curtmp):
        if (self.isTool):
            return os.path.abspath(f"{curtmp}/../{self.src_path}")
        return os.path.abspath(f"{curtmp}build/{self.name}-{self.version}")

    def package_dir(self, curtmp):
        return os.path.abspath(f"{curtmp}artifacts/{self.name}-{self.version}")

    def computed_enviroment(self, common, config):
        env = dict()
        env |= common
        env |= self.enviroment
        env['INSTALL_DIR'] = self.package_dir(config["build_root"])
        env['SYSROOT'] = os.path.abspath(config["sysroot"])

        return env

    def fetch(self, config) -> bool:
        tmp_dir = self.build_dir(config["build_root"])
        self.util_createpath(tmp_dir)
        if (self.acquisition == 'local'):
            shutil.copytree(config["source_root"] + self.src_path, tmp_dir, dirs_exist_ok=True)
            return True
        elif (self.acquisition == 'git'):
            # Git clone into this directory
            status = subprocess.run(['git', 'clone', self.git_repository, f'--branch={self.git_branch}','--depth=1'], shell=False, cwd=tmp_dir)
            if status.returncode != 0:
                return True
            return True
        return False

    def patch(self, config) -> bool:
        return True

    def configure(self, config, common_enviroment) -> bool:
        return True

    def build(self, config, common_enviroment) -> bool:
        bdir = self.build_dir(config["build_root"]) +'/'
        env = self.computed_enviroment(common_enviroment, config)

        for x in self.build_steps:
            splitargs = x.split(' ')
            print(bdir)
            status = subprocess.run(splitargs, shell=False, cwd=bdir, env=env)
            if status.returncode != 0:
                return False
        return True

    def package(self, config, common_enviroment) -> bool:
        install_dir = os.path.abspath(self.package_dir(config["build_root"]))
        self.util_createpath(install_dir)

        env = self.computed_enviroment(common_enviroment, config)

        for x in self.package_steps:
            splitargs = x.split(' ')
            status = subprocess.run(splitargs, shell=False, cwd=self.build_dir(config["build_root"]), env=env)
            if status.returncode != 0:
                return False

        # Now, package up the files...
        shutil.make_archive(f"{config['package_root']}/{self.name}-{self.version}", "zip", install_dir)
        return True

    def install(self, config) -> bool:
        sysroot_dir = os.path.abspath(f"{config['sysroot']}/{self.installroot}")
        self.util_createpath(sysroot_dir)
        with zipfile.ZipFile(f"{config['package_root']}/{self.name}-{self.version}.zip","r") as zip_ref:
            zip_ref.extractall(sysroot_dir)

        return True

    def print_info(self):
        print(f"{self.name}-{self.version} | arch: {self.architecture}")
        print(f"src: {self.src_path} cached: {self.cached}")
        print(f"tools: {self.tools}")
        print(f"requirements: {self.requirements}")

    def util_createpath(self, path):
        if not os.path.isdir(path):
            os.makedirs(path)
