from aurora import matcher
from distutils.version import LooseVersion

from aurora.package import Package

class PackageList:
    """ A list of packages"""

    def __init__(self):
        self.packages = dict() # name -> list of packages

    def add_package(self, pkg:Package):
        """ Adds a package to the list """
        if pkg.name not in self.packages:
            self.packages[pkg.name] = list()
        self.packages[pkg.name].append(pkg)

    def add_packages(self, packages:list):
        """ Adds a list of packages to the list """
        for pkg in packages:
            self.add_package(pkg)

    def search(self, search:dict, list_matching_policy = 'contains', version_policy = 'latest', version_comparitor = None):
        """Searches the package list for packages that match the search criteria"""
        results = list()
        # Commonly, we search for a package by name. If not, search all properties of the package.
        if 'name' in search:
            if search['name'] in self.packages:
                for pkg in self.packages[search['name']]:
                    res = self.__search_package_name(pkg, search, list_matching_policy, version_comparitor)
                    if res is not None:
                        results.append(pkg)
        else:
            # We don't have a name, so we have to go through all packages in this list.
            for pkg_list in self.packages.items():
                for pkg in pkg_list[1]:
                    res = self.__search_package_name(pkg, search, list_matching_policy, version_comparitor)
                    if res is not None:
                        results.append(pkg)

        # If we have results
        if len(results) > 0 and version_policy is not None:
            # Split results by name
            results_by_name = dict()

            # Sort each list by version
            for pkg in results:
                if pkg.name not in results_by_name:
                    results_by_name[pkg.name] = list()
                results_by_name[pkg.name].append(pkg)
            for pkg_list in results_by_name.items():
                pkg_list[1].sort(key=lambda x: LooseVersion(x.data['version']), reverse=True)

            if version_policy == 'latest':
                results = list()
                for pkg_list in results_by_name.items():
                    results.append(pkg_list[1][0])

        return results

    def search_exact_from_string(self, package_str: str):
        """ Finds a package that matches the string exactly """
        package_info = Package.interpret_package_string(package_str)

        version_policy = 'latest'
        search_params = {
            'name': package_info['name'],
        }

        if 'version' in package_info and package_info['version'] is not None:
            search_params['version'] = package_info['version']
            version_policy = 'exact'
        
        package = self.search(
            search_params,
            list_matching_policy='contains',
            version_policy=version_policy,
        )

        if len(package) == 0:
            return None
        return package[0]

    def __search_package_name(self, pkg, search, list_matching_policy, version_comparitor):
        res = matcher.match(pkg.data, search, list_matching_policy, version_comparitor)
        if res is None:
            return pkg
        else:
            #print(f'[D]: {pkg.name} failed search: {res}')
            return None