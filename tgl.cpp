#include "tgl.h"
#include "arraysort.h"

//#include "interleave.h"

TemporalGraphLog::TemporalGraphLog() {};

TemporalGraphLog::TemporalGraphLog(uint nodes, uint changes, uint maxtime) {
	this->nodes = nodes;
	this->changes = changes;
	this->maxtime = maxtime;
}

void TemporalGraphLog::set_nodes(uint nodes) {this->nodes = nodes;}
void TemporalGraphLog::set_changes(uint changes) {this->changes = changes;}
void TemporalGraphLog::set_maxtime(uint maxtime) {this->maxtime = maxtime;}


void TemporalGraphLog::set_log(usym *log, uint size_log, BitSequenceBuilder *bs) {
	this->size_log = size_log;
	/*this->log = new WaveletTree(log, size_log,
			new wt_coder_binary(log, size_log, new MapperNone()),
			new BitSequenceBuilderRG(20),
			new MapperNone());
  */
	/*
  this->log = new WaveletTree(log, size_log,
  			new wt_coder_huff_morton(log, size_log),
  			new BitSequenceBuilderRG(20));
	*/
	this->log = new WaveletKdMatrix(log, size_log, bs);
  
}

void TemporalGraphLog::set_time(uint *time, uint size_time, BitSequenceBuilder *bs) {
	this->size_time = size_time;
	this->time = bs->build(time, size_time);
}

void TemporalGraphLog::save(ofstream &fp) {
	saveValue<TemporalGraphLog>(fp, this, 1);
	this->log->save(fp);
	this->time->save(fp);
}

TemporalGraphLog* TemporalGraphLog::load(ifstream &fp) {
	TemporalGraphLog *ret;
	//ret = new TemporalGraphLog();
	ret = loadValue<TemporalGraphLog>(fp,1);
	//printf("changes: %u\n", ret->get_changes());
	ret->log = WaveletKdMatrix::load(fp);
	ret->time = BitSequence::load(fp);
	return ret;
}

size_t TemporalGraphLog::pos_time(size_t i) const {
//	return b->select1(i+1) - i; 	//in the paper this operation is
                                  //start(i) = select1(b, i) - i + 1

  uint ret = time->select1(i+1);
  //printf("ret: %u\n", ret);
  if (ret == (uint)(-1)) {
    return time->getLength() - i;
  }
  else {
    return ret - i;
  }
}

void TemporalGraphLog::direct_point(uint node, uint t, uint *res) const {
	size_t ptime;
	ptime = pos_time(t);
	
	vector<uint> ans;
	
	log->axis(0U, ptime, 0U, node, ans);
	
	
	uint j = 0;
	for(uint i = 0; i < ans.size(); i+=2) {
		if (ans[i+1]%2 == 1) {
			res[++j] = ans[i];
		}
	}
	*res = j;
}

void TemporalGraphLog::direct_interval(uint node, uint tstart, uint tend, uint semantic, uint *res) const {
	uint *buffer;

	buffer = new uint[BUFFER];
	
	
	size_t sptime;
	size_t eptime;
	sptime = pos_time(tstart);
	eptime = pos_time(tend);
	
	
	vector<uint> rng;
	log->axis(sptime, eptime, 0U, node, rng);

	uint j;
	uint i;
	j = 0;
	for (i = 0; i < rng.size(); i += 2) {
		buffer[++j] = rng[i];
	}
	*buffer = j;
	
	*res = 0;
	direct_point(node, tstart, res);
	
	if (semantic == 0) { //semantic weak
		j = *res;
		for (i = 1; i <= *buffer; i++) {
			res[++j] = buffer[i];
		}
		*res = j;

		qsort(&res[1], *res, sizeof(unsigned int), compare);

		remove_duplicates(res);
	}
	else if (semantic == 1) { //semantic strong

		//printf("direct neighbors: "); print_arraysort(buffer3);
		diff_arraysort(res, buffer);
	}
	
	
	delete [] buffer;
}

void TemporalGraphLog::direct_weak(uint node, uint tstart, uint tend, uint *res) const {
	direct_interval(node, tstart, tend, 0, res);
}

void TemporalGraphLog::direct_strong(uint node, uint tstart, uint tend, uint *res) const {
	direct_interval(node, tstart, tend, 1, res);
}


void TemporalGraphLog::reverse_point(uint node, uint t, uint *res) const {
	size_t ptime;
	ptime = pos_time(t);
	
	vector<uint> ans;
	log->axis(0U, ptime, 1U, node, ans);
	
	
	
	uint j = 0;
	for(uint i = 0; i < ans.size(); i+=2) {
		if (ans[i+1]%2 == 1) {
			res[++j] = ans[i];
		}
	}
	*res = j;
	
}


void TemporalGraphLog::reverse_interval(uint node, uint tstart, uint tend, uint semantic, uint *res) const {
	uint *buffer;

	buffer = new uint[BUFFER];
	
	
	size_t sptime;
	size_t eptime;
	sptime = pos_time(tstart);
	eptime = pos_time(tend);
	
	
	vector<uint> rng;
	log->axis(sptime, eptime, 1U, node, rng);

	uint j;
	uint i;
	j = 0;
	for (i = 0; i < rng.size(); i += 2) {
		buffer[++j] = rng[i];
	}
	*buffer = j;
	
	*res = 0;
	reverse_point(node, tstart, res);
	
	if (semantic == 0) { //semantic weak
		j = *res;
		for (i = 1; i <= *buffer; i++) {
			res[++j] = buffer[i];
		}
		*res = j;

		qsort(&res[1], *res, sizeof(unsigned int), compare);

		remove_duplicates(res);
	}
	else if (semantic == 1) { //semantic strong

		//printf("direct neighbors: "); print_arraysort(buffer3);
		diff_arraysort(res, buffer);
	}
	
	
	delete [] buffer;
}

void TemporalGraphLog::reverse_weak(uint node, uint tstart, uint tend, uint *res) const {
	reverse_interval(node, tstart, tend, 0, res);
}

void TemporalGraphLog::reverse_strong(uint node, uint tstart, uint tend, uint *res) const {
	reverse_interval(node, tstart, tend, 1, res);
}


uint TemporalGraphLog::snapshot(uint t) const {
	size_t etime;
	etime = pos_time(t);
	
	vector<pair<usym,uint> > ans;
	
	log->rankall(0, etime,ans);

	
	return etime;
}


int TemporalGraphLog::edge_point(uint u, uint v, uint t) {
	size_t ptime;
	ptime = pos_time(t);
        
        struct symbols s;
        s.x=u;
        s.y=v;
        
        if (log->rank(s, ptime) % 2 == 1) {
                return 1;
        }
        return 0;
}

int TemporalGraphLog::edge_interval(uint u, uint v, uint tstart, uint tend, uint semantic) {
	size_t sptime;
        size_t eptime;
	sptime = pos_time(tstart);
        eptime = pos_time(tend);
        
        struct symbols s;
        s.x=u;
        s.y=v;
        
        uint rstart;
        uint rend;
        
        rstart = log->rank(s, sptime);
        rend = log->rank(s, eptime);
        
        if (semantic == 0) {
                if ( ( rstart % 2 == 1) || (rend > rstart) ) 
                        return 1;
        }
        else if (semantic == 1) {
                if ( ( rstart % 2 == 1) && (rend == rstart) ) 
                        return 1;
        }
        
        return 0;
        
}

int TemporalGraphLog::edge_weak(uint u, uint v, uint tstart, uint tend) {
        return edge_interval(u, v, tstart, tend, 0);
}
int TemporalGraphLog::edge_strong(uint u, uint v, uint tstart, uint tend){
        return edge_interval(u, v, tstart, tend, 1);
}

int TemporalGraphLog::edge_next(uint u, uint v, uint t) {
	size_t ptime;
	ptime = pos_time(t);
        
        struct symbols s;
        s.x=u;
        s.y=v;
        
        uint r;
        r = log->rank(s, ptime);
        
        if (r % 2 == 1) {
                return t;
        }
        
        uint l;
        l = log->select(s, r+1);
        if ( l != log->n ) {
                return time->rank1(l);
        }
        
        return -1;
}