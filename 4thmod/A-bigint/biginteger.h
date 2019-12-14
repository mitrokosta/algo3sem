#include <iostream>
#include <vector>
#include <string>

using std::ostream;
using std::istream;
using std::string;
using std::vector;
using std::max;
using std::swap;

typedef unsigned long ArrElement;
static const ArrElement BASE = 0xFFFFFFFF;
static const int NEG_MASK = 0x80000000;

class BigInteger {
  public:
    BigInteger() = default;
    BigInteger(int num);
    BigInteger(unsigned long num);
    BigInteger& operator+=(const BigInteger& other);
    friend BigInteger operator+(BigInteger lhs, const BigInteger& rhs);
    BigInteger& operator*=(const BigInteger& other);
    friend BigInteger operator*(BigInteger lhs, const BigInteger& rhs);
    BigInteger& operator-=(const BigInteger& other);
    friend BigInteger operator-(BigInteger lhs, const BigInteger& rhs);
    BigInteger& operator/=(const BigInteger& other);
    friend BigInteger operator/(BigInteger lhs, const BigInteger& rhs);
    BigInteger& operator%=(const BigInteger& other);
    friend BigInteger operator%(BigInteger lhs, const BigInteger& rhs);
    friend BigInteger operator-(BigInteger num);
    BigInteger& operator++();
    BigInteger operator++(int);
    BigInteger& operator--();
    BigInteger operator--(int);
    explicit operator bool() const;
    string toString() const;
    bool operator==(const BigInteger& other) const;
    bool operator!=(const BigInteger& other) const;
    bool operator<(const BigInteger& other) const;
    bool operator>(const BigInteger& other) const;
    bool operator<=(const BigInteger& other) const;
    bool operator>=(const BigInteger& other) const;
    friend istream& operator>>(istream& in, BigInteger& num);

  private:
    void Add(const BigInteger& other); // Сумирует два числа по модулю, считая *this больше или равным other
    void Subtract(const BigInteger& other); // Вычитает из *this other по модулю, считая *this больше или равным other
    void Split(BigInteger& high, BigInteger& low) const; // Разбивает число на две равные по числу разрядов части
    void Shift(int shift); // Выполняет сдвиг числа на произвольное число разрядов
    void Trim(); // Обрезает ведущие нули числа
    ArrElement Divide(const BigInteger& other) const; // Возвращает частное от деления *this на other, полагая что оно укладывается в BASE
    bool Less(const BigInteger& other) const; // Сравнивает *this и other по модулю
    bool LessOrEqual(const BigInteger& other) const;

    vector<ArrElement> arr;
    int size;
    bool negative;
};

ostream& operator<<(ostream& out, const BigInteger& num);

BigInteger::BigInteger(int num) :
    arr(1),
    size(1),
    negative(num & NEG_MASK) {
  if (negative) {
    num *= -1;
  }
  arr[0] = num;
}

BigInteger::BigInteger(unsigned long num) :
    arr(1),
    size(1),
    negative(false) {
  arr[0] = num;
}

void BigInteger::Add(const BigInteger& other) {
  bool carry = false;
  unsigned long long temp;
  for (int i = 0; i < other.size; ++i) {
    temp = arr[i];
    temp += other.arr[i];
    temp += carry;
    arr[i] = temp % BASE;
    carry = temp / BASE;
  }
  for (int i = other.size; i < size; ++i) {
    temp = arr[i];
    temp += carry;
    arr[i] = temp % BASE;
    carry = temp / BASE;
  }
  if (carry) {
    arr.push_back(carry);
    ++size;
  }
}

void BigInteger::Subtract(const BigInteger& other) {
  bool carry = false;
  for (int i = 0; i < other.size; ++i) {
    if (carry) {
      if (arr[i]) {
        --arr[i];
        carry = false;
      } else {
        arr[i] = -1ll + BASE;
      }
    }
    if (arr[i] < other.arr[i]) {
      carry = true;
      arr[i] += static_cast<long long>(BASE) - other.arr[i];
    } else {
      arr[i] -= other.arr[i];
    }
  }
  if (carry) {
    int i = other.size;
    while (i < size && !arr[i]) {
      ++i;
    }
    if (i < size) {
      --arr[i];
      if (i == size - 1 && !arr[i]) {
        while (i >= 0 && !arr[i]) {
          --i;
        }
        size = i + 1;
        if (!size) {
          size = 1;
        }
        arr.resize(size);
      }
    } else {
      arr.resize(1);
      arr[0] = 0;
      size = 1;
    }
  }
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
  if (negative == other.negative) {
    if (Less(other)) {
      BigInteger temp = other;
      temp.Add(*this);
      arr = move(temp.arr);
      size = temp.size;
    } else {
      Add(other);
    }
  } else {
    if (Less(other)) {
      BigInteger temp = other;
      temp.Subtract(*this);
      arr = move(temp.arr);
      size = temp.size;
      negative = other.negative;
    } else {
      Subtract(other);
    }
    Trim();
    if (size == 1 && arr[0] == 0) {
      negative = false;
    }
  }
  return *this;
}

BigInteger operator+(BigInteger lhs, const BigInteger& rhs) {
  lhs += rhs;
  return lhs;
}

BigInteger& BigInteger::operator-=(const BigInteger& other) {
  BigInteger temp = -1 * other;
  *this += temp;
  return *this;
}

BigInteger operator-(BigInteger lhs, const BigInteger& rhs) {
  lhs -= rhs;
  return lhs;
}

void BigInteger::Split(BigInteger& high, BigInteger& low) const {
  low.arr.resize(0);
  high.arr.resize(0);
  for (int i = 0; i < size / 2; ++i) {
    low.arr.push_back(arr[i]);
  }
  for (int i = size / 2; i < size; ++i) {
    high.arr.push_back(arr[i]);
  }
  low.size = low.arr.size();
  high.size = high.arr.size();
  low.negative = high.negative = false;
}

void BigInteger::Shift(int shift) {
  if (*this == 0) {
    return;
  }
  arr.insert(arr.begin(), shift, 0);
  size += shift;
}

void BigInteger::Trim() {
  int i = size - 1;
  while (i >= 0 && arr[i] == 0) {
    --i;
  }
  if (i == -1) {
    size = 1;
  } else {
    size = i + 1;
  }
  arr.resize(size);
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
  if (other.negative) {
    negative = !negative;
  }
  if (size == 1) {
    unsigned long long carry = 0;
    unsigned long long orig = arr[0];
    if (orig > 0) {
      unsigned long long mul = orig * other.arr[0];
      carry = mul / BASE;
      arr[0] = mul % BASE;
      for (int i = 1; i < other.size; ++i) {
        mul = orig * other.arr[i] + carry;
        carry = mul / BASE;
        arr.push_back(mul % BASE);
        ++size;
      }
      if (carry) {
        arr.push_back(carry);
        ++size;
      }
    } else {
      negative = false;
    }
  } else if (other.size == 1) {
    if (other.arr[0] == 0) {
      arr.resize(1);
      arr[0] = 0;
      size = 1;
      negative = false;
    } else {
      unsigned long long orig = other.arr[0];
      unsigned long long carry = 0;
      unsigned long long mul;
      for (int i = 0; i < size; ++i) {
        mul = orig * arr[i] + carry;
        carry = mul / BASE;
        arr[i] = mul % BASE;
      }
      if (carry) {
        arr.push_back(carry);
        ++size;
      }
    }
  } else {
    int mod_size = ((max(size, other.size) - 1) / 2 + 1) * 2;
    BigInteger mod_other = other;
    mod_other.arr.resize(mod_size);
    arr.resize(mod_size);
    mod_other.size = size = mod_size;
    BigInteger high;
    BigInteger low;
    Split(high, low);
    BigInteger other_high;
    BigInteger other_low;
    mod_other.Split(other_high, other_low);
    high.Trim();
    low.Trim();
    other_high.Trim();
    other_low.Trim();
    BigInteger x1y1 = high * other_high;
    BigInteger x2y2 = low * other_low;
    BigInteger comp = (high + low) * (other_high + other_low) - x1y1 - x2y2;
    x1y1.Shift(mod_size);
    comp.Shift(mod_size / 2);
    comp += x1y1;
    comp += x2y2;
    arr = move(comp.arr);
    size = comp.size;
    Trim();
  }
  return *this;
}

BigInteger operator*(BigInteger lhs, const BigInteger& rhs) {
  lhs *= rhs;
  return lhs;
}

ArrElement BigInteger::Divide(const BigInteger& other) const {
  ArrElement low = 0;
  ArrElement high = BASE;
  while (low < high - 1) {
    ArrElement mid = (static_cast<unsigned long long>(low) + high) / 2;
    BigInteger temp = other * mid;
    if (Less(temp)) {
      high = mid;
    } else {
      low = mid;
    }
  }
  return low;
}

BigInteger& BigInteger::operator/=(const BigInteger& other) {
  if (Less(other)) {
    arr.resize(1);
    size = 1;
    arr[0] = 0;
    negative = false;
  } else {
    if (other.negative) {
      negative = !negative;
    }
    BigInteger curr = arr.back();
    vector<ArrElement> result;
    for (int i = size - 2; i >= 0; --i) {
      if (other.LessOrEqual(curr)) {
        ArrElement div = curr.Divide(other);
        result.push_back(div);
        curr.Subtract(other * div);
        curr.Trim();
      }
      curr.Shift(1);
      curr.arr[0] = arr[i];
    }
    if (other.LessOrEqual(curr)) {
      ArrElement div = curr.Divide(other);
      result.push_back(div);
    }
    size = result.size();
    arr.resize(size);
    for (int i = 0; i < size; ++i) {
      arr[i] = result[size - i - 1];
    }
  }
  return *this;
}

BigInteger operator/(BigInteger lhs, const BigInteger& rhs) {
  lhs /= rhs;
  return lhs;
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
  *this -= (other * (*this / other));
  return *this;
}

BigInteger operator%(BigInteger lhs, const BigInteger& rhs) {
  lhs %= rhs;
  return lhs;
}

bool BigInteger::operator==(const BigInteger& other) const {
  if (size != other.size || negative != other.negative) {
    return false;
  } else {
    for (int i = 0; i < size; ++i) {
      if (arr[i] != other.arr[i]) {
        return false;
      }
    }
  }
  return true;
}

bool BigInteger::operator!=(const BigInteger& other) const {
  return !(*this == other);
}

bool BigInteger::Less(const BigInteger& other) const {
  if (size < other.size) {
    return true;
  } else if (size > other.size) {
    return false;
  } else {
    for (int i = size - 1; i >= 0; --i) {
      if (arr[i] < other.arr[i]) {
        return true;
      } else if (other.arr[i] < arr[i]) {
        return false;
      }
    }
    return false;
  }
}

bool BigInteger::LessOrEqual(const BigInteger& other) const {
  if (size < other.size) {
    return true;
  } else if (size > other.size) {
    return false;
  } else {
    for (int i = size - 1; i >= 0; --i) {
      if (arr[i] < other.arr[i]) {
        return true;
      } else if (other.arr[i] < arr[i]) {
        return false;
      }
    }
    return true;
  }
}

bool BigInteger::operator<(const BigInteger& other) const {
  if (negative) {
    if (!other.negative) {
      return true;
    } else {
      return other.Less(*this);
    }
  } else {
    if (other.negative) {
      return false;
    } else {
      return Less(other);
    }
  }
}

bool BigInteger::operator>(const BigInteger& other) const {
  return (other < *this);
}

bool BigInteger::operator<=(const BigInteger& other) const {
  return !(other < *this);
}

bool BigInteger::operator>=(const BigInteger& other) const {
  return !(*this < other);
}

BigInteger operator-(BigInteger num) {
  if (num) {
    num.negative = !num.negative;
  }
  return num;
}

BigInteger& BigInteger::operator++() {
  operator+=(1);
  return *this;
}

BigInteger BigInteger::operator++(int) {
  BigInteger cpy(*this);
  operator++();
  return cpy;
}

BigInteger& BigInteger::operator--() {
  operator-=(1);
  return *this;
}

BigInteger BigInteger::operator--(int) {
  BigInteger cpy(*this);
  operator--();
  return cpy;
}

BigInteger::operator bool() const {
  if (*this == 0) {
    return false;
  } else {
    return true;
  }
}

string BigInteger::toString() const {
  BigInteger cpy(*this);
  BigInteger remainder;
  string repr = "";
  if (cpy.negative) {
    cpy.negative = false;
  }
  while (cpy) {
    remainder = cpy % 10;
    repr.push_back('0' + remainder.arr[0]);
    cpy /= 10;
  }
  if (negative) {
    repr.push_back('-');
  }
  for (int i = 0; i < static_cast<int>(repr.size()) / 2; ++i) {
    swap(repr[i], repr[repr.size() - i - 1]);
  }
  if (repr.empty()) {
    repr.push_back('0');
  }
  return repr;
}

ostream& operator<<(ostream& out, const BigInteger& num) {
  out << num.toString();
  return out;
}

istream& operator>>(istream& in, BigInteger& num) {
  string strnum;
  int temp;
  num = 0;
  if (in >> strnum && !strnum.empty()) {
    int pos = 0;
    if (strnum[0] == '-') {
      ++pos;
    }
    while (pos < static_cast<int>(strnum.size())) {
      temp = strnum[pos] - '0';
      num *= 10;
      num += temp;
      ++pos;
    }
    if (strnum[0] == '-' && num) {
      num.negative = true;
    }
  }
  return in;
}
