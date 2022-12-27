# Nyx

Nyx is build system for the Umbra operating system. It is written in Python and is designed to coordinate the build process for the Umbra operating system.

Nyx handles all packages in the Umbra operating system. It is responsible for downloading, building, and installing packages. The closest analog would be the [BSD Ports system](https://www.freebsd.org/ports/) or [Arch Linux's AUR](https://aur.archlinux.org/).

Nyx builds packages in a sandboxed environment. This allows package builds to be reproducable, and allows for the build process to be automated. Nyx can use docker to create the sandboxed environment.

Nyx can build the Umbra operating system from scratch into a directory by specifying a system root.

It will also run natively in Umbra, as the package manager. This is the last step of self hosting.

## .nyxrc
Nyx reads in a configuration file called `.nyxrc` that describes the build environment. It will read this file from multiple locations, in the following order:

1. The directory that the Nyx executable is located in
2. The user's home directory
3. The current working directory

## Concepts

### Repositories
Nyx can have multiple repositories that it will use, local or remote.

Packages are stored in a repository, and Nyx will use the repositories to download packages.

### Packages
Packages are programs, libraries, or other software that can be installed on the Umbra operating system.

Packages can either be built from source, or installed from a binary package.

A package contains metadata and instructions for how to build the package. The source is bundled seperately from the metadata.