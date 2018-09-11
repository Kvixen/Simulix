import shutil, uuid
from os import path, rename


def main(dst, src, name):
    if path.isfile(name + ".zip") or path.isfile(name + ".fmu"):
        name += str(uuid.uuid4().hex)
    try:
        shutil.make_archive(name, 'zip', src)
        rename(path.join(dst, name + '.zip'), name + ".fmu")
    except FileExistsError:
        exit("Archive %s already exists." % (name))
        