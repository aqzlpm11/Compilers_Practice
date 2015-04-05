#ifndef REGEX_TREE
#define REGEX_TREE

#include <cctype>
#include <cstdio>
#include <cstring>
#include <vector>

class RegexTree{
	public:
	friend class Trans;

	RegexTree();
	~RegexTree();

	enum {
		NOTHING = 0,
		STAR = 256,
		AND,
		OR
	};

	struct Node{
		int type;
		std::vector<Node *> nxt; 

		// for DFA
		bool nullable = false;
		std::vector<int> firstpos;
		std::vector<int> lastpos;

		std::vector<int> followpos;


		Node(int tp):type(tp){}
		~Node() {
			for (int i = 0; i < nxt.size(); i++) {
				delete nxt[i];
			}
		}

		void show() const {
			showTree(1);
		}

		void showTree(int tb) const {
			for (int i = 0; i < tb-1; i++) {
				printf("|  ");
			}
			printf("|--");

			// show firstPos
			printf(" { ");
			for (int i = 0; i < firstpos.size(); i++) {
				if (i != 0) printf(",");
				printf("%d", firstpos[i]);
			}
			printf(" } ");


			// show tag
			if (0 < type  && type < 256) { 
				printf("%c", type); 
			} else if (type == STAR) {
				printf("STAR");
			} else if (type == AND) {
				printf("AND"); 
			} else if (type == OR) {
				printf("OR");
			} else {
				puts("\nShow ERROR");
				return;
			}

			// show nullable
			if (nullable) printf("*");

			// show lastPos 
			printf(" { ");
			for (int i = 0; i < lastpos.size(); i++) {
				if (i != 0) printf(",");
				printf("%d", lastpos[i]);
			}
			printf(" } ");
			puts("");

			for (int i = 0; i <nxt.size(); i++) {
				nxt[i]->showTree(tb+1);
			}
		}

		void showParentheses() const {
			if (0 < type  && type < 256) { 
				printf("%c", type); 
			} else if (type == STAR) {
				printf("(");
				nxt[0]->showParentheses();
				printf(")*");
			} else if (type == AND) {
				printf("(");
				nxt[0]->showParentheses();
				nxt[1]->showParentheses();
				printf(")");
			} else if (type == OR) {
				printf("(");
				nxt[0]->showParentheses();
				printf("|");
				nxt[1]->showParentheses();
				printf(")");
			} else {
				puts("\nShow ERROR");
			}
		}

	};

	void build(const char *str);

	Node const* getRoot() const { 
		if (root == nullptr) 
			fprintf(stderr, "Warming: RegexTree::root == nullptr\n");
		return root; 
	}

	void show() const {
		if (isError) puts("Can't Show RegexTree: Syntax ERROR");
		else root->show();
	}

	private:
	inline Node *exp(); // exp ->  A | A | A ...
	inline Node *A();   // A   ->  BBBB ...
	inline Node *B();   // B   ->  C* [or]  C
	inline Node *C();   // C   ->  (exp)  [or]  {letter}  [or]  {number}  [or]  {null}

	void init(const char *buf);

	void match(char ch);
	void peekNext(bool skipSpace = false) ;
	char peek = ' ';

	void syntaxError();
	bool isError = false;

	Node *root = nullptr;

	// buffer of the regex
	const char *buffer = nullptr;
	int bufHead = 0;

};

#endif
