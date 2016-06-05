#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "machine.h"

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
	return m->name;
}

void resetMachine(Machine m){
	memset(m->reg, 0, sizeof(int)*REG_SIZE);
	m->pc = 0;
	m->status = NORMAL;
}

// ---------------------------------------------

Machine newMachine(){
	Machine m = malloc(sizeof(struct _machine));
	m->name = strdup("untitled.a");

	memset(m->mem, 0, sizeof(int)*MEM_SIZE);
	memset(m->reg, 0, sizeof(int)*REG_SIZE);

	m->pc = 0;
	m->ir.opcode = 0;
	m->ir.r = 0;
	m->ir.x = 0;
	m->ir.y = 0;
	
	m->status = NORMAL;
	return m;
}

void saveMachine(Machine m){
	FILE *fp = fopen(m->name,"w");
	if(fp==NULL) return;
	int i;
	fprintf(fp,"%s\n",m->name);
	for(i=0; i<MEM_SIZE; i++){
			fprintf(fp,"%d ",m->mem[i]);
	}
	fclose(fp);
}

void loadMachine(Machine m, char * filename){
	FILE *fp = fopen(filename,"r");
	if(fp==NULL) return;
	int i,temp;
	fgets(m->name, 16, fp);
	for(i=0; i<MEM_SIZE; i++){
			fscanf(fp, "%d", &temp);
			m->mem[i] = temp;
	}
	fclose(fp);
}

void destroyMachine(Machine m){
	free(m->name);
	free(m);
}

void renameMachine(Machine m, char * name){
	free(m->name);
	m->name = strdup(name);
}

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
	int opcode  = m->ir.opcode;
	int r       = m->ir.r;
	int x       = m->ir.x;
	int y       = m->ir.y;
	
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
			m->reg[r] = addFloats(m->reg[x], m->reg[y]);
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

// fix!
uc addFloats(uc a, uc b){
	Floating x,y;
	x = decodeFloat(a);
	y = decodeFloat(b);

	return 1;
}

// NO CHECKS DONE FOR OVERFLOWS!
// or rounding
Floating decodeFloat(uc x){
	Floating f;
	// any overflows will be cut off, because we used bitfields
	f.s = x >> 7;
	f.e = x >> 4;
	f.m = x;
	return f;
}

uc encodeFloat(Floating f){
	return (f.s << 7) | (f.e << 4) | f.m;
}

