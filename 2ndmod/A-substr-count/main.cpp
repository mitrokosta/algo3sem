#include <iostream>
#include <vector>

using std::swap;
using std::max;
using std::string;
using std::vector;

static const int ALPH_LEN = 26;

class SuffixArray {
  public:
    void BuildArr(const string& raw_text);
    void BuildLCP(); // build lcp using the stored suffix array
    vector<int> GetArr() const;
    vector<int> GetLCP() const;
  private:
    void FirstSort();
    void NextSort(int step);
    inline int Jump(int i, int len); // jump backwards

    vector<int> arr;
    vector<int> inverse;
    vector<int> new_arr;
    vector<int> lcp;
    vector<int> cls;
    vector<int> new_cls;
    vector<int> sort_counter;
    string text;
    int txt_sz;
};

long long SubstringsNumber(const string& text);

int main() {
  string text;
  std::cin >> text;
  std::cout << SubstringsNumber(text) << std::endl;
  return 0;
}

long long SubstringsNumber(const string& text) {
  SuffixArray sufarr;
  sufarr.BuildArr(text);
  sufarr.BuildLCP();
  long long sum = 0;
  for (int i : sufarr.GetArr()) {
    sum += text.size() - i;
  }
  for (int i : sufarr.GetLCP()) {
    sum -= i;
  }
  return sum;
}

int SuffixArray::Jump(int i, int len) {
  return (txt_sz + arr[i] - len) % txt_sz;
}

vector<int> SuffixArray::GetArr() const {
  return vector<int>(arr.begin(), arr.end());
}

vector<int> SuffixArray::GetLCP() const {
  return vector<int>(lcp.begin() + 1, lcp.end() - 1);
}

void SuffixArray::BuildArr(const string& raw_text) {
  if (raw_text.empty()) {
    arr.resize(0);
    return;
  }
  text = raw_text;
  text.push_back('$');
  txt_sz = text.size();
  cls.resize(txt_sz);
  new_cls.resize(txt_sz);
  arr.resize(txt_sz);
  new_arr.resize(txt_sz);
  sort_counter.resize(max(txt_sz, ALPH_LEN + 1));
  FirstSort();
  for (int step = 0; (1 << step) < txt_sz; ++step) {
    NextSort(step);
  }
}

void SuffixArray::FirstSort() {
  for (int i = 0; i < txt_sz; ++i) { // counting sort
    if (text[i] == '$') {
      ++sort_counter[0];
    } else {
      ++sort_counter[text[i] - 'a' + 1];
    }
  }
  sort_counter[txt_sz - 1] *= -1; // count offsets for each letter
  sort_counter[txt_sz - 1] += txt_sz;
  for (int i = txt_sz - 2; i >= 0; --i) {
    sort_counter[i] *= -1;
    sort_counter[i] += sort_counter[i + 1];
  }
  arr[0] = txt_sz - 1;
  for (int i = 0; i < txt_sz - 1; ++i) {
    arr[sort_counter[text[i] - 'a' + 1]] = i;
    ++sort_counter[text[i] - 'a' + 1];
  }
  cls[arr[0]] = 0;
  int cur_cl = 0;
  for (int i = 1; i < txt_sz; ++i) { // go through the suffix array
    if (text[arr[i - 1]] != text[arr[i]]) {
      ++cur_cl; // increment if two letters don't match
    }
    cls[arr[i]] = cur_cl;
  }
}

void SuffixArray::NextSort(int step) {
  int len = 1 << step;
  sort_counter.assign(max(txt_sz, ALPH_LEN + 1), 0);
  for (int i = 0; i < txt_sz; ++i) { // counting sort
    ++sort_counter[cls[Jump(i, len)]];
  }
  sort_counter[txt_sz - 1] *= -1; // count offsets for each class
  sort_counter[txt_sz - 1] += txt_sz;
  for (int i = txt_sz - 2; i >= 0; --i) {
    sort_counter[i] *= -1;
    sort_counter[i] += sort_counter[i + 1];
  }
  for (int i = 0; i < txt_sz; ++i) {
    new_arr[sort_counter[cls[Jump(i, len)]]] = Jump(i, len);
    ++sort_counter[cls[Jump(i, len)]];
  }
  swap(arr, new_arr); // arr <- new_arr, swapping to conserve time
  new_cls[arr[0]] = 0;
  int cur_cl = 0;
  for (int i = 1; i < txt_sz; ++i) { // go through the suffix array and count classes
    if (cls[arr[i - 1]] != cls[arr[i]] || cls[Jump(i, -len)] != cls[Jump(i - 1, -len)]) {
      ++cur_cl;
    }
    new_cls[arr[i]] = cur_cl;
  }
  swap(cls, new_cls);
}

void SuffixArray::BuildLCP() {
  inverse.resize(txt_sz);
  lcp.resize(txt_sz);
  for (int i = 0; i < txt_sz; ++i) {
    inverse[arr[i]] = i;
  }
  int curr_len = 0;
  for (int i = 0; i < txt_sz; ++i) { // go through the string
    if (curr_len) {
      --curr_len; // lcp[k] >= lcp[arr[inverse[k] + 1]] - 1
    }
    if (inverse[i] == txt_sz - 1) { // lcp with an empty suffix is 0
      curr_len = 0;
    } else {
      int nxt = arr[inverse[i] + 1]; // look at the next suffix in the suffix array
      while (i + curr_len < txt_sz && nxt + curr_len < txt_sz && text[i + curr_len] == text[nxt + curr_len]) {
        ++curr_len; // naive comparison
      }
      lcp[inverse[i]] = curr_len;
    }
  }
}
