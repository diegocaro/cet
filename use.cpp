/*
 * use.c
 *
 *  Created on: Mar 20, 2013
 *      Author: diegocaro
 */

#include <cstdio>
#include "tgl.h"


void print_arraysort(unsigned int *a) {
  uint i;

  for (i = 1; i <= *a; i++) {
    printf("%d ", a[i]);
  }
  printf("\n");

}

int main(int argc, char *argv[]) {
	TemporalGraphLog *index;
	ifstream f;
	uint *buffer;
	uint i, j;
	uint a,b;

	buffer = (uint *)malloc(sizeof(uint)*BUFFER);


	printf("Reading file: '%s'\n", argv[1]);

	f.open(argv[1], ios::binary);
	index = TemporalGraphLog::load(f);
	f.close();
/*
	printf("Write your query: ");
	while(scanf("%u %u", &a, &b)) {
		get_neighbors_point(buffer, &index, a, b);
		printf("t=%u -> ", 0);
		print_arraysort(buffer);
	}
	return 0;
*/
	vector<uint> bb;

	for ( i = 0; i < 5; i++) {
		//printf("start(%u): %u\n", i, start(index.map, i));
		printf("Direct of node %u\n", i);
		for( j = 0; j <= 5; j++) {
			index->direct_point(i, j, buffer);
			printf("t=%u -> ", j);print_arraysort(buffer);
			bb.clear();
		}
		
	}

	/*
	for ( i = 0; i < 5; i++) {
		printf("direct weak %u\n", i);
		for( j = 0; j <= index.maxtime; j++) {
			printf("time interval [0, %u)\n", j);
			get_neighbors_weak(buffer, &index, i, 0, j);
			printf("t=%u -> ", j);print_arraysort(buffer);
		}
	}

	for ( i = 0; i < 5; i++) {
		printf("direct strong %u\n", i);
		for( j = 0; j <= index.maxtime; j++) {
			printf("time interval [0, %u)\n", j);
			get_neighbors_strong(buffer, &index, i, 0, j);
			printf("t=%u -> ", j);print_arraysort(buffer);
		}
	}
*/
 
	for ( i = 0; i < 5; i++) {
		printf("Reverses of node %u\n", i);
		for( j = 0; j <= 5; j++) {
			index->reverse_point(i, j, buffer);
			printf("t=%u <- ", j);print_arraysort(buffer);
			bb.clear();
		}
		
	}
	/*
	for ( i = 0; i < 5; i++) {
		printf("Reverses of node %u (slow)\n", i);
		for( j = 0; j <= 5; j++) {
			get_reverse_point_slow(buffer, &index, i, j);
			printf("t=%u <- ", j);print_arraysort(buffer);
		}
	}

	for ( i = 0; i < 5; i++) {
			printf("Reverses weak %u\n", i);
			for( j = 0; j <= index.maxtime; j++) {
				printf("time interval [0, %u)\n", j);
				get_reverse_weak(buffer, &index, i, 0, j);
				printf("t=%u <- ", j);print_arraysort(buffer);
			}
		}


	for ( i = 0; i < 5; i++) {
			printf("Reverses strong %u\n", i);
			for( j = 0; j <= index.maxtime; j++) {
				printf("time interval [0, %u)\n", j);
				get_reverse_strong(buffer, &index, i, 0, j);
				printf("t=%u <- ", j);print_arraysort(buffer);
			}
		}
*/
	//free(buffer);
	return 0;
}

