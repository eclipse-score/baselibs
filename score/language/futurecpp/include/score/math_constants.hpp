/********************************************************************************
 * Copyright (c) 2016 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

///
/// \file
/// \copyright Copyright (c) 2016 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Math.Constants component:
///  The component contains math constants.
///

#ifndef SCORE_LANGUAGE_FUTURECPP_MATH_CONSTANTS_HPP
#define SCORE_LANGUAGE_FUTURECPP_MATH_CONSTANTS_HPP

namespace score::cpp
{
namespace math
{
namespace constants
{
/// \cond detail
namespace detail
{
// half
template <class T>
struct constant_half
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 5.000000000000000000000000000000000000e-01F; }
};
template <>
struct constant_half<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 5.000000000000000000000000000000000000e-01; }
};

// third
template <class T>
struct constant_third
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 3.333333333333333333333333333333333333e-01F; }
};
template <>
struct constant_third<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 3.333333333333333333333333333333333333e-01; }
};

// root_two
template <class T>
struct constant_root_two
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 1.414213562373095048801688724209698078e+00F; }
};
template <>
struct constant_root_two<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 1.414213562373095048801688724209698078e+00; }
};

// root_three
template <class T>
struct constant_root_three
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 1.732050807568877293527446341505872366e+00F; }
};
template <>
struct constant_root_three<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 1.732050807568877293527446341505872366e+00; }
};

// ln_two
template <class T>
struct constant_ln_two
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 6.931471805599453094172321214581765680e-01F; }
};
template <>
struct constant_ln_two<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 6.931471805599453094172321214581765680e-01; }
};

// one_div_root_two
template <class T>
struct constant_one_div_root_two
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 7.071067811865475244008443621048490392e-01F; }
};
template <>
struct constant_one_div_root_two<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 7.071067811865475244008443621048490392e-01; }
};

// pi
template <class T>
struct constant_pi
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 3.141592653589793238462643383279502884e+00F; }
};
template <>
struct constant_pi<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 3.141592653589793238462643383279502884e+00; }
};

// two_pi
template <class T>
struct constant_two_pi
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 6.283185307179586476925286766559005768e+00F; }
};
template <>
struct constant_two_pi<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 6.283185307179586476925286766559005768e+00; }
};

// half_pi
template <class T>
struct constant_half_pi
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 1.570796326794896619231321691639751442e+00F; }
};
template <>
struct constant_half_pi<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 1.570796326794896619231321691639751442e+00; }
};

// quarter_pi
template <class T>
struct constant_quarter_pi
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 0.785398163397448309615660845819875721e+00F; }
};
template <>
struct constant_quarter_pi<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 0.785398163397448309615660845819875721e+00; }
};

// root_pi
template <class T>
struct constant_root_pi
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 1.772453850905516027298167483341145182e+00F; }
};
template <>
struct constant_root_pi<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 1.772453850905516027298167483341145182e+00; }
};

// root_half_pi
template <class T>
struct constant_root_half_pi
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 1.253314137315500251207882642405522626e+00F; }
};
template <>
struct constant_root_half_pi<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 1.253314137315500251207882642405522626e+00; }
};

// root_two_pi
template <class T>
struct constant_root_two_pi
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 2.506628274631000502415765284811045253e+00F; }
};
template <>
struct constant_root_two_pi<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 2.506628274631000502415765284811045253e+00; }
};

// one_div_root_pi
template <class T>
struct constant_one_div_root_pi
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 5.641895835477562869480794515607725858e-01F; }
};
template <>
struct constant_one_div_root_pi<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 5.641895835477562869480794515607725858e-01; }
};

// one_div_root_two_pi
template <class T>
struct constant_one_div_root_two_pi
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 3.989422804014326779399460599343818684e-01F; }
};
template <>
struct constant_one_div_root_two_pi<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 3.989422804014326779399460599343818684e-01; }
};

// e
template <class T>
struct constant_e
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 2.718281828459045235360287471352662497e+00F; }
};
template <>
struct constant_e<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 2.718281828459045235360287471352662497e+00; }
};

// deg_to_rad
template <class T>
struct constant_deg_to_rad
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 1.745329251994329576923690768488612713e-02F; }
};
template <>
struct constant_deg_to_rad<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 1.745329251994329576923690768488612713e-02; }
};

// rad_to_deg
template <class T>
struct constant_rad_to_deg
{
    using value_type = T;

    static constexpr value_type get() noexcept { return 5.729577951308232087679815481410517033e+01F; }
};
template <>
struct constant_rad_to_deg<double>
{
    using value_type = double;

    static constexpr value_type get() noexcept { return 5.729577951308232087679815481410517033e+01; }
};

} // namespace detail
/// \endcond

///
/// \brief Mathematical constant \f$\frac{1}{2}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance,  \code{.cpp}half<int>()\endcode will evaluate to '3'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T half() noexcept
{
    return detail::constant_half<T>::get();
}

///
/// \brief Mathematical constant \f$\frac{1}{3}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance,  \code{.cpp}third<int>()\endcode will evaluate to '3'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T third() noexcept
{
    return detail::constant_third<T>::get();
}

///
/// \brief Mathematical constant \f$\sqrt{2}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance,  \code{.cpp}root_two<int>()\endcode will evaluate to '3'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T root_two() noexcept
{
    return detail::constant_root_two<T>::get();
}

///
/// \brief Mathematical constant \f$\sqrt{3}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance,  \code{.cpp}root_three<int>()\endcode will evaluate to '3'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T root_three() noexcept
{
    return detail::constant_root_three<T>::get();
}

///
/// \brief Mathematical constant \f$\ln{2}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance,  \code{.cpp}ln_two<int>()\endcode will evaluate to '3'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T ln_two() noexcept
{
    return detail::constant_ln_two<T>::get();
}

///
/// \brief Mathematical constant \f$\frac{1}{\sqrt{2}}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance,  \code{.cpp}one_div_root_two<int>()\endcode will evaluate to '3'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T one_div_root_two() noexcept
{
    return detail::constant_one_div_root_two<T>::get();
}

///
/// \brief Mathematical constant \f$\pi\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance,  \code{.cpp}pi<int>()\endcode will evaluate to '3'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T pi() noexcept
{
    return detail::constant_pi<T>::get();
}

///
/// \brief Mathematical constant \f$2\pi\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance, \code{.cpp}two_pi<int>()\endcode will evaluate to '6'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T two_pi() noexcept
{
    return detail::constant_two_pi<T>::get();
}

///
/// \brief Mathematical constant \f$\frac{\pi}{2}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance, \code{.cpp}half_pi<int>()\endcode will evaluate to '1'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T half_pi() noexcept
{
    return detail::constant_half_pi<T>::get();
}

///
/// \brief Mathematical constant \f$\frac{\pi}{4}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance, \code{.cpp}quarter_pi<int>()\endcode will evaluate to '1'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T quarter_pi() noexcept
{
    return detail::constant_quarter_pi<T>::get();
}

///
/// \brief Mathematical constant \f$\sqrt{\pi}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance, \code{.cpp}root_pi<int>()\endcode will evaluate to '1'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T root_pi() noexcept
{
    return detail::constant_root_pi<T>::get();
}

///
/// \brief Mathematical constant \f$\sqrt{\frac{\pi}{2}}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance, \code{.cpp}root_half_pi<int>()\endcode will evaluate to '1'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T root_half_pi() noexcept
{
    return detail::constant_root_half_pi<T>::get();
}

///
/// \brief Mathematical constant \f$\sqrt{2\pi}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance, \code{.cpp}root_two_pi<int>()\endcode will evaluate to '1'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T root_two_pi() noexcept
{
    return detail::constant_root_two_pi<T>::get();
}

///
/// \brief Mathematical constant \f$\frac{1}{\sqrt{\pi}}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance, \code{.cpp}one_div_root_pi<int>()\endcode will evaluate to '1'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T one_div_root_pi() noexcept
{
    return detail::constant_one_div_root_pi<T>::get();
}

///
/// \brief Mathematical constant \f$\frac{1}{\sqrt{2\pi}}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance, \code{.cpp}one_div_root_two_pi<int>()\endcode will evaluate to '1'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T one_div_root_two_pi() noexcept
{
    return detail::constant_one_div_root_two_pi<T>::get();
}

///
/// \brief Mathematical constant \f$e\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost. For instance, \code{.cpp}e<int>()\endcode will evaluate to '2'.
///
/// \tparam T value type
///
/// \asil QM
///
template <class T>
constexpr T e() noexcept
{
    return detail::constant_e<T>::get();
}

} // namespace constants
} // namespace math
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_MATH_CONSTANTS_HPP
