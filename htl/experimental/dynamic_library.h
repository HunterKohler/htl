#ifndef HTL_DYNAMIC_LIBRARY_H_
#define HTL_DYNAMIC_LIBRARY_H_

#include <concepts>
#include <cstdint>
#include <ranges>
#include <stdexcept>
#include <string>
#include <dlfcn.h>
#include <htl/detail/bitmask.h>

namespace htl {

class DynamicLibraryError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
    using std::runtime_error::operator=;
};

enum class DynamicLibraryOptions {
    Lazy = RTLD_LAZY,
    Now = RTLD_NOW,
    Global = RTLD_GLOBAL,
    Local = RTLD_LOCAL,
    NoDelete = RTLD_NODELETE,
    NoLoad = RTLD_NOLOAD,
#ifdef RTLD_DEEPBIND
    DeepBind = RTLD_DEEPBIND,
#endif
#ifdef RTLD_FIRST
    First = RTLD_FIRST,
#endif
};

constexpr DynamicLibraryOptions operator&(
    DynamicLibraryOptions x, DynamicLibraryOptions y) noexcept
{
    return detail::bitmask_and(x, y);
}

constexpr DynamicLibraryOptions operator|(
    DynamicLibraryOptions x, DynamicLibraryOptions y) noexcept
{
    return detail::bitmask_or(x, y);
}

constexpr DynamicLibraryOptions operator^(
    DynamicLibraryOptions x, DynamicLibraryOptions y) noexcept
{
    return detail::bitmask_xor(x, y);
}

constexpr DynamicLibraryOptions operator~(DynamicLibraryOptions x) noexcept
{
    return detail::bitmask_not(x);
}

constexpr DynamicLibraryOptions &operator&=(
    DynamicLibraryOptions &x, DynamicLibraryOptions y) noexcept
{
    return x = x & y;
}

constexpr DynamicLibraryOptions &operator|=(
    DynamicLibraryOptions &x, DynamicLibraryOptions y) noexcept
{
    return x = x | y;
}

constexpr DynamicLibraryOptions &operator^=(
    DynamicLibraryOptions &x, DynamicLibraryOptions y) noexcept
{
    return x = x ^ y;
}

class DynamicLibrary {
public:
    using native_handle_type = void *;

    DynamicLibrary() noexcept : _handle() {}

    explicit DynamicLibrary(
        const char *path, DynamicLibraryOptions options = {})
        : _handle(_open(path, options))
    {}

    explicit DynamicLibrary(
        const std::string &path, DynamicLibraryOptions options = {})
        : DynamicLibrary(path.c_str(), options)
    {}

    DynamicLibrary(const DynamicLibrary &) = delete;

    DynamicLibrary(DynamicLibrary &&other) noexcept
        : _handle(std::exchange(other._handle, nullptr))
    {}

    DynamicLibrary &operator=(const DynamicLibrary &) = delete;

    DynamicLibrary &operator=(DynamicLibrary &&other)
    {
        reset();
        _handle = other._handle;
        return *this;
    }

    ~DynamicLibrary()
    {
        reset();
    }

    void reset()
    {
        if (_handle) {
            _close(_handle);
            _handle = nullptr;
        }
    }

    void reset(const char *path, DynamicLibraryOptions options = {})
    {
        reset();
        _handle = _open(path, options);
    }

    void reset(const std::string &path, DynamicLibraryOptions options = {})
    {
        reset(path.c_str(), options);
    }

    bool has_symbol(std::nullptr_t) const = delete;

    bool has_symbol(const char *name) const
    {
        dlerror();
        dlsym(_handle, name);
        return !dlerror();
    }

    bool has_symbol(const std::string &name) const
    {
        return has_symbol(name.c_str());
    }

    template <class T = void>
    T *get_symbol(std::nullptr_t) const = delete;

    template <class T = void>
    T *get_symbol(const char *name) const
    {
        ::dlerror();
        void *sym = ::dlsym(_handle, name);
        const char *msg = ::dlerror();
        if (msg) {
            throw DynamicLibraryError(msg);
        }
        return sym;
    }

    template <class T = void>
    T *get_symbol(const std::string &name) const
    {
        return get_symbol(name.c_str());
    }

    const char *pathname() const
    {
        ::Dl_info info;
        if (::dladdr(_handle, std::addressof(info))) {
            throw DynamicLibraryError(::dlerror());
        }
        return info.dli_fname;
    }

    [[nodiscard]] native_handle_type native_handle() const noexcept
    {
        return _handle;
    }

    [[nodiscard]] bool loaded() const noexcept
    {
        return _handle;
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return _handle;
    }

    friend bool operator==(
        const DynamicLibrary &a, const DynamicLibrary &b) noexcept = default;

private:
    native_handle_type _handle;

    static void *_open(const char *path, DynamicLibraryOptions options)
    {
        if (static_cast<bool>(~options & DynamicLibraryOptions::Now)) {
            options |= DynamicLibraryOptions::Lazy;
        }

        if (static_cast<bool>(~options & DynamicLibraryOptions::Global)) {
            options |= DynamicLibraryOptions::Local;
        }

        void *handle = ::dlopen(path, static_cast<int>(options));

        if (!handle) {
            throw DynamicLibraryError(::dlerror());
        }

        return handle;
    }

    static void _close(void *handle)
    {
        if (::dlclose(handle)) {
            throw DynamicLibraryError(::dlerror());
        }
    }
};

} // namespace htl

#endif
