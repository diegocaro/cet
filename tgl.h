#ifndef TGL_H_
#define TGL_H_

#include <BitSequence.h>
#include "wavelet_tree.h"

class TemporalGraphLog {
public:
	TemporalGraphLog();
	TemporalGraphLog(uint nodes, uint changes, uint maxtime);
	
	void set_log(u_long *log, uint size_log);
	void set_time(uint *time, uint size_time);
	
	void set_nodes(uint nodes);
	void set_changes(uint changes);
	void set_maxtime(uint maxtime);
	
	void save(ofstream &fp);
	static TemporalGraphLog* load(ifstream &fp);
	
	WaveletTree *get_log() {return this->log;};

	uint get_changes() {return this->changes;}
	size_t getSize() { return this->log->getSize() + this->time->getSize() + sizeof(TemporalGraphLog);}
	size_t pos_time(size_t i) const;
	
protected:
        uint nodes;
        uint changes;
        uint maxtime;
        WaveletTree *log;
        BitSequence *time;
        uint size_log;
        uint size_time;
		
		
		
		
		
};

#endif