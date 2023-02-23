from pkg_resources import parse_version


def data_compare_version(ver_x, ver_y):
    """ Compares two versions. """
    x_ver = parse_version(ver_x)
    y_ver = parse_version(ver_y)

    if x_ver > y_ver:
        return -1
    elif x_ver < y_ver:
        return 1
    return 0


def data_type(key):
    """ Returns the data type of the key. """
    if key == 'version':
        return 'version'
    elif key == 'installed' or key == 'installed_architecture' or key == 'is_tool':
        return 'boolean'
    elif key == 'install_root':
        return 'path'
    elif key == 'keywords':
        return 'list'
    else:
        return 'string'


def data_compare(matched_type, a, b, list_matching_policy = 'contains', version_comparitor = None):
    """ Compares two data types. """
    if matched_type == 'version':
        ret =  data_compare_version(b, a)
        if version_comparitor == '=':
            ret = 0 if ret == 0 else 1
        elif version_comparitor == '>':
            ret = 0 if ret == 1 else 1
        elif version_comparitor == '>=':
            ret = 0 if ret == 1 or ret == 0 else 1
        elif version_comparitor == '<':
            ret = 0 if ret == -1 else 1
        elif version_comparitor == '<=':
            ret = 0 if ret == -1 or ret == 0 else 1
        return ret
    elif matched_type == 'list':
        if list_matching_policy == 'any':
            return 0 if set(a).intersection(set(b)) else 1
        elif list_matching_policy == 'contains':
            return 0 if set(b).issubset(set(a)) else 1
        elif list_matching_policy == 'exact':
            return 0 if set(a) == set(b) else 1
        elif list_matching_policy == 'all':
            return 0 if set(b).issuperset(set(a)) else 1
    else:
        return 0 if a == b else 1


def match(bucket: dict, needle: dict, list_matching_policy = 'contains', version_comparitor = None):
    """Match returns a dictionary of keys that did not match. If None is returned, then the bucket matches the needle."""
    matching_dict = dict()
    for key in needle:
        if key == 'name':
            continue
        if key not in bucket:
            matching_dict[key] = 1
            continue
        comp = data_compare(data_type(key), bucket[key], needle[key], list_matching_policy, version_comparitor)
        if comp != 0:
            matching_dict[key] = comp
    return None if len(matching_dict) == 0 else matching_dict


def explain_match_failure(match_list: dict):
    """ Explain the match failure."""
    if match_list is None:
        return None
    for key in match_list:
        key_type = data_type(key)
        if key_type == 'version':
            difference = 'newer' if match_list[key] == 1 else 'older'
            print(
                f'The version we searched for was {difference} than the current version.')
        else:
            print(f'{key} was different')
