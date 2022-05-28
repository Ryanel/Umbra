import os
from posixpath import curdir, dirname, split
import shutil
import subprocess
import tarfile
from string import Template

class NyxPackage:
    """A packaged application, potentially from source"""

    def __init__(self, name):
        self.name: str = name
        self.friendly_name = "Unknown"
        self.version = "1.0"
        self.destination = "disk"
        self.state = {
            "have_source": False,
            "patched": False,
            "built": False,
            "built_from_source": False,
            "has_package": False,
            "installed": False,
            "architecture": "x86_64"
        }
        self.supported_arch = "all"
        self.isTool = False
        self.dependencies = []
        self.patches = []
        self.steps = {}
        self.steps["configure"] = []
        self.steps["build"] = []
        self.steps["package"] = []
        self.install_root = '/lib/'

        # Build Stuff
        self.buildEnvironment = {}
        self.buildEnvironment["env"] = {}
        self.source = {}
        self.source["type"] = "local" # Local, Local Copy, Git, http, 

    def loadJson(self, pkg_json:any):
        self.friendly_name           = pkg_json["name"] or self.name
        self.supported_arch          = pkg_json.get("architecture", "all")
        self.version                 = pkg_json.get("version", "1.0")
        self.dependencies            = pkg_json.get("dependencies", [])
        self.patches                 = pkg_json.get("patches", [])
        self.buildEnvironment["env"] = pkg_json.get("enviroment", dict())
        self.isTool                  = bool(pkg_json.get("is_tool", False))
        self.steps["configure"]      = pkg_json.get("configure_steps", [])
        self.steps["build"]          = pkg_json.get("build_steps", [])
        self.steps["package"]        = pkg_json.get("package_steps", [])
        self.install_root            = pkg_json.get("install_root", "/")
        self.source["type"]          = pkg_json.get("acquisition", "local")
        self.source["path"]          = pkg_json.get("src_path", "")
        self.source["branch"]        = pkg_json.get("git_branch", "master")
        self.source["tag"]           = pkg_json.get("git_tag", "")
        self.destination             = pkg_json.get("destination", "local")
        pass

    def print_info(self):
        print(f"Package {self.name}-{self.version} for {self.state['architecture']}")
        print(f"isTool: {self.isTool}")
        print(f"install root: {self.install_root}")
        print(f"supports: {self.supported_arch}")
        print(f"depends on: {self.dependencies}")
        print(f"patches: {len(self.patches)}")
        print(f"steps: {self.steps}")
        print(f"source: {self.source}")
        print(f"environment: {self.buildEnvironment}")        

    def execute_commands(self, steps, cwd, config, env) -> bool:
        final_env = self.compute_environment(config, env)
        #print(f"Execute: {cwd}, {steps}, env {final_env}")

        for x in steps:
            temp_obj = Template(x)
            prefix = final_env['INSTALL_DIR']
            tool_prefix = os.path.abspath(config['build_env']['tool_path']) + "/" + self.install_root
            parsed_steps = temp_obj.substitute(SYSROOT=final_env["SYSROOT"], TARGET=f'{config["target"]}-umbra', PREFIX=prefix,TOOLPREFIX=tool_prefix, THREADS=f'{os.cpu_count()}')
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
        if (self.isTool):
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
            for patch in self.patches:
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

        if (self.destination == 'tools'):
            install_dir = os.path.abspath(f"{config['build_env']['tool_path']}/{self.install_root}")
        else:
            install_dir = os.path.abspath(config["build_env"]["build_path"] + f"tmp/install/{self.name}/")

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
        file_path = config["build_env"]["package_cache"]
        sysroot_dir = ""

        if (self.destination == 'tools'):
            sysroot_dir = os.path.abspath(f"{config['build_env']['tool_path']}/{self.install_root}")
        elif (self.destination == 'initrd'):
            sysroot_dir = os.path.abspath(f"{config['build_env']['initrd_root']}/{self.install_root}")
            print(sysroot_dir)
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