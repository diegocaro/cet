/*
 * MyWaveletKdMatrix.cpp
 *
 *  Created on: Mar 26, 2014
 *      Author: diegocaro
 */

#include "mywtkdmatrix.h"
#include "debug.h"


template void MyWaveletKdMatrix::axis<append_odd>(size_t start, size_t end, uint axis, uint node, uint *res);
template void MyWaveletKdMatrix::_axis<append_odd>(size_t start, size_t end, uint axis, usym node, uint level, uint value, uint *res);

template void MyWaveletKdMatrix::axis<append_symbol>(size_t start, size_t end, uint axis, uint node, uint *res);
template void MyWaveletKdMatrix::_axis<append_symbol>(size_t start, size_t end, uint axis, usym node, uint level, uint value, uint *res);


template<action F>
void MyWaveletKdMatrix::axis(size_t start, size_t end, uint axis, uint node, uint *res) {
	usym n = {0, 0};
	if(axis == 0) {
		n.x = node;
	}
	else {
		n.y = node;
	}
	*res = 0;
	LOG("neighbors of {%u, %u}", n.x, n.y);
	_axis<F>(start, end, axis, n, 0, 0, res);
}

template<action F>
void MyWaveletKdMatrix::_axis(size_t start, size_t end, uint axis, usym node, uint level, uint value, uint *res) {
	LOG("level: %u", level);
	if (start >= end) {
//		printf("start >= end\n");
		return;
	}

	if (level == height) {
		INFO("nodo hoja\n");

		F(value, end-start, res);

		return;
	}

	size_t s0,e0,s1,e1;

	s1 = bitmap[level]->rank1(start-1);
	e1 = bitmap[level]->rank1(end-1);

	//printf("s1: %u\ne1: %u\n", s1,e1);

	s0 = start - s1;
	e0 = end - e1;
//	printf("s0: %u\ne0: %u\n", s0,e0);

	if ( level % 2 == axis) {

		// visitando la parte del nodo que me interesa :)
		if (is_set(node,level)) {
		//	printf("1");
			//bajar nodo derecho
			_axis<F>(s1 + Z[level], e1 + Z[level], axis, node, level+1, value, res);
		}
		else {
		//	printf("0");
			//bajar nodo izq
			_axis<F>(s0, e0, axis, node, level+1, value, res);
		}
	}
	else { // bajar por ambos!
		//printf("X");
		_axis<F>(s0, e0, axis, node, level+1, value, res);

		value = value | (1 << (height/2-level/2-1));
		_axis<F>(s1 + Z[level], e1 + Z[level], axis, node, level+1, value, res);
	}

	return;
}




void MyWaveletKdMatrix::rankall(size_t start, size_t end, size_t &res) {
	usym a = {0,0};

	//_range(start, end, lowvoc, uppvoc, 0, 0, 1u << height/2, 1u << height/2 , 0, res);
	_rankall(start, end, a, 0, res);
}

void MyWaveletKdMatrix::_rankall(size_t start, size_t end, usym symbol, uint level, size_t &res) {


//	printf("level: %u\n", level);
	if (start >= end) {
//		printf("start >= end\n");
		return;
	}

	if (height == level) {
//		printf("leaf node\n");
		if ((end-start)%2 == 1) {
			//printf("[%u, %u] -> %u\n", symbol.x, symbol.y, end -start);
			//pair<usym,uint> p(symbol, end-start);
			//res.push_back(p);
			res += 1;
		}
		return;
	}

	size_t s0,e0,s1,e1;

	s1 = bitmap[level]->rank1(start-1);
	e1 = bitmap[level]->rank1(end-1);

	//printf("s1: %u\ne1: %u\n", s1,e1);

	s0 = start - s1;
	e0 = end - e1;
//	printf("s0: %u\ne0: %u\n", s0,e0);

	if (level%2 == 0) {
		// value for x :)

		_rankall(s0, e0, symbol, level+1, res);

		symbol.x = symbol.x | (1 << (height/2-level/2-1));
		_rankall(s1 + Z[level], e1 + Z[level], symbol, level+1, res);
	}
	else {
		_rankall(s0, e0,  symbol, level+1, res);

		symbol.y = symbol.y | (1 << (height/2-level/2-1));
		_rankall(s1 + Z[level], e1 + Z[level], symbol, level+1, res);
	}

}

