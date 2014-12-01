#include "skiplist.h"
#include <string>
#include <iostream>
#include <assert.h>
using namespace std;

int main() {
	SkipList<int, string> sl;
	sl.Insert(1, "jiangsongrong");
	assert(sl.Exists(1));
	string val = sl.Get(1);
	assert(val == "jiangsongrong");
	sl.Remove(1);
	assert(sl.Exists(1)==false);

	
	for (int i = 0; i < 1000; i++) {
		sl.Insert(i, "data"+i);
	}
	return 0;
}