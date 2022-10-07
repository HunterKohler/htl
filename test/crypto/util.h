#ifndef HLIB_TEST_CRYPTO_UTIL_H_
#define HLIB_TEST_CRYPTO_UTIL_H_

#include <vector>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <span>
#include <hlib/ascii.h>
#include <hlib/codec.h>
#include <hlib/json/document.h>
#include <hlib/json/parser.h>

namespace hlib::test {

struct HashTestVector {
    std::vector<std::uint8_t> message;
    std::vector<std::uint8_t> digest;
};

struct HashTestSet {
    std::string algorithm;
    std::vector<HashTestVector> vectors;
};

inline HashTestVector make_hash_test_vector(
    std::string_view message, std::string_view digest)
{
    HashTestVector vec;

    auto r1 = hex_decode(message, std::back_inserter(vec.message));
    auto r2 = hex_decode(digest, std::back_inserter(vec.digest));

    if (r1.err || r2.err || r1.in != message.end() || r2.in != digest.end()) {
        throw std::exception();
    }

    return vec;
}

inline HashTestVector make_hash_test_vector(const json::Object &src)
{
    auto message_pos = src.find("message");
    auto digest_pos = src.find("digest");

    if (message_pos == src.end() || digest_pos == src.end() ||
        !message_pos->second.is_string() || !message_pos->second.is_string()) {
        throw std::exception();
    }

    return make_hash_test_vector(
        message_pos->second.as_string(), digest_pos->second.as_string());
}

inline HashTestVector make_hash_test_vector(const json::Document &src)
{
    if (!src.is_object()) {
        throw std::exception();
    }

    return make_hash_test_vector(src.as_object());
}

inline HashTestSet make_hash_test_set(
    std::string_view algorithm, const json::Array &vectors)
{
    HashTestSet test_set;

    test_set.algorithm = algorithm;

    for (auto &vec: vectors) {
        test_set.vectors.emplace_back(make_hash_test_vector(vec));
    }

    return test_set;
}

inline HashTestSet make_hash_test_set(const json::Object &src)
{
    auto algorithm_pos = src.find("algorithm");
    auto vectors_pos = src.find("vectors");

    if (algorithm_pos == src.end() || vectors_pos == src.end() ||
        !algorithm_pos->second.is_string() || !vectors_pos->second.is_array()) {
        throw std::exception();
    }

    return make_hash_test_set(
        algorithm_pos->second.as_string(), vectors_pos->second.as_array());
}

inline HashTestSet make_hash_test_set(const json::Document &src)
{
    if (!src.is_object()) {
        throw std::exception();
    }

    return make_hash_test_set(src.as_object());
}

inline HashTestSet load_hash_test_set(std::string_view path)
{
    std::fstream ifs((std::string(path)));

    if (!ifs.is_open()) {
        throw std::system_error(
            std::make_error_code(std::errc::no_such_file_or_directory));
    }

    auto result = json::Parser().parse(
        std::istream_iterator<char>(ifs), std::istream_iterator<char>());

    if (result.err) {
        throw std::system_error(result.err);
    }

    return make_hash_test_set(result.value);
}

// struct HashTestVector {
//     std::vector<std::uint8_t> message;
//     std::vector<std::uint8_t> digest;
// };

// struct HashTestSet {
//     std::string algorithm;
//     std::vector<HashTestVector> test_vectors;
// };

// class InvalidHashTestSet : public std::logic_error {
// public:
//     InvalidHashTestSet() : std::logic_error("Invalid hash test data") {}
// };

// inline HashTestSet load_hash_test_set(std::string_view path)
// {
//     HashTestSet ts;
//     std::fstream ifs(path);

//     if (!ifs.is_open()) {
//         throw std::system_error(
//             std::make_error_code(std::errc::no_such_file_or_directory));
//     }

//     try {
//         std::string token;
//         ifs >> token;

//         if (!token.starts_with('[') || !token.ends_with(']') || ifs.eof()) {
//             throw InvalidHashTestSet();
//         }

//         ts.algorithm = token;
//         while (true) {
//             ifs >> token;

//             if (ifs.eof()) {
//                 break;
//             } else if (token != "Message") {
//                 throw InvalidHashTestSet();
//             }
//         }
//     } catch (InvalidHexEncoding) {
//         throw InvalidHashTestSet();
//     }
// }

} // namespace hlib::test

#endif
