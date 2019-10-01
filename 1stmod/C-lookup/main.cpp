#include <iostream>
#include <vector>
#include <queue>

using std::vector;
using std::string;

static const int ALPH_LEN = 26;

struct CutString { // cuts a string into mask-less templates
  int beg_quest; // first non-mask position
  int end_quest; // last non-mask position
  vector<int> str_begins; // templates' characteristics
  vector<int> str_sizes;
  CutString(const string& query);
};

class Trie {
  public:
    Trie();
    ~Trie();
    void AddWord(const string& word, int index); // adds a word to the trie
    vector<int> Search(const string& query, const string& text); // performs a search
  private:
    void BFS(); // performs a BFS in the trie, establishes suflinks and termlinks
    vector<int> CountEntries(const string& text, const CutString& cs); // looks for all mask-less patterns and marks their positions
    struct Node {
      vector<Node*> next = vector<Node*>(ALPH_LEN, nullptr); // next to be visited vertex
      vector<Node*> offspring = vector<Node*>(ALPH_LEN, nullptr); // children of a vertex
      vector<int> patterns; // contains indexes of patterns that terminate in this vertex
      Node* parent = nullptr;
      Node* suflink = nullptr;
      Node* termlink = nullptr;
      bool is_term = false;
      ~Node();
    };
    Node* root;
};

int main() {
  string query, text;
  std::cin >> query >> text;
  Trie trie;
  for (int i : trie.Search(query, text)) {
    std::cout << i << " ";
  }
  std::cout << std::endl;
  return 0;
}

CutString::CutString(const string& query) {
  beg_quest = 0;
  while (beg_quest < static_cast<int>(query.size()) && query[beg_quest] == '?') {
    ++beg_quest;
  }
  end_quest = query.size() - 1;
  while (end_quest >= 0 && query[end_quest] == '?') {
    --end_quest;
  }
  int sz = 0;
  for (int i = 0; i < static_cast<int>(query.size()); ++i) {
    if (query[i] != '?') {
      ++sz;
    } else if (sz > 0) {
      str_begins.push_back(i - sz - beg_quest);
      str_sizes.push_back(sz);
      sz = 0;
    }
  }
  if (sz) {
    str_begins.push_back(query.size() - sz - beg_quest);
    str_sizes.push_back(sz);
    sz = 0;
  }
}

vector<int> Trie::Search(const string& query, const string& text) {
  int txtsz = text.size();
  int qrsz = query.size();
  vector<int> results;
  CutString cs = CutString(query);
  int pts_num = cs.str_begins.size();
  if (txtsz < qrsz) {
    return results; // empty vector
  }
  if (cs.beg_quest == qrsz) {
    for (int i = 0; i < txtsz - qrsz + 1; ++i) {
      results.push_back(i);
    }
    return results;
  }
  for (int i = 0; i < pts_num; ++i) {
    AddWord(query.substr(cs.str_begins[i] + cs.beg_quest, cs.str_sizes[i]), i);
  }
  BFS();
  vector<int> entries = CountEntries(text, cs);
  for (int i = 0; i < static_cast<int>(entries.size()) && i - cs.beg_quest + query.size() <= text.size(); ++i) {
    if (entries[i] == pts_num) { // if all patterns were found at a position, mark it as found
      results.push_back(i - cs.beg_quest);
    }
  }
  return results;
}

void Trie::BFS() {
  std::queue<Node*> bfsqueue;
  bfsqueue.push(root);
  while (!bfsqueue.empty()) {
    Node* top = bfsqueue.front();
    bfsqueue.pop();
    top->suflink = root;

    for (int i = 0; i < ALPH_LEN; ++i) { // assign suflinks
      if (top->parent != nullptr && top->parent != root && top->parent->offspring[i] == top) {
        top->suflink = top->parent->suflink->next[i];
      }
    }

    for (int i = 0; i < ALPH_LEN; ++i) { // assign next to be visited array
      if (top == root && top->offspring[i] == nullptr) {
        top->next[i] = root;
      } else if (top->offspring[i] != nullptr) {
        top->next[i] = top->offspring[i];
      } else {
        top->next[i] = top->suflink->next[i];
      }
      if (top->offspring[i] != nullptr) {
        bfsqueue.push(top->offspring[i]);
      }
    }

    if (top->suflink->is_term) { // assign termlinks
      top->termlink = top->suflink;
    } else if (top->suflink != root) {
      top->termlink = top->suflink->termlink;
    }
  }
}

vector<int> Trie::CountEntries(const string& text, const CutString& cs) {
  vector<int> entries(text.size());
  Node* curr = root;
  for (int i = 0; i < static_cast<int>(text.size()); ++i) {
    curr = curr->next[text[i] - 'a'];
    if (curr->is_term) { // if a terminal vertex is reached, count the local patterns
      for (int local_pat : curr->patterns) {
        if (i - cs.str_begins[local_pat] - cs.str_sizes[local_pat] + 1 >= cs.beg_quest) {
          ++entries[i - cs.str_begins[local_pat] - cs.str_sizes[local_pat] + 1];
        }
      }
    }
    Node* tmp = curr; // also try to find terminal vertices by using termlinks
    tmp = curr->termlink;
    while (tmp != nullptr) {
      for (int local_pat : tmp->patterns) {
        if (i - cs.str_begins[local_pat] - cs.str_sizes[local_pat] + 1 >= cs.beg_quest) {
          ++entries[i - cs.str_begins[local_pat] - cs.str_sizes[local_pat] + 1];
        }
      }
      tmp = tmp->termlink;
    }
  }
  return entries;
}

Trie::Trie() :
    root(new Node) {

}

void Trie::AddWord(const string& word, int index) {
  Node* curr = root;
  for (int i = 0; i < static_cast<int>(word.size()); ++i) {
    if (curr->offspring[word[i] - 'a'] == nullptr) {
      curr->offspring[word[i] - 'a'] = new Node;
      curr->offspring[word[i] - 'a']->parent = curr;
    }
    curr = curr->offspring[word[i] - 'a'];
  }
  curr->is_term = true;
  curr->patterns.push_back(index);
}

Trie::~Trie() {
  delete root;
}

Trie::Node::~Node() {
  for (int i = 0; i < ALPH_LEN; ++i) {
    delete offspring[i];
  }
}
