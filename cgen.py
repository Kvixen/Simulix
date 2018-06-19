from pathlib import Path
from string import Template

templateReplace = {
}


def createFile(templatePath=r"C:/Users/Kvixen/Documents/Work/Scania/Work/Coding/Python/Fmugen/ctemplate.c", filePath="main.c"):
    '''

    '''
    templateReplace['classname'] = 'bil'
    try:
        if Path(templatePath).is_file():
            with open(templatePath, "r") as tempf:
                with open(filePath, "w") as f:
                  for line in tempf:
                     try:
                         f.write(line.format(**templateReplace))
                     except ValueError:
                         f.write(line)
        else:
            raise IOError('No Template File!')
    except IOError as e:
        print(e)

    



def main():
    createFile()

if __name__ == "__main__":
    main()



