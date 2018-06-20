#include "alignment_of.hpp"
#include "static_assert.hpp"

using namespace my::type_traits;

int main() {
#if defined(__GNUC__)
  STATIC_ASSERT(__alignof__ (char) == alignment_of<char>::value, char_alignment_failuer);
  STATIC_ASSERT(__alignof__ (short) == alignment_of<short>::value, short_alignment_failuer);
  STATIC_ASSERT(__alignof__ (int) == alignment_of<int>::value, int_alignment_failuer);
  STATIC_ASSERT(__alignof__ (long) == alignment_of<long>::value, long_alignment_failuer);
  STATIC_ASSERT(__alignof__ (long long) == alignment_of<long long>::value, long_long_alignment_failuer);
  STATIC_ASSERT(__alignof__ (float) == alignment_of<float>::value, float_alignment_failuer);
  STATIC_ASSERT(__alignof__ (double) == alignment_of<double>::value, double_alignment_failuer);
  STATIC_ASSERT(__alignof__ (long double) == alignment_of<long double>::value, long_double_alignment_failuer);
  STATIC_ASSERT(__alignof__ (void*) == alignment_of<void*>::value, pointer_alignment_failuer);
#endif
  return 0;
}

