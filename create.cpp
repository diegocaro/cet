#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <getopt.h>
#include <cstring>

#include <map>
#include <vector>
#include <algorithm>

#include "debug.h"
#include "symbols.h"
#include "tgl.h"

struct infolog {
	uint nodes;
	uint changes;
	uint maxtime;
};

struct adjlog {
	uint nodes; //number of nodes from [0 ... nodes -1]
	uint changes; //total number of changes
	uint maxtime; //maximum time in the dataset from [0 ... maxtime-1]

	uint size_log; //size of the log
	usym *log; //including time and edges

	uint *time; // bitmap of time in nodes
	uint size_time;
};


enum bitseq {
	RG, R3, SD
};

struct opts {
	enum bitseq bs; //bits for wavelet tree
	enum bitseq bb; //bits for B bitmap
	char *outfile;
};


// read temporal graph and populate adjlog
void read_stdin(struct adjlog *l) {
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
	l->log = (usym *) malloc(l->size_log * sizeof(usym));
	
	INFO("Memory acquiered");
	
	//	printf("int1: %X\n", in1((uint)1, (uint)1));
	
	uint bitpos=0;
	uint i;
	t = 0;
	//u_long *p;
	//p = l->log;
	uint k=0;
	
#ifdef DEBUG
	uint changes_read=0;
#endif
	
	while(EOF != scanf("%u %u %u %u", &a[0], &a[1], &a[2], &a[3])) {
		//*p++ = in1(a[0], a[1]);
		l->log[k].x = a[0];
		l->log[k].y = a[1];    
		k++;
    
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
		
#ifdef DEBUG
		if(changes_read++%100000==0) fprintf(stderr,"Progress: %.2f%%\r", (float)changes_read/changes*100);
#endif
	}
	bitset(l->time, bitpos);
	
}

bool usymsort(const usym &a, const usym &b) {
    if (a.x < b.x) return true;
    if (a.x == b.x) return (a.y < b.y);
    return false;
}

// read temporal graph from contacts
void read_contacts(struct adjlog *l) {
	uint nodes, edges, lifetime, contacts;
		uint u,v,a,b;

		map<uint, vector<usym> > btable;

		scanf("%u %u %u %u", &nodes, &edges, &lifetime, &contacts);

		uint c_read = 0;

		usym c;
    uint changes = 0;
		while( EOF != scanf("%u %u %u %u", &u, &v, &a, &b)) {
			c_read++;
			if(c_read%500000==0) fprintf(stderr, "Processing %.1f%%\r", (float)c_read/contacts*100);
			c.x = u;
			c.y = v;
			btable[a].push_back(c);
      changes++;
			if (b == lifetime-1) continue;
      changes++;
			btable[b].push_back(c);
		}
		fprintf(stderr, "Processing %.1f%%\r", (float)c_read/contacts*100);
		assert(c_read == contacts);

//		uint changes = 2*contacts;

		l->nodes = nodes;
		l->changes= changes;
		l->maxtime = lifetime;

		l->size_log = changes; 
		l->size_time = lifetime + changes; 
		LOG("Nodes:\t%u", l->nodes);
		LOG("Changes:\t%u", l->changes);
		LOG("Maxtime:\t%u", l->maxtime);

		LOG("space for log:\t%.2lf MBytes (%lu bytes)", (double)l->size_log*sizeof(usym)/1024/1024, l->size_log*sizeof(usym));

		LOG("space for time:\t%.2lf MBytes (%lu bytes)", (double)(l->size_time/W+1)*sizeof(uint)/1024/1024, l->size_time/W*sizeof(uint));

		l->time = (uint *) calloc((l->size_time/W + 1), sizeof(uint));
		l->log = (usym *) malloc(l->size_log * sizeof(usym));

		INFO("Memory acquiered");


		uint p=0;


		for(uint i=0; i < lifetime; i++) {
            sort (btable[i].begin(), btable[i].end(), usymsort);
			for (uint j=0; j < btable[i].size(); ++j) {
                c = btable[i][j];
				l->log[p] = btable[i][j];
				p++;
			}
			bitset(l->time, i+p);
      
      btable[i].clear();
		}
    assert(p == changes);
		//l->size_log = p; //actual size (could be less than lenS)
		//l->size_time = lifetime + p; // in bits, actual value

}

void create_index(TemporalGraphLog &tgl, struct adjlog *adjlog, struct opts *opts) {
	BitSequenceBuilder *bs;
	BitSequenceBuilder *bb;
	
	tgl.set_nodes(adjlog->nodes);
	tgl.set_changes(adjlog->changes);
	tgl.set_maxtime(adjlog->maxtime);
	
	switch(opts->bs) {
		case RG:
		bs = new BitSequenceBuilderRG(20); // by default, 5% of extra space for bitmaps
		break;
		case R3:
		bs = new BitSequenceBuilderRRR(32); // DEFAULT_SAMPLING for RRR is 32 
		break;
		case SD:
		bs = new BitSequenceBuilderSDArray();
		break;
	}
	
	
	switch(opts->bb) {
		case RG:
		bb = new BitSequenceBuilderRG(20); // by default, 5% of extra space for bitmaps
		break;
		case R3:
		bb = new BitSequenceBuilderRRR(32); // DEFAULT_SAMPLING for RRR is 32 
		break;
		case SD:
		bb = new BitSequenceBuilderSDArray();
		break;
	}
	
	
	
	tgl.set_log(adjlog->log, adjlog->size_log, bs);
	
	tgl.set_time(adjlog->time, adjlog->size_time, bb);
	
}

int readopts(int argc, char **argv, struct opts *opts) {
	int o;
	
	
	// Default options
	opts->bs = RG;
	opts->bb = SD;
	
	while ((o = getopt(argc, argv, "b:t:")) != -1) {
		switch (o) {
			case 'b':
			if(strcmp(optarg, "RG")==0) {
				INFO("Using RG for wavelet matrix");
				opts->bs = RG;
			}
			else if(strcmp(optarg, "RRR")==0) {
				INFO("Using RRR for wavelet matrix");
				opts->bs = R3;
			}
			else if(strcmp(optarg, "SD")==0) {
				INFO("Using SDarray for wavelet matrix");
				opts->bs = SD;
			}
			break;
			
			case 't':
			if(strcmp(optarg, "RG")==0) {
				INFO("Using RG for bitmap B");
				opts->bb = RG;
			}
			else if(strcmp(optarg, "RRR")==0) {
				INFO("Using RRR for bitmap B");
				opts->bb = R3;
			}
			else if(strcmp(optarg, "SD")==0) {
				INFO("Using SDarray for bitmap B");
				opts->bb = SD;
			}
			break;
			
			
			default: /* '?' */
			break;
		}
	}
	
        if (optind >= argc || (argc-optind) < 1) {
		fprintf(stderr, "%s [-b RG,RRR,SD] [-t RG,RRR,SD] <outputfile> \n", argv[0]);
		fprintf(stderr, "Expected argument after options\n");
		exit(EXIT_FAILURE);
        }
	
	opts->outfile = argv[optind];
	
	return optind;

}


int main(int argc, char *argv[]) {
	struct adjlog tg;
	struct opts opts;
	int optind;
	
	TemporalGraphLog tgl;
	
	optind = readopts(argc, argv, &opts);
	
	INFO("Loading graph...");
	//read_stdin(&tg);
	read_contacts(&tg);
	
	INFO("Creating index...");
	create_index(tgl, &tg, &opts);
	LOG("Size of index: %lu\n", tgl.getSize());
	
	//LOG("Depth: %u", tgl.get_log()->getDepth());
	
	ofstream file;
	LOG("Saving graph file in '%s'\n", opts.outfile);
	file.open(opts.outfile, ios::binary);
	tgl.save(file);
	file.close();
	
	return 0;
}
