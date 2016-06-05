# amlsfc

a machine language simulator for COMP1917 

Compile with: 

    gcc -o amlsfc amlsfc.c machine.c -lncurses

Run with: 

    ./amlsfc [filename]


## Instructions
### Commands
* [enter] - edit a memory cell, use [enter] again to save changes
* [tab] - toggle between the 3 sections: memory, registers and PC
* c - clear registers and PC
* r - run program until HALT or 500 program cycles
* n - step through a single 4 byte command
* s - save file (no prompt)
* q - quit