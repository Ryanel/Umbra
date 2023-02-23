import os
from aurora.packagelist import PackageList
from aurora.package import Package
from aurora import matcher
from graphlib import TopologicalSorter
from typing import List

import yaml


class Universe(PackageList):
    """The Universe holds onto all packages in all repositories"""

    def __init__(self, repositories: list, config):
        PackageList.__init__(self)
        self.repositories = repositories
        self.load_packages()
        self.world_set = PackageList()
        self.config = config

    def load_packages(self):
        """Loads all packages from all repositories"""
        for repo in self.repositories:
            self.add_packages(repo.packages)

    def load_world_set(self):
        """Loads the world set from disk"""
        ws_path = self.config.get('world_set')

        if os.path.exists(ws_path):
            with open(ws_path, 'r', encoding='utf-8') as file:
                for line in file:
                    # Each line is a package encoded as a string
                    found_package = self.search_exact_from_string(line)
                    if found_package is not None:
                        self.world_set.add_package(found_package)
                    else:
                        print(f"Warning: Package {line} not found in universe")
                    
        pass

    def save_world_set(self):
        """ Saves the world set to disk """
        ws_path = self.config.get('world_set')

        # Ensure the path exists...
        os.makedirs(os.path.dirname(ws_path), exist_ok=True)

        with open(ws_path, 'w', encoding='utf-8') as file:
            # Each line is a package encoded as a string
            for package in self.world_set.packages:
                for spec_package in self.world_set.packages[package]:
                    name = package
                    version = spec_package.data['version']
                    file.write(f"{name}-{version}\n")

        pass

    def search_package_dependencies(self, package: Package) -> List[Package]:
        """Searches for the dependencies of a package"""
        dependencies = list()
        for dep in package.dependencies:
            # Interpret the dependency string
            dependency_filter = Package.interpret_package_string(dep)

            # Extract comparitor if it exists
            comparitor = None

            # TODO: Clean this up or redesign this part. It's a bit messy.
            if 'comparitor' in dependency_filter:
                comparitor = dependency_filter['comparitor']
                del dependency_filter['comparitor']

            if 'name_no_category' in dependency_filter:
                del dependency_filter['name_no_category']

            if 'category' in dependency_filter:
                del dependency_filter['category']

            # Resolve the dependency from the universe
            resolved_dependencies = self.search(
                dependency_filter, 'contains', 'latest', comparitor)

            if len(resolved_dependencies) == 1:
                #print( f"{resolved_dependencies[0]} satisfies dependency {dep} ")
                dependencies.append(resolved_dependencies[0])
            elif len(resolved_dependencies) > 1:
                print(
                    f"Multiple packages satisfy dependency {dep}: {resolved_dependencies}")
                raise Exception(f'Could not resolve dependency {dep} (too many candidates)')
            else:
                raise Exception(f'Could not resolve dependency {dep} (no matching candidates)')
        return dependencies

    def generate_dependency_graph(self, packages: list):
        """ Generates a recursive dependency graph for a list of packages"""
        # Generates a graph in the form of a dictionary where each node is a package, and each node's value is a list of dependencies
        graph = dict()

        # Add all packages to the graph
        for package in packages:
            self.__add_to_dependency_graph(package, graph)
        
        return graph
        

    def __add_to_dependency_graph(self, package: Package, graph: dict):
        """Adds a package to the dependency graph"""
        # Add the package to the graph
        graph[package] = list()

        # Add the package's dependencies to the graph
        for dep in self.search_package_dependencies(package):
            # Check every package in the graph if it has a dependency with the same name as the current dependency
            for pkg in graph.keys():
                for pkg_dep in graph[pkg]:
                    if pkg_dep.name == dep.name:
                        if matcher.data_compare_version(pkg_dep.data['version'], dep.data['version']) != 0:
                            raise Exception(f"Conflict, {pkg} depends on {dep}, but {package} depends on {pkg_dep}")
            graph[package].append(dep)
            self.__add_to_dependency_graph(dep, graph)

    def resolve_dependency_graph(self, graph: dict):
        """Resolves a dependency graph"""
        # Create a sorter
        sorter = TopologicalSorter(graph)
        sorted_graph = sorter.static_order()
        sorted_graph = list(sorted_graph)
        return sorted_graph

    def changeset_from_dependency_graph(self, graph: dict, rebuild: list):
        """Generates a changeset from a dependency graph"""
        changeset = []
        for package in graph:
            # Does the package exist in the world set?
            if self.world_set.search({'name': package.name}, 'exact', 'exact', 'exact'):
                # Yes, check if the package is the same
                if package.data['version'] == self.world_set.search({'name': package.name}, 'exact', 'exact', 'exact')[0].data['version']:
                    
                    # We rebuilding this package?
                    if package in rebuild:
                        changeset.append({'package': package, 'operation': 'rebuild'})
                        continue
                    continue
                else:
                    # No, it's a different version. Upgrade it
                    changeset.append({'package': package, 'operation': 'upgrade'})
                    continue
            changeset.append({'package': package, 'operation': 'install'})
        return changeset