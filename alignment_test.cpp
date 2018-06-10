#include "aligned_storage.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

struct Test1 {
  std::vector<int> v;
  std::size_t s;
  long double ld;
};

int main() {
  my::type_traits::aligned_storage<Test1> t;
  // Test1
  std::cout << alignof(decltype(t)) << std::endl;
  std::cout << alignof(Test1) << std::endl;
  std::cout << sizeof(decltype(t)) << std::endl;
  std::cout << sizeof(Test1) << std::endl;
  return 0;
}

