// An interval object.
#pragma once

#include <string>

#include <Eigen/Core>
#include <boost/numeric/interval.hpp>

namespace ccd {

namespace interval_options {
    typedef boost::numeric::interval_lib::checking_base<double> CheckingPolicy;
} // namespace interval_options

#if defined(__APPLE__)

// clang-format off
#warning "Rounding modes seem to be broken with trigonometric functions on macOS, unable to compute exact interval arithmetic!"
// clang-format on
typedef boost::numeric::interval<
    double,
    boost::numeric::interval_lib::policies<
        boost::numeric::interval_lib::save_state<
            boost::numeric::interval_lib::rounded_transc_exact<double>>,
        interval_options::CheckingPolicy>>
    Interval;

#else

// Use proper rounding arithmetic
typedef boost::numeric::interval<
    double,
    boost::numeric::interval_lib::policies<
        boost::numeric::interval_lib::save_state<
            boost::numeric::interval_lib::rounded_transc_std<double>>,
        interval_options::CheckingPolicy>>
    Interval;

#endif

} // namespace ccd

namespace Eigen {
typedef Matrix<ccd::Interval, 2, 1> Vector2I;
typedef Matrix<ccd::Interval, 3, 1> Vector3I;
typedef Matrix<ccd::Interval, Dynamic, 1> VectorXI;
typedef Matrix<ccd::Interval, Dynamic, 1, ColMajor, 3, 1> VectorX3I;

#if EIGEN_MAJOR_VERSION >= 3
namespace internal {
    template <typename X, typename S, typename P>
    struct is_convertible<X, boost::numeric::interval<S, P>> {
        enum { value = is_convertible<X, S>::value };
    };

    template <typename S, typename P1, typename P2>
    struct is_convertible<
        boost::numeric::interval<S, P1>,
        boost::numeric::interval<S, P2>> {
        enum { value = true };
    };
} // namespace internal
#endif
} // namespace Eigen