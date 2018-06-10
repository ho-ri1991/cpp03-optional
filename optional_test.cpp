#include <iostream>
#include <string>
#include <vector>
#include "optional.hpp"

struct Test {
  std::string str;
  std::vector<int> vi;
  virtual ~Test(){ std::cout << "dest Test" << std::endl; }
};

struct Test1: public Test {
  long long ll;
  virtual ~Test1() { std::cout << "dest Test1" << std::endl; }
};

struct Test2: public Test1 {
  double d;
  virtual ~Test2() { std::cout << "dest Test2" << std::endl; }
};

int main() {
  my::optional<int> oi(1);
  std::cout << static_cast<bool>(oi) << std::endl;
  std::cout << *oi << std::endl;
//  my::optional<Test> ot((Test()));
  my::optional<Test1> ot1((Test1()));
  std::cout << static_cast<bool>(ot1) << std::endl;
  return 0;
}

