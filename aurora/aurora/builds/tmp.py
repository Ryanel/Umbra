""" Handles the creation of temporary files and directories. """

import os

def create_dir_always(path: str) -> bool:
    """ Create a directory, always """
    if not os.path.exists(path):
        os.makedirs(path)
    return True

def create_tmp_dir(config: dict, package_string: str) -> bool:
    """ Create a temporary directory """
    create_dir_always(config['temp_path'])

    # Create the temporary directory for the package
    tmp_dir = os.path.join(config['temp_path'], package_string)
    create_dir_always(tmp_dir)

    # Create the temporary directory for the package's needed directories
    create_dir_always(os.path.join(tmp_dir, 'install'))
    create_dir_always(os.path.join(tmp_dir, 'src'))
    create_dir_always(os.path.join(tmp_dir, 'tmp'))
