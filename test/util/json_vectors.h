#ifndef TEST_JSON_VECTORS_H_
#define TEST_JSON_VECTORS_H_

#include <array>
#include <utility>
#include <string_view>
#include <hlib/json/document.h>

namespace hlib::test {

using namespace json;

struct JSONTestVector {
    std::string_view type;
    std::string_view name;
    std::string_view data;
    Document value;
};

template <class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(
    std::basic_ostream<CharT, Traits> &os, const JSONTestVector &test_vector)
{
    std::cout << "type: " << test_vector.type << "\n"
              << "name: " << test_vector.name << "\n"
              << "data: " << test_vector.data << "\n"
              << "value: " << test_vector.value;
    return os;
}

inline const JSONTestVector json_test_vectors[]{
    // {
    //     "y",
    //     "array_arraysWithSpaces",
    //     "[[]   ]",
    //     { {} },
    // },
    // {
    //     "y",
    //     "array_empty-string",
    //     "[\"\"]",
    //     { "" },
    // },
    // {
    //     "y",
    //     "array_empty",
    //     "[]",
    //     {},
    // },
    // {
    //     "y",
    //     "array_ending_with_newline",
    //     "[\"a\"]",
    //     { "a" },
    // },
    // {
    //     "y",
    //     "array_false",
    //     "[false]",
    //     { false },
    // },
    // {
    //     "y",
    //     "array_heterogeneous",
    //     "[null, 1, \"1\", {}]",
    //     { nullptr, 1, "1", {} },
    // },
    // {
    //     "y",
    //     "array_null",
    //     "[null]",
    //     { nullptr },
    // },
    // {
    //     "y",
    //     "array_with_1_and_newline",
    //     "[1\n]",
    //     { 1 },
    // },
    // {
    //     "y",
    //     "array_with_leading_space",
    //     " [1]",
    //     { 1 },
    // },
    // {
    //     "y",
    //     "array_with_several_null",
    //     "[1,null,null,null,2]",
    //     { 1, nullptr, nullptr, nullptr, 2 },
    // },
    // {
    //     "y",
    //     "array_with_trailing_space",
    //     "[2] ",
    //     { 2 },
    // },
    // {
    //     "y",
    //     "number_0e+1",
    //     "[0e+1]",
    //     { 0e+1 },
    // },
    // {
    //     "y",
    //     "number_0e1",
    //     "[0e1]",
    //     { 0e1 },
    // },
    // {
    //     "y",
    //     "number_after_space",
    //     "[ 4]",
    //     { 4 },
    // },
    // // {
    // //     "y",
    // //     "number_double_close_to_zero",
    // //     "[-0.0000000000000000000000000000000000000000000000000000000000000000000"
    // //     "00000000001]\n",
    // // },
    // {
    //     "y",
    //     "number_int_with_exp",
    //     "[20e1]",
    //     { 20e1 },
    // },
    // {
    //     "y",
    //     "number_minus_zero",
    //     "[-0]",
    //     { -0 },
    // },
    // {
    //     "y",
    //     "number_negative_int",
    //     "[-123]",
    //     { -123 },
    // },
    // {
    //     "y",
    //     "number_negative_one",
    //     "[-1]",
    //     { -1 },
    // },
    // {
    //     "y",
    //     "number_negative_zero",
    //     "[-0]",
    //     { 0 },
    // },
    // {
    //     "y",
    //     "number_real_capital_e_neg_exp",
    //     "[1E-2]",
    //     { 1E-2 },
    // },
    // {
    //     "y",
    //     "number_real_capital_e_pos_exp",
    //     "[1E+2]",
    //     { 1E+2 },
    // },
    // {
    //     "y",
    //     "number_real_capital_e",
    //     "[1E22]",
    //     { 1E22 },
    // },
    // {
    //     "y",
    //     "number_real_exponent",
    //     "[123e45]",
    //     { 123e45 },
    // },
    // {
    //     "y",
    //     "number_real_fraction_exponent",
    //     "[123.456e78]",
    //     { 123.456e78 },
    // },
    // {
    //     "y",
    //     "number_real_neg_exp",
    //     "[1e-2]",
    //     { 1e-2 },
    // },
    // {
    //     "y",
    //     "number_real_pos_exponent",
    //     "[1e+2]",
    //     { 1e+2 },
    // },
    // {
    //     "y",
    //     "number_simple_int",
    //     "[123]",
    //     { 123 },
    // },
    // {
    //     "y",
    //     "number_simple_real",
    //     "[123.456789]",
    //     { 123.456789 },
    // },
    // {
    //     "y",
    //     "number",
    //     "[123e65]",
    //     { 123e65 },
    // },
    // {
    //     "y",
    //     "object_basic",
    //     "{\"asd\":\"sdf\"}",
    //     { { "asd", "sdf" } },
    // },
    // {
    //     "y",
    //     "object_empty_key",
    //     "{\"\":0}",
    //     { { "", 0 } },
    // },
    // {
    //     "y",
    //     "object_empty",
    //     "{}",
    //     Object{},
    // },
    // // {
    // //     "y",
    // //     "object_escaped_null_in_key",
    // //     "{\"foo\\u0000bar\": 42}",
    // // },
    // {
    //     "y",
    //     "object_extreme_numbers",
    //     "{ \"min\": -1.0e+28, \"max\": 1.0e+28 }",
    //     { { "min", -1.0e+28 }, { "max" : 1.0e+28 } },
    // },
    // {
    //     "y",
    //     "object_long_strings",
    //     "{\"x\":[{\"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}], \"id"
    //     "\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}",
    //     {
    //         { "x", { { "id", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" } } },
    //         { "id", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" },
    //     },
    // },
    // {
    //     "y",
    //     "object_simple",
    //     "{\"a\":[]}",
    //     { "a" : {} },
    // },
    // // {
    // //     "y",
    // //     "object_string_unicode",
    // //     "{\"title\":\"\\u041f\\u043e\\u043b\\u0442\\u043e\\u0440\\u0430 \\u0417"
    // //     "\\u0435\\u043c\\u043b\\u0435\\u043a\\u043e\\u043f\\u0430\" }",
    // // },
    // {
    //     "y",
    //     "object_with_newlines",
    //     "{\n\"a\": \"b\"\n}",
    //     { "a" : "b" },
    // },
    // {
    //     "y",
    //     "object",
    //     "{\"asd\":\"sdf\", \"dfg\":\"fgh\"}",
    //     { { "asd", "sdf" }, { "dfg", "fgh" } },
    // },
    // // {
    // //     "y",
    // //     "string_1_2_3_bytes_UTF-8_sequences",
    // //     "[\"\\u0060\\u012a\\u12AB\"]",
    // // },
    // // {
    // //     "y",
    // //     "string_accepted_surrogate_pair",
    // //     "[\"\\uD801\\udc37\"]",
    // // },
    // // {
    // //     "y",
    // //     "string_accepted_surrogate_pairs",
    // //     "[\"\\ud83d\\ude39\\ud83d\\udc8d\"]",
    // // },
    // {
    //     "y",
    //     "string_allowed_escapes",
    //     "[\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"]",
    //     { "\"\\/\b\f\n\r\t" },
    // },
    // // {
    // //     "y",
    // //     "string_backslash_and_u_escaped_zero",
    // //     "[\"\\\\u0000\"]",
    // // },
    // {
    //     "y",
    //     "string_backslash_doublequotes",
    //     "[\"\\\"\"]",
    //     { "\\\"" },
    // },
    // {
    //     "y",
    //     "string_comments",
    //     "[\"a/*b*/c/*d//e\"]",
    //     { "a/*b*/c/*d//e" },
    // },
    // {
    //     "y",
    //     "string_double_escape_a",
    //     "[\"\\\\a\"]",
    //     { "\\\\a" },
    // },
    // {
    //     "y",
    //     "string_double_escape_n",
    //     "[\"\\\\n\"]",
    //     { "\\\\n" },
    // },
    // {
    //     "y",
    //     "string_escaped_control_character",
    //     "[\"\\u0012\"]",
    //     { "\u0012" },
    // },
    // // {
    // //     "y",
    // //     "string_escaped_noncharacter",
    // //     "[\"\\uFFFF\"]",
    // // },
    // {
    //     "y",
    //     "string_in_array_with_leading_space",
    //     "[ \"asd\"]",
    //     { "asd" },
    // },
    // { "y", "string_in_array", "[\"asd\"]", { "asd" } },
    // // {
    // //     "y",
    // //     "string_last_surrogates_1_and_2",
    // //     "[\"\\uDBFF\\uDFFF\"]",
    // // },
    // // {
    // //     "y",
    // //     "string_nbsp_uescaped",
    // //     "[\"new\\u00A0line\"]",
    // // },
    // // {
    // //     "y",
    // //     "string_nonCharacterInUTF-8_U+FFFF",
    // //     "[\"\uffff\"]",
    // // },
    // // {
    // //     "y",
    // //     "string_null_escape",
    // //     "[\"\\u0000\"]",
    // // },
    // // {
    // //     "y",
    // //     "string_one-byte-utf-8",
    // //     "[\"\\u002c\"]",
    // // },
    // // {
    // //     "y",
    // //     "string_pi",
    // //     "[\"\"]",
    // //     { "" },
    // // },
    // {
    //     "y",
    //     "string_simple_ascii",
    //     "[\"asd \"]",
    //     { "asd " },
    // },
    // {
    //     "y",
    //     "string_space",
    //     "\" \"",
    //     Document(" "),
    // },
    // {
    //     "y",
    //     "string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF",
    //     "[\"\\uD834\\uDd1e\"]",
    // },
    // {
    //     "y",
    //     "string_three-byte-utf-8",
    //     "[\"\\u0821\"]",
    // },
    // {
    //     "y",
    //     "string_two-byte-utf-8",
    //     "[\"\\u0123\"]",
    // },
    // {
    //     "y",
    //     "string_u+2028_line_sep",
    //     "[\"\u2028\"]",
    // },
    // {
    //     "y",
    //     "string_u+2029_par_sep",
    //     "[\"\u2029\"]",
    // },
    // {
    //     "y",
    //     "string_uEscape",
    //     "[\"\\u0061\\u30af\\u30EA\\u30b9\"]",
    // },
    // {
    //     "y",
    //     "string_uescaped_newline",
    //     "[\"new\\u000Aline\"]",
    //     {
    //         "new\\u000Aline",
    //     },
    //     ,
    // },
    // {
    //     "y",
    //     "string_unicode_2",
    //     "[\"\u2342\u3234\u2342\"]",
    // },
    // {
    //     "y",
    //     "string_unicode_escaped_double_quote",
    //     "[\"\\u0022\"]",
    // },
    // {
    //     "y",
    //     "string_unicode_U+10FFFE_nonchar",
    //     "[\"\\uDBFF\\uDFFE\"]",
    // },
    // {
    //     "y",
    //     "string_unicode_U+1FFFE_nonchar",
    //     "[\"\\uD83F\\uDFFE\"]",
    // },
    // {
    //     "y",
    //     "string_unicode_U+200B_ZERO_WIDTH_SPACE",
    //     "[\"\\u200B\"]",
    // },
    // {
    //     "y",
    //     "string_unicode_U+2064_invisible_plus",
    //     "[\"\\u2064\"]",
    // },
    // {
    //     "y",
    //     "string_unicode_U+FDD0_nonchar",
    //     "[\"\\uFDD0\"]",
    // },
    // {
    //     "y",
    //     "string_unicode_U+FFFE_nonchar",
    //     "[\"\\uFFFE\"]",
    // },
    // {
    //     "y",
    //     "string_unicode",
    //     "[\"\\uA66D\"]",
    // },
    // {
    //     "y",
    //     "string_unicodeEscapedBackslash",
    //     "[\"\\u005C\"]",
    // },
    // {
    //     "y",
    //     "structure_lonely_false",
    //     "false",
    //     Document("false"),
    // },
    // {
    //     "y",
    //     "structure_lonely_int",
    //     "42",
    //     Document(42),
    // },
    // {
    //     "y",
    //     "structure_lonely_negative_real",
    //     "-0.1",
    //     Document(-0.1),
    // },
    // {
    //     "y",
    //     "structure_lonely_null",
    //     "null",
    //     Document(nullptr),
    // },
    // {
    //     "y",
    //     "structure_lonely_string",
    //     "\"asd\"",
    //     Document("asd"),
    // },
    // {
    //     "y",
    //     "structure_lonely_true",
    //     "true",
    //     Document(true),
    // },
    // {
    //     "y",
    //     "structure_string_empty",
    //     "\"\"",
    //     Document("\"\""),
    // },
    // {
    //     "y",
    //     "structure_trailing_newline",
    //     "[\"a\"]\n",
    // },
    // {
    //     "y",
    //     "structure_true_in_array",
    //     "[true]",
    //     { "true" },
    // },
    // {
    //     "y",
    //     "structure_whitespace_array",
    //     " [] ",
    //     {},
    // },
    // // { "y", "object_duplicated_key_and_value", "{\"a\":\"b\",\"a\":\"b\"}" },
    // // { "y", "object_duplicated_key", "{\"a\":\"b\",\"a\":\"c\"}" },
    // // { "y", "string_nonCharacterInUTF-8_U+10FFFF", "[\"\udbff\udfff\"]" },
    // // { "y", "string_reservedCharacterInUTF-8_U+1BFFF", "[\"\ud82f\udfff\"]" },
    // // { "y", "string_unescaped_char_delete", "[\"\u007f\"]" },
    // // { "y", "string_utf8", "[\"\u20ac\ud834\udd1e\"]" },
    // { "y", "string_with_del_character", "[\"a\u007fa\"]" },
};

} // namespace hlib::test

#endif
