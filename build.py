from subprocess import call

compprog = '"MinGW Makefiles"'
makeprog = '"mingw32-make"'

def build(compile_program,make_program):
    call("cmake -G " + compile_program + " ..", shell=True)
    call(make_program, shell=True)
    call("main", shell=True)

def hejare():
    call("python hej.py", shell=True)

#hejare()
build(compprog,makeprog)
    