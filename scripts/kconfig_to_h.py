import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "config", type=argparse.FileType("r"), help="input .config file"
    )
    parser.add_argument("out", type=argparse.FileType("w"), help="output .h file")
    args = parser.parse_args()
    with args.config as f:
        config = f.read().splitlines()
    output = [
        "#pragma once",
    ]
    for item in config:
        if not item.startswith("CONFIG_") or "=" not in item:
            continue
        (key, _, value) = item.partition("=")
        if value == "n" or not value:
            continue
        elif value == "y":
            value = "1"
        output.append(f"#define {key} {value}")
    with args.out as f:
        f.write("\n".join(output))
