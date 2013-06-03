#ifndef TGL_H_
#define TGL_H_

#include <BitSequence.h>
#include "wavelet_kd_matrix.h"

#define BUFFER 1024*1024

class TemporalGraphLog {
public:
	TemporalGraphLog();
	TemporalGraphLog(uint nodes, uint changes, uint maxtime);
	
	void set_log(usym *log, uint size_log);
	void set_time(uint *time, uint size_time);
	
	void set_nodes(uint nodes);
	void set_changes(uint changes);
	void set_maxtime(uint maxtime);
	
	void save(ofstream &fp);
	static TemporalGraphLog* load(ifstream &fp);
	
	WaveletKdMatrix *get_log() {return this->log;};

	uint get_changes() {return this->changes;}
	size_t getSize() { /*return this->log->getSize() + this->time->getSize() + sizeof(TemporalGraphLog);*/ return 0;}
	size_t pos_time(size_t i) const;
	
	void direct_point(uint node, uint t, uint *res) const;
	void reverse_point(uint node, uint t, uint *res) const;
	
	
protected:
        uint nodes;
        uint changes;
        uint maxtime;
        WaveletKdMatrix *log;
        BitSequence *time;
        uint size_log;
        uint size_time;
		
		
		
		
		
};

#endif