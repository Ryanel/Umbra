import json
def read_json(path: str, default: any = {}) -> any:
    """Reads a JSON file in as a native python object"""
    try:
        with open(path, 'r', encoding='utf-8') as jdata:
            return json.load(jdata)
    except:
        return default

def write_json(path: str, data: any = {}) -> any:
    """Writes a JSON file in as a native python object"""
    try:
        with open(path, 'w', encoding='utf-8') as jdata:
            json.dump(data, jdata, ensure_ascii=False, indent=4)
    except:
        print(f"Unable to save {path}!")