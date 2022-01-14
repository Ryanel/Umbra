import os
from posixpath import curdir, split
import shutil
import subprocess
import tarfile

from string import Template
class NyxPackage:
    def __init__(self, name):
        self.name = name
        self.version = 'dev'
        self.acquisition = "local_copy" # Local, git, or http...
        self.destination = "disk" # Disk or Initrd
        self.cached = False
        self.src_path = "lib/test/"
        self.architecture = "all"
        self.tools = []
        self.requirements = []
        self.patches = []
        self.configure_steps = []
        self.build_steps = []
        self.package_steps = []
        self.installroot = '/lib/'
        self.enviroment = dict()
        self.isTool = False

    def loadFromJson(self, pkg_json, config):
        self.friendly_name   = pkg_json["name"] or self.name
        self.architecture    = pkg_json.get("architecture", "all")
        self.version         = pkg_json.get("version", "dev")
        self.acquisition     = pkg_json.get("acquisition", "local")
        self.src_path        = pkg_json.get("src_path", "")
        self.tools           = pkg_json.get("tools", [])
        self.requirements    = pkg_json.get("requirements", [])
        self.patches         = pkg_json.get("patches", [])
        self.configure_steps = pkg_json.get("configure_steps", [])
        self.build_steps     = pkg_json.get("build_steps", [])
        self.package_steps   = pkg_json.get("package_steps", [])
        self.installroot     = pkg_json.get("install_root", "/")
        self.destination     = pkg_json.get("destination", "local")
        self.enviroment      = pkg_json.get("enviroment", dict())
        self.isTool          = bool(pkg_json.get("is_tool", False))
        self.git_branch      = pkg_json.get("git_branch", "master")
        self.git_tag         = pkg_json.get("git_tag", "")
        self.cached          = os.path.exists(self.pkg_path(config))

    def pkg_path(self, config):
        return f"{config['package_root']}{self.name}-{self.version}.tar.gz";

    def src_dir(self, config):
        if (self.acquisition == 'local'):
            return config["source_root"] + self.src_path
        else:
            return self.build_dir(config["build_root"])

    def build_dir(self, curtmp):
        return os.path.abspath(f"{curtmp}build/{self.name}-{self.version}")

    def package_dir(self, curtmp):
        return os.path.abspath(f"{curtmp}artifacts/{self.name}-{self.version}")

    def computed_enviroment(self, common, config):
        env = dict()
        env |= common
        env['DESTDIR'] = self.package_dir(config["build_root"])
        env['INSTALL_DIR'] = self.package_dir(config["build_root"])
        env['SYSROOT'] = os.path.abspath(config["sysroot"])
        env['BUILD_DIR'] = self.build_dir(config["build_root"])

        if (self.isTool):
            env['CC'] = 'gcc' # Native
            env['CXX'] = 'g++'# Native
            env['AR'] = 'ar'  # Native

        env['PATH'] = os.path.abspath(config['tool_root']) + "/host-tools/bin" + ":" + env['PATH']
        env |= self.enviroment
        return env

    def fetch(self, config) -> bool:
        tmp_dir = self.build_dir(config["build_root"])

        if (self.acquisition == 'local_copy'):
            self.util_createpath(tmp_dir)
            shutil.copytree(config["source_root"] + self.src_path, tmp_dir, dirs_exist_ok=True)
            return True
        elif (self.acquisition == 'local'):
            # Out of source builds here
            self.util_createpath(tmp_dir)
            return True
        elif (self.acquisition == 'git'):
            # Git clone into this directory
            if os.path.isdir(os.path.abspath(tmp_dir)):
                return True
            self.util_createpath(tmp_dir)
            if (self.git_tag != ""):
                status = subprocess.run(['git', 'clone', self.src_path, f'--branch={self.git_tag}','--depth=1','.'], shell=False, cwd=tmp_dir)
                return status.returncode == 0
            else:
                status = subprocess.run(['git', 'clone', self.src_path, f'--branch={self.git_branch}','--depth=1','.'], shell=False, cwd=tmp_dir)
                return status.returncode == 0
        return False

    def patch(self, config) -> bool:
        tmp_dir = os.path.abspath(self.build_dir(config["build_root"]) + "/")
        if len(self.patches) > 0:
            for patch in self.patches:
                print(f"[nyx]: Applying patch {patch} to {tmp_dir}")
                with open(os.path.abspath(patch), "rb") as f:
                    patch_data = f.read()
                process = subprocess.Popen(['patch', '-ruN', '-p1', '-d', '.'], shell=False, cwd=tmp_dir, stdin=subprocess.PIPE)
                process.communicate(input=patch_data)
        return True

    def configure(self, config, common_enviroment) -> bool:
        cwd = self.src_dir(config)
        return self.execute_commands(common_enviroment, config, self.configure_steps, cwd)

    def build(self, config, common_enviroment) -> bool:
        cwd = self.src_dir(config)
        return self.execute_commands(common_enviroment, config, self.build_steps, cwd)

    def package(self, config, common_enviroment) -> bool:
        install_dir = os.path.abspath(self.package_dir(config["build_root"]))
        self.util_createpath(install_dir)

        cwd = self.src_dir(config)
        if not self.execute_commands(common_enviroment, config, self.package_steps, cwd):
            return False

        # Now, package up the files...
        self.util_createpath(self.pkg_path(config) + "/../")
        with tarfile.open(self.pkg_path(config), "w:gz") as zip_ref:
            zip_ref.add(install_dir, arcname='', recursive=True)
        return True

    def install(self, config) -> bool:
        sysroot_dir = os.path.abspath(f"{config['sysroot']}/{self.installroot}")

        if (self.destination == 'tools'):
            sysroot_dir = os.path.abspath(f"{config['tool_root']}/{self.installroot}")
        elif (self.destination == 'initrd'):
            sysroot_dir = os.path.abspath(f"{config['initrd_root']}/{self.installroot}")

        self.util_createpath(sysroot_dir)
        with tarfile.open(self.pkg_path(config), mode="r") as zip_ref:
            zip_ref.extractall(sysroot_dir)
        return True

    def print_info(self):
        print(f"{self.name}-{self.version} | arch: {self.architecture}")
        if (self.isTool):
            print("Tool")
        print(f"path: {self.src_path} source: {self.acquisition}")
        print(f"install: {self.installroot}")
        print(f"tools: {self.tools}")
        print(f"requirements: {self.requirements}")
        print(f"patches: {self.patches}")
        print(f"configuration: {self.configure_steps}")
        print(f"build: {self.build_steps}")
        print(f"package_steps: {self.package_steps}")
        print(f"env: {self.enviroment}")

    def execute_commands(self, common_enviroment, config, commands, cwd):
        bdir = self.build_dir(config["build_root"]) +'/'
        env = self.computed_enviroment(common_enviroment, config)

        for x in commands:
            temp_obj = Template(x)
            prefix = os.path.abspath(self.package_dir(config['build_root']))
            tool_prefix = os.path.abspath(config['tool_root']) + "/" + self.installroot
            parsed_steps = temp_obj.substitute(SYSROOT=os.path.abspath(config["sysroot"]), TARGET='x86_64-umbra', PREFIX=prefix,TOOLPREFIX=tool_prefix, THREADS='16')
            splitargs = [parsed_steps]
            cwd = self.src_dir(config)
            status = subprocess.run(splitargs, shell=True, cwd=cwd, env=env)
            if status.returncode != 0:
                return False
        return True

    def util_createpath(self, path):
        if not os.path.isdir(os.path.abspath(path)):
            os.makedirs(os.path.abspath(path))
