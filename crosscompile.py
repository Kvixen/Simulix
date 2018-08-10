import platform
from os import path, getcwd
from build import prepare_build_directory, find_make_prog
from shutil import which
import subprocess
# platform.linux_distribution() and .dist() will be removed in 3.8, this will work for now
if "ubuntu" in platform.platform().lower():
    try:
        import apt
        cache = apt.Cache()
        cache.open()
        SYSTEM = "Ubuntu"
    except ImportError:
        exit("Can't find python package apt in your ubuntu installation")


def execute_command(command):
    subprocess_result = subprocess.Popen(command)
    subprocess_result.communicate()
    return

def compile_windows(src, dst, make_prog):
    command = "cmake .. -DCMAKE_TOOLCHAIN_FILE=\"{0}/compile-tools/toolchains/Ubuntu-mingw32.cmake\"".format(src)
    execute_command(command)
    execute_command(make_prog)
    command = "cmake -DCMAKE_TOOLCHAIN_FILE=\"{0}/compile-tools/toolchains/Ubuntu-mingw64.cmake\" ..".format(src)
    execute_command(command)
    execute_command(make_prog)
    return

def compile_linux(src, dst, make_prog):
    command = "cmake .. -DCMAKE_TOOLCHAIN_FILE={0}/compile-tools/toolchains/Ubuntu-gnueabihf.cmake".format(src)
    execute_command(command)
    execute_command(make_prog)
    command = "cmake .."
    execute_command(command)
    execute_command(make_prog)
    return


def cross_compile(src, dst, make_prog):
    compile_linux(src, dst, make_prog)
    try:
        cache['mingw-w64'].is_installed
        compile_windows(src, dst, make_prog)
    except KeyError:
        print("Missing package mingw-w64, can't crosscompile to windows")

def main(src, dst, folder_name):
    if not path.isabs(dst):
        dst = path.join(getcwd(), dst)
    if not path.isdir(dst):
        exit("Path specified doesn't exist")

    make_prog = find_make_prog()[1]   
    if not make_prog or not which(make_prog):
        exit("Couldn't find Make program")
    print("Building")

    prepare_build_directory(dst, folder_name)
    cross_compile(src, dst, make_prog)
    
    
