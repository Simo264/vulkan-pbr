export module types;

import std;

export {
  template<typename T>
  concept Arithmetic = std::is_arithmetic_v<T>;

  // --- Signed integers ---
  using i8  = std::int8_t;
  using i16 = std::int16_t;
  using i32 = std::int32_t;
  using i64 = std::int64_t;

  // --- Unsigned integers ---
  using u8  = std::uint8_t;
  using u16 = std::uint16_t;
  using u32 = std::uint32_t;
  using u64 = std::uint64_t;

  // --- Floating point ---
  using f32 = float;
  using f64 = double;

  // --- Size types ---
  using usize = std::size_t;
  using isize = std::ptrdiff_t;

  constexpr f32 MIN_FLOAT_32 = std::numeric_limits<f32>::min();
  constexpr f32 MAX_FLOAT_32 = std::numeric_limits<f32>::max();

  constexpr f64 MIN_FLOAT_64 = std::numeric_limits<f64>::min();
  constexpr f64 MAX_FLOAT_64 = std::numeric_limits<f64>::max();

  constexpr i32 MIN_INT_32 = std::numeric_limits<i32>::min();
  constexpr i32 MAX_INT_32 = std::numeric_limits<i32>::max();

  constexpr i64 MIN_INT_64 = std::numeric_limits<i64>::min();
  constexpr i64 MAX_INT_64 = std::numeric_limits<i64>::max();

  constexpr u64 MAX_UINT_64 = std::numeric_limits<u64>::max();
  constexpr u32 MAX_UINT_32 = std::numeric_limits<u32>::max();
};
