/*
	main.c
	Algorithms and Analysis: Assignment 2

	Pathfinding
	Uniform-Cost Search
*/


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

/* needed for the curses animation */
#include <curses.h>
#include <unistd.h>

#include "pq.h"

#define PROGRAM_NAME	"mappath"


/*
	Bit flags are used by the animation, can be used elsewhere.
	Set a bit flag to true with: flags |= BIT_FLAG
	Set a bit flag to false with: flags &= ~BIT_FLAG
	Check the value with flags & BIT_FLAG
*/
#define SQ_FLAG_VISITED    0x01
#define SQ_FLAG_ENQUEUED   0x02
#define SQ_FLAG_IMPASSABLE 0x04
#define SQ_FLAG_START      0x08
#define SQ_FLAG_GOAL       0x10
#define SQ_FLAG_PATH       0x20

struct square {
	unsigned int flags;	/* bit flags */
	char glyph;		/* symbol to display this square */
	int cost;	
	int totalCost;
	struct square *prev;	
};

struct map {
	struct square *grid;	/* 2D grid of squares as an array */
	int width;		/* width of the map */
	int height;		/* height of the map */
};

/* global for time interval and also whether to use curses animation */
int g_curses_interval = 0;

/* initialises the map structure */
struct map *map_init(int width, int height);

/* clears the map structure and frees any memory */
void map_destroy(struct map *map);

/* loads the map text file into a new map structure */
struct map *load_map(char *fname);

/* display the map to stdout */
void print_map(struct map *map);

/* sets the values for the grid square */
void set_square(struct square *sq, char c);

/* calculate a lowest cost path from (x0, y0) to (x1, y1) */
void make_path(struct map *map,
	int x0, int y0, int x1, int y1);
/* output the lowest cost path to stdout */
void print_path(struct map *map,
	int x0, int y0, int x1, int y1);

/* path function implemented using Uniform Cost Search */
void ucs_path(struct map *map,int x0, int y0, int x1, int y1);

/* display path function implemented using Uniform Cost Search */
void display_ucs_path(struct map *map,int x0, int y0, int x1, int y1);

/* Prints the path */
void print_ucs_path(struct map *map,struct square *sq);

/* curses animation display functions */
void init_curses(void);
void end_curses(void);
/* call this to draw the map after each step */
void curses_draw_map(struct map *map);

/* You may and are encouraged to add your own functions. */

/**************************************************************/
/* find the lowest cost path and mark it on the map */
void make_path(struct map *map,	int x0, int y0, int x1, int y1)
{
	ucs_path(map, x0, y0, x1, y1);
}	

/* print the lowest cost path to stdout*/
void print_path(struct map *map, int x0, int y0, int x1, int y1)
{
	display_ucs_path(map, x0, y0, x1, y1);
}

/* sets the values for the grid square */
void set_square(struct square *sq, char c)
{
	sq->glyph = c;
	sq->flags = 0;
	switch (c) {
	case '.':
		sq->cost = 1;
		break;
	case ':':
		sq->cost = 2;
		break;
	case '=':
		sq->cost = 5;
		break;	
	case '+':
		sq->cost = 12;
		break;	
	case '#':
	default:
		/* impassable */
		sq->cost = -1;	/* set to invalid */
		sq->flags |= SQ_FLAG_IMPASSABLE;
		break;
	}
	/*
		If you add more variables to the square struct, 
		initialise them here.
	*/
	sq->totalCost = 0;
	sq->prev = NULL;
}
/**************************************************************/

/* initialises the map structure */
struct map *map_init(int width, int height)
{
	struct map *new_map;
	struct square *sq;
	int i;

	new_map = malloc(sizeof(struct map));
	if (!new_map) {	return NULL; }

	new_map->grid = malloc(sizeof(struct square) * width * height);
	if (!new_map->grid) {return NULL;}

	new_map->width = width;
	new_map->height = height;

	sq = new_map->grid;
	for (i = 0; i < width * height; i++) {
		set_square(sq, '#');	/* impassable by default */
		sq++;
	}

	return new_map;
}

/* clears the map structure and frees any memory */
void map_destroy(struct map *map)
{
	if (map) {
		free(map->grid);
		free(map);
	}
}

/* loads the map text file into a new map structure */
struct map *load_map(char *fname)
{
	struct map *map = NULL;
	struct square *sq;
	FILE *fp;
	int width, height;
	int n;
	char c;

	/* You should not need to have to alter this function. */

	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr,"Error: cannot open file %s.\n",	fname);
		exit(EXIT_FAILURE);
	}

	/* first pass: calculate width and height of map */
	width = height = n = 0;
	while ((c = getc(fp)) != EOF) {
		if (c == '\n') {
			height++;
			if (n > width) { width = n; }
			n = 0;
		} else {
			n++;
		}
	}	
	/* Causes Segmentation fault if last row of the map has no EOL, correct height by adding 1 */
	if (n > 0) {
		height += 1;
	}
	
	/* rewind file */
	if (fseek(fp, SEEK_SET, 0)) { return NULL; }

	/* pad out the map to make a sentinel border around the edge */
	width += 2;
	height += 2;

	/* now we can allocate memory to the map structure */
	map = map_init(width, height);
	if (!map) {
		fprintf(stderr, "Cannot allocate memory for map structure.\n");
		exit(EXIT_FAILURE);
	}

	/* second pass: load in the data */
	sq = map->grid + width + 1;	/* skip first row as a border */
	while ((c = getc(fp)) != EOF) {
		if (c == '\n') {
			sq += 2;
		} else {
			set_square(sq, c);
			sq++;
		}
	}

	fclose(fp);
	return map;
}

/* display the map to stdout */
void print_map(struct map *map)
{
	int x, y, i;
	struct square *sq;

	if (!map) { return; }

	i = map->width;	/* skip the sentinel border */
	for (y = 1; y < map->height - 1; y++) {
		i++;	/* skip the left edge */
		for (x = 1; x < map->width - 1; x++) {
			sq = map->grid + i;
			putchar(sq->glyph);
			i++;
		}
		putchar('\n');
		i++;	/* skip the right edge */
	}
}


/* path function using Uniform cost search */
void ucs_path(struct map *map,	int x0, int y0, int x1, int y1)
{
	int val, prio, nVal, nPrio;
	int pathFound = 0;
	pq_t *pq;

	int neighbours[4] = {-1,+1,0,0};
	int n;
	struct square *sq;
	neighbours[2] = -(map->width); 
	neighbours[3] = +(map->width);

	/* Set flags for the start and end */
	map->grid[y0 * (map->width) + x0].flags |= SQ_FLAG_START;
	map->grid[y1 * (map->width) + x1].flags |= SQ_FLAG_GOAL;
	curses_draw_map(map);

	/* Create pq */
	if((pq = pq_create()) == NULL){
		fprintf(stderr, "Not enough memory.\n");
		exit(EXIT_FAILURE);
	}

	/* Add start node*/	
	val = y0 * (map->width) + x0;
	pq_enqueue(pq, val, 0);
	map->grid[val].flags |= SQ_FLAG_ENQUEUED;
	while(pq_size(pq) > 0){		
		pq_dequeue(pq, &val, &prio);		
		if((map->grid[val].totalCost) < prio){
			continue;
		}	
		if(SQ_FLAG_GOAL & (map->grid[val].flags)){			
			pathFound = 1;
			break;
		}

		/* Set visited nodes */
		map->grid[val].flags |= SQ_FLAG_VISITED;
		
		for (n = 0; n < 4; ++n) {
			nVal = val + neighbours[n];
			if((SQ_FLAG_IMPASSABLE & (map->grid[nVal].flags)) == 0){
				if((SQ_FLAG_VISITED & (map->grid[nVal].flags)) == 0){
					if((SQ_FLAG_ENQUEUED & (map->grid[nVal].flags)) == 0){
						nPrio = prio + (map->grid[nVal].cost);
						map->grid[nVal].totalCost = nPrio;
						map->grid[nVal].prev = (map->grid) + val;
						if (!(pq_enqueue(pq, nVal, nPrio))){
							fprintf(stderr, "Min heap is full).\n");
							break;
						}
						map->grid[nVal].flags |= SQ_FLAG_ENQUEUED;
					}
					else{
						nPrio = prio + (map->grid[nVal].cost);
						if(nPrio < (map->grid[nVal].totalCost)){
							/* Update total cost */
							map->grid[nVal].totalCost = nPrio;
							map->grid[nVal].prev = (map->grid) + val;
							if(!(pq_enqueue(pq, nVal, nPrio))){
								fprintf(stderr, "Min heap is full.\n");
								break;
							}
						}
					}
				}
			}
		}		
		curses_draw_map(map);
	}	
	pq_destroy(pq);	
	if (pathFound){		
		val = y1 * (map->width) + x1;
		sq = (map->grid + val);

		sq->flags |= SQ_FLAG_PATH;
		while(!(SQ_FLAG_START & sq->flags )){
			sq = sq->prev;
			sq->flags |= SQ_FLAG_PATH;
		}
		curses_draw_map(map);
	}
}

/*Display path using Uniform cost search*/
void display_ucs_path(struct map *map, int x0, int y0, int x1, int y1)
{
	struct square *sq;
	int cost;

	ucs_path(map, x0, y0, x1, y1);
	sq = (map->grid) + y1 * (map->width) + x1;
	if (SQ_FLAG_PATH &(sq->flags)){	
		cost = (sq->totalCost);
		sq->glyph = 'B';
		while (!(SQ_FLAG_START & sq->flags)){
			sq = sq->prev;
			sq->glyph = 'o';
		}
		sq->glyph = 'A';			
		print_map(map);		
		sq = (map->grid) + y1 * (map->width) + x1;
		print_ucs_path(map, sq);		
		printf("Path cost: %d\n", cost);
	}
	else{
		printf("No path found.\n");
	}
}

/* Prints path */
void print_ucs_path(struct map *map,struct square *sq)
{	
	int val = (sq - (map->grid));
	int x = val % (map->width);
	int y = val / (map->width);

	if(!(SQ_FLAG_START & (sq->flags))){
		print_ucs_path(map, sq->prev);
	}		
	printf("(%d, %d)", x, y);	
	if(!(SQ_FLAG_GOAL & (sq->flags))){
		printf(" -> ");
	}
	else{
		printf("\n");
	}
}

/*****************************
  curses animation functions
*****************************/

/* Initialise curses, set the colour pairs */
void init_curses(void)
{
	if (!g_curses_interval) { return; }
	initscr();
	start_color();
	init_pair(0, COLOR_WHITE, COLOR_BLACK); /* normal */
	init_pair(1, COLOR_RED, COLOR_BLACK);   /* visited */
	init_pair(2, COLOR_CYAN, COLOR_BLACK);  /* enqueued */
	init_pair(3, COLOR_GREEN, COLOR_BLACK); /* start */
	init_pair(4, COLOR_YELLOW, COLOR_BLACK); /* goal */
	init_pair(5, COLOR_BLUE, COLOR_BLUE); /* impassable */
	init_pair(6, COLOR_GREEN, COLOR_BLACK); /* on path */

	atexit((void(*)())endwin);
}

/* Do any cleanup needed, provide a system pause at the end */
void end_curses(void)
{
	if (!g_curses_interval) { return; }
	getchar();	/* pause at end */
	/* endwin will be called at exit */
}

/* Call this function to draw the map after every step */
void curses_draw_map(struct map *map)
{
	int i,j;
	struct square *sq;
	if (!g_curses_interval) { return; }
	if (map == NULL) { return; }
	sq = map->grid + map->width;
	for (j = 1; j < map->height-1; j++) {
		move(j-1, 0);
		sq++;
		for (i = 1; i < map->width-1; i++) {
			char ch = sq->glyph;
			int color = 0;

			if (sq->flags & SQ_FLAG_IMPASSABLE) {
				color = 5;
			}
			if (sq->flags & SQ_FLAG_VISITED) {
				color = 1;
			}
			if (sq->flags & SQ_FLAG_ENQUEUED) {
				color = 2;
				ch = 'o';
			}
			if (sq->flags & SQ_FLAG_PATH) {
				color = 6;
				ch = 'o';
			}
			if (sq->flags & SQ_FLAG_START) {
				color = 3;
				ch = 'A';
			}
			if (sq->flags & SQ_FLAG_GOAL) {
				color = 4;
				ch = 'B';
			}

			if (sq->flags & ~SQ_FLAG_IMPASSABLE) { attron(A_BOLD); }

			attron(COLOR_PAIR(color));
			addch(ch);
			attroff(COLOR_PAIR(color));

			if (sq->flags & ~SQ_FLAG_IMPASSABLE) { attroff(A_BOLD); }

			sq++;
		}
		sq++;
	}
	move(j, 0);
	refresh();
	usleep(g_curses_interval);
}

/*****************************
  main function
*****************************/
int main(int argc, char **argv)
{
	struct map *map;
	int x0, y0, x1, y1;

	/* parse args */
	if (argc < 6) {
		fprintf(stderr, "%s needs at least 5 arguments (%d were given)\n",
			PROGRAM_NAME, argc-1);
		fprintf(stderr, "usage: %s mapfile x0 y0 x1 y1 (time_interval - optional)\n",
			PROGRAM_NAME);
		exit(EXIT_FAILURE);
	}
	x0 = atoi(argv[2]);
	y0 = atoi(argv[3]);
	x1 = atoi(argv[4]);
	y1 = atoi(argv[5]);
	g_curses_interval = 0;
	if (argc > 6) { g_curses_interval = atoi(argv[6]); }

	map = load_map(argv[1]);
	if (!map) {
		fprintf(stderr, "Error loading map.\n");
		exit(EXIT_FAILURE);
	}
	if ((x0 <= 0 || x0 >= map->width-1) ||
		(x1 <= 0 || x1 >= map->width-1) ||
		(y0 <= 0 || y0 >= map->height-1) ||
		(y1 <= 0 || y1 >= map->height-1)) {
		fprintf(stderr, "Coordinates are not in range.\n");
		exit(EXIT_FAILURE);		
	}

	/* initialise the curses animation if we're using that */
	if (g_curses_interval > 0) {
		init_curses();
	}

	make_path(map, x0, y0, x1, y1);

	if (g_curses_interval > 0) {
		end_curses();
	} else {
		print_path(map, x0, y0, x1, y1);
	}

	map_destroy(map);

	return EXIT_SUCCESS;
}
