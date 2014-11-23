#include <stdio.h>
#include <map>
#include <queue>
using namespace std;

typedef short StepInfo; // step + last move
typedef int   MapStat;
typedef int   Map;
typedef short Grid;
typedef short Dir;

#define LEFT  0
#define RIGHT 1
#define UP    2
#define DOWN  3
#define WHITE 0
#define RED   1
#define BLUE  2

#define MAKESTEP(step, lastmove) (((step)<<2)+lastmove)
#define STEP(info) ((info)>>2)
#define LASTMOVE(info) (info&0b11)

#define MAKEMAPSTAT(amap, step) ((amap<<6)+step)

queue<MapStat> qu;  // compressed map and step number
map<Map, StepInfo> hashmap; // Map -> (step number & last move)
Map dest = 0;

Map toMap(Grid* grids) {
	Map init = 0;
	for (int i = 0; i < 16; i++) {
		Grid grid = grids[i];
		if (grid == WHITE) { //2bits
			init <<= 2;   //2bits
			init += 0b11;
		} else if (grid == RED) { //14bits
			init <<= 2;   // 2bits
			init += 0b10;
		} else { //8bits
			init <<= 1; // 1bit
		}
	}
	return init;
}

Grid* toGrid(Map amap) {
	Grid* grids = new Grid[16];
	bool in = false;
	int white;
	int j = 0;
	for (int i = 23; i >= 0; i--) {
		int bit = (1<<i)&amap;
		if (in) {
			if (bit == 0) { //11 => white
				grids[j++] = RED;
			} else {
				white = j;
				grids[j++] = WHITE;
			}
			in = false;
		} else {
			if (bit == 0) {
				grids[j++] = BLUE;
			} else {
				in = true;
			}
		}
	}
	return grids;
}

void init() {
	Map init = 0;
	Grid srcMap[16] = {
		WHITE, RED, BLUE, BLUE,
		RED,   RED, BLUE, BLUE,
		RED,   RED, BLUE, BLUE,
		RED,   RED, BLUE, BLUE
	};
	init = toMap((Grid*)srcMap);
	hashmap[init] = MAKESTEP(0, 0);
	init = MAKEMAPSTAT(init, 0);
	qu.push(init);

	Grid destMap[16] = {
		WHITE, BLUE, RED, BLUE,
		BLUE,  RED,  BLUE,RED,
		RED,   BLUE, RED, BLUE,
		BLUE,  RED,  BLUE,RED
	};
	dest = toMap((Grid*)destMap);
}

Map move(Map curMap, Dir dir) {
	Grid* grids;
	bool in = false;
	int j = 0;
	int white = 0;
	grids = toGrid(curMap);

	for (int i = 0; i < 16; i++) {
		if (grids[i] == WHITE) {
			white = i;
		}
	}
	int whitex = white % 4;
	int whitey = white / 4;
	switch(dir) {
		case LEFT:
			whitex--;
			break;
		case RIGHT:
			whitex++;
			break;
		case UP:
			whitey--;
			break;
		case DOWN:
			whitey++;
			break;
	}

	if (whitex < 0 || whitex > 3 || whitey < 0 || whitey > 3) {
		return -1;
	}
	Grid tmp = grids[whitey*4+whitex];
	grids[whitey*4+whitex] = grids[white];
	grids[white] = tmp;

	return toMap(grids);
}

Dir reverse(Dir dir) {
	switch (dir) {
		case LEFT:
			return RIGHT;
		case RIGHT:
			return LEFT;
		case UP:
			return DOWN;
		case DOWN:
			return UP;
	}
	return 0;
}

void printAns(Map finalMap) {
	StepInfo info = hashmap[finalMap];
	short step = STEP(info);

	if (step == 0) {
		return;
	}

	Dir lastMove = LASTMOVE(info);
	Dir reverseDir = reverse(lastMove);

	printAns(move(finalMap, reverseDir));
	switch (lastMove) {
		case LEFT:
			printf("L");
			break;
		case RIGHT:
			printf("R");
			break;
		case UP:
			printf("U");
			break;
		case DOWN:
			printf("D");
			break;
	}
}

void bfs() {
	Dir dirs[4] = {
		UP, RIGHT, DOWN, LEFT
	};
	while (!qu.empty()) {
		MapStat curStat = qu.front();
		qu.pop();

		Map curMap = curStat>>6;
		if (curMap == dest) {
			printAns(curMap);
			return;
		}

		// Get the current step.
		StepInfo curInfo = hashmap[curMap];
		short step = curStat&0b111111;	

		for (int i = 0; i < 4; i++) {
			Map next = move(curMap, dirs[i]);
			map<Map, StepInfo>::iterator it;
			bool go = false;
			if (next > 0) {
				StepInfo bestInfo;
				if ((it = hashmap.find(next)) == hashmap.end()) {
					go = true;
				} else {
					bestInfo = it->second;
					short bestStep = STEP(curInfo);
					if (step < bestStep) {
						// need to go
						go = true;
					}
				}
				if (go) {
					hashmap[next] = MAKESTEP(step+1, dirs[i]);
					qu.push(MAKEMAPSTAT(next, step+1));
				}
			}
		}
	}
}

int main(void) {
	init();
	bfs();
	return 0;
}
