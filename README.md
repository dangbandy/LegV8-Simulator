# About
This program purpose is to simulate the executions of a LegV8 program. The simulator will read from the text file, which contains the LegV8 code. The LegV8 text file can be pulled from an existing file or created using the text editor utilities of this program. The results of the execution, such as the stack, memory, and registers, will be displayed in the Memory, Stack, and Registers views.

# Requirements
The LegV8 code must be syntically and semantically correct for this program to work. Registers such as LR, SP, and XZR must be written as X30, X28, and X31. The simulator will execute the instruction with label MAIN as the first instruction. Therefore, please identify a MAIN label instruction in the LegV8 code.

# How to execute single-step
After uploading or creating a valid LegV8 program in the text editor view, select the wrench and hammer icon to simulate the first instruction of the LegV8 program. Then select the arrow icon to simulate the next instruction on forth. Similarly, the same steps will be applied when using the Single-Step and Next-Step options in the toolbar's Build selections.

# How to execute to completion
After uploading or creating a valid LegV8 program in the text editor view, select the green triangle icon to simulate the LegV8 program tocompletion. Similarly, the same step will be applied when using the All-Step option in the toolbar's Build selections.

# How to set memory value
Select the memory location you wish to modify. Enter the desired decimal value into the input dialog and press OK.

# How to reset memory
Select the reset icon to reset all memory values back to zero.The same applies for selecting Reset-Mem option in the toolbar's Build selections.

# List of valid commands
R format: ADD, AND, ORR, EOR, SUB, LSR, LSL

R flags format: ADDS, ANDS, SUBS

I format: ORRI, EORI, ADDI, ANDI, SUBI

I flags format: ADDIS, SUBIS, ANDIS

B format: B, B.EQ, B.NE, B.LT, B.LE, B.GT, B.GE, B.LO, B.LS, B.HI, B.HS, CBZ, CBNZ

D format: STUR, LDUR

D variant format: STURB, LDURB, STURH, LDURH, STURW, LDURSW

BL statement: BL

BR statement: BR 

