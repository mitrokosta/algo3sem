#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

typedef unsigned int uint;

struct Searcher {
  vector<uint> pi;  // prefix function of the query
  uint step;
  uint prev;
};

vector<uint> PrefixFunc(const string& source);
Searcher* StartPrefQuery(const string& query);
uint NextPref(Searcher* srchandler, const string& text,
              const string& query);  // returns next prefix value
void EndPrefQuery(Searcher* srchandler);
vector<uint> Search(const string& text, const string& query);

int main() {
  string query, text;
  cin >> query >> text;
  for (size_t entry : Search(text, query)) {
    cout << entry << " ";
  }
  cout << endl;
  return 0;
}

vector<uint> PrefixFunc(const string& source) {
  vector<uint> pi;
  pi.reserve(source.size());
  pi.push_back(0);  // a prefix has to be shorter than the word
  for (int i = 1; i < static_cast<int>(source.size()); ++i) {
    uint curr = pi[i - 1];
    while (curr && source[i] != source[curr]) {
      curr = pi[curr - 1];  // looking for equal letters
    }
    if (source[i] == source[curr]) {
      ++curr;
    }
    pi.push_back(curr);
  }
  return pi;
}

Searcher* StartPrefQuery(const string& query) {
  Searcher* srchandler = new Searcher;
  srchandler->pi = PrefixFunc(query);
  srchandler->step = 0;
  srchandler->prev = 0;
  return srchandler;
}

uint NextPref(Searcher* srchandler, const string& text, const string& query) {
  if (srchandler->step == text.size()) {  // just in case
    throw;
  }
  uint curr = srchandler->prev;
  while (curr && text[srchandler->step] !=
                     query[curr]) {  // going back until chars match
    curr = srchandler->pi[curr - 1];
  }
  if (text[srchandler->step] == query[curr]) {
    ++curr;
  }
  ++(srchandler->step);
  srchandler->prev = curr;
  return curr;
}

void EndPrefQuery(Searcher* srchandler) {
  delete srchandler;
}

vector<uint> Search(const string& text, const string& query) {
  vector<uint> entries;
  string ext_query = query;
  ext_query += "$";
  Searcher* srchandler = StartPrefQuery(ext_query);
  uint pref;
  for (int i = 0; i < static_cast<int>(text.size()); ++i) {
    pref = NextPref(srchandler, text, ext_query);
    if (pref == query.size()) {  // entry found!
      entries.push_back(i - query.size() + 1);
    }
  }
  EndPrefQuery(srchandler);  // perform a cleanup
  return entries;
}
