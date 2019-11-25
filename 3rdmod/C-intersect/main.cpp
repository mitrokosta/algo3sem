#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

using std::cin;
using std::cout;
using std::istream;
using std::ostream;
using std::vector;

static const double EPS = std::numeric_limits<double>::epsilon();

class Vector;

class Point {
  public:
    Point();
    Point(double x, double y);
    double GetX() const;
    double GetY() const;
    Point& operator+=(const Vector& vec);
    friend bool operator==(const Point& lhs, const Point& rhs);
    friend Point operator+(Point point, const Vector& vec);
    friend istream& operator>>(istream& in, Point& point);
    friend ostream& operator<<(ostream& out, const Point& point);
  private:
    double x;
    double y;
};

class Vector {
  public:
    Vector();
    Vector(double x, double y);
    Vector(const Point& begin, const Point& end);
    explicit Vector(const Point& point);
    double GetX() const;
    double GetY() const;
    double GetLength() const;
    double GetPolarAngle() const;
    Vector& operator*=(double mod);
    friend Vector operator*(Vector vec, double mod);
    friend Vector operator*(double mod, const Vector& vec);
    Vector& operator+=(const Vector& other);
    friend Vector operator+(Vector lhs, const Vector& rhs);
    Vector& operator-=(const Vector& other);
    friend Vector operator-(Vector lhs, const Vector& rhs);
    friend istream& operator>>(istream& in, Vector& vec);
    friend ostream& operator<<(ostream& out, const Vector& vec);
  private:
    double x;
    double y;
};

bool ComparePoints(const Point& lhs, const Point& rhs); // для поиска левой нижней точки
void SetPrepare(vector<Point>& set); // разворот в порядке против часовой, поиск левой нижней точки
double DotProduct(const Vector& first_vec, const Vector& second_vec); // скалярное произведение
vector<Point> MinkSum(vector<Point> first_set, vector<Point> second_set);
bool IsInSet(const Point& p, const vector<Point>& set);
double VectorComp(const Vector& first_vec, const Vector& second_vec); // векторное произведение

int main() {
  int first_size;
  int second_size;
  cin >> first_size;
  vector<Point> first_set(first_size);
  for (int i = 0; i < first_size; ++i) {
    cin >> first_set[i];
  }
  cin >> second_size;
  vector<Point> second_set(second_size);
  for (int i = 0; i < second_size; ++i) {
    cin >> second_set[i];
    second_set[i] = Point() + -1 * Vector(second_set[i]);
  }
  vector<Point> mink_sum = MinkSum(first_set, second_set);
  cout << (IsInSet(Point(), mink_sum) ? "YES\n" : "NO\n");
  return 0;
}

double VectorComp(const Vector& first_vec, const Vector& second_vec) {
  return first_vec.GetX() * second_vec.GetY() - first_vec.GetY() * second_vec.GetX();
}

bool IsInSet(const Point& p, const vector<Point>& set) {
  int size = set.size();
  for (int i = 0; i < size; ++i) {
    if (VectorComp(Vector(set[i], set[(i + 1) % size]), Vector(set[i], Point())) < -EPS) {
      return false;
    }
  }
  return true;
}

bool ComparePoints(const Point& lhs, const Point& rhs) {
  return lhs.GetY() < rhs.GetY() || (lhs.GetY() == rhs.GetY() && lhs.GetX() < rhs.GetX());
}

void SetPrepare(vector<Point>& set) {
  reverse(set.begin(), set.end());
  auto beg_point = min_element(set.begin(), set.end(), ComparePoints);
  rotate(set.begin(), beg_point, set.end());
}

vector<Point> MinkSum(vector<Point> first_set, vector<Point> second_set) {
  vector<Point> result;
  SetPrepare(first_set);
  SetPrepare(second_set);
  int first_ptr = 0; // указатель на элемент первого множества
  int second_ptr = 0; // указатель на элемент второго множества
  int first_size = first_set.size();
  int second_size = second_set.size();
  while (first_ptr < first_size || second_ptr < second_size) { // цикл, создающий сумму
    Vector first_vec(first_set[first_ptr % first_size], first_set[(first_ptr + 1) % first_size]);
    Vector second_vec(second_set[second_ptr % second_size], second_set[(second_ptr + 1) % second_size]);
    result.push_back(Point() + Vector(first_set[first_ptr % first_size]) + Vector(second_set[second_ptr % second_size]));
    if (second_ptr == second_size || (first_ptr < first_size && VectorComp(first_vec, second_vec) > EPS)) { // выбор указателя, который нужно сдвинуть
      ++first_ptr;
    } else {
      ++second_ptr;
    }
  }
  return result;
}

double DotProduct(const Vector& first_vec, const Vector& second_vec) {
  return first_vec.GetX() * second_vec.GetX() +
         first_vec.GetY() * second_vec.GetY();
}

Point::Point() :
    x(0),
    y(0) {

}

Point::Point(double x, double y) :
    x(x),
    y(y) {

}

double Point::GetX() const {
  return x;
}

double Point::GetY() const {
  return y;
}

bool operator==(const Point& lhs, const Point& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

Point& Point::operator+=(const Vector& vec) {
  x += vec.GetX();
  y += vec.GetY();
  return *this;
}

Point operator+(Point point, const Vector& vec) {
  point += vec;
  return point;
}

istream& operator>>(istream& in, Point& point) {
  in >> point.x >> point.y;
  return in;
}

ostream& operator<<(ostream& out, const Point& point) {
  out << point.x << " " << point.y;
  return out;
}

Vector::Vector() :
    x(0),
    y(0) {

}

Vector::Vector(double x, double y) :
    x(x),
    y(y) {

}

Vector::Vector(const Point& begin, const Point& end) :
    x(end.GetX() - begin.GetX()),
    y(end.GetY() - begin.GetY()) {

}

Vector::Vector(const Point& point) :
    Vector(Point(), point) {

}

double Vector::GetX() const {
  return x;
}

double Vector::GetY() const {
  return y;
}

double Vector::GetLength() const {
  return sqrt(x * x + y * y);
}

double Vector::GetPolarAngle() const {
  double ang = atan2(y, x);
  if (ang < -EPS) {
    ang += 2 * M_PI;
  }
  return ang;
}

Vector& Vector::operator*=(double mod) {
  x *= mod;
  y *= mod;
  return *this;
}

Vector operator*(Vector vec, double mod) {
  vec *= mod;
  return vec;
}

Vector operator*(double mod, const Vector& vec) {
  return vec * mod;
}

Vector& Vector::operator+=(const Vector& other) {
  x += other.x;
  y += other.y;
  return *this;
}

Vector operator+(Vector lhs, const Vector& rhs) {
  lhs += rhs;
  return lhs;
}

Vector& Vector::operator-=(const Vector& other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

Vector operator-(Vector lhs, const Vector& rhs) {
  lhs -= rhs;
  return lhs;
}

istream& operator>>(istream& in, Vector& vec) {
  in >> vec.x >> vec.y;
  return in;
}

ostream& operator<<(ostream& out, const Vector& vec) {
  out << vec.x << " " << vec.y;
  return out;
}
