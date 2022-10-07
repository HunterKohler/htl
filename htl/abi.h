#ifndef HTL_ABI_H_
#define HTL_ABI_H_

#include <string>
#include <memory>
#include <concepts>
#include <type_traits>
#include <cxxabi.h>
#include <htl/expected.h>

namespace htl::abi {

inline expected<std::string, std::errc> demangle(const char *name)
{
    int status;
    std::unique_ptr<char, decltype([](void *p) { std::free(p); })> result(
        ::abi::__cxa_demangle(name, nullptr, nullptr, std::addressof(status)));

    switch (status) {
    case -1:
        return unexpected(std::errc::not_enough_memory);
    case -2:
    case -3:
        return unexpected(std::errc::invalid_argument);
    }

    return result.get();
}

inline expected<std::string, std::errc> demangle(const std::string &name)
{
    return demangle(name.c_str());
}

} // namespace htl::abi

#endif
