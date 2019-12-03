#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <stack>
#include <cstdint>

using std::cin;
using std::cout;
using std::endl;
using std::istream;
using std::ostream;
using std::vector;
using std::swap;

static const double EPS = std::numeric_limits<double>::epsilon();

typedef std::pair<int16_t, int16_t> pair;

class Vector;

class Point {
  public:
    Point();
    Point(double x, double y, double z);
    double GetX() const;
    double GetY() const;
    double GetZ() const;
    Point& operator+=(const Vector& vec);
    friend Point operator+(Point point, const Vector& vec);
    friend istream& operator>>(istream& in, Point& point);
    friend ostream& operator<<(ostream& out, const Point& point);
    friend bool ComparePoints(const Point& lhs, const Point& rhs);
  private:
    double x;
    double y;
    double z;
};

bool ComparePoints(const Point& lhs, const Point& rhs);

class Vector {
  public:
    Vector();
    Vector(double x, double y, double z);
    Vector(const Point& begin, const Point& end);
    explicit Vector(const Point& point);
    double GetX() const;
    double GetY() const;
    double GetZ() const;
    double GetLength() const;
    Vector& operator*=(double mod);
    friend double DotProduct(const Vector& lhs, const Vector& rhs);
    friend Vector CrossProduct(const Vector& lhs, const Vector& rhs);
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

class Plane {
  public:
    Plane(const Point& first_point, const Point& second_point, const Point& third_point);
    Vector NormVector();
  private:
    double a;
    double b;
    double c;
    double d;
};

struct Face {
  Face(int a, int b, int c);
  void Fix(const vector<Point>& pts); // Находит правильный порядок для грани
  bool operator==(const Face& other);
  int16_t a;
  int16_t b;
  int16_t c;
};

struct FaceCompare {
  bool operator()(const Face& lhs, const Face& rhs);
};

int Turn(int first_p, int second_p, const vector<Point>& pts, int pts_num); // Выполняет поворот вокруг ребра, вычисляя третью точку
double Volume(int p1, int p2, int p3, int p4, const vector<Point>& pts); // Находит ориентированный объем тетраэдра
double Area(int p1, int p2, int p3, const vector<Point>& pts); // Находит площадь треугольника
vector<Face> BuildConvexHull(vector<Point> pts);

int main() { // РЕШЕНИЕ ЗА O(N^2)
  int tests_num;
  cin >> tests_num;
  vector<Point> pts;
  int size;
  FaceCompare comp;
  for (int i = 0; i < tests_num; ++i) {
    cin >> size;
    pts.resize(size);
    for (int j = 0; j < size; ++j) {
      cin >> pts[j];
    }
    vector<Face> result = BuildConvexHull(std::move(pts));
    sort(result.begin(), result.end(), comp);
    cout << result.size() << endl;
    for (const Face& face : result) {
      cout << "3 " << face.a << " " << face.b << " " << face.c << std::endl;
    }
  }
  return 0;
}

double Area(int p1, int p2, int p3, const vector<Point>& pts) {
  return fabs(CrossProduct(Vector(pts[p1], pts[p2]), Vector(pts[p2], pts[p3])).GetLength() / 2);
}

double Volume(int p1, int p2, int p3, int p4, const vector<Point>& pts) {
  return DotProduct(Vector(pts[p1], pts[p4]), CrossProduct(Vector(pts[p1], pts[p2]), Vector(pts[p1], pts[p3])));
}

int Turn(int first_p, int second_p, const vector<Point>& pts, int pts_num) {
  int third_p = 0;
  double max_ar = Area(first_p, second_p, 0, pts);
  double vol;
  double ar;
  for (int i = 1; i < pts_num; ++i) {
    vol = Volume(first_p, second_p, third_p, i, pts);
    if (vol < -EPS) {
      third_p = i;
    } else if (fabs(vol) < EPS) {
      ar = Area(first_p, second_p, i, pts);
      if (ar > max_ar) {
        ar = max_ar;
        third_p = i;
      }
    }
  }
  return third_p;
}

void Face::Fix(const vector<Point>& pts) {
  if (b < a) { // Поиск минимальной по номеру вершины
    swap(a, b);
  }
  if (c < a) {
    swap(a, c);
  }
  int other_p;
  for (other_p = 0; other_p == a || other_p == b || other_p == c; ++other_p) {

  }
  Plane pl(pts[a], pts[b], pts[c]);
  Vector norm = pl.NormVector();
  if (DotProduct(norm, Vector(pts[a], pts[other_p])) < -EPS) {
    norm *= -1;
  }
  Vector orient = CrossProduct(Vector(pts[a], pts[b]), Vector(pts[a], pts[c]));
  if (DotProduct(orient, norm) > EPS) {
    swap(b, c);
  }
}

vector<Face> BuildConvexHull(vector<Point> pts) {
  int pts_num = pts.size();
  vector<Face> hull;
  vector<vector<int16_t>> processed(pts_num, vector<int16_t>(pts_num));
  int first_point = std::min_element(pts.begin(), pts.end(), ComparePoints) - pts.begin();
  int second_point = first_point; // Поиск начального ребра
  for (int i = 0; i < pts_num; ++i) {
    if (pts[second_point].GetZ() == pts[i].GetZ() &&
        pts[second_point].GetY() == pts[i].GetY() &&
        pts[second_point].GetX() < pts[i].GetX()) {
      second_point = i;
    }
  }
  if (first_point == second_point) {
    pts.push_back(pts[first_point] + Vector(1, 0, 0));
    second_point = pts_num;
    second_point = Turn(first_point, second_point, pts, pts_num);
  }
  int third_point = Turn(first_point, second_point, pts, pts_num); // Поиск начального треугольника
  std::stack<pair> edges; // Начало обработки
  Face lowest(first_point, second_point, third_point);
  lowest.Fix(pts);
  hull.push_back(lowest);
  edges.emplace(second_point, first_point);
  edges.emplace(third_point, second_point);
  edges.emplace(first_point, third_point);
  while (!edges.empty()) {
    pair top = edges.top();
    edges.pop();
    if (!processed[top.first][top.second]) {
      third_point = Turn(top.first, top.second, pts, pts_num);
      if (first_point != second_point && first_point != third_point && second_point != third_point) {
        Face result(top.first, top.second, third_point);
        result.Fix(pts);
        if (std::find(hull.begin(), hull.end(), result) == hull.end()) {
          hull.push_back(result);
        }
        if (!processed[top.second][top.first]) {
          edges.emplace(top.second, top.first);
        }
        if (!processed[third_point][top.second]) {
          edges.emplace(third_point, top.second);
        }
        if (!processed[top.first][third_point]) {
          edges.emplace(top.first, third_point);
        }
        processed[top.first][top.second] = processed[top.second][top.first] = true;
      }

    }
  }
  return hull;
}

bool FaceCompare::operator()(const Face& lhs, const Face& rhs) {
  return (lhs.a < rhs.a ||
         (lhs.a == rhs.a && lhs.b < rhs.b) ||
         (lhs.a == rhs.a && lhs.b == rhs.b && lhs.c < rhs.c));
}

bool ComparePoints(const Point& lhs, const Point& rhs) {
  return (lhs.z < rhs.z ||
         (lhs.z == rhs.z && lhs.x < rhs.x) ||
         (lhs.z == rhs.z && lhs.x == rhs.x && lhs.y < rhs.y));
}

Plane::Plane(const Point& first_point, const Point& second_point, const Point& third_point) {
  Vector norm = CrossProduct(Vector(first_point, second_point), Vector(first_point, third_point));
  a = norm.GetX();
  b = norm.GetY();
  c = norm.GetZ();
  d = -a * first_point.GetX() - b * first_point.GetY() - c * first_point.GetZ();
}

Vector Plane::NormVector() {
  return Vector(a, b, c);
}

double DotProduct(const Vector& lhs, const Vector& rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vector CrossProduct(const Vector& lhs, const Vector& rhs) {
  return Vector(lhs.y * rhs.z - lhs.z * rhs.y,
                lhs.z * rhs.x - lhs.x * rhs.z,
                lhs.x * rhs.y - lhs.y * rhs.x);
}

Face::Face(int a, int b, int c) :
    a(a),
    b(b),
    c(c) {

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

Point& Point::operator+=(const Vector& vec) {
  x += vec.GetX();
  y += vec.GetY();
  return *this;
}

Point operator+(Point point, const Vector& vec) {
  point += vec;
  return point;
}

bool Face::operator==(const Face& other) {
  return (a == other.a && b == other.b && c == other.c);
}
