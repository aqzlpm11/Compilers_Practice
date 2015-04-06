#ifndef DFA_H
#define DFA_H

#define N 2000
#define SIZE 256

#include <cstdio>
#include <cstring>

class DFA{
	friend class Trans;

	// TODO getfall

	public:
	void clear()  {
		size = 0;
		memset(next, -1, sizeof next);
		memset(isFinal, false, sizeof isFinal);
	}

	void show() {
		printf("size = %d\n", size);
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < SIZE; j++) {
				if (next[i][j] != -1) {
					if (isFinal[i]) printf("[%d]", i);
					else printf(" %d " , i);
					
					printf(" --(%c)--> ",  j);

					if (isFinal[next[i][j]]) printf("[%d]", next[i][j]);
					else printf(" %d ", next[i][j]);
					puts("");
				}
			}
		}
	}

	void resetMatch() {
		nowMatchState = 0;
	}
	
	bool match(char ch) {
		if (next[nowMatchState][ch] != -1) {
			nowMatchState = next[nowMatchState][ch];
			return true;
		} else {
			return false;
		}
	}

	bool isAccept() {
		return isFinal[nowMatchState];
	}

	// match a string, return the length
	int match(char *str) {
		resetMatch();
		int res = -1;
		for (int i = 0; str[i]; i++) {
			if (match(str[i]) == false) break;
			if (isAccept()) res = i;
		}
		return res+1;
	}


	protected:
	int nowMatchState = 0;

	int next[N][SIZE];
	bool isFinal[N];
	int size = 0; };

#endif
