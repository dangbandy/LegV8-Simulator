currentfile = ""
filetext = "No File Selected"
regarray = [0]*32
print(len(regarray))
from os import path
from tkinter import filedialog
from tkinter import *
from tkinter import Text
window = Tk()
choosefile = False
window.title("CS351 Final Project")
lb11 = Label(window, text="",bg="#8cade2",relief=GROOVE)
lb11.grid(column=0, row=0,rowspan=1,columnspan=1,sticky=W)

window.geometry('1000x1000')
window.configure(bg="#8cade2")

from os import path
def choosefile():
    global currentfile
    global filetext
    file = filedialog.askopenfilename(initialdir=path.dirname(__file__))
    currentfile = file
    filetext = "Current file: " + currentfile
    global lb11
    lb11.configure(text=filetext)



lbl = Label(window, text="Memory Initialization (Format: i1 v1 i2 v2...)",bg="#8cade2",relief=GROOVE)
lbl.grid(column=0, row=2,columnspan=1,rowspan=1,sticky=W)
txt1 = Entry(window, width=100)
txt1.grid(column=0, row=1,columnspan=50,rowspan=1,sticky=W)

def updateregs(regarray):
    for i in range(0,len(regarray)):
        curreg = "r"+str(i)+":"
        text = curreg + " "+str(regarray[i])
        new = Label(window,text=text,bg="#5981c1",anchor=W,width=20,bd=5,relief=RIDGE)
        new.grid(column=50,row=2 +i,sticky=E,padx=50)

def initializemem():
    output = txt1.get()
    output = output.split(" ")
    output = list(map(int,output))

    # First value is an index, last is a value
    print(output)
    mem = [0] * 1000
    for i in range(0,len(output),2):
        index = output[i]
        mem[index] = output[i+1]

    print(mem[:50])


def textentry():
    w = Text(height = 40, width=50,relief=SUNKEN)
    w.grid(column=0,row=12,columnspan=50,rowspan=40,padx=10, pady=10,sticky=E+W+S+N)
    def writetofile():
        file = w.get("1.0",END)
        f = open('TEMP.txt',"w+")
        f.write(file)
        print("FILE SAVED AS 'TEMP.txt' IN PROGRAM DIRECTORY")

    sbmt = Button(window, text="Save as File", command=writetofile)
    sbmt.grid(column=10, row=80,sticky=W+N)


lb11 = Label(window, text=filetext,bg="#8cade2",relief=GROOVE)
lb11.grid(column=0, row=0,sticky=E+W+S+N)

btn1 = Button(window, text="Initialize Memory", command=initializemem)
btn1.grid(column=0, row=3,sticky=W)


btn2 = Button(window, text="Set Current File",command=choosefile)
btn2.grid(column=40,row=7,sticky=W)

btn4 = Button(window, text="Run Current File",command=choosefile)
btn4.grid(column=40,row=8,sticky=W)

btn5 = Button(window, text="Step Run Current File",command=choosefile)
btn5.grid(column=40,row=9,sticky=W)


btn3 = Button(window, text="Open File Editor",command=textentry)
btn3.grid(column=40,row=10,sticky=W)


#zz =Button(window, text="Open File Editor",command=textentry)
#zz.grid(column=2,row=9)



updateregs(regarray)



window.mainloop()