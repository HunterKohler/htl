#include <gtest/gtest.h>
#include <htl/codec.h>

namespace htl {

struct CodecTestCase {
    std::vector<std::uint8_t> data;
    std::string_view bin;
    std::string_view hex;
    std::string_view base64;
    std::string_view base64url;
};

const std::vector<CodecTestCase> test_cases{
    {
        .data = {},
        .bin = "",
        .hex = "",
        .base64 = "",
        .base64url = "",
    },
    {
        .data = { 0xEB },
        .bin = "11101011",
        .hex = "EB",
        .base64 = "6w==",
        .base64url = "6w==",
    },
    {
        .data = { 0x3B, 0xF3 },
        .bin = "0011101111110011",
        .hex = "3BF3",
        .base64 = "O/M=",
        .base64url = "O_M=",
    },
    {
        .data = { 0xE7, 0x3F, 0x2B },
        .bin = "111001110011111100101011",
        .hex = "E73F2B",
        .base64 = "5z8r",
        .base64url = "5z8r",
    },
    {
        .data = { 0xCF, 0x83, 0xB8, 0x76 },
        .bin = "11001111100000111011100001110110",
        .hex = "CF83B876",
        .base64 = "z4O4dg==",
        .base64url = "z4O4dg==",
    },
    {
        .data = { 0x81, 0x69, 0x0E, 0xD3, 0x11 },
        .bin = "1000000101101001000011101101001100010001",
        .hex = "81690ED311",
        .base64 = "gWkO0xE=",
        .base64url = "gWkO0xE=",
    },
    {
        .data = { 0xD8, 0xEE, 0x66, 0xA6, 0x52, 0x16 },
        .bin = "110110001110111001100110101001100101001000010110",
        .hex = "D8EE66A65216",
        .base64 = "2O5mplIW",
        .base64url = "2O5mplIW",
    },
    {
        .data = { 0xF6, 0xA1, 0xDC, 0x3B, 0x17, 0xDA, 0x81 },
        .bin = "11110110101000011101110000111011000101111101101010000001",
        .hex = "F6A1DC3B17DA81",
        .base64 = "9qHcOxfagQ==",
        .base64url = "9qHcOxfagQ==",
    },
    {
        .data = { 0x59, 0x67, 0xCC, 0x3C, 0xFA, 0xE6, 0xFF, 0xE6 },
        .bin =
            "0101100101100111110011000011110011111010111001101111111111100110",
        .hex = "5967CC3CFAE6FFE6",
        .base64 = "WWfMPPrm/+Y=",
        .base64url = "WWfMPPrm_-Y=",
    },
    {
        .data = { 0x76, 0xEF, 0x90, 0x64, 0x36, 0x1C, 0x64, 0xD9, 0xE0 },
        .bin =
            "01110110111011111001000001100100001101100001110001100100110110011"
            "1100000",
        .hex = "76EF9064361C64D9E0",
        .base64 = "du+QZDYcZNng",
        .base64url = "du-QZDYcZNng",
    },
    {
        .data = { 0x49, 0x06, 0x1C, 0x00, 0xCB, 0x9D, 0x26, 0x13, 0x62, 0x28 },
        .bin =
            "01001001000001100001110000000000110010111001110100100110000100110"
            "110001000101000",
        .hex = "49061C00CB9D26136228",
        .base64 = "SQYcAMudJhNiKA==",
        .base64url = "SQYcAMudJhNiKA==",
    },
    {
        .data = { 0xE2, 0x49, 0x99, 0x87, 0xEA, 0xEB, 0x9B, 0xC8, 0x37, 0xA5,
                  0xB6 },
        .bin =
            "11100010010010011001100110000111111010101110101110011011110010000"
            "01101111010010110110110",
        .hex = "E2499987EAEB9BC837A5B6",
        .base64 = "4kmZh+rrm8g3pbY=",
        .base64url = "4kmZh-rrm8g3pbY=",
    },
    {
        .data = { 0xD7, 0x96, 0xAE, 0xE3, 0x2E, 0xE0, 0xEE, 0x8C, 0xCD, 0x02,
                  0xF7, 0xED },
        .bin =
            "11010111100101101010111011100011001011101110000011101110100011001"
            "1001101000000101111011111101101",
        .hex = "D796AEE32EE0EE8CCD02F7ED",
        .base64 = "15au4y7g7ozNAvft",
        .base64url = "15au4y7g7ozNAvft",
    },
    {
        .data = { 0x14, 0x64, 0x99, 0x72, 0x45, 0x8C, 0x2D, 0xB6, 0x3B, 0xE1,
                  0x9E, 0x34, 0x50 },
        .bin =
            "00010100011001001001100101110010010001011000110000101101101101100"
            "011101111100001100111100011010001010000",
        .hex = "14649972458C2DB63BE19E3450",
        .base64 = "FGSZckWMLbY74Z40UA==",
        .base64url = "FGSZckWMLbY74Z40UA==",
    },
    {
        .data = { 0x2E, 0xF3, 0x52, 0x81, 0x07, 0x27, 0x78, 0x56, 0x79, 0xF1,
                  0x83, 0x57, 0xD9, 0x3D },
        .bin =
            "00101110111100110101001010000001000001110010011101111000010101100"
            "11110011111000110000011010101111101100100111101",
        .hex = "2EF352810727785679F18357D93D",
        .base64 = "LvNSgQcneFZ58YNX2T0=",
        .base64url = "LvNSgQcneFZ58YNX2T0=",
    },
    {
        .data = { 0x91, 0x9D, 0x1F, 0x68, 0x1C, 0x79, 0x0F, 0x86, 0x58, 0x16,
                  0x47, 0x1B, 0x05, 0x49, 0xBD },
        .bin =
            "10010001100111010001111101101000000111000111100100001111100001100"
            "1011000000101100100011100011011000001010100100110111101",
        .hex = "919D1F681C790F865816471B0549BD",
        .base64 = "kZ0faBx5D4ZYFkcbBUm9",
        .base64url = "kZ0faBx5D4ZYFkcbBUm9",
    },
    {
        .data = { 0x17, 0xD5, 0x78, 0xBB, 0xCC, 0xC5, 0x2B, 0x31, 0x00, 0x78,
                  0xC6, 0xB6, 0xE6, 0x41, 0xC1, 0x5E },
        .bin =
            "00010111110101010111100010111011110011001100010100101011001100010"
            "000000001111000110001101011011011100110010000011100000101011110",
        .hex = "17D578BBCCC52B310078C6B6E641C15E",
        .base64 = "F9V4u8zFKzEAeMa25kHBXg==",
        .base64url = "F9V4u8zFKzEAeMa25kHBXg==",
    },
    {
        .data = { 0xD0, 0x04, 0xE3, 0x78, 0x5B, 0x0F, 0xCA, 0xAF, 0x4B, 0x72,
                  0x55, 0x6A, 0x88, 0x22, 0x49, 0x56, 0xB2 },
        .bin =
            "11010000000001001110001101111000010110110000111111001010101011110"
            "10010110111001001010101011010101000100000100010010010010101011010"
            "110010",
        .hex = "D004E3785B0FCAAF4B72556A88224956B2",
        .base64 = "0ATjeFsPyq9LclVqiCJJVrI=",
        .base64url = "0ATjeFsPyq9LclVqiCJJVrI=",
    },
    {
        .data = { 0xF2, 0xAB, 0x44, 0x06, 0xF1, 0x0C, 0x98, 0xDA, 0x67, 0xB0,
                  0x7D, 0xF4, 0x35, 0x92, 0x6F, 0x66, 0xE0, 0x14 },
        .bin =
            "11110010101010110100010000000110111100010000110010011000110110100"
            "11001111011000001111101111101000011010110010010011011110110011011"
            "10000000010100",
        .hex = "F2AB4406F10C98DA67B07DF435926F66E014",
        .base64 = "8qtEBvEMmNpnsH30NZJvZuAU",
        .base64url = "8qtEBvEMmNpnsH30NZJvZuAU",
    },
    {
        .data = { 0x6B, 0x8B, 0x97, 0xD0, 0xCC, 0x24, 0x0F, 0xD1, 0x30, 0xCD,
                  0xC7, 0xA1, 0x88, 0xBD, 0x57, 0xC8, 0x62, 0x8F, 0x77 },
        .bin =
            "01101011100010111001011111010000110011000010010000001111110100010"
            "01100001100110111000111101000011000100010111101010101111100100001"
            "1000101000111101110111",
        .hex = "6B8B97D0CC240FD130CDC7A188BD57C8628F77",
        .base64 = "a4uX0MwkD9EwzcehiL1XyGKPdw==",
        .base64url = "a4uX0MwkD9EwzcehiL1XyGKPdw==",
    },
    {
        .data = { 0x3F, 0x46, 0x77, 0x33, 0x97, 0xED, 0xB0, 0x3F, 0x34, 0x6E,
                  0x7F, 0x5B, 0x83, 0x02, 0x10, 0xFF, 0xD7, 0x83, 0x7A, 0x10 },
        .bin =
            "00111111010001100111011100110011100101111110110110110000001111110"
            "01101000110111001111111010110111000001100000010000100001111111111"
            "010111100000110111101000010000",
        .hex = "3F46773397EDB03F346E7F5B830210FFD7837A10",
        .base64 = "P0Z3M5ftsD80bn9bgwIQ/9eDehA=",
        .base64url = "P0Z3M5ftsD80bn9bgwIQ_9eDehA=",
    },
    {
        .data = { 0x06, 0x01, 0x7A, 0x74, 0x10, 0x56, 0x6B,
                  0xD5, 0x72, 0x64, 0x1A, 0x22, 0x24, 0x98,
                  0x09, 0x73, 0xEE, 0x7C, 0xD2, 0x4A, 0xD3 },
        .bin =
            "00000110000000010111101001110100000100000101011001101011110101010"
            "11100100110010000011010001000100010010010011000000010010111001111"
            "10111001111100110100100100101011010011",
        .hex = "06017A7410566BD572641A2224980973EE7CD24AD3",
        .base64 = "BgF6dBBWa9VyZBoiJJgJc+580krT",
        .base64url = "BgF6dBBWa9VyZBoiJJgJc-580krT",
    },
    {
        .data = { 0x4B, 0x20, 0xCF, 0xDE, 0xC1, 0xD5, 0x3F, 0xD2,
                  0x77, 0xDE, 0x3C, 0x1B, 0x95, 0xBD, 0x88, 0x8E,
                  0xC0, 0x65, 0x13, 0x69, 0x40, 0xDE },
        .bin =
            "01001011001000001100111111011110110000011101010100111111110100100"
            "11101111101111000111100000110111001010110111101100010001000111011"
            "0000000110010100010011011010010100000011011110",
        .hex = "4B20CFDEC1D53FD277DE3C1B95BD888EC065136940DE",
        .base64 = "SyDP3sHVP9J33jwblb2IjsBlE2lA3g==",
        .base64url = "SyDP3sHVP9J33jwblb2IjsBlE2lA3g==",
    },
    {
        .data = { 0xA7, 0xD6, 0x3B, 0x05, 0x2F, 0xAD, 0x2E, 0xE0,
                  0xB9, 0xD1, 0xA4, 0xE9, 0x3F, 0x00, 0x8A, 0x39,
                  0x7D, 0x72, 0x14, 0x26, 0x32, 0x03, 0x01 },
        .bin =
            "10100111110101100011101100000101001011111010110100101110111000001"
            "01110011101000110100100111010010011111100000000100010100011100101"
            "111101011100100001010000100110001100100000001100000001",
        .hex = "A7D63B052FAD2EE0B9D1A4E93F008A397D721426320301",
        .base64 = "p9Y7BS+tLuC50aTpPwCKOX1yFCYyAwE=",
        .base64url = "p9Y7BS-tLuC50aTpPwCKOX1yFCYyAwE=",
    },
    {
        .data = { 0xB6, 0xDB, 0x39, 0x0F, 0x11, 0xB7, 0x23, 0x47,
                  0xD0, 0xBD, 0xB0, 0x84, 0xE9, 0x1D, 0x3B, 0xDD,
                  0x9B, 0xEF, 0xB2, 0x56, 0x94, 0x85, 0x75, 0x2E },
        .bin =
            "10110110110110110011100100001111000100011011011100100011010001111"
            "10100001011110110110000100001001110100100011101001110111101110110"
            "01101111101111101100100101011010010100100001010111010100101110",
        .hex = "B6DB390F11B72347D0BDB084E91D3BDD9BEFB2569485752E",
        .base64 = "tts5DxG3I0fQvbCE6R073ZvvslaUhXUu",
        .base64url = "tts5DxG3I0fQvbCE6R073ZvvslaUhXUu",
    },
    {
        .data = { 0xDD, 0xA5, 0x17, 0xB8, 0x1E, 0x01, 0xE1, 0x67, 0x17,
                  0xCD, 0x96, 0xDB, 0x18, 0xF0, 0x75, 0xF6, 0x1C, 0x54,
                  0x61, 0xA4, 0xE8, 0xB5, 0xB9, 0x5E, 0x43 },
        .bin =
            "11011101101001010001011110111000000111100000000111100001"
            "01100111000101111100110110010110110110110001100011110000011101011"
            "11101100001110001010100011000011010010011101000101101011011100101"
            "01111001000011",
        .hex = "DDA517B81E01E16717CD96DB18F075F61C5461A4E8B5B95E43",
        .base64 = "3aUXuB4B4WcXzZbbGPB19hxUYaTotbleQw==",
        .base64url = "3aUXuB4B4WcXzZbbGPB19hxUYaTotbleQw==",
    },
    {
        .data = { 0x42, 0x49, 0x85, 0x36, 0x2A, 0xF7, 0x6A, 0x56, 0x59,
                  0xCF, 0xB7, 0x11, 0xBA, 0x30, 0xA2, 0xFA, 0xCF, 0x0B,
                  0x29, 0x2E, 0x13, 0xA5, 0xC5, 0xFB, 0xF3, 0xEA },
        .bin =
            "010000100100100110000101001101100010101011110111011010100"
            "10101100101100111001111101101110001000110111010001100001010001011"
            "11101011001111000010110010100100101110000100111010010111000101111"
            "110111111001111101010",
        .hex = "424985362AF76A5659CFB711BA30A2FACF0B292E13A5C5FBF3EA",
        .base64 = "QkmFNir3alZZz7cRujCi+s8LKS4TpcX78+o=",
        .base64url = "QkmFNir3alZZz7cRujCi-s8LKS4TpcX78-o=",
    },
    {
        .data = { 0x00, 0x5F, 0x7D, 0x19, 0x73, 0xFE, 0x44, 0x9F, 0x0A,
                  0x6D, 0xB1, 0xBE, 0xF7, 0x60, 0xAB, 0x8C, 0x43, 0x0D,
                  0x61, 0xA1, 0x4F, 0x4A, 0xA4, 0x8B, 0x48, 0x71, 0x65 },
        .bin =
            "00000000010111110111110100011001011100111111111001000100100111110"
            "00010100110110110110001101111101111011101100000101010111000110001"
            "00001100001101011000011010000101001111010010101010010010001011010"
            "010000111000101100101",
        .hex = "005F7D1973FE449F0A6DB1BEF760AB8C430D61A14F4AA48B487165",
        .base64 = "AF99GXP+RJ8KbbG+92CrjEMNYaFPSqSLSHFl",
        .base64url = "AF99GXP-RJ8KbbG-92CrjEMNYaFPSqSLSHFl",
    },
    {
        .data = { 0x68, 0xA1, 0xCF, 0x50, 0x14, 0x2A, 0xB3, 0xC5, 0x86, 0x2A,
                  0xB1, 0x3B, 0xA2, 0xD9, 0x0F, 0x5F, 0x83, 0xD0, 0xAA, 0x61,
                  0xB6, 0x20, 0x41, 0x01, 0x13, 0xD3, 0x2B, 0x15 },
        .bin =
            "01101000101000011100111101010000000101000010101010110011110001011"
            "00001100010101010110001001110111010001011011001000011110101111110"
            "00001111010000101010100110000110110110001000000100000100000001000"
            "10011110100110010101100010101",
        .hex = "68A1CF50142AB3C5862AB13BA2D90F5F83D0AA61B620410113D32B15",
        .base64 = "aKHPUBQqs8WGKrE7otkPX4PQqmG2IEEBE9MrFQ==",
        .base64url = "aKHPUBQqs8WGKrE7otkPX4PQqmG2IEEBE9MrFQ==",
    },
    {
        .data = { 0xF4, 0x6D, 0x6A, 0xB0, 0xA3, 0x38, 0x36, 0x2F, 0x4D, 0xF3,
                  0xD7, 0xC9, 0x50, 0x4F, 0x16, 0x6A, 0x25, 0xA7, 0x88, 0xA5,
                  0xFD, 0xEE, 0xD6, 0x8D, 0xA5, 0x33, 0xDD, 0xF0, 0xC0 },
        .bin =
            "11110100011011010110101010110000101000110011100000110110001011110"
            "10011011111001111010111110010010101000001001111000101100110101000"
            "10010110100111100010001010010111111101111011101101011010001101101"
            "0010100110011110111011111000011000000",
        .hex = "F46D6AB0A338362F4DF3D7C9504F166A25A788A5FDEED68DA533DDF0C0",
        .base64 = "9G1qsKM4Ni9N89fJUE8WaiWniKX97taNpTPd8MA=",
        .base64url = "9G1qsKM4Ni9N89fJUE8WaiWniKX97taNpTPd8MA=",
    },
    {
        .data = { 0xA8, 0x08, 0x32, 0xAF, 0x93, 0x02, 0x1B, 0x64, 0x1A, 0x82,
                  0xEC, 0x60, 0xC7, 0x74, 0xA3, 0x85, 0x60, 0x8A, 0x59, 0xA8,
                  0xDF, 0x38, 0x65, 0xCB, 0xDB, 0x47, 0x78, 0x58, 0xDC, 0xCC },
        .bin =
            "10101000000010000011001010101111100100110000001000011011011001000"
            "00110101000001011101100011000001100011101110100101000111000010101"
            "10000010001010010110011010100011011111001110000110010111001011110"
            "110110100011101111000010110001101110011001100",
        .hex = "A80832AF93021B641A82EC60C774A385608A59A8DF3865CBDB477858DCCC",
        .base64 = "qAgyr5MCG2Qaguxgx3SjhWCKWajfOGXL20d4WNzM",
        .base64url = "qAgyr5MCG2Qaguxgx3SjhWCKWajfOGXL20d4WNzM",
    },
    {
        .data = { 0x48, 0x05, 0x04, 0xBC, 0xE6, 0xD8, 0xC2, 0x50,
                  0xFE, 0x73, 0xCB, 0x3C, 0x42, 0x25, 0x44, 0x0C,
                  0xED, 0x9A, 0x53, 0x12, 0xAE, 0x13, 0xD9, 0x5B,
                  0x16, 0x25, 0xE0, 0x23, 0xA5, 0x4A, 0x8D },
        .bin =
            "01001000000001010000010010111100111001101101100011000010010100001"
            "11111100111001111001011001111000100001000100101010001000000110011"
            "10110110011010010100110001001010101110000100111101100101011011000"
            "10110001001011110000000100011101001010100101010001101",
        .hex = "480504BCE6D8C250FE73CB3C4225440CED9A5312AE13D95B1625E023A54A8D",
        .base64 = "SAUEvObYwlD+c8s8QiVEDO2aUxKuE9lbFiXgI6VKjQ==",
        .base64url = "SAUEvObYwlD-c8s8QiVEDO2aUxKuE9lbFiXgI6VKjQ==",
    },
};

TEST(CodecTest, BinEncodeUnbounded)
{
    for (auto &test_case: test_cases) {
        auto &data = test_case.data;
        auto &bin = test_case.bin;

        std::string dest(bin.size(), 0);

        auto result = bin_encode(data, dest.begin());

        ASSERT_FALSE(result.err);
        ASSERT_EQ(result.in, data.end());
        ASSERT_EQ(result.out, dest.end());
        ASSERT_EQ(dest, bin);
    }
}

TEST(CodecTest, BinEncodeBounded)
{
    for (auto &test_case: test_cases) {
        auto &data = test_case.data;
        auto &bin = test_case.bin;

        std::string dest(bin.size(), 0);

        auto result = bin_encode(data, dest.begin(), dest.end());

        ASSERT_FALSE(result.err);
        ASSERT_EQ(result.in, data.end());
        ASSERT_EQ(result.out, dest.end());
        ASSERT_EQ(dest, bin);
    }
}

TEST(CodecTest, BinEncodeBoundedError)
{
    for (auto &test_case: test_cases | std::views::drop(1)) {
        auto &data = test_case.data;
        auto &bin = test_case.bin;

        std::string dest(bin.size() / 2, 0);

        auto result = bin_encode(data, dest.begin(), dest.end());

        ASSERT_TRUE(result.err);
        ASSERT_EQ(result.err, std::make_error_code(std::errc::value_too_large));
        ASSERT_EQ(result.in, data.begin() + (data.size() / 2) + 1);
        ASSERT_EQ(result.out, dest.end());
        ASSERT_NE(dest, bin);
    }
}

TEST(CodecTest, HexEncodeUnbounded)
{
    for (auto &test_case: test_cases) {
        auto &data = test_case.data;
        auto &hex = test_case.hex;

        std::string dest(hex.size(), 0);

        auto result = hex_encode(data, dest.begin());

        ASSERT_FALSE(result.err);
        ASSERT_EQ(result.in, data.end());
        ASSERT_EQ(result.out, dest.end());
        ASSERT_EQ(dest, hex);
    }
}

TEST(CodecTest, HexEncodeBounded)
{
    for (auto &test_case: test_cases) {
        auto &data = test_case.data;
        auto &hex = test_case.hex;

        std::string dest(hex.size(), 0);

        auto result = hex_encode(data, dest.begin(), dest.end());

        ASSERT_FALSE(result.err);
        ASSERT_EQ(result.in, data.end());
        ASSERT_EQ(result.out, dest.end());
        ASSERT_EQ(dest, hex);
    }
}

TEST(CodecTest, HexEncodeBoundedError)
{
    for (auto &test_case: test_cases | std::views::drop(1)) {
        auto &data = test_case.data;
        auto &hex = test_case.hex;

        std::string dest(hex.size() / 2, 0);

        auto result = hex_encode(data, dest.begin(), dest.end());

        ASSERT_TRUE(result.err);
        ASSERT_EQ(result.err, std::make_error_code(std::errc::value_too_large));
        ASSERT_EQ(result.in, data.begin() + (data.size() / 2) + 1);
        ASSERT_EQ(result.out, dest.end());
        ASSERT_NE(dest, hex);
    }
}

} // namespace htl
