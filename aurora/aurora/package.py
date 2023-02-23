class Package:
    """The metadata for an Aurora package"""

    def __init__(self, name, source = None, data = None):
        if data is None:
            self.data = dict()
        else:
            self.data = data

        self.source = source

        pkg_name_info = Package.interpret_package_string(name)
        self.name = pkg_name_info['name']
        self.data['version'] = pkg_name_info['version']

        self.build_dependencies = self.data['build_dependencies'] if 'build_dependencies' in self.data else list()
        self.dependencies = self.data['dependencies'] if 'dependencies' in self.data else list()

    def get_option(self, option, default = None):
        """ Get a package option """
        if option in self.data:
            return self.data[option]
        return default

    @staticmethod
    def interpret_package_string(package_string):
        """Interprets a package string and returns a filter"""

        # Check for comparitor.
        comparitor = None
        if package_string[0] == '=':
            comparitor = '='
            package_string = package_string[1:]
        elif package_string[0] == '>':
            comparitor = '>'
            package_string = package_string[1:]
            if package_string[0] == '=':
                comparitor = '>='
                package_string = package_string[1:]
        elif package_string[0] == '<':
            comparitor = '<'
            package_string = package_string[1:]
            if package_string[0] == '=':
                comparitor = '<='
                package_string = package_string[1:]

        # Check for version.
        name = package_string
        version = None
        if '-' in package_string:
            name = package_string[:package_string.rfind('-')]
            version = package_string[package_string.rfind('-')+1:]

        split_name = name.split('/')

        data_filter = dict()
        data_filter['name'] = name.strip()
        data_filter['version'] = version
        data_filter['comparitor'] = comparitor
        data_filter['category'] = split_name[0]
        data_filter['name_no_category'] = split_name[-1]
        return data_filter

    def __str__(self):
        return self.name +'-' + self.data['version']

    def __repr__(self):
        return self.name +'-' + self.data['version']
