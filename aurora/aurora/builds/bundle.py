""" Bundles a package into a tarball. """
from aurora.package import Package
import os
import subprocess


def bundle_package(package_str: str, config: dict, env: dict) -> None:
    package_info = Package.interpret_package_string(package_str)

    # Clone env and remove unneeded variables
    new_env = env.copy()
    del new_env['FILESDIR']
    del new_env['WORKDIR']
    del new_env['TMPDIR']
    del new_env['D']
    del new_env['S']
    del new_env['PATH']
    del new_env['SYSROOT']
    del new_env['MAKE']

    # Create metadata directory
    metadata_dir = os.path.join(env['D'], '__metadata__')
    os.makedirs(metadata_dir, exist_ok=True)

    # We also need to create a dump of the environment variables used to build the package.

    # Output ENV as YAML
    env_yaml = open(os.path.join(metadata_dir, 'metadata.yaml'),
                    'w', encoding='utf-8')
    env_yaml.write('env:\n')
    for key, value in new_env.items():
        env_yaml.write('  ' + key + ': ' + value + '\n')
    env_yaml.close()

    # We need to create a tarball of the install dir
    tarball_path = os.path.join(config['package_dir'], package_str + '.tar.gz')
    os.makedirs(os.path.dirname(tarball_path), exist_ok=True)
    tarball = open(tarball_path, 'w', encoding='utf-8')
    subprocess.run(['tar', '-czf', tarball_path, '-C', env['D'], '.'], check=True)
    tarball.close()
