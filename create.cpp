#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include "debug.h"
#include "interleave.h"
#include "tgl.h"

struct infolog {
        uint nodes;
        uint changes;
        uint maxtime;
        uint size;
};

struct adjlog {
        uint nodes; //number of nodes from [0 ... nodes -1]
        uint changes; //total number of changes
        uint maxtime; //maximum time in the dataset from [0 ... maxtime-1]

        uint size_log; //size of the log
        u_long *log; //including time and edges

        uint *time; // bitmap of time in nodes
        uint size_time;
};


// read temporal graph and populate adjlog
void read(struct adjlog *l) {
	uint nodes, changes, maxtime;
	uint a[4];
	uint t;
	scanf("%u %u %u", &nodes, &changes, &maxtime);
	
	l->nodes = nodes;
	l->changes= changes;
	l->maxtime = maxtime;
	l->size_log = changes;
	l->size_time = maxtime + changes; // in bits
	LOG("Nodes:\t%u", l->nodes);
	LOG("Changes:\t%u", l->changes);
	LOG("Maxtime:\t%u", l->maxtime);
	
	LOG("space for log:\t%.2lf MBytes (%lu bytes)", (double)l->size_log*sizeof(u_long)/1024/1024, l->size_log*sizeof(u_long));
	
	LOG("space for time:\t%.2lf MBytes (%lu bytes)", (double)(l->size_time/W+1)*sizeof(uint)/1024/1024, l->size_time/W*sizeof(uint));
	
	l->time = (uint *) calloc((l->size_time/W + 1), sizeof(uint));
	l->log = (u_long *) malloc(l->size_log * sizeof(u_long));
	
//	printf("int1: %X\n", in1((uint)1, (uint)1));
	
	uint bitpos=0;
	uint i;
	t = 0;
	u_long *p;
	p = l->log;
	while(EOF != scanf("%u %u %u %u", &a[0], &a[1], &a[2], &a[3])) {
		*p++ = in1(a[0], a[1]);
		if (t != a[2]) {
			//put time mark
			for( i = t; i < a[2]; i++) {
				//printf("setting in pos %u\n", bitpos);
				bitset(l->time, bitpos);
				bitpos++;
			}
		}
		
		t = a[2];
		bitpos++;
	}
	bitset(l->time, bitpos);
	
}


void create_index(TemporalGraphLog &tgl, struct adjlog *adjlog) {
	tgl.set_nodes(adjlog->nodes);
	tgl.set_changes(adjlog->changes);
	tgl.set_maxtime(adjlog->maxtime);
	tgl.set_log(adjlog->log, adjlog->size_log);
	tgl.set_time(adjlog->time, adjlog->size_time);
	
}

int main(int argc, char *argv[]) {
	struct adjlog tg;
	TemporalGraphLog tgl;
	
        if (argc < 3) {
                fprintf(stderr, "%s <dataset> <outputfile>\n", argv[0]);
                exit(-1);
        }
	
	INFO("Loading graph...");
	read(&tg);
	
	INFO("Creating index...");
	create_index(tgl, &tg);
	LOG("Size of index: %lu\n", tgl.getSize());
	
	LOG("Depth: %u", tgl.get_log()->getDepth());
	
	ofstream file;
	file.open(argv[2], ios::binary);
	tgl.save(file);
	file.close();
	
	return 0;
}