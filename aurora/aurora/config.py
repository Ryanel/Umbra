""" Determine the configuration for Aurora"""

import os
import yaml
from aurora.repository import Repository
from aurora.universe import Universe

class Config: 
    """ Load the configuration from the config file """
    def __init__(
        self, 
        config_path = '/etc/aurora/config.yml',
        live_sources = '',
        gmake_path = ''
        ):
        self.config = {}
        self.repos = {}

        # Does config path exist? If not, use the default config.
        if os.path.exists(config_path):
            with open(config_path, 'r', encoding='utf-8') as file:
                data = yaml.safe_load(file)
                self.config = data['config']
                self.repos = data['repos']

        else:
            self.config['system_root'] = '/'
            self.config['cross_compile'] = False
            self.config['temp_path'] = '/tmp/aurora'

        self.config_path = os.path.abspath(os.path.dirname(config_path))

        # Force system_root and temp_path to be absolute paths
        self.config['system_root'] = os.path.abspath(self.config['system_root'])
        self.config['temp_path'] = os.path.abspath(self.config['temp_path'])
        self.config['toolchain_path'] = os.path.abspath(self.config['toolchain_path'])
        self.config['package_dir'] = os.path.abspath(self.config['package_dir'])
        self.config['media_dir'] = os.path.abspath(self.config['media_dir'])
        self.config['source_dir'] = os.path.abspath(self.config['source_dir'])
        self.config['world_set'] = os.path.abspath(self.config['world_set'])
        
        # Values that should be configured by the user (but cannot yet)
        self.config['opt_ignore_cache'] = True      # Should the cache be ignored?
        self.config['opt_build'] = True            # Allow building packages?
        self.config['opt_readonly'] = False         # Should the system be mounted read-only?
        self.config['opt_prompt_changes'] = False    # Should the user be prompted for changes?
        self.config['aur_build_driver'] = 'docker'  # Build driver to use. (docker, local)

        # Handle live source configuration
        self.config['live_sources'] = self.config['source_dir']

        if gmake_path != '':
            self.config['gmake_path'] = gmake_path
        else:
            # Check if GMAKE environment variable is set
            if 'GMAKE' in os.environ:
                self.config['gmake_path'] = os.environ['GMAKE']
            else:
                self.config['gmake_path'] = 'make'

    def get(self, key):
        """ Get a configuration value """
        return self.config[key]

    def override(self, key, value):
        """ Override a configuration value """
        original_value = self.config[key]
        self.config[key] = value
        return original_value

    def reset(self, key, value):
        """ Reset a configuration value """
        self.config[key] = value

    def load_universe(self) -> Universe:
        """ Load the universe """
        repo_list = []
        for repo in self.repos:
            repo_list.append(Repository(self.repos[repo]['path']))
        return Universe(repo_list, self)
