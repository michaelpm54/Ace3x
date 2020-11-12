/* SPDX-License-Identifier: GPLv3-or-later */

#include "format-readers/validation-error.hpp"

ValidationError::ValidationError(const std::string &str)
    : std::runtime_error("Validation failed: " + str)
{
}
