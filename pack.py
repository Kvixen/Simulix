import shutil
from os import path, rename


def main(dst, src, name):
    if path.isfile(name + ".zip"):
        name = name + "_fmu"
    shutil.make_archive(name, 'zip', src)
    rename(path.join(dst, name + '.zip'), name + ".fmu")