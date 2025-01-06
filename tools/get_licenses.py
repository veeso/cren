#!/usr/bin/env python3

from sys import argv
import json

licenses_json = argv[1]

licenses = json.load(open(licenses_json))

for license in licenses["licenses"]:
    identifier = license["licenseId"]
    c_enum = f"LICENSE_{(
        identifier.replace("-", "_").replace(".", "_").replace("+", "_PLUS").upper()
    )}"

    print(f"LICENSE_{c_enum}, // {identifier}")
