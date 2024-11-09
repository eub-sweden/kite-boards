"""
Utility to autogenerate Zephyr DT pinctrl and GPIO nexus mapping include files.

Usage::

    python3 gen-connectors-dtsi.py -d /path/to/boards

Copyright (c) 2023 Elektronikutvecklingsbyrån EUB AB

SPDX-License-Identifier: Apache-2.0
"""

import argparse
import logging
from pathlib import Path
import re
from jinja2 import Environment, FileSystemLoader
import yaml

logger = logging.getLogger(__name__)

SCRIPT_DIR = Path(__file__).absolute().parent
"""Script directory."""

REPO_ROOT = SCRIPT_DIR / ".."
"""Repository root (used for defaults)."""

GPIO_TEMPLATE = "connectors-gpio-template.j2"
"""connectors-gpio.dtsi template file."""

PINCTRL_TEMPLATE = "connectors-pinctrl-template.j2"
"""connectors-pinctrl.dtsi template file."""

def parse_pin(pindef):
    m = re.match(r"P(\d)\.(\d{1,2})", pindef)
    return { "port": m[1], "pin": int(m[2]) }

def main(data_path):
    env = Environment(
        trim_blocks=True, lstrip_blocks=True, loader=FileSystemLoader(SCRIPT_DIR)
    )
    gpio_template = env.get_template(GPIO_TEMPLATE)
    pinctrl_template = env.get_template(PINCTRL_TEMPLATE)

    for path in Path(data_path).rglob("connectors.yml"):
        board_path = path.parent

        infile = open(path, "r", encoding="utf-8")
        data = yaml.safe_load(infile)

        for conn in data["connectors"]:
            pins = {}
            for pinname, pindef in conn["pins"].items():
                pins[pinname.lower()] = parse_pin(pindef)
            conn["pins"] = pins

        gpio_file = board_path / (data["name"].lower() + "-connectors-gpio.dtsi")
        with open(gpio_file, "w") as out:
            out.write(
                gpio_template.render(name=data["name"], connectors=data["connectors"])
            )

        pinctrl_file = board_path / (data["name"].lower() + "-connectors-pinctrl.dtsi")
        with open(pinctrl_file, "w") as out:
            out.write(
                pinctrl_template.render(name=data["name"], connectors=data["connectors"])
            )

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-d",
        "--data-path",
        type=Path,
        default=REPO_ROOT / "boards",
        help="Board directory",
    )
    args = parser.parse_args()

    main(args.data_path)
