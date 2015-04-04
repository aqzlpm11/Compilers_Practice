#include <cstdio>
#include <cstring>
#include <cctype>
#include <string>
#include <vector>
using namespace std;

class RegexTree{
	public:
	enum {
		STAR = 256,
		AND,
		OR
	};

	struct Node{
		Node(int tp):type(tp){}
		~Node() {
			for (int i = 0; i < nxt.size(); i++) {
				delete nxt[i];
			}
		}
		void show() {
			if (0 <= type  && type < 256) {
				printf("%c", type);
			} else if (type == STAR) {
				printf("(");
				nxt[0]->show();
				printf(")*");
			} else if (type == AND) {
				printf("(");
				nxt[0]->show();
				nxt[1]->show();
				printf(")");
			} else if (type == OR) {
				printf("(");
				nxt[0]->show();
				printf("|");
				nxt[1]->show();
				printf(")");
			} else {
				puts("\nShow ERROR");
			}
		}
		int type;
		vector<Node *> nxt;
	};

	Node *root;

	RegexTree():root(nullptr){}
	~RegexTree(){
		delete root;
		root = nullptr;
	}

	inline Node *A() {
		Node *l = B();

		while (true) {
			Node *r = B();
			if (r == nullptr) break; 

			Node *now = new Node(AND);
			now->nxt.push_back(l);
			now->nxt.push_back(r);

			l = now;
		}

		return l;
	}

	inline Node *B(){
		Node *l = C();
		if (l != nullptr && peek == '*') {
			match('*');

			Node *now = new Node(STAR);
			now->nxt.push_back(l);

			l = now;
		}
		return l;
	}

	inline Node *C(){
		Node *l = nullptr;
		if (peek == '(') {
			match('(');
			l = exp();
			match(')');
		} else if (isalnum(peek)) {
			l = new Node(peek);

			match(peek);
		}

		return l;
	}

	inline Node *exp() {
		Node *l = A();
		while (l && peek == '|') {
			match('|');

			Node *now = new Node(OR);
			now->nxt.push_back(l);
			now->nxt.push_back(A());

			l = now;
		}
		return l;
	}

	void match(char ch) {
		if (peek != ch) {
			printf("expect %c, find '%c'\n", ch, peek);
			return; 
		} 
		peekNext(); 
	}
	void peekNext(bool skipSpace = false) {
		while (isspace(peek = getchar()) && skipSpace);
	}

	inline void exec() {
		peekNext(true);

		delete root;
		root = exp();

		if (peek != '\n') {
			puts("error");
			while (getchar() != '\n');
			peek = '\n';
		} else {
			root->show();
			puts("");
		}
	}

	char peek;

}tree;

int main() {
	while (true){
		tree.exec();
	}

	return 0;
}
