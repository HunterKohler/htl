
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <hlib/crypto/sha1.h>
#include <hlib/json/document.h>
#include <hlib/json/parser.h>
#include <gtest/gtest.h>
#include "./util.h"

namespace hlib::test {

TEST(SHA1Test, Hash)
{
    auto test_set =
        load_hash_test_set("./test/crypto/data/sha1/sha1-short-msg.json");

    std::cout << "algorithm: " << data.algorithm << "\n";

    for (auto &elem: data.vectors) {
        std::cout << "message.size(): " << elem.message.size() << "\n";
        std::cout << "digest.size(): " << elem.digest.size() << "\n";
        std::cout << "\n";
    }

    // HashTestSet testset;
    // json::BasicParser parser;
    // json::Document data;
    // std::ifstream ifs("./test/crypto/data/sha1/sha1-short-msg.json");

    // auto result = parser.parse(
    //     std::istream_iterator<char>(ifs), std::istream_iterator<char>());

    // auto &root = result.value.as_object();
    // auto &vectors = root["vectors"].as_array();

    // testset.algorithm = root["algorithm"].as_string();

    // std::cout << root << "\n";

    // for (auto &vec: vectors) {
    //     auto &dest = testset.vectors.emplace_back();

    //     hex_decode(vec.as_object()["message"].as_string(),
    //                std::back_inserter(dest.message));
    //     hex_decode(vec.as_object()["digest"].as_string(),
    //                std::back_inserter(dest.digest));

    //     std::cout << vec << "\n";
    // }
}

// TEST(SHA1Test, Hash)
// {
//     std::vector<std::uint8_t> dest(SHA1::digest_size);
//     CryptoTestData data =
//         load_crypto_test_data("./test/crypto/data/sha1/SHA1ShortMsg.rsp");

//     for (auto &elem: data.messages) {
//         auto &message = elem.message;
//         auto &digest = elem.digest;

//         SHA1::hash(message.data(), message.size(), dest.data());

//         ASSERT_EQ(digest, dest);
//     }
// }

} // namespace hlib::test
