#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

string PrefixFuncToString(const vector<unsigned>& pi);
char FirstGoodLetter(const string& result, int start_pos);

int main() {
  vector<unsigned> pi;
  unsigned input;
  while (cin >> input) {
    pi.push_back(input);
  }
  cout << PrefixFuncToString(pi) << endl;
  return 0;
}

char FirstGoodLetter(const vector<unsigned>& pi, const string& result, int start_pos) {
  char chr = 'a';
  while (true) { // looking for the first char that won't spoil the prefix func
    int j = start_pos;
    while (j && result[pi[j - 1]] != chr) {
      j = pi[j - 1];
    }
    if (j == 0) {
      break;
    } else {
      ++chr;
    }
  }
  return chr;
}

string PrefixFuncToString(const vector<unsigned>& pi) {
  char chr = 'a';
  string result;
  result.reserve(pi.size());
  result.push_back(chr);
  for (int i = 1; i < static_cast<int>(pi.size()); ++i) {
    if (pi[i] == 0) {
      chr = FirstGoodLetter(pi, result, i);
      result.push_back(chr);
    } else {
      result.push_back(result[pi[i] - 1]); // simply append the corresponding char
    }
  }
  return result;
}
