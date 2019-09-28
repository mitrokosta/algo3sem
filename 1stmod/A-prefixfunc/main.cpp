#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

class Searcher {
  public:
    Searcher(const string& query);
    vector<unsigned> Search(const string& text);

  private:
    void PrefixFunc();
    unsigned NextPref(const string& text);

    string ext_query;
    vector<unsigned> pi;  // prefix function of the query
    unsigned step;
    unsigned prev;
};

int main() {
  string query, text;
  cin >> query >> text;
  Searcher srchandler = Searcher(query);
  for (size_t entry : srchandler.Search(text)) {
    cout << entry << " ";
  }
  cout << endl;
  return 0;
}

Searcher::Searcher(const string& query) :
    ext_query(query + "$"),
    pi(),
    step(0),
    prev(0) {
  PrefixFunc();
}

void Searcher::PrefixFunc() {
  pi.reserve(ext_query.size());
  pi.push_back(0);  // a prefix has to be shorter than the word
  for (int i = 1; i < static_cast<int>(ext_query.size()); ++i) {
    unsigned curr = pi[i - 1];
    while (curr && ext_query[i] != ext_query[curr]) {
      curr = pi[curr - 1];  // looking for equal letters
    }
    if (ext_query[i] == ext_query[curr]) {
      ++curr;
    }
    pi.push_back(curr);
  }
}

unsigned Searcher::NextPref(const string& text) {
  unsigned curr = prev;
  while (curr && text[step] != ext_query[curr]) {  // going back until chars match
    curr = pi[curr - 1];
  }
  if (text[step] == ext_query[curr]) {
    ++curr;
  }
  ++(step);
  prev = curr;
  return curr;
}

vector<unsigned> Searcher::Search(const string& text) {
  vector<unsigned> entries;
  unsigned pref;
  for (int i = 0; i < static_cast<int>(text.size()); ++i) {
    pref = NextPref(text);
    if (pref == ext_query.size() - 1) {  // entry found!
      entries.push_back(i - ext_query.size() + 2);
    }
  }
  step = prev = 0; // cleanup
  return entries;
}
