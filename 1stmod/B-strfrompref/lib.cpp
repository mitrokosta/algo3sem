#include "lib.hpp"

using std::vector;
using std::string;
using std::max;
using std::min;

char FirstGoodLetter(const string& result, int start_pos);

vector<unsigned> ZetFunc(const string& str) {
  int sz = str.size();
  vector<unsigned> zet(sz);
  unsigned left = 0;
  unsigned right = 0;
  for (int i = 1; i < sz; ++i) {
    zet[i] = max(0U, min(right - i, zet[i - left]));
    while (i + static_cast<int>(zet[i]) < sz && str[zet[i]] == str[i + zet[i]]) {
      ++zet[i]; // naive
    }
    if (i + zet[i] > right) { // refreshing left and right
      left = i;
      right = i + zet[i];
    }
  }
  return zet;
}

vector<unsigned> ZetToPref(const vector<unsigned>& zet) {
  int sz = zet.size();
  vector<unsigned> pi(sz);
  for (int i = 1; i < sz; ++i) {
    for (int j = zet[i] - 1; j >= 0; --j) {
      if (pi[i + j] > 0) {
        break;
      } else {
        pi[i + j] = j + 1;
      }
    }
  }
  return pi;
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

string PrefToString(const vector<unsigned>& pi) {
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
