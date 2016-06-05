/*
    * machine ADT for amlsfc
    * header file
    * Written by Riyasat Saber (c) 2016
*/

#define MEM_SIZE 256
#define REG_SIZE 16

#define FALSE 0
#define TRUE 1

#define LOADMEM 0x1
#define LOADBIT 0x2
#define STOREMEM 0x3
#define COPYREG 0x4
#define ADDTWO 0x5
#define ADDFLOAT 0x6
#define OR 0x7
#define AND 0x8
#define XOR 0x9
#define ROTATE 0xA
#define BRANCH 0xB
#define HALT 0xC

#define NORMAL 0
#define HALTED 1
#define ERROR 2

typedef unsigned char uc;

typedef struct{
    uc opcode;
    uc r;
    uc x;
    uc y;
} Instruction;

typedef struct _machine{
    char *name;
    uc mem[MEM_SIZE];
    uc reg[REG_SIZE];
    uc pc;
    Instruction ir;
    uc status;                 
}*Machine;

// an 1 byte floating point structure
/*
    [s][e][e][e][m][m][m][m]
    where:
        s is the sign
        e is the exponent
        m is the mantissa
    each box is a bit
*/
typedef struct{
    uc s : 1;
    uc e : 3;
    uc m : 5;
} Floating;


Machine newMachine();
void destroyMachine(Machine m);
void renameMachine(Machine m, char * name);

void saveMachine(Machine m);
void loadMachine(Machine m, char * filename);

uc readMemoryAt(Machine m, uc loc);
uc readRegistersAt(Machine m, uc loc);
uc readPC(Machine m);
int readIR(Machine m);
uc getStatus(Machine m);
void resetMachine(Machine m);
char * getName(Machine m);

void writeMemoryAt(Machine m, uc loc, uc value);
void writeRegistersAt(Machine m, uc loc, uc value);
void writePC(Machine ms, uc value);

void run(Machine m);
void step(Machine m);
void decodeInstruction(Machine m);
void execute(Machine m);

uc addFloats(Floating a, Floating b);
Floating UCToFloat(uc x);
uc floatToUC(Floating f);