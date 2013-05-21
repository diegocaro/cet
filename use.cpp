#include "tgl.h"
#include "debug.h"

int main(int argc, char *argv[]) {
	ifstream file;
	TemporalGraphLog *tg;
	
	LOG("Loading temporalgraph in file '%s'", argv[1]);
	file.open(argv[1], ios::binary);
	tg = TemporalGraphLog::load(file);
	file.close();
	
	return 0;
}