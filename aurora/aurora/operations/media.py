import os
import docker

def operation_media_docker(universe, config, command) -> bool:
    """ Builds a iso within a docker container"""

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
            f"[build] => Building media using docker")

        container = client.containers.run(image_name,
                                          f"bash -c 'source /home/buildenv/buildenv-rc; {command}'",
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


def operation_media(universe, config) -> bool:
    print("Building media...")
    sysroot_dir = config.get('system_root') 
    media_dir = config.get('media_dir')
    toolchain_dir = config.get('toolchain_path')

    # Copy limine.sys limine-cd.bin limine-eltorito-efi.bin
    # from toolchain_dir/limine to sysroot_dir
    limine_dir = os.path.join(toolchain_dir, 'limine')
    limine_files = ['limine.sys', 'limine-cd.bin', 'limine-eltorito-efi.bin']
    for limine_file in limine_files:
        limine_file_path = os.path.join(limine_dir, limine_file)
        if os.path.exists(limine_file_path):
            os.system(f'cp {limine_file_path} {sysroot_dir}')
        else:
            print(f"Limine file {limine_file} not found.")
            return

    # Copy all files in media_dir/x86/x86_64 to sysroot_dir recursively
    media_arch_dir = os.path.join(media_dir, 'x86', 'x86_64')
    os.system(f'cp -r {media_arch_dir}/* {sysroot_dir}')

    if not os.path.exists(os.path.join(toolchain_dir, 'output')):
        os.mkdir(os.path.join(toolchain_dir, 'output'))
    os.system(f'xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-eltorito-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label {sysroot_dir} -o {toolchain_dir}/output/livecd.iso 2> /dev/null')

    # Run limine-install build/livecd.iso
    # Chmod file
    operation_media_docker(universe, config, f"/opt/aurora/tools/limine/limine-install /opt/aurora/tools/output/livecd.iso 2> /dev/null")