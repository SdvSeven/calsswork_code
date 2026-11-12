#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>

struct p_t {
  int x;
  int y;
};

class Ints {
public:
  Ints();
  Ints(size_t size, int value);
  Ints(const int *arr, size_t arr_size);
  Ints(Ints const& rhs);
  Ints(const size_t size);
  Ints(Ints && rhs) noexcept;

  Ints & operator=(const Ints & rhs);
  Ints & operator=(Ints && rhs) noexcept;
  Ints operator+(const Ints & rhs) const;

  ~Ints();

  size_t getSize() const noexcept;

  void set(size_t id, int i);
  int get(size_t id) const noexcept;

  int getSafe(size_t id) const;
  void setSafe(size_t id, int i);

  Ints append(int i) const;
  Ints append(const Ints &b) const;

private:
  size_t size;
  size_t capacity;
  int * data;
};

Ints::Ints():
  size(0),
  capacity(0),
  data(nullptr)
{}

Ints::Ints(const size_t size, const int value):
  size(size), capacity(size), data(new int[capacity])
{
  for (size_t i = 0; i < size; i++) {
    data[i] = value;
  }
}

Ints::Ints(const int * arr, const size_t arr_size):
  size(arr_size), capacity(size), data(new int[capacity])
{
  for (size_t i = 0; i < arr_size; i++) {
    data[i] = arr[i];
  }
}

Ints::Ints(const size_t size):
  size(size), capacity(size), data(new int[capacity])
{}

Ints::Ints(Ints const& rhs):
  size(rhs.size), capacity(rhs.capacity), data(new int [capacity])
{
  for (size_t i = 0; i < size; i++) {
    data[i] = rhs.data[i];
  }
}

Ints::Ints(Ints && rhs) noexcept:
  size(rhs.size),
  capacity(rhs.capacity),
  data(rhs.data)
{
  rhs.capacity = 0;
  rhs.size = 0;
  rhs.data = nullptr;
}

Ints & Ints::operator=(const Ints & rhs)
{
  if (this == &rhs) {
    return *this;
  }

  Ints temp(rhs);
  std::swap(size, temp.size);
  std::swap(data, temp.data);
  std::swap(capacity, temp.capacity);

  return *this;
}

Ints & Ints::operator=(Ints && rhs) noexcept
{
  if (this != &rhs) {
    delete [] data;

    capacity = rhs.capacity;
    size = rhs.size;
    data = rhs.data;

    rhs.data = nullptr;
    rhs.capacity = 0;
    rhs.size = 0;
  }

  return *this;
}

Ints Ints::operator+(const Ints & rhs) const {
  if (getSize() != rhs.getSize()) {
    throw std::invalid_argument("Arrays have diff size");
  }

  Ints result(getSize(), 0);
  for (size_t i = 0; i < getSize(); i++) {
    if ((get(i) > 0 && rhs.get(i) > std::numeric_limits< int >::max() - get(i)) || (get(i) < 0 && rhs.get(i) < std::numeric_limits< int >::min() - get(i))) {
      throw std::overflow_error("Overflow");
    }
    result.set(i, rhs.get(i) + get(i));
  }

  return result;
}

Ints::~Ints() {
  delete [] data;
}

void Ints::set(const size_t id, const int i) {
  data[id] = i;
}

void Ints::setSafe(const size_t id, const int i) {
  if (id >= size) {
    throw std::logic_error("Out of range");
  }

  set(id, i);
}

int Ints::get(const size_t id) const noexcept {
  return data[id];
}

int Ints::getSafe(const size_t id) const {
  if (id >= size) {
    throw std::logic_error("Out of range");
  }
  return get(id);
}

size_t Ints::getSize() const noexcept {
  return size;
}

Ints Ints::append(const int i) const {
  if (std::numeric_limits< size_t >::max() - 1 < size) {
    throw std::logic_error("Overflow");
  }

  Ints temp(size+1);
  size_t p = 0;

  while (p < size) {
    temp.data[p] = data[p];
    p++;
  }
  temp.data[p] = i;

  return temp;
}

Ints Ints::append(const Ints &b) const {
  if (std::numeric_limits< size_t >::max() - size < b.size) {
    throw std::logic_error("Overflow");
  }

  Ints temp(size+b.size);
  size_t p = 0;

  while (p < size) {
    temp.data[p] = data[p];
    p++;
  }
  for (size_t i = 0; i < b.size; i++) {
    temp.data[p++] = b.data[i];
  }

  return temp;
}

class Mtx {
  size_t m;
  size_t n;
  Ints data_mtx;
  // m - кол-во строк  n - кол-во столбцов

  public:
  Mtx();
  Mtx(size_t m, size_t n);
  Mtx(size_t m, size_t n, const Ints &data);
  ~Mtx() = default;

  Mtx sum(Mtx const& rhs) const;
  Mtx mul(Mtx const& rhs) const;

  Ints getCol(size_t id) const;
  Ints getRow(size_t id) const;

  Mtx addRow(const int pasta) const;

  Mtx transpose() const;
  int det() const;
};

Mtx::Mtx():
  m(0), n(0)
{}

Mtx::Mtx(size_t m, size_t n):
  m(m), n(n)
{
  if (n != 0 && m > std::numeric_limits< size_t >::max() / n) {
    throw std::overflow_error("Overflow");
  }
  data_mtx = Ints(n*m, 0);
}

Mtx::Mtx(size_t m, size_t n, const Ints &data):
  m(m), n(n), data_mtx(data)
{
  if (n != 0 && m > std::numeric_limits< size_t >::max() / n) {
    throw std::overflow_error("Overflow");
  }
  if (data.getSize() != m*n) {
    throw std::invalid_argument("Wrong data size");
  }
}

Mtx Mtx::sum(Mtx const& rhs) const {
  if (m != rhs.m || n != rhs.n) {
    throw std::invalid_argument("Matrices have diff size");
  }

  return Mtx(m, n, data_mtx + rhs.data_mtx);
}

Ints Mtx::getCol(const size_t id) const {
  if (id >= n) {
    throw std::logic_error("Out of range");
  }

  Ints res;
  for (size_t i = 0; i < m; ++i) {
    res = res.append(data_mtx.get(i * n + id));
  }
  return res;
}

Ints Mtx::getRow(const size_t id) const {
  if (id >= m) {
    throw std::logic_error("Out of range");
  }

  Ints res;
  for (size_t i = 0; i < n; ++i) {
    res = res.append(data_mtx.get(id * n + i));
  }
  return res;
}

Mtx Mtx::mul(const Mtx& rhs) const {
  if (n != rhs.m) {
    throw std::invalid_argument("Can't multiply matrices");
  }

  Mtx res(m, rhs.n);
  for (size_t i = 0; i < m; i++) {
    Ints row = getRow(i);
    for (size_t j = 0; j < rhs.n; j++) {
      Ints col = rhs.getCol(j);

      long long sm = 0;
      for (size_t k = 0; k < rhs.m; k++) {
        sm += static_cast< long long >(row.get(k)) * col.get(k);
      }

      if (sm > std::numeric_limits< int >::max() || sm < std::numeric_limits< int >::min()) {
        throw std::overflow_error("Overflow");
      }

      res.data_mtx.set(i * rhs.n + j, static_cast< int >(sm));
    }
  }

  return res;
}

Mtx Mtx::addRow(const int pasta) const {
  Mtx res(m+1, n);
  for (size_t i = 0; i < m*n; i++) {
    res.data_mtx.set(i, data_mtx.get(i));
  }
  for (size_t i = 0; i < n; i++) {
    res.data_mtx.set(n*m + i, pasta);
  }
  return res;
}

Mtx Mtx::transpose() const {
  Mtx res(n, m);

  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      res.data_mtx.set(j * m + i, data_mtx.get(i * n + j));
    }
  }

  return res;
}

int Mtx::det() const{
  if (m != n) throw std::logic_error("Can't estimate determinant");
  if (m == 0) return 1;
  if (m == 1) return data_mtx.get(0);
  if (m == 2) {
    long long d = static_cast< long long >(data_mtx.get(0)) * data_mtx.get(3) - static_cast< long long >(data_mtx.get(1)) * data_mtx.get(2);
    if (d > std::numeric_limits< int >::max() || d < std::numeric_limits< int >::min()) {
      throw std::overflow_error("Overflow");
    }
    return static_cast< int >(d);
  }

  long long res = 0;

  for (size_t i = 0; i < n; i++) {
    Mtx minor(m-1, n-1);

    size_t p = 0;

    for (size_t row = 1; row < m; row++) {
      for (size_t col = 0; col < n; col++) {
        if (col == i) continue;

        minor.data_mtx.set(p++, data_mtx.get(row * n + col));
      }
    }

    int sign = i % 2 == 0 ? 1 : -1;

    res += static_cast< long long >(sign) * data_mtx.get(i) * minor.det();
  }

  if (res > std::numeric_limits< int >::max() || res < std::numeric_limits< int >::min()) {
    throw std::overflow_error("Overflow");
  }

  return static_cast< int >(res);
}

class Mtxs {
public:
  Mtxs();
  Mtxs(size_t const size, const Mtx & mtx);
  Mtxs(const Mtxs & rhs);
  Mtxs(Mtxs && rhs) noexcept;
  ~Mtxs();

  Mtxs & operator=(const Mtxs & rhs);

  Mtxs &operator=(Mtxs &&rhs) noexcept;

  size_t getSize() const noexcept;
  const Mtx & get(size_t id) const;
  void set(size_t id, const Mtx & mtx);

private:
  size_t size;
  Mtx * arr_mtx;
};

Mtxs::Mtxs():
  size(0), arr_mtx(nullptr)
{}

Mtxs::Mtxs(const size_t size, const Mtx & mtx):
  size(size), arr_mtx(new Mtx[size])
{
  for (size_t i = 0; i < size; i++) {
    try {
      arr_mtx[i] = mtx;
    } catch (...) {
      delete [] arr_mtx;
      throw;
    }
  }
}

Mtxs::Mtxs(const Mtxs & rhs):
  size(rhs.size), arr_mtx(new Mtx[rhs.size])
{
  for (size_t i = 0; i < size; i++) {
    try {
      arr_mtx[i] = rhs.arr_mtx[i];
    } catch (...) {
      delete [] arr_mtx;
      throw;
    }
  }
}

Mtxs & Mtxs::operator=(const Mtxs & rhs) {
  if (this == &rhs) {
    return *this;
  }

  Mtxs temp(rhs);
  std::swap(size, temp.size);
  std::swap(arr_mtx, temp.arr_mtx);

  return *this;
}

Mtxs & Mtxs::operator=(Mtxs && rhs) noexcept
{
  if (this != &rhs) {
    delete [] arr_mtx;

    size = rhs.size;
    arr_mtx = rhs.arr_mtx;

    rhs.arr_mtx = nullptr;
    rhs.size = 0;
  }

  return *this;
}

Mtxs::Mtxs(Mtxs && rhs) noexcept:
  size(rhs.size),
  arr_mtx(rhs.arr_mtx)
{
  rhs.size = 0;
  rhs.arr_mtx = nullptr;
}

Mtxs::~Mtxs()
{
  delete [] arr_mtx;
}

size_t Mtxs::getSize() const noexcept {
  return size;
}

const Mtx & Mtxs::get(const size_t id) const {
  if (id >= size) {
    throw std::logic_error("Out of range");
  }
  return arr_mtx[id];
}

void Mtxs::set(const size_t id, const Mtx & mtx) {
  if (id >= size) {
    throw std::logic_error("Out of range");
  }
  arr_mtx[id] = mtx;
}


class Points {
  Ints data_points;
public:
  Points():
    data_points(Ints())
  {}

  Points(const Ints &data):
    data_points(data)
  {}

  ~Points() = default;

  size_t getSize() const noexcept {
    return data_points.getSize() / 2;
  }

  void set(const size_t id, const p_t p) noexcept {
    data_points.set(id*2, p.x);
    data_points.set(id*2+1, p.y);
  }

  p_t get(const size_t id) const noexcept {
    p_t res;
    res.x = data_points.get(id*2);
    res.y = data_points.get(id*2+1);

    return res;
  }

  Points append(p_t p) const {
    size_t const size = data_points.getSize();

    if (size > std::numeric_limits<size_t>::max() - 2) {
      throw std::logic_error("Overflow");
    }

    Ints tmp(size+2);

    for (size_t i = 0; i < size; i++) {
      tmp.set(i, data_points.get(i));
    }

    tmp.set(size, p.x);
    tmp.set(size+1, p.y);

    return tmp;
  }
};

class Plot
{
public:
  void set(const int x, const int y);
  int calc(int x) const;
  int solve(Ints & buf, int y) const;
  Points append(p_t p) const;
private:
  Ints data_plot;
};

void Plot::set(const int x, const int y) {
  for (size_t i = 0; i < data_plot.getSize(); i+=2) {
    if (data_plot.get(i) == x) {
      data_plot.set(i+1, y);
      return;
    }
  }

  data_plot = data_plot.append(x);
  data_plot = data_plot.append(y);
}

int Plot::calc(const int x) const {
  for (size_t i = 0; i < data_plot.getSize(); i+=2) {
    if (data_plot.get(i) == x) {
      return data_plot.get(i+1);
    }
  }

  throw std::logic_error("Point not found");
}

int Plot::solve(Ints & buf, const int y) const {
  int cnt = 0;
  for (size_t i = 1; i < data_plot.getSize(); i+=2) {
    if (y == data_plot.get(i)) {
      buf = buf.append(data_plot.get(i-1));
      cnt++;
    }
  }
  return cnt;
}

Points Plot::append(const p_t p) const {
  size_t const size = data_plot.getSize();

  if (size > std::numeric_limits<size_t>::max() - 2) {
    throw std::logic_error("Overflow");
  }

  Ints tmp(size + 2);

  for (size_t i = 0; i < size; i++) {
    tmp.set(i, data_plot.get(i));
  }

  tmp.set(size, p.x);
  tmp.set(size+1, p.y);

  return tmp;
}

void print(const Ints & a) {
  for (size_t i = 0; i < a.getSize(); i++) {
    std::cout << a.get(i) << ' ';
  }
  std::cout << '\n';
}

void print(const Mtx & mtx, size_t m) {
  for (size_t i = 0; i < m; i++) {
    print(mtx.getRow(i));
  }
  std::cout << '\n';
}

int main() {
  int arr[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  Ints a(arr, 9);
  Mtx mtx1(3, 3, a);

  try {
    print(a + a);
    print(a.append(10).append(a));

    print(mtx1.sum(mtx1), 3);
    print(mtx1.mul(mtx1), 3);
    print(mtx1.transpose(), 3);
    print(mtx1.addRow(0), 4);
    std::cout << mtx1.det() << '\n';

    int arr2[9] = {2, 0, 1, 1, 3, 2, 1, 1, 2};
    Mtx mtx2(3, 3, Ints(arr2, 9));
    std::cout << mtx2.det() << '\n';

    Mtxs ms(2, mtx1);
    ms.set(1, mtx2);
    std::cout << ms.getSize() << ' ' << ms.get(1).det() << '\n';

    Points pts;
    pts = pts.append({1, 2}).append({3, 4});
    pts.set(0, {5, 6});
    std::cout << pts.getSize() << ": " << pts.get(0).x << ' ' << pts.get(0).y << '\n';

    Plot plot;
    plot.set(1, 10);
    plot.set(2, 20);
    plot.set(3, 10);
    std::cout << plot.calc(2) << '\n';

    Ints buf;
    std::cout << plot.solve(buf, 10) << ": ";
    print(buf);

    Points fromPlot = plot.append({4, 40});
    std::cout << fromPlot.getSize() << '\n';

    mtx1.mul(Mtx(2, 2));
  } catch (const std::exception & e) {
    std::cout << e.what() << '\n';
  }

  return 0;
}
