import os
from posixpath import curdir, dirname, split
import shutil
import subprocess
import tarfile
from string import Template
from pkg_resources import parse_version


class NyxPackage:
    """A packaged application, potentially from source"""

    def __init__(self, name, path):
        self.name: str = name
        self.version = NyxPackage.pkgstr_version(name)
        self.state = {
            "have_source": False,
            "patched": False,
            "built": False,
            "built_from_source": False,
            "has_package": False,
            "installed": False,
            "architecture": "x86_64"
        }
        self.description = ""
        self.architectures = ["*"]
        self.installType = "sysroot"
        self.dependencies = []
        self.patches = []
        self.steps = {}
        self.steps["configure"] = []
        self.steps["build"] = []
        self.steps["package"] = []
        self.install_root = '/lib/'
        self.pkg_file_path = path

        # Build Stuff
        self.buildEnvironment = {}
        self.buildEnvironment["env"] = {}
        self.source = {}
        self.source["type"] = "local" # Local, Local Copy, Git, http, 


    def compareVersions(x, y):
        x_ver = parse_version(x.version)
        y_ver = parse_version(y.version)

        if x_ver > y_ver:
            return -1
        elif x_ver < y_ver:
            return 1
        return 0


    def loadJson(self, pkg_json:any):
        self.name                    = pkg_json["name"] or self.name
        self.architectures           = pkg_json.get("architecture", ["*"])
        self.description             = pkg_json.get("description", "")
        self.dependencies            = pkg_json.get("depends_on", [])
        self.patches                 = pkg_json.get("patches", [])
        self.buildEnvironment["env"] = pkg_json.get("environment", dict())
        self.installType             = pkg_json.get("install_type", "sysroot") # "sysroot", "tool", "initrd" are valid options
        self.steps["configure"]      = pkg_json.get("configure_steps", [])
        self.steps["build"]          = pkg_json.get("build_steps", [])
        self.steps["package"]        = pkg_json.get("package_steps", [])
        self.install_root            = pkg_json.get("install_root", "/")
        self.source["type"]          = pkg_json.get("acquisition", "local")
        self.source["path"]          = pkg_json.get("src_uri", "")
        self.source["branch"]        = pkg_json.get("git_branch", "master")
        self.source["tag"]           = pkg_json.get("git_tag", "")
        pass

    def print_info(self):
        print(f"Package {self.name}-{self.version} for {self.state['architecture']}")
        print(f"Install Type: {self.installType}")
        print(f"install root: {self.install_root}")
        print(f"supports: {self.supported_arch}")
        print(f"depends on: {self.dependencies}")
        print(f"patches: {len(self.patches)}")
        print(f"steps: {self.steps}")
        print(f"source: {self.source}")
        print(f"environment: {self.buildEnvironment}")        


    def pkgstr_version(s: str) -> str or None:
        ver_part = s.rpartition('-')
        return ver_part[2] if ver_part[0] != "" else None


    def pkgstr_name(s: str) -> str: return s.rpartition('-')[0] or s

    def execute_commands(self, steps, cwd, config, env) -> bool:
        final_env = self.compute_environment(config, env)
        #print(f"Execute: {cwd}, {steps}, env {final_env}")

        for x in steps:
            temp_obj = Template(x)
            prefix = final_env['INSTALL_DIR']
            tool_prefix = os.path.abspath(config['build_env']['tool_path']) + "/" + self.install_root
            parsed_steps = temp_obj.substitute(SYSROOT=final_env["SYSROOT"],INSTALL_DIR=final_env["INSTALL_DIR"], TARGET=f'{config["target"]}-umbra', PREFIX=prefix,TOOLPREFIX=tool_prefix, THREADS=f'{os.cpu_count()}')
            splitargs = [parsed_steps]
            status = subprocess.run(splitargs, shell=True, cwd=cwd, env=final_env)
            if status.returncode != 0:
                return False
        return True

    def get_source_dir(self, config) -> str:
        if (self.source["type"] == "local"):
            return config["build_env"]["source_path"] + self.source["path"] 
        return f"{config['build_env']['build_path']}src/{self.name}-{self.version}";

    def compute_environment(self, config, global_environment):
        env = dict()
        env |= global_environment
        env['INSTALL_DIR'] = os.path.abspath(config["build_env"]["build_path"] + f"tmp/install/{self.name}/")
        self.util_createpath(env['INSTALL_DIR'])

        env['SYSROOT']     = os.path.abspath(config["build_env"]["system_root"])
        self.util_createpath(env['SYSROOT'])

        env['BUILD_DIR']   = os.path.abspath(config["build_env"]["build_path"] + f"tmp/build/{self.name}")
        self.util_createpath(env['BUILD_DIR'])
        if (self.installType == "tool"):
            env['CC'] = 'gcc' # Native
            env['CXX'] = 'g++'# Native
            env['AR'] = 'ar'  # Native
        else:
            env['DESTDIR'] = os.path.abspath(config["build_env"]["build_path"] + f"tmp/install/{self.name}/")

        env['PATH'] = os.path.abspath(config["build_env"]["tool_path"]) + "/host-tools/bin" + ":" + env['PATH']
        env |= self.buildEnvironment["env"]
        return env

    def build(self, config, env, shouldInstall=True):
        self.state["built_from_source"] = True
        if (not self.state["have_source"]): 
            if self.get_source(config, env):
                self.state["have_source"] = True
            else:
                print(f"Failed in obtaining source files for {self.name}!")
                return False;

        if (not self.state["patched"]): 
            if self.patch(config, env):
                self.state["patched"] = True
            else:
                print(f"Failed in patching source files for {self.name}!")
                return False;

        if (not self.state["built"]): 
            if not self.configure(config, env):
                print(f"Failed in configuration for {self.name}!")
                return False;
            if not self.compile(config, env):
                print(f"Encountered a compiliaton error for {self.name}!")
                return False;
            self.state["built"] = True
            
        if (not self.state["has_package"]): 
            if self.package(config, env):
                self.state["has_package"] = True
            else:
                print(f"Failed in packaging files for {self.name}!")
                return False;
        if (not self.state["installed"] and shouldInstall):
            if self.install(config, env):
                self.state["installed"] = True
            else:
                print(f"Failed in installing file to system root for {self.name}!")
                return False; 

        print(f"Packaging succeeded for {self.name}!")
        return True;

    def get_source(self, config, env):
        dest_dir = self.get_source_dir(config)

        if (self.source["type"] == 'local'):
            # Out of source build, nothing to do if we already have the files.
            pass
        elif (self.source["type"] == 'local_copy'):
            self.util_createpath(dest_dir)
            shutil.copytree(config["build_env"]["source_path"] + self.source["path"], dest_dir, dirs_exist_ok=True)
        elif (self.source["type"] == 'git'):
            # TODO: How should this be handled?
            if os.path.isdir(os.path.abspath(dest_dir)):
                return True
            self.util_createpath(dest_dir)
            branch = self.source["tag"] if self.source["tag"] != "" else self.source["branch"]
            status = subprocess.run(['git', 'clone', self.source["path"], f'--branch={branch}','--depth=1','.'], shell=False, cwd=dest_dir)
            return status.returncode == 0
        else:
            return False    
        return True

    def patch(self, config, env) -> bool:
        tmp_dir = self.get_source_dir(config)
        if len(self.patches) > 0:
            for patch_path in self.patches:
                patch = os.path.join(self.pkg_file_path, patch_path)
                print(f"[nyx]: Applying patch {patch} to {tmp_dir}")
                with open(os.path.abspath(patch), "rb") as f:
                    patch_data = f.read()
                process = subprocess.Popen(['patch', '-ruN', '-p1', '-d', '.'], shell=False, cwd=tmp_dir, stdin=subprocess.PIPE)
                process.communicate(input=patch_data)
        return True

    def configure(self, config, env):
        return self.execute_commands(self.steps["configure"], self.get_source_dir(config), config, env)

    def compile(self, config, env):
        return self.execute_commands(self.steps["build"], self.get_source_dir(config), config, env)

    def package(self, config, env) -> bool:
        pkg_path = os.path.abspath(config["build_env"]["package_cache"])
        self.util_createpath(pkg_path)

        if (self.installType == 'tool'):
            install_dir = os.path.abspath(f"{config['build_env']['tool_path']}/{self.install_root}")
        else:
            install_dir = os.path.abspath(config["build_env"]["build_path"] + f"tmp/install/{self.name}/")
        self.util_createpath(install_dir)
        
        successful = self.execute_commands(self.steps["package"], self.get_source_dir(config), config, env)
        if not successful:
            return False

        # Package into a NPA...
        self.util_createpath(f"{pkg_path}/{self.name.split('/')[0]}")
        with tarfile.open(self.pkg_path(config), "w:gz") as zip_ref:
            zip_ref.add(install_dir, arcname='', recursive=True)
        return True

    def install(self, config, env):
        """Installs the package into the system root"""
        final_env = self.compute_environment(config, env)
        sysroot_dir = ""

        if (self.installType == 'tool'):
            sysroot_dir = os.path.abspath(f"{config['build_env']['tool_path']}/{self.install_root}")
        elif (self.installType == 'initrd'):
            sysroot_dir = os.path.abspath(f"{config['build_env']['initrd_root']}/{self.install_root}")
        else:
            sysroot_dir = final_env["SYSROOT"]
        
        self.util_createpath(sysroot_dir)
        with tarfile.open(self.pkg_path(config), mode="r") as zip_ref:
            zip_ref.extractall(sysroot_dir)
        return True

    def clean(self, config, env):
        """Cleans all compiled traces of this package"""
        final_env = self.compute_environment(config, env)
        self.state["installed"] = False
        if self.state["has_package"]:
            self.state["has_package"] = False
            os.remove(self.pkg_path(config))
        if self.state["built_from_source"]:
            self.state["built_from_source"] = False
            self.state["built"] = False
            self.state["patched"] = False
            self.state["have_source"] = False
            shutil.rmtree(os.path.abspath(config["build_env"]["build_path"] + f"tmp/install/{self.name}/"))
            shutil.rmtree(final_env['BUILD_DIR'])
            if self.source["type"] != "local":
                shutil.rmtree(os.path.abspath(self.get_source_dir(config)))

    def util_createpath(self, path):
        if not os.path.isdir(os.path.abspath(path)):
            os.makedirs(os.path.abspath(path))

    def pkg_path(self, config):
        return f'{os.path.abspath(config["build_env"]["package_cache"])}/{self.name}-{self.version}.tar.gz'