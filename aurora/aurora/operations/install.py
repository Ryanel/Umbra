import os
import shutil
import tarfile
import docker


def operation_build(universe, config, package) -> bool:
    """ Build a package using a build driver. """
    driver = config.get('aur_build_driver')
    if driver == 'docker':
        return operation_build_docker(universe, config, package)

    print('[build] => Unknown build driver: ' + driver)
    return False


def operation_build_docker(universe, config, package) -> bool:
    """ Builds a package within a docker container"""

    got_connection = False
    container = None
    try:
        client = docker.from_env()
        client.ping()  # Raises an exception if docker is unresponsive
        got_connection = True

        container_name = 'aurora-build-runner'
        image_name = 'aurora-buildenv:latest'

        # Check if the image exists
        images = client.images.list()
        image_exists = False
        for image in images:
            if image_name in image.tags:
                image_exists = True
                break
        if image_exists is False:
            print(
                f"[build] => Image {image_name} does not exist. Please build it first.")
            return False

        # Finally, launch the container.
        print(
            f"[build] => Building {package.name}-{package.data['version']} using docker")

        package_string = package.name + '-' + package.data['version']
        container = client.containers.run(image_name,
                                          f"bash -c 'source /home/buildenv/buildenv-rc; aurora_build {package_string}'",
                                          detach=True,
                                          stderr=True,
                                          stdout=True,
                                          remove=False,
                                          name=container_name,
                                          user='buildenv',
                                          volumes={
                                              config.get('temp_path'): {'bind': '/opt/aurora/tmp', 'mode': 'rw'},
                                              config.get('toolchain_path'): {'bind': '/opt/aurora/tools', 'mode': 'rw'},
                                              config.get('system_root'): {'bind': '/opt/aurora/image', 'mode': 'rw'},
                                              config.get('media_dir'): {'bind': '/opt/aurora/media', 'mode': 'rw'},
                                              config.get('package_dir'): {'bind': '/opt/aurora/packages', 'mode': 'rw'},
                                              config.get('source_dir'): {'bind': '/opt/aurora/src', 'mode': 'rw'},
                                              config.config_path: {'bind': '/opt/aurora/cfg', 'mode': 'rw'},
                                              os.path.abspath(config.repos['default']['path']): {'bind': '/opt/aurora/repos/default', 'mode': 'rw'},
                                          }
                                          )

        for line in container.logs(stream=True):
            print(line.decode("utf-8").strip())
        return_code = container.wait()
        container.remove()
        return return_code['StatusCode'] == 0

    except docker.errors.APIError as ex:
        print(f'[build] => Docker error: {str(ex)}')
    except KeyboardInterrupt:
        print('[build] => Interrupted by user')
        if container is not None:
            container.kill()
            container.remove()
    return False


def operation_install(universe, config, package) -> bool:
    """ Install a package """
    is_package_obtained = False
    can_find_binary = False
    can_build = config.get('opt_build') is True
    pkg_full = package.name + '-' + package.data['version']
    cache_path = os.path.join(config.get('package_dir'), pkg_full + '.tar.gz')

    # Do we have the package in the cache?
    if config.get('opt_ignore_cache') is False:
        exists = os.path.exists(cache_path)
        if exists:
            print('[fetch] => Cache hit for ' + pkg_full)

    # Try finding a binary package
    if can_find_binary and is_package_obtained is False:
        print('[fetch] => Downloading ' + pkg_full)
        is_package_obtained = False

    if can_build and is_package_obtained is False:
        # Try to build the package
        is_package_obtained = operation_build(universe, config, package)

    if is_package_obtained is False:
        print('[fetch] => Could not obtain package ' + pkg_full)
        return False

    # We now have a package...
    # Extract to the destination based on instructions in the package

    # Unzip the file at cache_path
    # Extract the package to the destination

    destination = config.get('system_root')
    print('[install] => Installing ' + pkg_full)

    tar = tarfile.open(cache_path)
    tar.extractall(destination)
    tar.close()
    
    #  TODO: This is a hack to remove the metadata, find something better
    shutil.rmtree(os.path.join(destination, '__metadata__'))
    universe.world_set.add_package(package) 
    return True