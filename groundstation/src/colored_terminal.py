# Small libary to add some colors to the consol output as well as a last line clear function
import sys

def print_yellow(*args):
    print("\033[33m", end="")
    print(*args, end="")
    print("\033[0m", end="")
    sys.stdout.flush()


def print_black(*args):
    print("\033[30m", end="")
    print(*args, end="")
    print("\033[0m", end="")
    sys.stdout.flush()


def print_red(*args):
    print("\033[31m", end="")
    print(*args, end="")
    print("\033[0m", end="")
    sys.stdout.flush()


def print_green(*args):
    print("\033[32m", end="")
    print(*args, end="")
    print("\033[0m", end="")
    sys.stdout.flush()


def print_blue(*args):
    print("\033[34m", end="")
    print(*args, end="")
    print("\033[0m", end="")
    sys.stdout.flush()


def print_magenta(*args):
    print("\033[35m", end="")
    print(*args, end="")
    print("\033[0m", end="")
    sys.stdout.flush()


def print_cyan(*args):
    print("\033[36m", end="")
    print(*args, end="")
    print("\033[0m", end="")
    sys.stdout.flush()


def print_white(*args):
    print("\033[37m", end="")
    print(*args, end="")
    print("\033[0m", end="")
    sys.stdout.flush()


def clear_console_line():
    """clears the last line in the console"""
    # print("\r" + " " * 50 + "\r", end="", flush=True)
    print("\033[2K\033[G", end="", flush=True)
    # sys.stdout.flush()

if __name__=="__main__":
    pass