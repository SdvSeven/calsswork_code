#include <iostream>
#include <stdexcept>
#include <vector>
#include <memory>
#include <utility>
#include <initializer_list>

class Mtx
{
public:
  Mtx():
    m_(0),
    n_(0)
  {}

  Mtx(size_t m, size_t n, int value = 0):
    m_(m),
    n_(n),
    data_(m * n, value)
  {}

  size_t getM() const
  {
    return m_;
  }

  size_t getN() const
  {
    return n_;
  }

  int get(size_t i, size_t j) const
  {
    if (i >= m_ || j >= n_) {
      throw std::out_of_range("Invalid matrix index");
    }

    return data_[i * n_ + j];
  }

  void set(size_t i, size_t j, int value)
  {
    if (i >= m_ || j >= n_) {
      throw std::out_of_range("Invalid matrix index");
    }

    data_[i * n_ + j] = value;
  }

  Mtx operator+(const Mtx& other) const
  {
    checkSize(other);

    Mtx result(m_, n_);

    for (size_t i = 0; i < data_.size(); ++i) {
      result.data_[i] = data_[i] + other.data_[i];
    }

    return result;
  }

  Mtx operator|(const Mtx& other) const
  {
    checkSize(other);

    Mtx result(m_, n_);

    for (size_t i = 0; i < data_.size(); ++i) {
      result.data_[i] = data_[i] | other.data_[i];
    }

    return result;
  }

  bool operator==(const Mtx& other) const
  {
    return m_ == other.m_ && n_ == other.n_ && data_ == other.data_;
  }

private:
  size_t m_;
  size_t n_;
  std::vector<int> data_;

  void checkSize(const Mtx& other) const
  {
    if (m_ != other.m_ || n_ != other.n_) {
      throw std::invalid_argument("Different matrix sizes");
    }
  }
};


class Mtxs
{
public:
  Mtxs():
    k_(0),
    mtxs_(nullptr)
  {}

  Mtxs(size_t k):
    k_(k),
    mtxs_(new Mtx[k])
  {}

  Mtxs(std::initializer_list<Mtx> values):
    k_(values.size()),
    mtxs_(new Mtx[k_])
  {
    size_t i = 0;

    for (const Mtx& value : values) {
      mtxs_[i++] = value;
    }
  }

  Mtxs(const Mtxs& other):
    k_(other.k_),
    mtxs_(new Mtx[k_])
  {
    for (size_t i = 0; i < k_; ++i) {
      mtxs_[i] = other.mtxs_[i];
    }
  }

  Mtxs(Mtxs&& other) noexcept:
    k_(other.k_),
    mtxs_(other.mtxs_)
  {
    other.k_ = 0;
    other.mtxs_ = nullptr;
  }

  ~Mtxs()
  {
    delete[] mtxs_;
  }

  Mtxs& operator=(const Mtxs& other)
  {
    if (this != &other) {
      Mtxs temp(other);
      swap(temp);
    }

    return *this;
  }

  Mtxs& operator=(Mtxs&& other) noexcept
  {
    if (this != &other) {
      delete[] mtxs_;

      k_ = other.k_;
      mtxs_ = other.mtxs_;

      other.k_ = 0;
      other.mtxs_ = nullptr;
    }

    return *this;
  }

  Mtxs operator+(const Mtxs& other) const
  {
    Mtxs result(k_ + other.k_);

    for (size_t i = 0; i < k_; ++i) {
      result.mtxs_[i] = mtxs_[i];
    }

    for (size_t i = 0; i < other.k_; ++i) {
      result.mtxs_[k_ + i] = other.mtxs_[i];
    }

    return result;
  }

  Mtx& operator[](size_t index)
  {
    if (index >= k_) {
      throw std::out_of_range("Invalid index");
    }

    return mtxs_[index];
  }

  const Mtx& operator[](size_t index) const
  {
    if (index >= k_) {
      throw std::out_of_range("Invalid index");
    }

    return mtxs_[index];
  }

  Mtxs append(const Mtx& value) const
  {
    return insert(k_, value);
  }

  Mtxs prepend(const Mtx& value) const
  {
    return insert(0, value);
  }

  Mtxs insert(size_t index, const Mtx& value) const
  {
    if (index > k_) {
      throw std::out_of_range("Invalid index");
    }

    Mtxs result(k_ + 1);

    for (size_t i = 0; i < index; ++i) {
      result.mtxs_[i] = mtxs_[i];
    }

    result.mtxs_[index] = value;

    for (size_t i = index; i < k_; ++i) {
      result.mtxs_[i + 1] = mtxs_[i];
    }

    return result;
  }

  Mtxs erase(size_t index) const
  {
    if (index >= k_) {
      throw std::out_of_range("Invalid index");
    }

    return erase(index, index + 1);
  }

  Mtxs erase(size_t first, size_t last) const
  {
    if (first > last || last > k_) {
      throw std::out_of_range("Invalid range");
    }

    Mtxs result(k_ - (last - first));

    for (size_t i = 0; i < first; ++i) {
      result.mtxs_[i] = mtxs_[i];
    }

    for (size_t i = last; i < k_; ++i) {
      result.mtxs_[i - (last - first)] = mtxs_[i];
    }

    return result;
  }

  Mtxs eraseLast() const
  {
    if (empty()) {
      throw std::out_of_range("Array is empty");
    }

    return erase(k_ - 1);
  }

  Mtx conv(Mtx accumulator, Mtx (*operation)(Mtx, Mtx)) const
  {
    if (operation == nullptr) {
      throw std::invalid_argument("Invalid operation");
    }

    for (size_t i = 0; i < k_; ++i) {
      accumulator = operation(accumulator, mtxs_[i]);
    }

    return accumulator;
  }

  size_t getSize() const
  {
    return k_;
  }

  bool empty() const
  {
    return k_ == 0;
  }

private:
  size_t k_;
  Mtx* mtxs_;

  void swap(Mtxs& other) noexcept
  {
    std::swap(k_, other.k_);
    std::swap(mtxs_, other.mtxs_);
  }
};


void print(const Mtx& a)
{
  for (size_t i = 0; i < a.getM(); ++i) {
    for (size_t j = 0; j < a.getN(); ++j) {
      std::cout << a.get(i, j) << ' ';
    }
    std::cout << '\n';
  }
}

void print(const Mtxs& a)
{
  for (size_t i = 0; i < a.getSize(); ++i) {
    std::cout << "Matrix " << i << ":\n";
    print(a[i]);
  }
}

Mtx add(const Mtx a, const Mtx b)
{
  return a + b;
}

Mtx bitOr(const Mtx a, const Mtx b)
{
  return a | b;
}

int main()
{
  Mtx a(2, 2);
  Mtx b(2, 2);
  Mtx c(2, 2);

  for (size_t i = 0; i < 2; ++i) {
    for (size_t j = 0; j < 2; ++j) {
      a.set(i, j, static_cast<int>(i * 2 + j + 1));
      b.set(i, j, 2);
      c.set(i, j, 3);
    }
  }

  Mtxs x;
  x = x.append(a).append(b).append(c);

  print(x);

  Mtxs y = x.prepend(c);
  y = y.insert(2, a);

  std::cout << "After insert:\n";
  print(y);

  y = y.erase(1);
  y = y.erase(1, 3);
  y = y.eraseLast();

  std::cout << "After erase:\n";
  print(y);

  Mtxs z = x + y;

  std::cout << "After operator+:\n";
  print(z);

  std::cout << "Element [0]:\n";
  print(x[0]);

  Mtx sum = x.conv(Mtx(2, 2), add);

  std::cout << "Sum:\n";
  print(sum);

  Mtx result = x.conv(Mtx(2, 2), bitOr);

  std::cout << "Bitwise OR:\n";
  print(result);

  std::cout << "Size: " << x.getSize() << '\n';
  std::cout << "Empty: " << x.empty() << '\n';

  return 0;
}
