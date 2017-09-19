#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <limits>
using namespace std;

struct trieNode {
	trieNode *children[26];
	bool leaf;
};

void print_h(trieNode* node, string& cur) {
	cout << cur << " " << (node->leaf ? 'y' : 'n') << " ";
	for(int i = 0; i < 26; ++i) {
		if(node->children[i]) cout << 1 << " ";
		else cout << 0 << " ";
	}
	cout << endl;
	for(int i = 0; i < 26; ++i) {
		if(node->children[i]) {
			cur.push_back('a'+i);
			print_h(node->children[i], cur);
			cur.pop_back();
		}
	}
}

void print(trieNode* root) {
	string word = "";
	print_h(root, word);
}

void insert(trieNode* root, string word) {
	if(!root) {
		root = new trieNode{{0}, 0};
	}
	trieNode* node = root;
	for(int i = 0; i < word.size(); ++i) {
		auto& next = node->children[word[i]-'a'];
		if(!next) {
			next = new trieNode{{0}, 0};
		}
		node = next;
	}
	node->leaf = true;
}

// -1: not in dictionary; 0: not a word yet; 1: full word;
int find(trieNode* root, string word) {
	trieNode* node = root;
	for(int i = 0; i < word.size(); ++i) {
		auto& next = node->children[word[i]-'a'];
		if(!next) return -1;
		node = next;
	}
	return node->leaf;
}

void fillDictionary(trieNode* root, ifstream& dict) {
	string word;
	while(dict.good()) {
		dict >> word;
		if(word.size() < 3) {
			continue;
		}
		bool goodToAdd = true;
		for(int i = 0; i < word.size(); ++i) {
			if(word[i] >= 'A' && word[i] <= 'Z') {
				goodToAdd = false;
				break;
			}
		}
		if(goodToAdd) {
			insert(root, word);
		}
	}
}

string getWord(trieNode* words, string& stem) {
	trieNode* itr = words;
	for(int i = 0; i < stem.size(); ++i) {
		itr = itr->children[stem[i]-'a'];
	}
	string completeWord = stem;
	while(!itr->leaf) {
		for(int i = 0; i < 26; ++i) {
			if(itr->children[i]) {
				completeWord.push_back('a'+i);
				itr = itr->children[i];
				break;
			}
		}
	}
	return completeWord;
}

void printGoodWords(trieNode* node, string& cur) {
	if(node->leaf) {
		cout << cur << endl;
		return;
	}
	for(int i = 0; i < 26; ++i) {
		if(node->children[i]) {
			cur.push_back('a'+i);
			printGoodWords(node->children[i], cur);
			cur.pop_back();
		}
	}
}

int solve(trieNode* node) {
	if(node->leaf) {
		return 0;
	}
	vector<int> wins(26, -1);
	bool win = 0;
	for(int i = 0; i < 26; ++i) {
		if(node->children[i]) {
			wins[i] = solve(node->children[i]);
			win |= wins[i];
		}
	}
	return win ? 0 : 1;
}

int main() {
	ifstream dict("words.txt");
	trieNode* words = new trieNode{{0}, 0};
	fillDictionary(words, dict);
	string cur = "";
	//printGoodWords(words, cur);
	int win = solve(words, cur);
	cout << win << endl;

	return 0;
}