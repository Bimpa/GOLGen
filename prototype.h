/*
 * prototype.h
 *
 *  Created on: 10 Dec 2013
 *      Author: paul
 */

#ifndef PROTOTYPE_H_
#define PROTOTYPE_H_

const char* prototype = "\
#timestamp on\n\
#monitor off\n\
\n\
#define ALIVE 1\n\
#define DEAD 0\n\
#define ITERATIONS %u\n\
\n\
#node gameoflife\n\
\n\
int state;\n\
int active_neighbours;\n\
int c;\n\
\n\
void receive(int n, int p, int x, int t);\n\
void clock();\n\
\n\
void main()\n\
{\n\
	c = 1;\n\
	active_neighbours = 0;\n\
	if (state>0)\n\
	    sendpkt(0, state);\n\
}\n\
\n\
void receive(int n, int p, int x, int t)\n\
{\n\
    //printf(\"%d, %d, %d, %d\\n\", n, p, x, t);\n\
    active_neighbours += 1;    \n\
}\n\
\n\
void clock()\n\
{\n\
    if (state==ALIVE){\n\
        if(active_neighbours < 2)\n\
            state = DEAD;\n\
        else if(active_neighbours > 3)\n\
            state = DEAD;\n\
    }else{\n\
        if(active_neighbours == 3)\n\
            state = ALIVE;\n\
    }\n\
    \n\
    c+=1;\n\
	if(c>ITERATIONS)\n\
	   exit(state);\n\
\n\
    active_neighbours = 0;\n\
    if (state>0)\n\
        sendpkt(0, state);\n\
}\n\
";

#endif /* PROTOTYPE_H_ */
