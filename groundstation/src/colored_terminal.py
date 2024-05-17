# Small libary to add some colors to the consol output as well as a last line clear function
import sys

def print_with_color(color_code, *args, indent=""):
    if indent:
        indent = " " * indent
    print(f"\033[{color_code}m{indent}", end="")
    print(*args, end="")
    print("\033[0m", end="")
    sys.stdout.flush()


def print_yellow(*args, indent=""):
    print_with_color("33", *args, indent=indent)


def print_black(*args, indent=""):
    print_with_color("30", *args, indent=indent)


def print_red(*args, indent=""):
    print_with_color("31", *args, indent=indent)


def print_green(*args, indent=""):
    print_with_color("32", *args, indent=indent)


def print_blue(*args, indent=""):
    print_with_color("34", *args, indent=indent)


def print_magenta(*args, indent=""):
    print_with_color("35", *args, indent=indent)


def print_cyan(*args, indent=""):
    print_with_color("36", *args, indent=indent)


def print_white(*args, indent=""):
    print_with_color("37", *args, indent=indent)


def clear_console_line():
    """clears the last line in the console"""
    print("\033[2K\033[G", end="", flush=True)


if __name__ == "__main__":
    pass
