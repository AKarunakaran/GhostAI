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

char move(trieNode* words, string& playWord, int players) {
	trieNode* itr = words;
	for(int i = 0; i < playWord.size(); ++i) {
		itr = itr->children[playWord[i]-'a'];
	}
	vector<int> wins(26, -1);
	char bestLetter = '-';
	for(int i = 0; i < 26; ++i) {
		if(itr->children[i]) {
			if(bestLetter == '-') {
				bestLetter = 'a'+i;
			}
			wins[i] = solve(itr->children[i]);
			if(wins[i]) {
				bestLetter = 'a'+i;
				break;
			}
		}
	}
	return bestLetter;
}

void play(trieNode* words) {
	srand(time(NULL));
	int turn, players;
	cout << "How many players are there? ";
	cin >> players;
	cout << "What number in the rotation am I? (1-" << players << "): ";
	cin >> turn;
	turn = (players+1-turn) % players;
	string playWord = "";
	while(find(words, playWord) == 0) {
		char c;
		if(turn == 0) {
			c = move(words, playWord, players);
			cout << "I'll play " << c << "." << endl;
		} else {
			cout << "The current stem is: " << playWord << endl << "What's your next letter? ";
			cin >> c;
			if(c == '?') {
				if(turn == 1) {
					cout << "Challenge? I was thinking of the word " << getWord(words, playWord) << ". I know pretty much every\nword, so I don't know what you were" <<
					" thinking with the challenge there, dude. You\nlose." << endl;
				}
				return;
			}
		}
		playWord.push_back(c);
		turn = (turn+1) % players;
	}
	if(find(words, playWord) == -1) {
		if(turn == 0) {
			cout << "Challenge. What word do you think it is?" << endl << "(Enter the word you were spelling): ";
			string challengeWord;
			cin >> challengeWord;
			if(challengeWord.size() < playWord.size()) {
				cout << "Alright bud. That doesn't even make sense given what we've played. Congratulations, you're an idiot. I win." << endl;
				return;
			} else {
				bool goodChallenge = true;
				for(int i = 0; i < playWord.size(); ++i) {
					if(playWord[i] != challengeWord[i]) {
						goodChallenge = false;
						break;
					}
				}
				if(!goodChallenge) {
					cout << "Alright bud. That doesn't even make sense given what we've played. Congratulations, you're an idiot. I win." << endl;
				}
			}
			if(find(words, playWord) == 1) {
				cout << "Well shit. Something must've gone wrong, but you win. Good job." << endl;
			} else {
				cout << "Ha. Not a word. I win." << endl;
			}
		} else if(turn == 1) {
			cout << "Well something definitely fucked up, because this was not supposed to happen. I guess you win." << endl;
		} else {
			cout << "Good job. One of you idiots spelled a made up word. I'm out." << endl;
		}
	} else {
		if(turn == 0) {
			int winResponse = rand() % 4;
			switch(winResponse) {
				case 0:
					cout << "Yeah, that's a word. Sorry to break it to you. I win." << endl;
					break;
				case 1:
					cout << "That's a word. I would say better luck next time, but I'm going to beat you regardless of luck." << endl;
					break;
				case 2:
					cout << "Mm, " << playWord << " sounds like a word to me. So you lose. It happens. I mean, not to me.\nBut it happens." << endl;
					break;
				case 3:
					cout << "You know the object of the game is NOT to spell a word, right? You did? You must not be very smart, cause that's a word. I win." << endl;
					break;
				default:
					break;
			}
		} else if(turn == 1) {
			int loseResponse = rand() % 2;
			switch(loseResponse) {
				case 0:
					cout << "Wow. You won. Congratulations, you must actually be an enormous fucking nerd." << endl;
					break;
				case 1:
					cout << "Well, you won. Knowing you, you probably cheated. Congrats?" << endl;
					break;
				default:
					cout << "You somehow broke the code and got this message. You may have noticed" << endl
					<< "I only hard-coded two losing responses for the computer. That's because this thing never loses." << endl;
					break;
			}
		} else {
			cout << "Alright, one of you chumps spelled a word. We're done here." << endl;
		}
	}
}

int main() {
	ifstream dict("words.txt");
	trieNode* words = new trieNode{{0}, 0};
	fillDictionary(words, dict);
	play(words);

	return 0;
}