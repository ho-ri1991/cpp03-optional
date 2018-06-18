#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "optional.hpp"

using namespace my;

template <class T, template <class> class Storage>
void valid_optional_access_tests(optional<T, Storage>& opt, const T& expect_value)
{
    BOOST_CHECK(opt);
    BOOST_CHECK(opt.has_value());
    BOOST_CHECK(*opt == expect_value);
    BOOST_CHECK(opt.value() == expect_value);
}
template <class T, template <class> class Storage>
void valid_optional_access_tests(const optional<T, Storage>& opt, const T& expect_value)
{
    BOOST_CHECK(opt);
    BOOST_CHECK(opt.has_value());
    BOOST_CHECK(*opt == expect_value);
    BOOST_CHECK(opt.value() == expect_value);
}
template <class T, template <class> class Storage>
void invalid_optional_access_tests(optional<T, Storage>& opt)
{
  BOOST_CHECK(!opt);
  BOOST_CHECK(!opt.has_value());
  BOOST_CHECK_THROW(opt.value(), bad_optional_access);
}
template <class T, template <class> class Storage>
void invalid_optional_access_tests(const optional<T, Storage>& opt)
{
  BOOST_CHECK(!opt);
  BOOST_CHECK(!opt.has_value());
  BOOST_CHECK_THROW(opt.value(), bad_optional_access);
}

template <class T, template <class> class Storage>
void ordinary_access_assign_tests(const T& init, const T& update)
{
  // construct test
  optional<T, Storage> opt(init);
  const optional<T, Storage>& const_ref_opt = opt;
  
  valid_optional_access_tests(opt, init);
  valid_optional_access_tests(const_ref_opt, init);
  
  // update value test
  opt = update;
  
  valid_optional_access_tests(opt, update);
  valid_optional_access_tests(const_ref_opt, update);

  // reset test
  opt.reset();

  invalid_optional_access_tests(opt);
  invalid_optional_access_tests(const_ref_opt);

  // substitute null to test
  opt = nullopt;

  invalid_optional_access_tests(opt);
  invalid_optional_access_tests(const_ref_opt);

  // nullopt substitute test
  opt = init;

  valid_optional_access_tests(opt, init);
  valid_optional_access_tests(const_ref_opt, init);

  // swap test
  {
    optional<T, Storage> opt1(nullopt);
    optional<T, Storage> opt2(init);
    optional<T, Storage> opt3(update);
    optional<T, Storage> opt4(nullopt);

    swap(opt2, opt3);
    valid_optional_access_tests(opt2, update);
    valid_optional_access_tests(opt3, init);

    swap(opt3, opt1);
    valid_optional_access_tests(opt1, init);
    invalid_optional_access_tests(opt3);

    swap(opt3, opt1);
    valid_optional_access_tests(opt3, init);
    invalid_optional_access_tests(opt1);

    swap(opt1, opt4);
    invalid_optional_access_tests(opt1);
    invalid_optional_access_tests(opt4);
  }
}

struct CountTest
{
  int i;
  static int live_count;
  CountTest(int i):i(i) { ++live_count; }
  CountTest(const CountTest& other): i(other.i) { ++live_count; }
  ~CountTest() { --live_count; }
};
int CountTest::live_count = 0;

bool operator==(const CountTest& x, const CountTest& y) { return x.i == y.i; }
BOOST_AUTO_TEST_SUITE(my_optional)
BOOST_AUTO_TEST_CASE(my_optional) {
  ordinary_access_assign_tests<int, LocalStorage>(42, 7);
  ordinary_access_assign_tests<int, DynamicStorage>(42, 7);
  ordinary_access_assign_tests<std::string, LocalStorage>("aa", "bb");
  ordinary_access_assign_tests<std::string, DynamicStorage>("aa", "bb");

  BOOST_CHECK_EQUAL(CountTest::live_count, 0);
  ordinary_access_assign_tests<CountTest, DynamicStorage>(CountTest(42), CountTest(77));
  BOOST_CHECK_EQUAL(CountTest::live_count, 0);
  ordinary_access_assign_tests<CountTest, LocalStorage>(CountTest(42), CountTest(77));
  BOOST_CHECK_EQUAL(CountTest::live_count, 0);
}
BOOST_AUTO_TEST_SUITE_END()

