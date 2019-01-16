#ifndef OPERATIONS_UTILS_HPP
#define OPERATIONS_UTILS_HPP

#include <array>
#include <type_traits>

/*
 * Check if each nibble of an addition result overlow and set the highest bit of the nibble if so.
 * A nibble is a group of 4 bit.
 * example:
 *  Let's say T = Byte and a = b = 1111 1111b
 *  check_add_overflows(a, b) -> 1000 1000b
 *                               ^    ^ Those bits are set because they made their nibble overflow
 */
template <typename T, typename = std::enable_if_t<std::is_unsigned<T>::value>>
T check_add_overflows(T a, T b) {
  constexpr T t_max = std::numeric_limits<T>::max();
  T ret = 0u;
  T op = a + b;
  for (T mask = 0x10; mask <= t_max; mask <<= 4) {
    if (op & mask) ret |= mask >> 1;
  }
  if (a > t_max - b) ret |= 1 << sizeof(T) * 8;
  return ret;
}

/*
 * Check if each nibble of an substraction result overlow and set the lowest bit of the nibble if so.
 * A nibble is a group of 4 bit.
 * example:
 *  Let's say T = Byte and a = b = 1111 1111b
 *  check_add_overflows(a, b) -> 0001 0001b
 *                                  ^    ^ Those bits are set because they made their nibble overflow
 */
template <typename T, typename = std::enable_if_t<std::is_unsigned<T>::value>>
T check_sub_overflows(
    T a, T b) {
  T ret = 0u;
  T op = a - b;
  for (T mask = 0xF; mask <= std::numeric_limits<T>::max(); mask <<= 4) {
    if (op & mask) ret |= mask << 1;
  }
  if (a < std::numeric_limits<T>::min() - b) ret |= 1;
  return ret;
}
#endif // OPERATIONS_UTILS_HPP