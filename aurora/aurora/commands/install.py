from aurora.operations.install import operation_install

def command_install(universe, aurora_config, packages, rebuild=False):
    to_install = []
    for package in packages:
        found_pkgs = universe.search({'name': package})

        if len(found_pkgs) == 0:
            print(f"Package {package} not found.")

        to_install += found_pkgs

    packages_to_rebuild = []
    if rebuild:
        packages_to_rebuild = to_install

    
    # Generate Dependency Graph
    target_package: list = to_install
    dependency_graph = universe.generate_dependency_graph(target_package)
    dependency_graph = universe.resolve_dependency_graph(dependency_graph)
    changeset = universe.changeset_from_dependency_graph(dependency_graph, packages_to_rebuild)

    # Render changes to a string and print per change
    # Changeset is in format of {package, action}
    print("Changeset:")
    for change in changeset:
        change_text = "["
        if change['operation'] == 'install':
            change_text += "N"
        elif change['operation'] == 'remove':
            change_text += "R"
        elif change['operation'] == 'upgrade':
            change_text += "U"
        else:
            change_text += "?"
        change_text += "] " + change['package'].name + "-" + change['package'].data['version']
        print(f"* {change_text}")

    if aurora_config.get('opt_readonly') is True:
        print("Changes not allowed (read only). Exiting.")
        return

    if len(changeset) > 0:
        should_ask_for_confirmation = aurora_config.get('opt_prompt_changes')
        if should_ask_for_confirmation:
            print("Enter 'y' to continue, or 'n' to abort: ", end='')
            user_input = input()
            if user_input != 'y':
                print("Aborting.")
                return
    else:
        print("No changes to apply.")
        return

    print("Applying changes to system image... ")
    for change in changeset:
        result = False
        if change['operation'] == 'install':
            result = operation_install(universe, aurora_config, change['package'])
        elif change['operation'] == 'rebuild':
            ov_opt_ignore_cache = aurora_config.override('opt_ignore_cache', True)
            ov_opt_build = aurora_config.override('opt_build', True)
            
            result = operation_install(universe, aurora_config, change['package'])
            
            # Reset config
            aurora_config.reset('opt_ignore_cache', ov_opt_ignore_cache)
            aurora_config.reset('opt_build', ov_opt_build)
        if not result:
            print(f"Failed to {change['operation']} {change['package'].name}-{change['package'].data['version']}")
            failures += 1
            break
        else:
            universe.save_world_set()