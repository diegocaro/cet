#include "tgl.h"
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


void TemporalGraphLog::set_log(usym *log, uint size_log) {
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
	this->log = new WaveletKdMatrix(log, size_log, new BitSequenceBuilderRG(20));
  
}

void TemporalGraphLog::set_time(uint *time, uint size_time) {
	this->size_time = size_time;
	this->time = new BitSequenceRG(time, size_time, 20);
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
	printf("changes: %u\n", ret->get_changes());
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


