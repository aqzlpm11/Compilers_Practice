#include "Trans.h"
#include "RegexTree.h"

#include <algorithm>
using namespace std;

// Nullable, Firstpos, Lastpos -- NFL

static int callNFL_idCnt;
static void callNFL(RegexTree::Node *root) {
	switch (root->type) {
		case RegexTree::NOTHING:
			root->nullable = true;
			root->firstpos.clear();
			root->lastpos.clear();
			break;

		case RegexTree::OR:
			callNFL(root->nxt[0]);
			callNFL(root->nxt[1]);

			root->nullable = root->nxt[0]->nullable || root->nxt[1]->nullable;

			// merge childs firstpos && lastpos
#define n0f (root->nxt[0]->firstpos)
#define n1f (root->nxt[1]->firstpos)
			root->firstpos.resize(n0f.size()+n1f.size());
			root->firstpos.resize( 
					set_union(n0f.begin(), n0f.end(), n1f.begin(), n1f.end(), root->firstpos.begin()) 
					-
					root->firstpos.begin()
					);
#undef n0f
#undef n1f

#define n0l (root->nxt[0]->lastpos)
#define n1l (root->nxt[1]->lastpos)
			root->lastpos.resize(n0l.size()+n1l.size());;
			root->lastpos.resize(
					set_union(n0l.begin(), n0l.end(), n1l.begin(), n1l.end(), root->lastpos.begin())
					-
					root->lastpos.begin()
					);
#undef n0l
#undef n1l
			break;

		case RegexTree::AND:
			callNFL(root->nxt[0]);
			callNFL(root->nxt[1]);

			root->nullable = root->nxt[0]->nullable && root->nxt[1]->nullable;

			if (root->nxt[0]->nullable == false) {
				root->firstpos = root->nxt[0]->firstpos;
			} else {
				root->firstpos = root->nxt[1]->firstpos;
			}

			if (root->nxt[1]->nullable == false) {
				root->lastpos = root->nxt[1]->lastpos; } else { root->lastpos = root->nxt[0]->lastpos;
			}
			break; 

		case RegexTree::STAR:
			callNFL(root->nxt[0]);

			root->nullable = root->nxt[0]->nullable;
			root->firstpos = root->nxt[0]->firstpos;
			root->lastpos = root->nxt[0]->lastpos;
			break;

		default:
			if (root->nxt.size() != 0) {
				fprintf(stderr, "ERROR(Trans::createDFAByTree): Detect Error at RegexTree when [callNFL(...)]");
				break;
			}

			root->nullable = false;
			root->firstpos.clear();
			root->lastpos.clear();

			root->firstpos.push_back(callNFL_idCnt);;
			root->lastpos.push_back(callNFL_idCnt);;

			callNFL_idCnt++;
			break; 
	}

}

void Trans::createDFAByTree(const RegexTree &tree, DFA &dfa_OUT) {
	callNFL_idCnt = 0;
	callNFL(tree.root);
}

