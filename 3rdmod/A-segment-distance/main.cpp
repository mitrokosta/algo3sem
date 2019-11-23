#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>

using std::istream;
using std::ostream;
using std::min;

static const double MAX_COORD = 1000;
static const double EPS = std::numeric_limits<double>::epsilon();

class Point {
  public:
    Point();
    Point(double x, double y, double z);
    double GetX() const;
    double GetY() const;
    double GetZ() const;
    friend istream& operator>>(istream& in, Point& point);
    friend ostream& operator<<(ostream& out, const Point& point);
  private:
    double x;
    double y;
    double z;
};

class Segment {
  public:
    Segment();
    Segment(const Point& begin, const Point& end);
    const Point& GetBegin() const;
    const Point& GetEnd() const;
    double GetLength() const;
  private:
    Point begin;
    Point end;
};

class Vector {
  public:
    Vector();
    Vector(double x, double y, double z);
    Vector(const Point& begin, const Point& end);
    explicit Vector(const Point& point);
    explicit Vector(const Segment& seg);
    double GetX() const;
    double GetY() const;
    double GetZ() const;
    double GetLength() const;
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
    double z;
};

double DotProduct(const Vector& first_vec, const Vector& second_vec);

double SystemSolve(const Vector& r01, const Vector& a, const Vector& r02, const Vector& b);
void CheckBoundaryTs(const Segment& first_seg, const Segment& second_seg, double& curr_min);
void UpdateIfSecondSegIsNotAPointAndTIsOne(const Vector& r02, const Vector& r11, const Vector& b, double t2, double& curr_min);
void UpdateIfSecondSegIsNotAPointAndTIsZero(const Vector& r01, const Vector& r02, const Vector& b, double t2, double& curr_min);

double SegmentDistance(const Segment& first_seg, const Segment& second_seg);

int main() {
  Point a0, a1, b0, b1;
  std::cin >> a0 >> a1 >> b0 >> b1;
  std::cout << std::fixed << std::setprecision(8) << SegmentDistance(Segment(a0, a1), Segment(b0, b1)) << std::endl;
  return 0;
}

double DotProduct(const Vector& first_vec, const Vector& second_vec) {
  return first_vec.GetX() * second_vec.GetX() +
         first_vec.GetY() * second_vec.GetY() +
         first_vec.GetZ() * second_vec.GetZ();
}

double SystemSolve(const Vector& r01, const Vector& a, const Vector& r02, const Vector& b) {
  double main_det = DotProduct(a, a) * DotProduct(b, b) - DotProduct(a, b) * DotProduct(a, b);
  if (main_det) {
    double first_det = -DotProduct(b, b) * DotProduct(a, r01 - r02) + DotProduct(a, b) * DotProduct(b, r01 - r02);
    double second_det = DotProduct(a, a) * DotProduct(b, r01 - r02) - DotProduct(a, b) * DotProduct(a, r01 - r02);
    double t1 = first_det / main_det;
    double t2 = second_det / main_det;
    if (t1 - 1 <= EPS && t2 - 1 <= EPS && t1 >= -EPS && t2 >= -EPS) {
      Vector dr = r01 - r02 + a * t1 - b * t2;
      return dr.GetLength();
    }
  }
  return -1;
}

void UpdateIfSecondSegIsNotAPointAndTIsOne(const Vector& r02, const Vector& r11, const Vector& b, double t2, double& curr_min) {
  Vector dr = r11 - r02 - t2 * b;
  curr_min = min(curr_min, dr.GetLength());
}

void UpdateIfSecondSegIsNotAPointAndTIsZero(const Vector& r01, const Vector& r02, const Vector& b, double t2, double& curr_min) {
  Vector dr = r01 - r02 - t2 * b;
  curr_min = min(curr_min, dr.GetLength());
}

void CheckBoundaryTs(const Segment& first_seg, const Segment& second_seg, double& curr_min) {
  Vector r01 = Vector(first_seg.GetBegin()); // радиус-вектор начала первого отрезка
  Vector r02 = Vector(second_seg.GetBegin());
  Vector r11 = Vector(first_seg.GetEnd()); // радиус-вектор конца первого отрезка
  Vector b = Vector(second_seg); // второй направляющий вектор
  double t2;
  t2 = DotProduct(r11 - r02, b) / DotProduct(b, b);
  if (t2 - 1 <= EPS && t2 >= -EPS) {
    UpdateIfSecondSegIsNotAPointAndTIsOne(r02, r11, b, t2, curr_min);
  }
  UpdateIfSecondSegIsNotAPointAndTIsOne(r02, r11, b, 0, curr_min);
  UpdateIfSecondSegIsNotAPointAndTIsOne(r02, r11, b, 1, curr_min);
  t2 = DotProduct(r01 - r02, b) / DotProduct(b, b);
  if (t2 - 1<= EPS && t2 >= -EPS) {
    UpdateIfSecondSegIsNotAPointAndTIsZero(r01, r02, b, t2, curr_min);
  }
  UpdateIfSecondSegIsNotAPointAndTIsZero(r01, r02, b, 0, curr_min);
  UpdateIfSecondSegIsNotAPointAndTIsZero(r01, r02, b, 1, curr_min);
}

double SegmentDistance(const Segment& first_seg, const Segment& second_seg) {
  const double max_vec_len = 2 * Vector(MAX_COORD, MAX_COORD, MAX_COORD).GetLength();
  Vector r01 = Vector(first_seg.GetBegin()); // радиус-вектор начала первого отрезка
  Vector r02 = Vector(second_seg.GetBegin());
  Vector a = Vector(first_seg); // первый напрвляющий вектор
  Vector b = Vector(second_seg); // второй направляющий вектор
  double curr_min = max_vec_len;
  double system_answer = SystemSolve(r01, a, r02, b); // находим минимальное расстояние через поиск экстремумов функции len(dr), где dr = r01 + t1 * a - r02 - t2 * b
  if (system_answer != -1) {
    curr_min = min(curr_min, system_answer);
  }
  if (DotProduct(b, b)) {
    CheckBoundaryTs(first_seg, second_seg, curr_min);
  }
  if (DotProduct(a, a)) {
    CheckBoundaryTs(second_seg, first_seg, curr_min);
  }
  if (!DotProduct(a, a) && !DotProduct(b, b)) {
    curr_min = min(curr_min, (r01 - r02).GetLength());
  }
  return curr_min;
}

Point::Point() :
    x(0),
    y(0),
    z(0) {

}

Point::Point(double x, double y, double z) :
    x(x),
    y(y),
    z(z) {

}

double Point::GetX() const {
  return x;
}

double Point::GetY() const {
  return y;
}

double Point::GetZ() const {
  return z;
}

istream& operator>>(istream& in, Point& point) {
  in >> point.x >> point.y >> point.z;
  return in;
}

ostream& operator<<(ostream& out, const Point& point) {
  out << point.x << " " << point.y << " " << point.z;
  return out;
}

Segment::Segment(const Point& begin, const Point& end) :
    begin(begin),
    end(end) {

}

const Point& Segment::GetBegin() const {
  return begin;
}

const Point& Segment::GetEnd() const {
  return end;
}

double Segment::GetLength() const {
  return Vector(*this).GetLength();
}

Vector::Vector() :
    x(0),
    y(0),
    z(0) {

}

Vector::Vector(double x, double y, double z) :
    x(x),
    y(y),
    z(z) {

}

Vector::Vector(const Point& begin, const Point& end) :
    x(end.GetX() - begin.GetX()),
    y(end.GetY() - begin.GetY()),
    z(end.GetZ() - begin.GetZ()) {

}

Vector::Vector(const Point& point) :
    Vector(Point(), point) {

}

Vector::Vector(const Segment& seg) :
    Vector(seg.GetBegin(), seg.GetEnd()) {

}

double Vector::GetX() const {
  return x;
}

double Vector::GetY() const {
  return y;
}

double Vector::GetZ() const {
  return z;
}

double Vector::GetLength() const {
  return sqrt(x * x + y * y + z * z);
}

Vector& Vector::operator*=(double mod) {
  x *= mod;
  y *= mod;
  z *= mod;
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
  z += other.z;
  return *this;
}

Vector operator+(Vector lhs, const Vector& rhs) {
  lhs += rhs;
  return lhs;
}

Vector& Vector::operator-=(const Vector& other) {
  x -= other.x;
  y -= other.y;
  z -= other.z;
  return *this;
}

Vector operator-(Vector lhs, const Vector& rhs) {
  lhs -= rhs;
  return lhs;
}

istream& operator>>(istream& in, Vector& vec) {
  in >> vec.x >> vec.y >> vec.z;
  return in;
}

ostream& operator<<(ostream& out, const Vector& vec) {
  out << vec.x << " " << vec.y << " " << vec.z;
  return out;
}
