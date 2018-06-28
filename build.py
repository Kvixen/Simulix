from os import mkdir, path, getcwd, chdir, system
from subprocess import call, Popen

def _build(compile_program,make_program):
    call("cmake -G " + compile_program + " ..", shell=True)
    call(make_program, shell=True)
    Popen([r"main.exe", "call"], shell=True)

def prepare_build_directory(folderName):
    if not folderName:
        folderName = "build"
    if not path.isdir(path.join(getcwd(), folderName)):
        mkdir(folderName)
    
    chdir("build")


def build(compprog, makeprog, folderName=None):
    prepare_build_directory(folderName)
    _build(compprog, makeprog)