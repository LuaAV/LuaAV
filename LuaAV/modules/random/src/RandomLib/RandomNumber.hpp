/**
 * \file RandomNumber.hpp
 * \brief Header for RandomNumber
 *
 * Infinite precision random numbers.
 *
 * Copyright (c) Charles Karney (2006-2011) <charles@karney.com> and licensed
 * under the MIT/X11 License.  For more information, see
 * http://randomlib.sourceforge.net/
 **********************************************************************/

#if !defined(RANDOMLIB_RANDOMNUMBER_HPP)
#define RANDOMLIB_RANDOMNUMBER_HPP \
  "$Id: 823bcf9f94e5b466b6fda273d1b0b6da0f97496a $"

#include <vector>
#include <iomanip>
#include <limits>
#include <cmath>                // for std::pow

#if !defined(STATIC_ASSERT)
/**
 * A simple compile-time assert.
 **********************************************************************/
#define STATIC_ASSERT(cond,reason) { enum{ STATIC_ASSERT_ENUM = 1/int(cond) }; }
#endif

namespace RandomLib {
  /**
   * \brief Infinite precision random numbers.
   *
   * Implement infinite precision random numbers.  Integer part is non-random.
   * Fraction part consists of any some number of digits in base
   * 2<sup><i>b</i></sup>.  If \e m digits have been generated then the
   * fraction is uniformly distributed in the open interval
   * sum<sub><i>k</i>=1</sub><sup><i>m</i></sup>
   * <i>f</i><sub><i>k</i>-1</sub>/2<sup><i>kb</i></sup> +
   * (0,1)/2<sup><i>mb</i></sup>.  When a RandomNumber is first constructed the
   * integer part is zero and \e m = 0, and the number represents (0,1).  A
   * RandomNumber is able to represent all numbers in the symmetric open
   * interval (-2<sup>31</sup>, 2<sup>31</sup>).  In this implementation, \e b
   * must one of 1, 2, 3, 4, 8, 12, 16, 20, 24, 28, or 32.  (This restriction
   * allows printing in hexadecimal and can easily be relaxed.  There's also no
   * essential reason why the base should be a power of 2.)
   *
   * @tparam bits the number of bits in each digit.
   **********************************************************************/
  template<int bits = 1> class RandomNumber {
  public:
    /**
     * Constructor sets number to a random number uniformly distributed in
     * \e n + (0,1).
     *
     * @param[in] n the integer part of the number (default 0).
     **********************************************************************/
    RandomNumber(int n = 0) : _n(n) {}
    /**
     * @return the integer part of RandomNumber.
     **********************************************************************/
    int Integer() const { return _n; }
    /**
     * @tparam Random the type of the random generator.
     * @param[in,out] r a random generator.
     * @param[in] k the index of a digit of the fraction
     * @return digit number \e k, generating it if necessary.
     **********************************************************************/
    template<class Random> unsigned Digit(Random& r, unsigned k) {
      ExpandTo(r, k + 1);
      return _f[k];
    }
    /**
     * @tparam Random the type of the random generator.
     * @param[in] k the index of a digit of the fraction
     * @return digit number \e k, without generating new digits.
     **********************************************************************/
    unsigned RawDigit(unsigned k) const {
      return _f.at(k);
    }
    /**
     * Add integer \e k to RandomNumber.
     *
     * @param[in] k the integer to add.
     **********************************************************************/
    void AddInteger(int k) { _n += k; }
    /**
     * Set integer part of RandomNumber to \e k
     *
     * @param[in] k the new integer part.
     **********************************************************************/
    void SetInteger(int k) { _n = k; }
    /**
     * Set fraction part of RandomNumber to \e n 2<sup><i>-k</i></sup>; only th
     * e least significant \e k bits of \e n are used.
     *
     * @tparam Random the type of the random generator.
     * @param[in,out] r a random generator.
     * @param[in] n the bits in the fraction
     * @param[in] k the number of bits in the fraction
     **********************************************************************/
    template<class Random> void SetFraction(Random& r,
                                            unsigned n, unsigned k) {
      _f.clear();
      unsigned
        m = (k + bits - 1) / bits, // Number of digits
        j = m * bits - k,          //  Number of extra bits needed
        u = j > 0 ? (Digit(r, 0) >> (bits - j)) : 0; // The extra bits
      n = (n << j) + u;
      _f.resize(m);
      while (m--) {
        _f[m] = n & mask;
        n >>= bits < w ? bits : 0;
      }
    }
    /**
     * Shift the number left \e k bits (\e k >= 0)
     *
     * @tparam Random the type of the random generator.
     * @param[in,out] r a random generator.
     * @param[in] k the amount to shift by.
     **********************************************************************/
    template<class Random> void ShiftLeft(Random& r, unsigned k) {
      unsigned wshift = k / bits; // Number of whole digits to shift
      if (wshift > 0) {
        for (unsigned j = 0; j < wshift; ++j) {
          _n <<= bits < w ? bits : 0; // Avoid warning when bits == w
          _n += Digit(r, j);
        }
        for (unsigned j = wshift; j < Size(); ++j)
          _f[j - wshift] = _f[j];
        _f.resize(Size() - wshift);
        k -= wshift * bits;       // Remaining bits to shift
      }
      if (k > 0) {
        _n <<= k;
        _n += Digit(r, 0) >> (bits - k);
        for (unsigned j = 0; j < Size(); ++j)
          _f[j] = (mask & (_f[j] << k)) |
            ((j + 1 < Size() ? _f[j + 1] : unsigned(r.template Integer<bits>()))
             >> (bits - k)) ;
      }
    }
    /**
     * Swap with another RandomNumber
     *
     * @param[in,out] t the RandomNumber to swap with.
     **********************************************************************/
    void swap(RandomNumber& t) throw() {
      std::swap(_n, t._n);
      _f.swap(t._f);
    }
    /**
     * Return to initial state, uniformly distributed in \e n + (0,1).
     *
     * @param[in] n the integer part of the number (default 0).
     **********************************************************************/
    void Init(int n = 0) {
      STATIC_ASSERT(bits > 0 && bits <= w && (bits < 4 || bits % 4 == 0),
                    "RandomNumber: unsupported value for bits");
      _n = n;
      _f.clear();
    }
    /**
     * Compare two RandomNumbers, *this < \e t
     *
     * @tparam Random the type of the random generator.
     * @param[in,out] r a random generator.
     * @param[in,out] t a RandomNumber to compare.
     * @return true if *this < \e t
     **********************************************************************/
    template<class Random> bool LessThan(Random& r, RandomNumber& t) {
      if (this == &t)
        return false;           // same object
      if (_n != t._n)
        return _n < t._n;
      for (unsigned k = 0; ; ++k) {
        // Impose an order on the evaluation of the digits.
        const unsigned x = Digit(r,k);
        const unsigned y = t.Digit(r,k);
        if (x != y)
          return x < y;
        // Two distinct numbers are never equal
      }
    }
    /**
     * Change sign of a RandomNumber
     **********************************************************************/
    void Negate() {
      for (size_t k = 0; k < Size(); ++k)
        _f[k] = ~_f[k] & mask;
      // Need the -1 shift because the fraction part is always positive
      _n = - _n - 1;
    }
    /**
     * @return the number of digits in fraction
     **********************************************************************/
    unsigned Size() const {
      return unsigned(_f.size());
    }
    /**
     * Return the fraction part of the RandomNumber as a floating point number
     * of type RealType rounded to the nearest multiple of
     * 1/2<sup><i>p</i></sup>, where \e p =
     * std::numeric_limits<RealType>::digits, and, if necessary, creating
     * additional digits of the number.
     *
     * @tparam RealType the floating point type to convert to.
     * @tparam Random the type of the random generator.
     * @param[in,out] r a random generator for generating the necessary digits.
     * @return the fraction of the RandomNumber rounded to a RealType.
     **********************************************************************/
    template<typename RealType, typename Random> RealType Fraction(Random& r) {
      STATIC_ASSERT(!std::numeric_limits<RealType>::is_integer,
                    "RandomNumber::Fraction: invalid real type RealType");
      const int d = std::numeric_limits<RealType>::digits;
      const int k = (d + bits - 1)/bits;
      const int kg = (d + bits)/bits; // For guard bit
      RealType y = 0;
      if (Digit(r, kg - 1) & (1U << (kg * bits - d - 1)))
        // if guard bit is set, round up.
        y += std::pow(RealType(2), -d);
      const RealType fact = std::pow(RealType(2), -bits);
      RealType mult = RealType(1);
      for (int i = 0; i < k; ++i) {
        mult *= fact;
        y += mult * RealType(i < k - 1 ? RawDigit(i) :
                             RawDigit(i) & (~0U << (k * bits - d)));
      }
      return y;
    }
    /**
     * Return the value of the RandomNumber rounded to nearest floating point
     * number of type RealType and, if necessary, creating additional digits of
     * the number.
     *
     * @tparam RealType the floating point type to convert to.
     * @tparam Random the type of the random generator.
     * @param[in,out] r a random generator for generating the necessary digits.
     * @return the value of the RandomNumber rounded to a RealType.
     **********************************************************************/
    template<typename RealType, class Random> RealType Value(Random& r) {
      // Ignore the possibility of overflow here (OK because int doesn't
      // currently overflow any real type).  Assume the real type supports
      // denormalized numbers.  Need to treat rounding explicitly since the
      // missing digits always imply rounding up.
      STATIC_ASSERT(!std::numeric_limits<RealType>::is_integer,
                    "RandomNumber::Value: invalid real type RealType");
      const int digits = std::numeric_limits<RealType>::digits,
        min_exp = std::numeric_limits<RealType>::min_exponent;
      const bool negative = Integer() < 0;
      if (negative)
        Negate();
      unsigned n = Integer();
      int lead;                 // Position of leading bit (0.5 = position 0)
      if (n)
        lead = highest_bit_idx(n);
      else {
        int i = 0;
        while ( Digit(r, i) == 0 && i < (-min_exp)/bits )
          ++i;
        lead = highest_bit_idx(RawDigit(i)) - (i + 1) * bits;
        // To handle denormalized numbers set lead = max(lead, min_exp)
        lead = lead > min_exp ? lead : min_exp;
      }
      int trail = lead - digits; // Position of guard bit (0.5 = position 0)
      RealType y;
      if (trail > 0) {
        y = RealType(n & (~0U << trail));
        if (n & (1U << (trail - 1)))
          y += std::pow(RealType(2), trail);
      } else {
        y = RealType(n);
        int k = (-trail)/bits;  // Byte with guard bit
        if (Digit(r, k) & (1U << ((k + 1) * bits + trail - 1)))
          y += std::pow(RealType(2), trail);
        // Byte with trailing bit (can be negative)
        k = (-trail - 1 + bits)/bits - 1;
        const RealType fact = std::pow(RealType(2), -bits);
        RealType mult = RealType(1);
        for (int i = 0; i <= k; ++i) {
          mult *= fact;
          y += mult * RealType(i < k ? RawDigit(i) :
                               RawDigit(i) & (~0U << ((k + 1) * bits + trail)));
        }
      }
      if (negative) {
        Negate();
        y *= -1;
      }
      return y;
    }
    /**
     * Return the range of possible values for the RandomNumber as pair of
     * doubles.  This doesn't create any additional digits of the result and
     * doesn't try to control roundoff.
     *
     * @return a pair denoting the range with first being the lower limit and
     *   second being the upper limit.
     **********************************************************************/
    std::pair<double, double> Range() const {
      double y = Integer();
      const double fact = std::pow(double(2), -bits);
      double mult = double(1);
      for (unsigned i = 0; i < Size(); ++i) {
        mult *= fact;
        y += mult * RawDigit(i);
      }
      return std::pair<double, double>(y, y + mult);
    }

  private:
    /**
     * The integer part
     **********************************************************************/
    int _n;
    /**
     * The fraction part
     **********************************************************************/
    std::vector<unsigned> _f;
    /**
     * Fill RandomNumber to \e k digits.
     **********************************************************************/
    template<class Random> void ExpandTo(Random& r, size_t k) {
      size_t l = _f.size();
      if (k <= l)
        return;
      _f.resize(k);
      for (size_t i = l; i < k; ++i)
        _f[i] = unsigned(r.template Integer<bits>());
    }
    /**
     * Return index [0..32] of highest bit set.  Return 0 if x = 0, 32 is if x
     * = ~0.  (From Algorithms for programmers by Joerg Arndt.)
     **********************************************************************/
    static int highest_bit_idx(unsigned x) {
      if (x == 0) return 0;
      int r = 1;
      if (x & 0xffff0000U) { x >>= 16; r += 16; }
      if (x & 0x0000ff00U) { x >>=  8; r +=  8; }
      if (x & 0x000000f0U) { x >>=  4; r +=  4; }
      if (x & 0x0000000cU) { x >>=  2; r +=  2; }
      if (x & 0x00000002U) {           r +=  1; }
      return r;
    }
    /**
     * The number of bits in unsigned.
     **********************************************************************/
    static const int w = std::numeric_limits<unsigned>::digits;
  public:
    /**
     * A mask for the digits.
     **********************************************************************/
    static const unsigned mask =
      bits == w ? ~0U : ~(~0U << (bits < w ? bits : 0));
  };

  /**
   * \relates RandomNumber
   * Print a RandomNumber.  Format is n.dddd... where the base for printing is
   * 2<sup>max(4,<i>b</i>)</sup>.  The ... represents an infinite sequence of
   * ungenerated random digits (uniformly distributed).  Thus with \e b = 1,
   * 0.0... = (0,1/2), 0.00... = (0,1/4), 0.11... = (3/4,1), etc.
   **********************************************************************/
  template<int bits>
  std::ostream& operator<<(std::ostream& os, const RandomNumber<bits>& n) {
    const std::ios::fmtflags oldflags = os.flags();
    RandomNumber<bits> t = n;
    unsigned i;
    if (t.Integer() < 0) {
      os << "-";
      t.Negate();
    }
    i = unsigned(t.Integer());
    os << std::hex  << std::setfill('0');
    if (i == 0)
      os << "0";
    else {
      bool first = true;
      const int w = std::numeric_limits<unsigned>::digits;
      const unsigned mask = RandomNumber<bits>::mask;
      for (int s = ((w + bits - 1)/bits) * bits - bits; s >= 0; s -= bits) {
        unsigned d = mask & (i >> s);
        if (d || !first) {
          if (first) {
            os << d;
            first = false;
          }
          else
            os << std::setw((bits+3)/4) << d;
        }
      }
    }
    os << ".";
    unsigned s = t.Size();
    for (unsigned i = 0; i < s; ++i)
      os << std::setw((bits+3)/4) << t.RawDigit(i);
    os << "..." << std::setfill(' ');
    os.flags(oldflags);
    return os;
  }
} // namespace RandomLib

#endif  // RANDOMLIB_RANDOMNUMBER_HPP
