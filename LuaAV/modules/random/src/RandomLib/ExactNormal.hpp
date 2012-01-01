/**
 * \file ExactNormal.hpp
 * \brief Header for ExactNormal
 *
 * Sample exactly from a normal distribution.
 *
 * Copyright (c) Charles Karney (2011) <charles@karney.com> and licensed under
 * the MIT/X11 License.  For more information, see
 * http://randomlib.sourceforge.net/
 **********************************************************************/

#if !defined(RANDOMLIB_EXACTNORMAL_HPP)
#define RANDOMLIB_EXACTNORMAL_HPP \
  "$Id: 1867612ef904ca0dd199ee37f31fd374e9792f78 $"

#include <RandomLib/RandomNumber.hpp>
#include <cmath>

namespace RandomLib {
  /**
   * \brief Sample exactly from an normal distribution.
   *
   * The algorithm is as follows:
   * - Select an integer \e k >= 0 with probability
   *   exp(-<i>k</i>/2) (1-exp(-1/2)).
   * - If \e k > 0, set \e K = floor(log2(<i>k</i>)).
   * - If \e k = 0, accept; if \e k > 0, accept with probability 
   *   exp((\e k - 2<sup>2<i>K</i></sup>) / 2);
   *   otherwise, reject and start over.
   * - Sample a random number \e x uniformly from [0,1).
   * - If \e k = 0, accept with probability exp(-<i>x</i><sup>2</sup>/2);
   *   if \e k > 0, accept with exp(-2<sup>2<i>K</i>+1</sup> \e x (\e x + 2)/4);
   *   otherwise, reject and start over.
   * - If \e k > 0, set \e x = 2<sup><i>K</i></sup> (\e x + 1).
   * - With probability 1/2, negate \e x.
   * - Return \e x.
   * .
   * All these steps can be carried out exactly using RandomNumber.  With \e
   * bits = 1, this consumes 34.1 digits on average and the result has 1.17
   * digits in the fraction.  It takes about 770 ns to generate a result (1520
   * ns, including the time to round it to a double).  By contrast,
   * NormalDistribution takes about 44 ns to generate a double result.
   *
   * For example the following samples from a normal exponential distribution
   * and prints various representations of the result.
   * \code
#include <RandomLib/RandomNumber.hpp>
#include <RandomLib/ExactNormal.hpp>

  RandomLib::Random r;
  const int bits = 1;
  RandomLib::ExactNormal<bits> ndist;
  for (size_t i = 0; i < 10; ++i) {
    RandomLib::RandomNumber<bits> x = ndist(r); // Sample
    std::pair<double, double> z = x.Range();
    std::cout << x << " = "     // Print in binary with ellipsis
              << "(" << z.first << "," << z.second << ")"; // Print range
    double v = x.Value<double>(r); // Round exactly to nearest double
    std::cout << " = " << v << "\n";
  }
\endcode
   * Here's a possible result:
\verbatim
   -0.10... = (-0.75,-0.5) = -0.673475
   10.00110... = (2.1875,2.21875) = 2.21395
   1.0110... = (1.375,1.4375) = 1.42012
   -1.0... = (-1.5,-1) = -1.47799
   1.1010... = (1.625,1.6875) = 1.66709
   -0.11010... = (-0.84375,-0.8125) = -0.81929
   -0.... = (-1,0) = -0.595751
   1.1110... = (1.875,1.9375) = 1.92853
   0.... = (0,1) = 0.398307
   -0.... = (-1,0) = -0.17156
\endverbatim
   * First number is in binary with ... indicating an infinite sequence of
   * random bits.  Second number gives the corresponding interval.  Third
   * number is the result of filling in the missing bits and rounding exactly
   * to the nearest representable double.
   *
   * This class uses some mutable RandomNumber objects.  So a single
   * ExactNormal object cannot safely be used by multiple threads.  In a
   * multi-processing environment, each thread should use a thread-specific
   * ExactNormal object.
   *
   * @tparam bits the number of bits in each digit.
   **********************************************************************/
  template<int bits = 1> class ExactNormal {
  public:
    /**
     * Return a random deviate with a normal distribution of mean 0 and
     * variance 1.  The frequency of bits in fractional part of the returned
     * result with \e bits = 1:\n
\verbatim
     bits freq(%)
      0   43.45
      1   27.15
      2   14.16
      3    7.45
      4    3.85
      5    1.96
      6    0.99
      7    0.50
      8    0.25
      9    0.12
     10+   0.12
\endverbatim
     * The average number of bits in the fraction is 1.169.
     *
     * @tparam Random the type of the random generator.
     * @param[in,out] r a random generator.
     * @return the random sample.
     **********************************************************************/
    template<class Random> RandomNumber<bits> operator()(Random& r) const;
  private:
    /**
     * Return true with probability exp(-1/2).  For \e bits = 1, this consumes,
     * on average, \e t = 2.84525 random digits.  We have \e t = \e a
     * (1-exp(-1/2)) + \e b exp(-1/2), where \e a is the mean bit count for
     * false result = 5.3441 and \e b is the mean bit count for true result =
     * 6.8258.
     **********************************************************************/
    template<class Random> bool ExpProbH(Random& r) const;
    /**
     * Return true with probability exp(-<i>n</i>/2).  For \e bits = 1, this
     * consumes, on average, \e t (1-exp(-<i>n</i>/2))/(1-exp(-1/2)) random
     * digits.  A true result uses \e n \e b random digits.  A false result
     * uses \e a + \e b [exp(-1/2)/(1-exp(-1/2)) -
     * <i>n</i> exp(-<i>n</i>/2)/(1-exp(-<i>n</i>/2))] random digits.
     **********************************************************************/
    template<class Random> bool ExpProb(Random& r, unsigned n) const;
    // Return n with prob 
    /**
     * Return \e n with probability exp(-<i>n</i>/2) (1-exp(-1/2)).  For \e
     * bits = 1, this consumes \e n \e a + \e b random digits if the result is
     * \e n.  Averaging over \e n this becomes (\e b - (\e b - \e a)
     * exp(-1/2))/(1 - exp(-1/2)) digits.
     **********************************************************************/
    template<class Random> unsigned ExpProbN(Random& r) const;
    mutable RandomNumber<bits> _x;
    mutable RandomNumber<bits> _p;
    mutable RandomNumber<bits> _q;
  };

  template<int bits> template<class Random>
  bool ExactNormal<bits>::ExpProbH(Random& r) const {
    // Bit counts
    // ExpProbH: 2.84525 = 1.48902(f) + 1.35623(t)
    //                   = 3.7843 * (1-exp(-0.5)) + 2.236 * exp(-0.5)
    //                 t = a * (1-exp(-1/2)) + b * exp(-1/2)
    // a = mean bit count for false result = 5.3441
    // b = mean bit count for true result = 6.8258
    _p.Init();
    if (_p.Digit(r, 0) & (1u << (bits - 1)))
      return true;
    while (true) {
      _q.Init();
      if (!_q.LessThan(r, _p))
        return false;
      _p.Init();
      if (!_p.LessThan(r, _q))
        return true;
    }
  }

  template<int bits> template<class Random>
  bool ExactNormal<bits>::ExpProb(Random& r, unsigned n) const {
    // Bit counts
    // ExpProb(n): t * (1-exp(-n/2))/(1-exp(-1/2))
    // ExpProb(n) = true: n * b
    // ExpProb(n) = false: a +
    //    b * (exp(-1/2)/(1-exp(-1/2)) - n*exp(-n/2)/(1-exp(-n/2)))
    while (n--) {
      if (!ExpProbH(r))
        return false;
    }
    return true;
  }

  template<int bits> template<class Random>
  unsigned ExactNormal<bits>::ExpProbN(Random& r) const {
    // Bit counts
    // ExpProbN() = n: n * a + b
    unsigned n = 0;
    while (true) {
      if (ExpProbH(r))
        ++n;
      else
        return n;
    }
  }

  template<int bits> template<class Random> RandomNumber<bits>
  ExactNormal<bits>::operator()(Random& r) const {
    // Table of digits consumed and cpu time
    // - rv = RandomNumber
    // - flt = RandomNumber + round to float
    // - dbl = RandomNumber + round to double
    // - ldbl = RandomNumber + round to long double
    //
    //        digits consumed      cpu time, ns/sample
    // bits  rv   flt  dbl ldbl    rv   flt  dbl ldbl
    //   1  34.1 58.4 87.4 98.4    770 1200 1520 1640
    //   2  23.5 35.7 50.2 55.7    580  870  960 1140
    //   3  20.9 29.1 38.7 42.4    520  760  830  830
    //   4  19.9 26.0 33.3 36.0    490  710  750  780
    //   8  18.9 22.0 25.5 27.0    460  640  670  690
    //  16  18.9 20.3 22.3 23.0    460  630  640  650
    //  32  18.9 19.3 20.3 21.0    490  590  660  670
    //
    // Frequency (%) of bits in fractional part of result
    //  0 43.45
    //  1 27.15
    //  2 14.16
    //  3  7.45
    //  4  3.85
    //  5  1.96
    //  6  0.99
    //  7  0.50
    //  8  0.25
    //  9  0.12
    // 10+ 0.12
    // Mean number of bits in result = 1.169
    //
    // Frequency (%) of bits used
    //   4 12.50
    //   6  3.12
    //   7  3.12
    //   8  2.34
    //   9  2.34
    //  10  2.73
    //  11  2.73
    //  12  2.49
    //  13  2.34
    //  14  2.21
    //  15  2.02
    //  16  2.02
    //  17  1.83
    //  18  1.80
    //  19  1.65
    //  2x 13.48
    //  3x  9.87
    //  4x  7.87
    //  5x  6.02
    //  6x  4.48
    //  7x  3.32
    //  8x  2.47
    //  9x  1.84
    // 1xx  5.10
    // 2xx  0.27
    // 3xx  0.01
    //
    while (true) {
      // Executed sqrt(2/pi)/(1-exp(-1/2)) = 2.027818889827955 times on average
      // Sample k from half exponential dist
      // K = k == 0 ? 0 : floor(log2(k))
      // m = k == 0 ? 1 : 2^(2*K+1)
      // bias = k == 0 ? 0 : (2^(2*K) - k)/2
      //   k   K  w  bias
      //   0   0  0   0
      //   1   0 0.5  0
      //   2   1  2   1
      //   3   1  2  0.5
      //   4   2  8   6
      //   5   2  8  5.5
      //   6   2  8   5
      //   7   2  8  4.5
      //   8   3 32  28
      //   9   3 32  27.5
      //  10   3 32  27
      //  11   3 32  26.5
      // ...
      // Prob we get past the initial rejection phase
      // (1 - exp(1/2)) * sum(exp(-n/2) * exp(-bias[n])) = 0.7391491050895925
      unsigned
        // Notionally k is the integer part of the result.  This is not
        // quite true since each range of k in [2^K, 2^(K+1)] is treated
        // together.
        k = ExpProbN(r),
        K = 0,
        m = 2;
      // find K such that k is in [2^K, 2^(K+1)) for k > 0
      // for k == 0, use K = 0.
      while (m <= k) {
        ++K;
        m <<= 1;
      }
      if (ExpProb(r, k == 0 ? 0u : (1u << (2 * K)) - k)) {
        // Prob that this test succeeds is
        // (1 - exp(-1/2)) * sum(exp(-k/2) * exp(-bias[k]), k=0..inf)) 
        //  = (1 - exp(-1/2)) * G = 0.7391491050895928
        // where (1 + sum(2^K * exp(-2^(2*K-1)), K=0..inf) = 1.87854307669757
        // For k == 0, sample from exp(-x^2/2) for x in [0,1].  This succeeds
        // with probability int(exp(-x^2/2),x,0,1) = 0.8556243918921487.
        //
        // For k > 0, find K such that 2^K <= k < 2^(K+1), substitute
        // x' = 2^K * (x + 1) in exp(-x'^2/2), and obtain
        // exp(-2^(2*K-1)) * exp(-2^(2*K+1) * x*(x+2)/4).
        //
        // So sample from exp(-2^(2*K+1) * x*(x+2)/4).
        // This succeeds with probability
        // int(exp(-2^(2*K+1) * x*(x+2)/4),x,0,1) =
        // int(exp(-x^2/2),x,2^K,2^(K+1))*exp(2^(2*K-1))/2^K =
        //
        //    0.561659358806134 for K = 0 (1 <= k < 2)
        //    0.210391313339397 for K = 1 (2 <= k < 4)
        //    0.059163095727228 for K = 2 (4 <= k < 8)
        //    0.015391495407241 for K = 3 (8 <= k < 16)
        //
        // After substituting x' = 2^K * (x + 1), this is equivalent to
        // sampling from exp(-x'^2/2) * exp(2^(2*K-1))/2^K in [2^K,2^(K+1)]

        // Returns a result with prob sqrt(pi/2) / G = 0.6671734882538834
        // otherwise another trip through the outer loop is taken
        _x.Init();
        // Only saves 0.2 bits, on average, so skip...
        // _x.SetFraction(r, k, K);
        unsigned s = 1u;
        m = k == 0 ? 1u : 1u << (2 * K + 1);
        for (unsigned j = 0; j < m; ++j) {
          bool first = true;
          for (s = 1; ; s ^= 1u) {
            _q.Init();
            if (first) {
              _p.Init();
              // p = k == 0 ? 2 * y : 4 * y - 2
              int y = (bits == 1 || k == 0) ? int(r.Boolean()) :
                r.template Integer<int, 2>();
              if (k > 0)
                y += (bits == 1 && r.Boolean() ? 2 : 0) - 2;
              _p.SetInteger(y);
              if (!(_q.LessThan(r, _p) ? _p : _q).LessThan(r,_x))
                break;          // !(max(q, y) < x)
            } else {
              if (!_q.LessThan(r, _p))
                break;
              _p.Init();
              // p = k == 0 ? 2 * y : 4 * y - 2
              int y = (bits == 1 || k == 0) ? int(r.Boolean()) :
                r.template Integer<int, 2>();
              if (k > 0)
                y += (bits == 1 && r.Boolean() ? 2 : 0) - 2;
              _p.SetInteger(y);
              if (!_p.LessThan(r, _x))
                break;
            }
            first = false;
            _p.swap(_q);        // A fast way of doing p = q
          }
          if (s == 0)
            break;
        }
        if (s != 0) {
          if (k > 0) {
            _x.AddInteger(1);
            _x.ShiftLeft(r, K);
          }
          if (r.Boolean())
            _x.Negate();        // Half of the numbers are negative
          return _x;
        }
      }
    }
  }

} // namespace RandomLib

#endif  // RANDOMLIB_EXACTNORMAL_HPP
