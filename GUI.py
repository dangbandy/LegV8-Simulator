from os import path
from tkinter import filedialog
from tkinter import *
from tkinter import Text
from tkinter import messagebox
import shlex,subprocess
step = 0
currentfile = ""
filetext = "No File Selected"
regarray = [0]*32
mem = [0] * 1000
meminput = []
cout = []
commands = []
cmdtxt = "Last Instruction: "
actualfile = "N/A"

window = Tk()
window.title("CS351 Final Project")
lb11 = Label(window, text="",bg="#8cade2",relief=GROOVE)
lb11.grid(column=0, row=0,rowspan=1,columnspan=1,sticky=W)
window.lift()

window.geometry('1400x1200')
window.configure(bg="#8cade2")

from os import path

def updateinfo(filename):
    global regarray
    global mem
    global meminput
    final = False
    valid = True
    if filename=="out.txt":
        final = True
    f = open(filename,'w+')
    if final:
        f.write("File: ")
    f.write(actualfile)
    f.write("\n")

    if final:
        f.write("Register Values: ")
    outreg = str(regarray).strip("[")
    outreg = outreg.strip("]")
    if not final:
        outreg = outreg.replace(',','')
    if final:
        f.write(outreg)
        f.write("\n")

    if final:
        f.write("Memory Values: ")
    if len(meminput) == 0:
        valid = False
    outmem = str(mem).strip("[")
    outmem= outmem.strip("]")
    if not final:
        outmem = outmem.replace(',', '')
        if valid == True:
            f.write("Y\n")
            f.write(str(meminput))
        else:
            f.write("N")

    if final:
        f.write(str(outmem))

def readinfo(filename):
    if filename == "":
        messagebox.showerror("Invalid File!", "Invalid file, or no file selected. Please select a valid file.")
        return
    global regarray
    global cout
    global commands
    cout = []
    commands = []

    f = open(filename,"r+")
    lines = f.readlines()
    for i in range(0,len(lines),2):
        c = str(lines[i])
        n = str(lines[i+1]).strip("[")
        n = n.strip("]")
        n = n.replace(',', '')
        n = n.strip("\n")
        n = n.split(" ")
        cout.append(n)
        del n[-1]
        commands.append(c)






def choosefile(): # Chooses the file, but also runs the C++ Code
    global currentfile
    global filetext
    global actualfile
    file = filedialog.askopenfilename(initialdir=path.dirname(__file__))
    currentfile = file
    filetext = "Current file: " + currentfile
    actualfile = currentfile

    global lb11
    lb11.configure(text=filetext)
    updateinfo("info.txt")

    runC()
    currentfile = "output.txt"


def runC():
    print("Running",actualfile)
    command_line = "./a.out"
    args = shlex.split(command_line)
    p = subprocess.Popen(args,shell=True)

    print("")



lbl = Label(window, text="Memory Initialization (Format: i1 v1 i2 v2...)",bg="#8cade2",relief=GROOVE)
lbl.grid(column=0, row=2,columnspan=1,rowspan=1,sticky=W)
txt1 = Entry(window, width=100)
txt1.grid(column=0, row=1,columnspan=50,rowspan=1,sticky=W)
cmd = Label(window, text=cmdtxt, bg="#0e4db2", anchor=W, width=40,height=2, bd=5, relief=SUNKEN)
cmd.grid(column=50, row=1, sticky=E, padx=50,columnspan=30)
def updateregs(regs):
    global regarray
    regarray = regs


    for i in range(len(regs)):
        curreg = "r"+str(i)+":"
        text = curreg + " "+str(regs[i])
        new = Label(window,text=text,bg="#5981c1",anchor=W,width=20,bd=5,relief=RIDGE)
        new.grid(column=50,row=2 +i,sticky=E,padx=50)
    updateinfo("info.txt")

def updatecommand(command):
    global commands
    global cmdtxt
    if command == -1:
        cmdtxt = "Last Instruction: "
    else:
        command = commands[command]
        cmdtxt = "Last Instruction: " + command
    cmd.configure(text=cmdtxt)

def initializemem():
    global mem
    global meminput
    output = txt1.get()
    output = output.split(" ")

    if output[0] != '':
        try:
            output = list(map(int,output))
        except ValueError:
            print("Invalid character in memory input. Format is i1 v1 i2 v2... \n")
            return

    else:
        return
    if (len(output) % 2) != 0:
        output.append(0)
    meminput = str(output).strip("[")
    meminput = meminput.strip("]")
    meminput = meminput.replace(',', '')
    # First value is an index, last is a value
    for i in range(0,len(output),2):
        index = output[i]
        mem[index] = output[i+1]
    updateinfo("info.txt")


def textentry():
    w = Text(height = 40, width=50,relief=SUNKEN)
    w.grid(column=0,row=12,columnspan=50,rowspan=40,padx=10, pady=10,sticky=E+W+S+N)
    def writetofile():
        file = w.get("1.0",END)
        f = open('TEMP.txt',"w+")
        f.write(file)
        sav = Label(window, text="FILE SAVED AS 'TEMP.txt' IN PROGRAM DIRECTORY",bg="#8cade2")
        sav.grid(column=11,row=80,sticky=W,columnspan=50)
        print("FILE SAVED AS 'TEMP.txt' IN PROGRAM DIRECTORY")

    sbmt = Button(window, text="Save as File", command=writetofile,highlightbackground="#8cade2")
    sbmt.grid(column=10, row=80,sticky=W+N)

def quit():
    updateinfo("out.txt")
    window.destroy()
updateregs(regarray)

def resetreg():
    global regarray
    regarray = [0] * 32
    updateregs(regarray)
    btn5.configure(text="Step Run File", command=steprun)
    updatecommand(-1)

def run():
    global currentfile
    readinfo(currentfile)
    updateregs(cout[-1])
    updatecommand(len(commands)-1)
    btn5.configure(text="Reset Values", command=resetreg)
    step = 0



def steprun():
    global currentfile
    readinfo(currentfile)
    global step
    if len(cout) == 0:
        return
    if step == len(cout):
        messagebox.showinfo("Step Run Completed","Step Run has completed, click again to reset values.\n "
                                                 "...Or Write Data and Quit to Save them.")
        btn5.configure(text="Reset Values",command=resetreg)
        step = 0


    else:
        updatecommand(step)
        updateregs(cout[step])
        step+=1
        btn5.configure(text="Next Step")



lb11 = Label(window, text=filetext,bg="#8cade2",relief=GROOVE)
lb11.grid(column=0, row=0,sticky=E+W+S+N)

btn1 = Button(window, text="Initialize Memory", command=initializemem,highlightbackground="#8cade2")
btn1.grid(column=0, row=3,sticky=W)


btn2 = Button(window, text="Set Current File",command=choosefile, highlightbackground="#8cade2")
btn2.grid(column=40,row=7,sticky=W)

btn4 = Button(window, text="Run Current File",command=run, highlightbackground="#8cade2")
btn4.grid(column=40,row=8,sticky=W)

btn5 = Button(window, text="Step Run Current File", command=steprun, highlightbackground="#8cade2")
btn5.grid(column=40,row=9,sticky=W)

btn6 = Button(window,text="Write Data and Quit", command=quit, highlightbackground="#8cade2")
btn6.grid(column=40,row=10,sticky=W)

btn3 = Button(window, text="Open File Editor", command=textentry, highlightbackground="#8cade2")
btn3.grid(column=40,row=11,sticky=W)


#zz =Button(window, text="Open File Editor",command=textentry)
#zz.grid(column=2,row=9)



updateregs(regarray)
updateinfo("info.txt")

window.lift()
window.mainloop()
