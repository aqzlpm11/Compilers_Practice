#include "Trans.h"
#include "RegexTree.h"

#include <cassert>
#include <algorithm>
#include <vector>
#include <stack>
#include <queue>
#include <map>
using namespace std;

template <class T>
static void vector_unin(vector<T> &OUT, const vector<T> &a, const vector<T> &b){
	assert(&OUT != &a && &OUT != &b);

	OUT.resize(a.size()+b.size());
	OUT.resize(
			set_union(a.begin(), a.end(), b.begin(), b.end(), OUT.begin())
			-
			OUT.begin()
			);
}

// Nullable Firstpos Lastpos Followpos static int callNFLF_idCnt;
static vector<RegexTree::Node *> id2node;
static int callNFLF_idCnt;

static void init_callNFLF() {
	callNFLF_idCnt = 1;
	id2node.clear();
	id2node.push_back(nullptr);
}

static void callNFLF(RegexTree::Node *root) {
	root->followpos.clear();

	// callNFL
	switch (root->type) {
		case RegexTree::NOTHING:
			root->nullable = true;
			root->firstpos.clear();
			root->lastpos.clear();
			break;

		case RegexTree::OR:
			callNFLF(root->nxt[0]);
			callNFLF(root->nxt[1]);

			// nullable
			root->nullable = root->nxt[0]->nullable || root->nxt[1]->nullable;

			// merge childs firstpos && lastpos
			vector_unin(root->firstpos, root->nxt[0]->firstpos, root->nxt[1]->firstpos);
			vector_unin(root->lastpos,  root->nxt[0]->lastpos,  root->nxt[1]->lastpos);

			break;

		case RegexTree::AND:
			callNFLF(root->nxt[0]);
			callNFLF(root->nxt[1]);

			// nullable
			root->nullable = root->nxt[0]->nullable && root->nxt[1]->nullable;

			// firstpos
			if (root->nxt[0]->nullable == false) {
				root->firstpos = root->nxt[0]->firstpos;
			} else {
				vector_unin(root->firstpos, root->nxt[0]->firstpos, root->nxt[1]->firstpos);
			}

			// lastpos
			if (root->nxt[1]->nullable == false) {
				root->lastpos = root->nxt[1]->lastpos; 
			} else { 
				vector_unin(root->lastpos, root->nxt[0]->lastpos, root->nxt[1]->lastpos);
			}

			break; 

		case RegexTree::STAR:
			callNFLF(root->nxt[0]);

			// nullable
			root->nullable = true;
			
			// firstpos && lastpos
			root->firstpos = root->nxt[0]->firstpos;
			root->lastpos = root->nxt[0]->lastpos;
			break;

		default:
			if (root->nxt.size() != 0) {
				fprintf(stderr, "ERROR(Trans::createDFAByTree): Detect Error at RegexTree when [callNFLF(...)]");
				break;
			}

			// nullable
			root->nullable = false;

			// firstpos
			root->firstpos.clear();
			root->firstpos.push_back(callNFLF_idCnt);;

			// lastpos
			root->lastpos.clear();
			root->lastpos.push_back(callNFLF_idCnt);;

			// restore point
			id2node.push_back(root);

			// idCnt++
			callNFLF_idCnt++;
			break; 
	}

	// call followpos
	switch (root->type) {
		case RegexTree::AND:
			{
				auto firstpos = root->nxt[1]->firstpos;
				for (unsigned i = 0; i < root->nxt[0]->lastpos.size(); i++) {
					auto nowptr = id2node[ root->nxt[0]->lastpos[i] ];

					// add to back
					for (unsigned j = 0; j < firstpos.size(); j++) {
						nowptr->followpos.push_back(firstpos[j]);
					}

					// unique
					sort(nowptr->followpos.begin(), nowptr->followpos.end());
					nowptr->followpos.resize( unique(nowptr->followpos.begin(), nowptr->followpos.end()) - nowptr->followpos.begin() );
				}
			}
			break;
		case RegexTree::STAR:
			{
				auto firstpos = root->nxt[0]->firstpos;
				for (unsigned i = 0; i < root->nxt[0]->lastpos.size(); i++) {
					auto nowptr = id2node[ root->nxt[0]->lastpos[i] ];

					// add to back
					for (unsigned j = 0; j < firstpos.size(); j++) {
						nowptr->followpos.push_back(firstpos[j]);
					}

					// unique
					sort(nowptr->followpos.begin(), nowptr->followpos.end());
					nowptr->followpos.resize( unique(nowptr->followpos.begin(), nowptr->followpos.end()) - nowptr->followpos.begin() );
				}
			}
			break;
	}
}

void Trans::createDFAByTree(const RegexTree &tree, DFA &dfa_OUT) {
	init_callNFLF();

	RegexTree::Node sharpNode('#');

	RegexTree::Node sharpAndNode(RegexTree::AND);
	sharpAndNode.nxt.push_back(tree.root);
	sharpAndNode.nxt.push_back(&sharpNode);

	callNFLF(&sharpAndNode);


	puts("--- followpos ---");
	for (int i = 1; i < id2node.size(); i++) {
		printf("%d\t", i);
		printf(" { ");
		for (int j = 0; j < id2node[i]->followpos.size(); j++) {
			if (j != 0) printf(", ");
			printf("%d", id2node[i]->followpos[j]);
		}
		printf(" } \n");
	}


	dfa_OUT.clear();

	map< vector<int>, int> stateId;
	queue< vector<int> > que;
	int idtot = 0;

	que.push(sharpAndNode.firstpos);
	stateId[sharpAndNode.firstpos] = idtot++;
	while (!que.empty()) {
		auto followNodes = que.front();
		que.pop();

		// 如果包含 '#' 结点， 则这个状态是接受状态
		if (followNodes.back() == callNFLF_idCnt-1) {
			dfa_OUT.isFinal[ stateId[followNodes] ] = true;
		}

		for (int ch = 0; ch < 255; ch++) {
			vector<int> nextFollows;
			for (int i = 0; i < followNodes.size(); i++) {
				if (id2node[ followNodes[i] ]->type == ch) {
					vector<int> tmp = nextFollows;
					vector_unin(nextFollows, tmp, id2node[ followNodes[i] ]->followpos);
				}
			}

			if (!nextFollows.empty()) {
				if (stateId.find(nextFollows) == stateId.end()) {
					stateId[nextFollows] = idtot++;
					que.push(nextFollows);
				}

				dfa_OUT.next[ stateId[followNodes] ][ch] = stateId[nextFollows];
			}
		}
	}

	dfa_OUT.size = idtot;






























//	stack<vector<int> > Dstates;
//	Dstates.push(sharpAndNode.firstpos);
//
//	map<vector<int>, int> vis;
//
//	dfa_OUT.clear();
//
//	int dfaIds = 0;
//	vis[Dstates.top()] = dfaIds++;
//	while (!Dstates.empty()) {
//		auto S = Dstates.top();
//		Dstates.pop();
//
//
//		for (int i = 0; i < 255; i++) {
//			vector<int> U;
//			for (int j = 0; j < S.size(); j++) {
//				if (id2node[ S[j] ]->type == i) {
//					vector<int> tmp = U;
//					vector_unin(U, tmp, id2node[ S[j] ]->followpos);
//				}
//			}
//
//			if (U.size()) {
//				if (vis.find(U) == vis.end()) {
//					vis[U] = dfaIds++;
//					Dstates.push(U);
//				}
//
//				dfa_OUT.next[vis[S]][i] = vis[U];
//			}
//		}
//	}
//
//	dfa_OUT.size = dfaIds;

	sharpAndNode.nxt.clear(); // important: Don't delete it's children  when it release
}
