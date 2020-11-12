#ifndef ACE3X_FORMAT_READERS_VALIDATION_ERROR_HPP_
#define ACE3X_FORMAT_READERS_VALIDATION_ERROR_HPP_

#include <stdexcept>

class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string &str);
};

#endif    // ACE3X_FORMAT_READERS_VALIDATION_ERROR_HPP_
