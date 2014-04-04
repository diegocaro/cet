#ifndef TGL_H_
#define TGL_H_

#include <BitSequence.h>
#include "wavelet_kd_matrix.h"

#define BUFFER 1024*1024*10

class TemporalGraphLog {
public:
	TemporalGraphLog();
	TemporalGraphLog(uint nodes, uint changes, uint maxtime);
	
	void set_log(usym *log, size_t size_log, BitSequenceBuilder *bs);
	void set_time(uint *time, size_t size_time, BitSequenceBuilder *bs);
	
	void set_nodes(uint nodes);
	void set_changes(uint changes);
	void set_maxtime(uint maxtime);
	
	void save(ofstream &fp);
	static TemporalGraphLog* load(ifstream &fp);
	
	WaveletKdMatrix *get_log() {return this->log;};

	uint get_changes() {return this->changes;}
	size_t getSize() { 
    fprintf(stderr, "S size: %lu\n", this->log->getSize());
    fprintf(stderr, "B size: %lu\n", this->time->getSize());
    return this->log->getSize() + this->time->getSize() + sizeof(TemporalGraphLog);
  }
	size_t pos_time(size_t i) const;
	
	uint getMaxtime() { return maxtime;}
	
	size_t snapshot(uint t) const;
	
	void direct_point(uint node, uint t, uint *res) const;
	void direct_weak(uint node, uint tstart, uint tend, uint *res) const;
	void direct_strong(uint node, uint tstart, uint tend, uint *res) const;	
	
	void reverse_point(uint node, uint t, uint *res) const;
	void reverse_weak(uint node, uint tstart, uint tend, uint *res) const;
	void reverse_strong(uint node, uint tstart, uint tend, uint *res) const;
	
        
        int edge_point(uint u, uint v, uint t);
        int edge_weak(uint u, uint v, uint tstart, uint tend);
        int edge_strong(uint u, uint v, uint tstart, uint tend);
        
        int edge_next(uint u, uint v, uint t);
        
protected:
	
	void direct_interval(uint node, uint tstart, uint tend, uint semantic, uint *res) const;
	void reverse_interval(uint node, uint tstart, uint tend, uint semantic, uint *res) const;
	
        int edge_interval(uint u, uint v, uint tstart, uint tend, uint semantic);
        
        uint nodes;
        uint changes;
        uint maxtime;
        WaveletKdMatrix *log;
        BitSequence *time;

        size_t size_log;
        size_t size_time;
		
		
		
		
		
};

#endif
