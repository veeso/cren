#!/usr/bin/env python3

from sys import argv
import json

licenses_json = argv[1]

licenses = json.load(open(licenses_json))

for license in licenses["licenses"]:
    identifier = license["licenseId"]
    const_name = f"{(
        identifier.replace("-", "_").replace(".", "_").replace("+", "_PLUS").upper()
    )}"

    print(f'const char* {const_name} = "{identifier}";')
