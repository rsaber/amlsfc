#include <stdio.h>
#include <ncurses.h>
#include "machine.h"

#define SECTION_MEMORY		0
#define SECTION_REGISTERS	1
#define SECTION_PC			2

#define WINDOW_SIZE_WIDTH	80
#define WINDOW_SIZE_HEIGHT 	24

#define START_X 4
#define START_Y 1

int highlight[3] = {0};
int section = SECTION_MEMORY;

Machine m;

void updateView(WINDOW * win);
void createDefaultView(WINDOW * win);
unsigned char readInput(WINDOW *win);
void printAuthor(WINDOW *win);

int main(int argc, char * argv[]){
	WINDOW * win;
	initscr();
	clear();
	noecho();
	cbreak();
	curs_set(0);

	m = newMachine();
	if(argc == 2){
		// if load machine fails, then we are creating a new file with name argv[1]
		loadMachine(m,argv[1]);
		renameMachine(m,argv[1]);
	}

	win = newwin(WINDOW_SIZE_HEIGHT,WINDOW_SIZE_WIDTH,2,2);
	keypad(win,TRUE);
	refresh();
	createDefaultView(win);
	updateView(win);

	char read;
	int c = 0;
	while(c!='q'){
		c = wgetch(win);
		switch(c){
			// all these if statements are just edge cases where we wrap around
			// the table
			case KEY_UP:
				if(section == SECTION_MEMORY){
					if(highlight[section] < 16) highlight[section] = 255 - (15 - highlight[section]);
					else highlight[section]-=16;
				}
				break;
			case KEY_DOWN:
				if(section == SECTION_MEMORY){
					if(highlight[section] < 256 && highlight[section] >= 240) highlight[section] = highlight[section] - 16*15;
					else highlight[section]+=16;
				}
				break;
			case KEY_LEFT:
				if(section == SECTION_MEMORY){
					if(highlight[section]%16==0) highlight[section] += 15;
					else highlight[section]--;
				}else if(section == SECTION_REGISTERS){
					if(highlight[section] == 0) highlight[section] = 15;
					else highlight[section]--;
				}
				break;
			case KEY_RIGHT:
				if(section == SECTION_MEMORY){
					if((highlight[section]-15) % 16 ==0 ) highlight[section] -= 15;
					else highlight[section]++;
				}else if(section == SECTION_REGISTERS){
					if(highlight[section] == 15) highlight[section] = 0;
					else highlight[section]++;
				}
				break;

			// tab key
			case 9:
				// toggle section
				section++;
				section = section%3;
				break;

			case 'r':
				run(m);
				break;

			case 's':
				saveMachine(m);
				break;

			case 'c':
				resetMachine(m);
				break;

			// enter key
			case 10:
				read = readInput(win);
				if(section == SECTION_MEMORY) writeMemoryAt(m, highlight[section], read);
				else if(section == SECTION_REGISTERS) writeRegistersAt(m, highlight[section], read);
				else writePC(m, read);
				break;

			case 'n':
				step(m);
				break;

			default:
				refresh();
				break;
		}
		updateView(win);
	}

	clrtoeol();
	refresh();
	endwin();
	return 0;
}


// add a nice dialog box later
unsigned char readInput(WINDOW *win){
	unsigned char value;
	wscanw(win,"%x", &value);
	return value;
}


void createDefaultView(WINDOW * win){
	box(win, 0, 0);
	// RIYO AUTHORSHIP
	int t = 3;
	
	mvwprintw(win,0,START_X,"[amlsfc]");
	mvwprintw(win,0,START_X+9,"[%s]",getName(m));
	mvwprintw(win,t++,53,"%s", "a machine language");
	mvwprintw(win,t++,53,"%s", "simulator for COMP1917");
	t++;
	mvwprintw(win,t++,53,"%s", "commands:");
	mvwprintw(win,t++,53,"%s", "[enter] - edit cell");
	mvwprintw(win,t++,53,"%s", "[tab] - toggle section");
	mvwprintw(win,t++,53,"%s", "c - clear regs and pc");
	mvwprintw(win,t++,53,"%s", "r - run program");
	mvwprintw(win,t++,53,"%s", "n - step");
	mvwprintw(win,t++,53,"%s", "s - save file");
	mvwprintw(win,t++,53,"%s", "q - quit");
	//mvwprintw(win,t++,53,"%s", "w - credits");

	mvwprintw(win,WINDOW_SIZE_HEIGHT-1,50,"%s", "[written by Riyasat Saber]");
}

void updateView(WINDOW * win){
	int i;
	int temp = 0;
	int x,y;

	y=START_Y;
	x=START_X+1;

	// print axis row
	for(i=0; i<16; i++){
		mvwprintw(win,y,x,"%X", temp++);
		x+=3;
	}
	temp = 0;
	x = START_X-2;
	y=START_Y+1;

	// print axis col
	for(i=0; i<16; i++){
		mvwprintw(win,y,x,"%X", temp++);
		y++;
	}
	y = START_Y;
	
	// Print memory
	for(i=0; i<256; i++){
		if(i%16==0){
			y++;
			x=START_X;
		}	
		if(highlight[section] == i && section == SECTION_MEMORY){
			wattron(win, A_REVERSE); 
			mvwprintw(win,y,x,"%.2X", readMemoryAt(m,i));
			wattroff(win, A_REVERSE);
		}else{
			mvwprintw(win,y,x,"%.2X", readMemoryAt(m,i));
		}
		x+=3;
	}

	// Print Registers
	x = START_X;
	y+=2;
	for(i=0; i<16; i++){
		mvwprintw(win,y,x,"R%X",i );
		x+= 3;
	}
	x = START_X;
	y++;
	for(i=0; i<16; i++){
		if(highlight[section] == i && section == SECTION_REGISTERS){
			wattron(win, A_REVERSE); 
			mvwprintw(win,y,x,"%.2X", readRegistersAt(m,i));
			wattroff(win, A_REVERSE); 
		}else{
			mvwprintw(win,y,x,"%.2X", readRegistersAt(m,i));
		}
		x+= 3;
	}
	x = START_X;
	y += 2;
	// print pc and ic
	mvwprintw(win,y,x,"PC : ");
	if(section == SECTION_PC){
		wattron(win, A_REVERSE); 
		mvwprintw(win,y,x+5,"%.2X", readPC(m));
		wattroff(win, A_REVERSE); 
	}else{
		mvwprintw(win,y,x+5,"%.2X", readPC(m));
	}
	
	x+= 12;
	mvwprintw(win,y,x,"IR : %.4X", readIR(m));
	x+= 20;
	mvwprintw(win,y,x,"STATUS : ");

	if(getStatus(m) == NORMAL){
		mvwprintw(win,y,x+9,"NORMAL");
	}else if(getStatus(m) == HALTED){
		mvwprintw(win,y,x+9,"HALTED");
	}
	else if(getStatus(m) == ERROR){
		mvwprintw(win,y,x+9,"ERROR ");
	}
	wrefresh(win);
}

