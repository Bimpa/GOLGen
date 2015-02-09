/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "prototype.h"

//defines
#define 		ALIVE 					1
#define 		DEAD 					0
#define			INITIAL_ACTIVITY_RATIO 	0.25

//random
#define 		RAND_A 16807
#define 		RAND_C 1
int 			seed = 1234;

//globals
unsigned int 	width;
unsigned int	height;
unsigned int 	iterations;
FILE 			*input_file = NULL;
FILE 			*output_file = NULL;
FILE 			*damson_file = NULL;
char 			*states = NULL;
char		 	*tmp_states = NULL;


void print_usage();
void simulate();
unsigned int get_wrapped_position(unsigned int x, unsigned int y, int x_offset, int y_offset);

void read_initial_states();
void random_initial_states();
void write_output();
void write_damson();

float rand0to1();
void safeexit();

int main(int argc, char* argv[]) {
	//check format
	if (argc < 4)
	{
		print_usage();
		exit(0);
	}
	width = atoi(argv[1]);
	height = atoi(argv[2]);
	iterations = atoi(argv[3]);

	if ((width != height)||(width<1))
	{
		printf("Error: Square layouts only for now!\n");
	}

	//allocate space
	states = (char*)malloc(width*height*sizeof(char));
	tmp_states = (char*)malloc(width*height*sizeof(char));

	int i=4;
	while (argc > i)
	{
		//check usage
		if (argc == i+1){
			printf("Error: incomplete arguments\n\n");
			print_usage();
			safeexit();
		}

		if (strcmp(argv[i], "-i") == 0)
		{
			input_file = fopen(argv[i+1], "r");
			if (input_file == NULL)
			{
				printf("Error: Cannot open input file %s\n", argv[i+1]);
				safeexit();
			}
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			output_file = fopen(argv[i+1], "w");
			if (output_file == NULL)
			{
				printf("Error: Cannot open output file %s\n", argv[i+1]);
				safeexit();
			}
		}
		else if (strcmp(argv[i], "-d") == 0)
		{
			damson_file = fopen(argv[i+1], "w");
			if (damson_file == NULL)
			{
				printf("Error: Cannot open DAMSON output file %s\n", argv[i+1]);
				safeexit();
			}
		}else
		{
			printf("Error: Unrecognised argument %s\n", argv[i]);
			print_usage();
			safeexit();
		}

		i+=2;
	}

	//input or random initialisation
	if (input_file)
	{
		read_initial_states();
	}
	else
	{
		random_initial_states();
	}

	//DAMSON output or simulation
	if (damson_file)
	{
		write_damson();
	}else{

		//simulate
		for (i=0;i<iterations;i++)
		{
			printf("Simulation iteration %d!\n", i+1);
			simulate();
		}
		//output
		if (output_file)
		{
			write_output();
		}
	}

	free(states);
	free(tmp_states);
	printf("Done!\n");

	return 0;
}

void print_usage()
{
	printf("Usage: gol WIDTH HEIGHT ITERATIONS [OPTION]\n");
	printf("\n");
	printf("  -i \tInput file with initial state date\n");
	printf("  -o \tOutput file for final states after simulation\n");
	printf("  -d \tDAMSON output file (no simulation)\n");
}

void simulate()
{
	unsigned int x, y;
	int i, j;
	unsigned int p, n;
	unsigned int state, active_neighbours;
	char *swap;

	for (y=0; y<height; y++)
	{
		for (x=0; x<width; x++)
		{
			p = (y*width) + x;
			state = states[p];
			active_neighbours = 0;
			//check neighbours
			for (j=-1; j<=1; j++)
			{
				for (i=-1; i<=1; i++)
				{
					if ((i!=0)||(j!=0)){ //dont count self
						n = get_wrapped_position(x, y, i, j);
						active_neighbours += states[n];
					}
				}
			}

			if (state==ALIVE){
				if(active_neighbours < 2)
					state = DEAD;
				else if(active_neighbours > 3)
					state = DEAD;
			}else{
				if(active_neighbours == 3)
					state = ALIVE;
			}
			tmp_states[p] = state;


		}
	}

	//swap
	swap = states;
	states = tmp_states;
	tmp_states = swap;
}

unsigned int get_wrapped_position(unsigned int x, unsigned int y, int x_offset, int y_offset)
{
	x_offset += x;
	y_offset += y;

	//wrap x bounds
	if (x_offset<0)
		x = width-1;
	else if (x_offset==width)
		x = 0;
	else
		x = x_offset;

	//wrap y bounds
	if (y_offset<0)
		y = height-1;
	else if (y_offset==height)
		y = 0;
	else
		y = y_offset;

	//return
	return (y*width) + x;
}

void read_initial_states()
{
	unsigned int x, y;
	unsigned int w, h;
	unsigned int p;
	char s;

	printf("Reading initial states...\n");

	//read width and height
	fscanf(input_file, "width=%u\n", &w);
	fscanf(input_file, "height=%u\n", &h);

	//check dimensions
	if (width != w){
		printf("Error: input file width '%d' does not match program argument '%d'\n", w, width);
		safeexit();
	}
	if (height != h){
		printf("Error: input file height '%d' does not match program argument '%d'\n", h, height);
		safeexit();
	}

	//read the rest
	for (y=0; y<height; y++)
	{
		for (x=0; x<width; x++)
		{
			p = (y*width) + x;
			if (fscanf(input_file, "%c", &s) != 1)
			{
				printf("Error: Incorrect formatting of input file on line %d", y+2);
				safeexit();
			}
			states[p] = s - '0';
		}
		fscanf(input_file, "\n");
	}
}

void random_initial_states()
{
	unsigned int x, y;
	unsigned int p;
	float s;

	printf("Randomising initial states...\n");
	srand (time(NULL));

	for (y=0; y<height; y++)
	{
		for (x=0; x<width; x++)
		{
			p = (y*width) + x;
			s = rand0to1();
			if (s > INITIAL_ACTIVITY_RATIO)
				states[p] = ALIVE;
			else
				states[p] = DEAD;
		}
	}
}

void write_output()
{
	unsigned int x, y;
	unsigned int p;

	printf("Writing final states...\n");

	fprintf(output_file, "width=%u\n", width);
	fprintf(output_file, "height=%u\n", height);

	for (y=0; y<height; y++)
	{
		for (x=0; x<width; x++)
		{
			p = (y*width) + x;
			fprintf(output_file, "%u", states[p]);
		}
		fprintf(output_file, "\n");
	}
	fprintf(output_file, "\n");
}

void write_damson()
{
	unsigned int x, y;
	int i, j;
	unsigned int p, n;

	printf("Writing DAMSON file...\n");

	//write prototype
	fprintf(damson_file, prototype, iterations);
	fprintf(damson_file, "\n");

	for (y=0; y<height; y++)
	{
		for (x=0; x<width; x++)
		{
			//write alias data
			p = (y*width) + x;
			fprintf(damson_file, "#alias gameoflife %d\n", p+1);
			fprintf(damson_file, "clock: 0\n");
			fprintf(damson_file, "receive: ");
			//print receive indices
			for (j=-1; j<=1; j++)
			{
				for (i=-1; i<=1; i++)
				{
					n = get_wrapped_position(x, y, i, j) ;
					if ((i!=0)||(j!=0)){
						fprintf(damson_file, "%u", n+1);
						if ((i<1)||(j<1))
							fprintf(damson_file, ",");
						else
							fprintf(damson_file, "\n");
					}
				}
			}

			fprintf(damson_file, "state=%u;\n", (unsigned int)(states[p]));
			//fprintf(damson_file, "#log \"out\" 0.0 1.0 1.0 \"state=%%d  N=%%d\\n\" state  active_neighbours\n");
			fprintf(damson_file, "\n");
		}
	}
	fprintf(damson_file, "\n");

}

float rand0to1(){
	float uniform;
	seed = seed*RAND_A + RAND_C;
	uniform = (float)seed/(float)0xffffffff;
	return uniform;
}

void safeexit()
{
	free(states);
	free(tmp_states);
	exit(0);
}
