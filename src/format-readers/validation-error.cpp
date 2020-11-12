#include "format-readers/validation-error.hpp"

ValidationError::ValidationError(const std::string &str)
    : std::runtime_error("Validation failed: " + str)
{
}
