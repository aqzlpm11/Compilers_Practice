#ifndef DFA_H
#define DFA_H

#define N 2000
#define SIZE 256
struct DFA{
	int next[N][SIZE];
	int fall[N];
	int val[N];
};

#endif
