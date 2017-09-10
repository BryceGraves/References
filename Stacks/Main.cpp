//Bryce Graves
//Exercise 04: Using stacks to invert text into a readable form. Including palindrome input and check. Now works with both upper and lower case inputs.

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stack>

using namespace std;
int main(int argc, const char *argv[]) {
	ifstream myFile;
	stack<string> lines;
	stack<string> newLines;
	stack<char> p;
	string currentLine;
	string palindrome;

	myFile.open("story.txt");
	while (getline(myFile, currentLine)) {
		lines.push(currentLine);
	}
	myFile.close();
	swap(newLines, lines);

	for (int i = 0; i < lines.size(); ++i) {
		newLines.push(lines.top());
		lines.pop();
	}

	for (int i = 0; i < newLines.size(); ++i) {
		cout << newLines.top() << endl;
		newLines.pop();
	}

	cout << "Input Palindrome: ";
	cin >> palindrome;


	for (int i = 0; i < palindrome.size(); ++i) {
		p.push(palindrome[i]);
	}



	for (int i = 0; i < p.size(); ++i) {
		if (tolower(palindrome[i]) == tolower(p.top())) {
			p.pop();
			if (palindrome[p.size() - 1]) {
				cout << "This is a palindrome!" << endl;
				return 0;
			}
		} else {
			cout << "This isn't a palindrome!" << endl;
			return 0;
		}
	}

	return 0;
}
