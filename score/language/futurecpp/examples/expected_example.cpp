/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///

#include <cmath>
#include <iostream>
#include <score/expected.hpp>

score::cpp::expected<double, int32_t> compute_sqrt(double arg) noexcept
{
    if (arg < 0.0)
    {
        return score::cpp::make_unexpected(1);
    }
    return sqrt(arg);
}

score::cpp::expected_blank<std::error_code> print_positive_number(double number)
{
    if (number < 0)
    {
        return score::cpp::make_unexpected(std::make_error_code(std::errc::invalid_argument));
    }
    std::cout << "Number is: " << number << std::endl;
    return score::cpp::blank{};
}

int main()
{
    for (double number : {-1.0, 2.0})
    {

        std::cout << "Getting a square root of number " << number << std::endl;
        auto sqrt_result{compute_sqrt(number)};
        if (sqrt_result.has_value())
        {
            std::cout << "Got sqrt_result: " << sqrt_result.value() << std::endl;
        }
        else
        {
            std::cerr << "Got error: " << sqrt_result.error() << std::endl;
        }

        std::cout << "Printing number if positive: " << number << std::endl;
        auto print_result{print_positive_number(number)};
        if (print_result)
        {
            std::cout << "Number printed successfully. " << std::endl;
        }
        else
        {
            std::cerr << "Got error:  " << print_result.error() << std::endl;
        }
    }
    return 0;
}
