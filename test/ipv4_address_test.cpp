// #include <unordered_set>
// #include <gtest/gtest.h>
// #include <htl/ip.h>

// namespace htl::test {

// // struct IPv4AddressTestDataEntry {
// //     std::string_view string_repr;
// //     IPv4Address::bytes_type bytes_repr;
// //     IPv4Address::uint_type uint_repr;
// // };

// // const std::vector<IPv4AddressTestDataEntry> ipv4_address_test_data{
// //     { "0.0.0.0", { 0x00, 0x00, 0x00, 0x00 }, 0x00000000 },
// //     { "0.0.0.1", { 0x00, 0x00, 0x00, 0x01 }, 0x00000001 },
// //     { "0.0.1.1", { 0x00, 0x00, 0x01, 0x01 }, 0x00000101 },
// //     { "0.1.1.1", { 0x00, 0x01, 0x01, 0x01 }, 0x00010101 },
// //     { "1.1.1.1", { 0x01, 0x01, 0x01, 0x01 }, 0x01010101 },
// //     { "255.255.255.255", { 0xFF, 0xFF, 0xFF, 0xFF }, 0xFFFFFFFF },
// //     { "15.71.51.3", { 0x0F, 0x47, 0x33, 0x03 }, 0x0F473303 },
// //     { "87.111.115.97", { 0x57, 0x6F, 0x73, 0x61 }, 0x576F7361 },
// // };

// // const std::vector<std::string_view> ipv4_address_bad_string_data{
// //     "",
// //     "0",
// //     "0.",
// //     "0.0",
// //     "0.0.",
// //     "0.0.0",
// //     "0.0.0.",
// //     "0.0.0.0 ",
// //     "00.0.0.0",
// //     "0.00.0.0",
// //     "0.0.00.0",
// //     "0.0.0.00",
// //     "0.0.0.0.0",
// //     "256.0.0.0",
// //     "0.256.0.0",
// //     "0.0.256.0",
// //     "0.0.256.256",
// //     "613.999.1.33",
// //     "125.51.11.24.55",
// //     "125.51.511.2440",
// //     "125.51.511.244a",
// //     "AF.51.FF.244",
// // };

// // TEST(IPv4AddressTest, BytesConstructor)
// // {
// //     for (auto &data: ipv4_address_test_data) {
// //         IPv4Address address{ data.bytes_repr };

// //         ASSERT_EQ(address.to_bytes(), data.bytes_repr);
// //         ASSERT_EQ(address.to_uint(), data.uint_repr);
// //     }
// // }

// // TEST(IPv4AddressTest, UIntConstructor)
// // {
// //     for (auto &data: ipv4_address_test_data) {
// //         IPv4Address address{ data.uint_repr };

// //         ASSERT_EQ(address.to_bytes(), data.bytes_repr);
// //         ASSERT_EQ(address.to_uint(), data.uint_repr);
// //     }
// // }

// // TEST(IPv4AddressTest, StringConstructor)
// // {
// //     for (auto &data: ipv4_address_test_data) {
// //         IPv4Address address{ data.string_repr };

// //         ASSERT_EQ(address.to_bytes(), data.bytes_repr);
// //         ASSERT_EQ(address.to_uint(), data.uint_repr);
// //     }

// //     for (auto &data: ipv4_address_bad_string_data) {
// //         ASSERT_THROW((IPv4Address(data)), InvalidIPAddress);
// //     }
// // }

// // TEST(IPv4AddressTest, ToString)
// // {
// //     for (auto &data: ipv4_address_test_data) {
// //         IPv4Address address{ data.bytes_repr };
// //         std::string out{ to_string(address) };

// //         ASSERT_EQ(out, data.string_repr);
// //     }
// // }

// // TEST(IPv4AddressTest, Hash)
// // {
// //     std::unordered_set<std::size_t> hashes;
// //     std::hash<IPv4Address> hasher;

// //     for (auto &data: ipv4_address_test_data) {
// //         IPv4Address address{ data.bytes_repr };
// //         std::size_t address_hash{ hasher(address) };

// //         hashes.emplace(address_hash);
// //     }

// //     ASSERT_EQ(hashes.size(), ipv4_address_test_data.size());
// // }

// } // namespace htl::test
