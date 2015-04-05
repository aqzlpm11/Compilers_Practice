#include "Trans.h"
#include "RegexTree.h"

#include <cassert>
#include <algorithm>
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

// Nullable Firstpos Lastpos Followpos
static int callNFLF_idCnt;
static vector<RegexTree::Node *> id2node;

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
	callNFLF(tree.root);

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
}

