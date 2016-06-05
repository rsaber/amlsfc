/*
	* machine ADT for amlsfc
	* Written by Riyasat Saber (c) 2016
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "machine.h"

// eg: examples/hello.a
// will return a pointer to h
// func will break if final char is a /
char * extractNameFromPath(char * path){
	int i, head=0;
	for(i=0; path[i]!='\0'; i++){
		if(path[i] == '/') head = i+1;
	}
	return path+head;
}

// --ADT HELPERS------------------------------
uc readMemoryAt(Machine m, uc loc){
	return m->mem[loc];
}

uc readRegistersAt(Machine m, uc loc){
	return m->reg[loc];
}

uc readPC(Machine m){
	return m->pc;
}

int readIR(Machine m){
	int ir = 0;
	ir |= m->ir.opcode << 12;
	ir |= m->ir.r << 8;
	ir |= m->ir.x << 4;
	ir |= m->ir.y;
	return ir;
}

uc getStatus(Machine m){
	return m->status;
}

void writeMemoryAt(Machine m, uc loc, uc value){
	m->mem[loc] = value;
}

void writeRegistersAt(Machine m, uc loc, uc value){
	m->reg[loc] = value;
}

void writePC(Machine m, uc value){
	m->pc = value;
}

char * getName(Machine m){
	return m->programName;
}

void resetMachine(Machine m){
	int i;
	for(i=0; i<REG_SIZE; i++){
		m->reg[i] = 0;
	}
	m->pc = 0;
	m->status = NORMAL;
}

Machine newMachine(){
	Machine m = malloc(sizeof(struct _machine));
	m->programName = strdup("untitled.a");
	m->filepath = strdup("untitled.a");

	int i;
	for(i=0; i<MEM_SIZE; i++) m->mem[i] = 0;
	for(i=0; i<REG_SIZE; i++) m->reg[i] = 0;

	m->pc = 0;
	m->ir.opcode = 0;
	m->ir.r = 0;
	m->ir.x = 0;
	m->ir.y = 0;
	
	m->status = NORMAL;
	return m;
}

void saveMachine(Machine m){
	FILE *fp = fopen(m->filepath,"w");
	if(fp==NULL) return;
	int i;
	for(i=0; i<MEM_SIZE; i++){
		fprintf(fp,"%d ",m->mem[i]);
	}
	fclose(fp);
}

void loadMachine(Machine m, char * filename){
	FILE *fp = fopen(filename,"r");

	char * progname = extractNameFromPath(filename);
	free(m->programName);
	free(m->filepath);
	m->programName = strdup(progname);
	m->filepath = strdup(filename);

	if(fp==NULL) return;

	int i,temp;
	for(i=0; i<MEM_SIZE; i++){
		fscanf(fp, "%d", &temp);
		m->mem[i] = temp;
	}
	fclose(fp);
}

void destroyMachine(Machine m){
	free(m->programName);
	free(m->filepath);
	free(m);
}

// ---------------------------------------------

void run(Machine m){
	int i = 0;
	m->status = NORMAL;
	while(i<500 && m->status == NORMAL){
		step(m);
		i++;
	}
}

// will fetch the next instruction and store in m->ir
void decodeInstruction(Machine m){
	m->ir.opcode = m->mem[m->pc] >> 4;
	m->ir.r = m->mem[m->pc] ^ (m->ir.opcode << 4);
	m->ir.x = m->mem[m->pc+1] >> 4;
	m->ir.y = m->mem[m->pc+1] ^ (m->ir.x << 4);

	m->pc+=2 % MEM_SIZE;
}

void step(Machine m){
	decodeInstruction(m);
	execute(m);
}

void execute(Machine m){
	int opcode = m->ir.opcode;
	int r = m->ir.r;
	int x = m->ir.x;
	int y = m->ir.y;
	
	switch (opcode){
		case LOADMEM:
			m->reg[r] = m->mem[x * 16 + y];
			break;
		case LOADBIT:
			m->reg[r] = x * 16 + y;
			break;
		case STOREMEM:
			m->mem[x * 16 + y] = m->reg[r];
			break;
		case COPYREG:
			m->reg[y] = m->reg[x];
			break;
		case ADDTWO:
			m->reg[r] = m->reg[x] + m->reg[y];
			break;
		case ADDFLOAT:
			m->reg[r] = addFloats(UCToFloat(m->reg[x]), UCToFloat(m->reg[y]));
			break;
		case OR:
			m->reg[r] = m->reg[x] | m->reg[y];
			break;
		case AND:
			m->reg[r] = m->reg[x] & m->reg[y];
			break;
		case XOR:
			m->reg[r] = m->reg[x] ^ m->reg[y];
			break;
		case ROTATE:
			m->reg[r] = (r >> y) | (r << (8 - y));
			break;
		case BRANCH:
			if (m->reg[r] == m->reg[0]) m->pc = x * 16 + y;
			break;
		case HALT:
			m->status = HALTED;
			break;
		default:
			// invalid opcode
			m->status = ERROR;
	}
}

// FLOATING POINT ARITHMETIC
uc addFloats(Floating a, Floating b){
	// NOT DONE!

	return 1;
}

// NO CHECKS DONE FOR OVERFLOWS!
// or rounding
Floating UCToFloat(uc x){
	Floating f;
	// any overflows will be cut off, because we used bitfields
	f.s = x >> 7;
	f.e = x >> 4;
	f.m = x;
	return f;
}

uc floatToUC(Floating f){
	return (f.s << 7) | (f.e << 4) | f.m;
}