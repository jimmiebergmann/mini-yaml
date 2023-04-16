/*
* MIT License
*
* Copyright(c) 2023 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

#include "test_utility.hpp"

template<typename Titerator>
void ok_scalar_loop_test(Titerator begin, Titerator end, bool is_const_iterator, const std::vector<std::string>& lines)
{
    auto node_count = static_cast<size_t>(std::distance(begin, end));
    ASSERT_EQ(node_count, lines.size());

    size_t loop_count = 0;
    for (auto it = begin; it != end; ++it) {
        ASSERT_LT(loop_count, lines.size());

        auto& value = *it;
        EXPECT_EQ(std::is_const<typename std::remove_reference<decltype(value)>::type>::value, is_const_iterator);
        EXPECT_STREQ(std::string(value.data(), value.size()).c_str(), lines[loop_count].c_str());

        ++loop_count;
    }

    EXPECT_EQ(loop_count, lines.size());
}

TEST(dom_scalar_node, as_bool)
{
    using char_type = char;
    auto node = yaml::dom::node<char_type>::create_scalar();
    auto& scalar_node = node.as_scalar();
    scalar_node.push_back("");

    // Empty
    {
        scalar_node.at(0) = "";
        EXPECT_FALSE(scalar_node.as<bool>());
        EXPECT_TRUE(scalar_node.as<bool>(true));
    }
    {
        scalar_node.at(0) = " ";
        EXPECT_FALSE(scalar_node.as<bool>());
        EXPECT_TRUE(scalar_node.as<bool>(true));
    }
    {
        scalar_node.at(0) = "hello";
        EXPECT_FALSE(scalar_node.as<bool>());
        EXPECT_TRUE(scalar_node.as<bool>(true));
    }
    // true
    {
        scalar_node.at(0) = "true";
        EXPECT_TRUE(scalar_node.as<bool>());

        scalar_node.at(0) = "True";
        EXPECT_TRUE(scalar_node.as<bool>());

        scalar_node.at(0) = "TRUE";
        EXPECT_TRUE(scalar_node.as<bool>());

        scalar_node.at(0) = "yes";
        EXPECT_TRUE(scalar_node.as<bool>());

        scalar_node.at(0) = "Yes";
        EXPECT_TRUE(scalar_node.as<bool>());

        scalar_node.at(0) = "yes";
        EXPECT_TRUE(scalar_node.as<bool>());
    }
    // false
    {
        scalar_node.at(0) = "false";
        EXPECT_FALSE(scalar_node.as<bool>());
        EXPECT_FALSE(scalar_node.as<bool>(true));

        scalar_node.at(0) = "False";
        EXPECT_FALSE(scalar_node.as<bool>());
        EXPECT_FALSE(scalar_node.as<bool>(true));

        scalar_node.at(0) = "FALSE";
        EXPECT_FALSE(scalar_node.as<bool>());
        EXPECT_FALSE(scalar_node.as<bool>(true));

        scalar_node.at(0) = "no";
        EXPECT_FALSE(scalar_node.as<bool>());
        EXPECT_FALSE(scalar_node.as<bool>(true));

        scalar_node.at(0) = "No";
        EXPECT_FALSE(scalar_node.as<bool>());
        EXPECT_FALSE(scalar_node.as<bool>(true));

        scalar_node.at(0) = "NO";
        EXPECT_FALSE(scalar_node.as<bool>());
        EXPECT_FALSE(scalar_node.as<bool>(true));
    }
}

TEST(dom_scalar_node, as_int32_t)
{
    using char_type = char;
    auto node = yaml::dom::node<char_type>::create_scalar();
    auto& scalar_node = node.as_scalar();
    scalar_node.push_back("");

    // Empty
    {
        scalar_node.at(0) = "";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 0 });
        EXPECT_EQ(scalar_node.as<int32_t>(1337), int32_t{ 1337 });
    }
    {
        scalar_node.at(0) = " ";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 0 });
        EXPECT_EQ(scalar_node.as<int32_t>(1337), int32_t{ 1337 });
    }

    // Base 10
    {
        scalar_node.at(0) = "0";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 0 });
        EXPECT_EQ(scalar_node.as<int32_t>(1337), int32_t{ 0 });

        scalar_node.at(0) = "-0";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 0 });
        EXPECT_EQ(scalar_node.as<int32_t>(1337), int32_t{ 0 });

        scalar_node.at(0) = "+0";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 0 });
        EXPECT_EQ(scalar_node.as<int32_t>(1337), int32_t{ 0 });
    }
    {
        scalar_node.at(0) = "1";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 1 });

        scalar_node.at(0) = "-1";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ -1 });

        scalar_node.at(0) = "123456";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 123456 });

        scalar_node.at(0) = "-123456";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ -123456 });

        scalar_node.at(0) = "+123456";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ +123456 });
    }
    {
        scalar_node.at(0) = std::to_string(std::numeric_limits<int32_t>::max());
        EXPECT_EQ(scalar_node.as<int32_t>(), std::numeric_limits<int32_t>::max());

        scalar_node.at(0) = std::to_string(std::numeric_limits<int32_t>::max()) + "0";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 0 });
        EXPECT_EQ(scalar_node.as<int32_t>(1337), int32_t{ 1337 });

        scalar_node.at(0) = std::to_string(std::numeric_limits<int32_t>::min());
        EXPECT_EQ(scalar_node.as<int32_t>(), std::numeric_limits<int32_t>::min());

        scalar_node.at(0) = std::to_string(std::numeric_limits<int32_t>::min()) + "0";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 0 });
        EXPECT_EQ(scalar_node.as<int32_t>(1337), int32_t{ 1337 });
    }

    // Base 8
    {
        scalar_node.at(0) = "00";
        EXPECT_EQ(scalar_node.as<int32_t>(1337), int32_t{ 0 });

        scalar_node.at(0) = "0144";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 100 });

        scalar_node.at(0) = "063003711";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 13371337 });
    }
    {
        scalar_node.at(0) = "0123123123123123123123";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 0 });
        EXPECT_EQ(scalar_node.as<int32_t>(1337), int32_t{ 1337 });
    }

    // Base 16
    {
        scalar_node.at(0) = "0x0";
        EXPECT_EQ(scalar_node.as<int32_t>(1337), int32_t{ 0 });
        scalar_node.at(0) = "0X0";
        EXPECT_EQ(scalar_node.as<int32_t>(1337), int32_t{ 0 });

        scalar_node.at(0) = "0x1";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 1 });

        scalar_node.at(0) = "0xBEEF";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 48879 });
    }
    {
        scalar_node.at(0) = "0xBEEFBEEFF";
        EXPECT_EQ(scalar_node.as<int32_t>(), int32_t{ 0 });
        EXPECT_EQ(scalar_node.as<int32_t>(1337), int32_t{ 1337 });
    }
}

TEST(dom_scalar_node, as_int64_t)
{
    using char_type = char;
    auto node = yaml::dom::node<char_type>::create_scalar();
    auto& scalar_node = node.as_scalar();
    scalar_node.push_back("");

    // Empty
    {
        scalar_node.at(0) = "";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 0LL });
        EXPECT_EQ(scalar_node.as<int64_t>(1337LL), int64_t{ 1337LL });
    }
    {
        scalar_node.at(0) = " ";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 0LL });
        EXPECT_EQ(scalar_node.as<int64_t>(1337LL), int64_t{ 1337LL });
    }

    // Base 10
    {
        scalar_node.at(0) = "0";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 0LL });
        EXPECT_EQ(scalar_node.as<int64_t>(1337LL), int64_t{ 0LL });

        scalar_node.at(0) = "-0";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 0LL });
        EXPECT_EQ(scalar_node.as<int64_t>(1337LL), int64_t{ 0LL });

        scalar_node.at(0) = "+0";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 0LL });
        EXPECT_EQ(scalar_node.as<int64_t>(1337LL), int64_t{ 0LL });
    }
    {
        scalar_node.at(0) = "1";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 1LL });

        scalar_node.at(0) = "-1";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ -1LL });

        scalar_node.at(0) = "123456";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 123456LL });

        scalar_node.at(0) = "-123456";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ -123456LL });

        scalar_node.at(0) = "+123456";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 123456LL });
    }
    {
        scalar_node.at(0) = std::to_string(std::numeric_limits<int64_t>::max());
        EXPECT_EQ(scalar_node.as<int64_t>(), std::numeric_limits<int64_t>::max());

        scalar_node.at(0) = std::to_string(std::numeric_limits<int64_t>::max()) + "00123";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 0LL });
        EXPECT_EQ(scalar_node.as<int64_t>(1337LL), int64_t{ 1337LL });

        scalar_node.at(0) = std::to_string(std::numeric_limits<int64_t>::min() + 1);
        EXPECT_EQ(scalar_node.as<int64_t>(), std::numeric_limits<int64_t>::min() + 1);

        scalar_node.at(0) = std::to_string(std::numeric_limits<int64_t>::min()) + "0";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 0LL });
        EXPECT_EQ(scalar_node.as<int64_t>(1337LL), int64_t{ 1337LL });
    }

    // Base 8
    {
        scalar_node.at(0) = "00";
        EXPECT_EQ(scalar_node.as<int64_t>(1337LL), int64_t{ 0LL });

        scalar_node.at(0) = "0144";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 100LL });

        scalar_node.at(0) = "063003711";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 13371337LL });
    }
    {
        scalar_node.at(0) = "0123123123123123123123123123123";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 0LL });
        EXPECT_EQ(scalar_node.as<int64_t>(1337LL), int64_t{ 1337LL });
    }

    // Base 16
    {
        scalar_node.at(0) = "0x0";
        EXPECT_EQ(scalar_node.as<int64_t>(1337LL), int64_t{ 0LL });
        scalar_node.at(0) = "0X0";
        EXPECT_EQ(scalar_node.as<int64_t>(1337LL), int64_t{ 0LL });

        scalar_node.at(0) = "0x1";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 1LL });

        scalar_node.at(0) = "0xBEEFBEEFBEEF";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 209936909844207LL });
    }
    {
        scalar_node.at(0) = "0xBEEFBEEFFBEEFBEEF";
        EXPECT_EQ(scalar_node.as<int64_t>(), int64_t{ 0LL });
        EXPECT_EQ(scalar_node.as<int64_t>(1337), int64_t{ 1337LL });
    }
}

TEST(dom_scalar_node, as_uint32_t)
{
    using char_type = char;
    auto node = yaml::dom::node<char_type>::create_scalar();
    auto& scalar_node = node.as_scalar();
    scalar_node.push_back("");

    // Empty
    {
        scalar_node.at(0) = "";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 0 });
        EXPECT_EQ(scalar_node.as<uint32_t>(1337), uint32_t{ 1337 });
    }
    {
        scalar_node.at(0) = " ";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 0 });
        EXPECT_EQ(scalar_node.as<uint32_t>(1337), uint32_t{ 1337 });
    }

    // Base 10
    {
        scalar_node.at(0) = "0";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 0 });
        EXPECT_EQ(scalar_node.as<uint32_t>(1337), uint32_t{ 0 });

        scalar_node.at(0) = "-0";
        EXPECT_EQ(scalar_node.as<uint32_t>(1337), uint32_t{ 0 });

        scalar_node.at(0) = "+0";
        EXPECT_EQ(scalar_node.as<uint32_t>(1337), uint32_t{ 0 });
    }
    {
        scalar_node.at(0) = "1";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 1 });

        scalar_node.at(0) = "-1";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 0 });
        EXPECT_EQ(scalar_node.as<uint32_t>(1337), uint32_t{ 1337 });

        scalar_node.at(0) = "123456";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 123456 });

        scalar_node.at(0) = "-123456";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 0 });
        EXPECT_EQ(scalar_node.as<uint32_t>(1337), uint32_t{ 1337 });

        scalar_node.at(0) = "+123456";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 123456 });
    }
    {
        scalar_node.at(0) = std::to_string(std::numeric_limits<uint32_t>::max());
        EXPECT_EQ(scalar_node.as<uint32_t>(), std::numeric_limits<uint32_t>::max());

        scalar_node.at(0) = std::to_string(std::numeric_limits<uint32_t>::max()) + "0";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 0 });
        EXPECT_EQ(scalar_node.as<uint32_t>(1337), uint32_t{ 1337 });
    }

    // Base 8
    {
        scalar_node.at(0) = "00";
        EXPECT_EQ(scalar_node.as<uint32_t>(1337), uint32_t{ 0 });

        scalar_node.at(0) = "0144";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 100 });

        scalar_node.at(0) = "063003711";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 13371337 });
    }
    {
        scalar_node.at(0) = "0123123123123123123123";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 0 });
        EXPECT_EQ(scalar_node.as<uint32_t>(1337), uint32_t{ 1337 });
    }

    // Base 16
    {
        scalar_node.at(0) = "0x0";
        EXPECT_EQ(scalar_node.as<uint32_t>(1337), uint32_t{ 0 });
        scalar_node.at(0) = "0X0";
        EXPECT_EQ(scalar_node.as<uint32_t>(1337), uint32_t{ 0 });

        scalar_node.at(0) = "0x1";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 1 });

        scalar_node.at(0) = "0xBEEF";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 48879 });
    }
    {
        scalar_node.at(0) = "0xBEEFBEEFF";
        EXPECT_EQ(scalar_node.as<uint32_t>(), uint32_t{ 0 });
        EXPECT_EQ(scalar_node.as<uint32_t>(1337), uint32_t{ 1337 });
    }
}

TEST(dom_scalar_node, as_uint64_t)
{
    using char_type = char;
    auto node = yaml::dom::node<char_type>::create_scalar();
    auto& scalar_node = node.as_scalar();
    scalar_node.push_back("");

    // Empty
    {
        scalar_node.at(0) = "";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 0ULL });
        EXPECT_EQ(scalar_node.as<uint64_t>(1337ULL), uint64_t{ 1337ULL });
    }
    {
        scalar_node.at(0) = " ";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 0ULL });
        EXPECT_EQ(scalar_node.as<uint64_t>(1337ULL), uint64_t{ 1337ULL });
    }

    // Base 10
    {
        scalar_node.at(0) = "0";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 0ULL });
        EXPECT_EQ(scalar_node.as<uint64_t>(1337ULL), uint64_t{ 0ULL });

        scalar_node.at(0) = "-0";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 0ULL });
        EXPECT_EQ(scalar_node.as<uint64_t>(1337ULL), uint64_t{ 0ULL });

        scalar_node.at(0) = "+0";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 0ULL });
        EXPECT_EQ(scalar_node.as<uint64_t>(1337ULL), uint64_t{ 0ULL });
    }
    {
        scalar_node.at(0) = "1";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 1ULL });

        scalar_node.at(0) = "-1";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 18446744073709551615ULL });
        EXPECT_EQ(scalar_node.as<uint64_t>(1337), uint64_t{ 18446744073709551615ULL });

        scalar_node.at(0) = "123456789012";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 123456789012ULL });

        scalar_node.at(0) = "-123456789012123123123123123123123123123";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 0ULL });
        EXPECT_EQ(scalar_node.as<uint64_t>(1337), uint64_t{ 1337ULL });

        scalar_node.at(0) = "+123456789012";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 123456789012ULL });
    }
    {
        scalar_node.at(0) = std::to_string(std::numeric_limits<uint64_t>::max() - 1ULL);
        EXPECT_EQ(scalar_node.as<uint64_t>(), std::numeric_limits<uint64_t>::max() - 1ULL);

        scalar_node.at(0) = std::to_string(std::numeric_limits<uint64_t>::max()) + "00123";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 0ULL });
        EXPECT_EQ(scalar_node.as<uint64_t>(1337ULL), uint64_t{ 1337ULL });

        scalar_node.at(0) = std::to_string(std::numeric_limits<uint64_t>::min() + 1);
        EXPECT_EQ(scalar_node.as<uint64_t>(), std::numeric_limits<uint64_t>::min() + 1);
    }

    // Base 8
    {
        scalar_node.at(0) = "00";
        EXPECT_EQ(scalar_node.as<uint64_t>(1337ULL), uint64_t{ 0ULL });

        scalar_node.at(0) = "0144";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 100ULL });

        scalar_node.at(0) = "063003711";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 13371337ULL });
    }
    {
        scalar_node.at(0) = "0123123123123123123123123123123";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 0ULL });
        EXPECT_EQ(scalar_node.as<uint64_t>(1337ULL), uint64_t{ 1337ULL });
    }

    // Base 16
    {
        scalar_node.at(0) = "0x0";
        EXPECT_EQ(scalar_node.as<uint64_t>(1337ULL), uint64_t{ 0ULL });
        scalar_node.at(0) = "0X0";
        EXPECT_EQ(scalar_node.as<uint64_t>(1337ULL), uint64_t{ 0ULL });

        scalar_node.at(0) = "0x1";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 1ULL });

        scalar_node.at(0) = "0xBEEFBEEFBEEF";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 209936909844207ULL });
    }
    {
        scalar_node.at(0) = "0xBEEFBEEFFBEEFBEEF";
        EXPECT_EQ(scalar_node.as<uint64_t>(), uint64_t{ 0ULL });
        EXPECT_EQ(scalar_node.as<uint64_t>(1337), uint64_t{ 1337ULL });
    }
}

TEST(dom_scalar_node, as_float)
{
    using char_type = char;
    auto node = yaml::dom::node<char_type>::create_scalar();
    auto& scalar_node = node.as_scalar();
    scalar_node.push_back("");

    // Empty
    {
        scalar_node.at(0) = "";
        EXPECT_FLOAT_EQ(scalar_node.as<float>(), 0.0f);
        EXPECT_FLOAT_EQ(scalar_node.as<float>(4.0f), float{ 4.0f });
    }
    {
        scalar_node.at(0) = " ";
        EXPECT_FLOAT_EQ(scalar_node.as<float>(), float{ 0.0f });
        EXPECT_FLOAT_EQ(scalar_node.as<float>(4.0f), float{ 4.0f });
    }

    // Ok tests
    {
        scalar_node.at(0) = "0";
        EXPECT_FLOAT_EQ(scalar_node.as<float>(), float{ 0.0f });
        EXPECT_FLOAT_EQ(scalar_node.as<float>(4.0f), float{ 0.0f });

        scalar_node.at(0) = "+0";
        EXPECT_FLOAT_EQ(scalar_node.as<float>(), float{ 0.0f });
        EXPECT_FLOAT_EQ(scalar_node.as<float>(4.0f), float{ 0.0f });

        scalar_node.at(0) = "-0";
        EXPECT_FLOAT_EQ(scalar_node.as<float>(), float{ 0.0f });
        EXPECT_FLOAT_EQ(scalar_node.as<float>(4.0f), float{ 0.0f });

        scalar_node.at(0) = "512";
        EXPECT_FLOAT_EQ(scalar_node.as<float>(), float{ 512.0f });
        EXPECT_FLOAT_EQ(scalar_node.as<float>(4.0f), float{ 512.0f });

        scalar_node.at(0) = "512.125";
        EXPECT_FLOAT_EQ(scalar_node.as<float>(), float{ 512.125f });
        EXPECT_FLOAT_EQ(scalar_node.as<float>(4.0f), float{ 512.125f });

        scalar_node.at(0) = "+512.125";
        EXPECT_FLOAT_EQ(scalar_node.as<float>(), float{ 512.125f });
        EXPECT_FLOAT_EQ(scalar_node.as<float>(4.0f), float{ 512.125f });

        scalar_node.at(0) = "-512.125";
        EXPECT_FLOAT_EQ(scalar_node.as<float>(), float{ -512.125f });
        EXPECT_FLOAT_EQ(scalar_node.as<float>(4.0f), float{ -512.125f });

        scalar_node.at(0) = "3.40282e+37";
        EXPECT_FLOAT_EQ(scalar_node.as<float>(), float{ 3.40282e+37f });
        EXPECT_FLOAT_EQ(scalar_node.as<float>(4.0f), float{ 3.40282e+37f });
    }

    // Out of range tests
    {
        scalar_node.at(0) = "3.40282e+39";
        EXPECT_FLOAT_EQ(scalar_node.as<float>(), float{ 0.0f });
        EXPECT_FLOAT_EQ(scalar_node.as<float>(4.0f), float{ 4.0f });

        scalar_node.at(0) = "-3402823466385288598117041834845169254401337";
        EXPECT_FLOAT_EQ(scalar_node.as<float>(), float{ 0.0f });
        EXPECT_FLOAT_EQ(scalar_node.as<float>(4.0f), float{ 4.0f });

        scalar_node.at(0) = "3402823466385288598117041834845169254401337";
        EXPECT_FLOAT_EQ(scalar_node.as<float>(), float{ 0.0f });
        EXPECT_FLOAT_EQ(scalar_node.as<float>(4.0f), float{ 4.0f });
    }
}

TEST(dom_scalar_node, as_double)
{
    using char_type = char;
    auto node = yaml::dom::node<char_type>::create_scalar();
    auto& scalar_node = node.as_scalar();
    scalar_node.push_back("");

    // Empty
    {
        scalar_node.at(0) = "";
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(), 0.0);
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(4.0), double{ 4.0 });
    }
    {
        scalar_node.at(0) = " ";
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(), double{ 0.0 });
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(4.0), double{ 4.0 });
    }

    // Ok tests
    {
        scalar_node.at(0) = "0";
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(), double{ 0.0 });
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(4.0), double{ 0.0 });

        scalar_node.at(0) = "+0";
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(), double{ 0.0 });
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(4.0), double{ 0.0 });

        scalar_node.at(0) = "-0";
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(), double{ 0.0 });
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(4.0), double{ 0.0 });

        scalar_node.at(0) = "512";
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(), double{ 512.0 });
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(4.0), double{ 512.0 });

        scalar_node.at(0) = "512.125";
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(), double{ 512.125 });
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(4.0), double{ 512.125 });

        scalar_node.at(0) = "+512.125";
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(), double{ 512.125 });
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(4.0), double{ 512.125 });

        scalar_node.at(0) = "-512.125";
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(), double{ -512.125 });
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(4.0), double{ -512.125 });

        scalar_node.at(0) = "1.79769e+307";
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(), double{ 1.79769e+307 });
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(4.0), double{ 1.79769e+307 });
    }

    // Out of range tests
    {
        scalar_node.at(0) = "1.79769e+309";
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(), double{ 0.0 });
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(4.0), double{ 4.0 });

        scalar_node.at(0) = "-179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368001337";
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(), double{ 0.0 });
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(4.0), double{ 4.0 });

        scalar_node.at(0) = "179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368001337";
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(), double{ 0.0 });
        EXPECT_DOUBLE_EQ(scalar_node.as<double>(4.0), double{ 4.0 });
    }
}

TEST(dom_scalar_node, as_long_double)
{
    using char_type = char;
    auto node = yaml::dom::node<char_type>::create_scalar();
    auto& scalar_node = node.as_scalar();
    scalar_node.push_back("");

    auto is_near = [](const long double lhs, const long double rhs) {
        const auto epsilon = 1.0e-5L;
        return lhs >= (rhs - epsilon) && lhs <= (rhs + epsilon);
    };

    // Empty
    {
        scalar_node.at(0) = "";
        EXPECT_TRUE(is_near(scalar_node.as<long double>(), 0.0L));
        EXPECT_TRUE(is_near(scalar_node.as<long double>(4.0L), 4.0L));
    }
    {
        scalar_node.at(0) = " ";
        EXPECT_TRUE(is_near(scalar_node.as<long double>(), 0.0L));
        EXPECT_TRUE(is_near(scalar_node.as<long double>(4.0L), 4.0L));
    }

    // Ok tests
    {
        scalar_node.at(0) = "0";
        EXPECT_TRUE(is_near(scalar_node.as<long double>(), 0.0L));
        EXPECT_TRUE(is_near(scalar_node.as<long double>(4.0L), 0.0L));

        scalar_node.at(0) = "+0";
        EXPECT_TRUE(is_near(scalar_node.as<long double>(), 0.0L));
        EXPECT_TRUE(is_near(scalar_node.as<long double>(4.0), 0.0L));

        scalar_node.at(0) = "-0";
        EXPECT_TRUE(is_near(scalar_node.as<long double>(), 0.0L));
        EXPECT_TRUE(is_near(scalar_node.as<long double>(4.0L), 0.0L));

        scalar_node.at(0) = "512";
        EXPECT_TRUE(is_near(scalar_node.as<long double>(), 512.0L));
        EXPECT_TRUE(is_near(scalar_node.as<long double>(4.0L), 512.0L));

        scalar_node.at(0) = "512.125";
        EXPECT_TRUE(is_near(scalar_node.as<long double>(), 512.125L));
        EXPECT_TRUE(is_near(scalar_node.as<long double>(4.0L), 512.125L));

        scalar_node.at(0) = "+512.125";
        EXPECT_TRUE(is_near(scalar_node.as<long double>(), 512.125L));
        EXPECT_TRUE(is_near(scalar_node.as<long double>(4.0L), 512.125L));

        scalar_node.at(0) = "-512.125";
        EXPECT_TRUE(is_near(scalar_node.as<long double>(), -512.125L));
        EXPECT_TRUE(is_near(scalar_node.as<long double>(4.0L), -512.125L));

        scalar_node.at(0) = "1.79769e+307";
        EXPECT_TRUE(is_near(scalar_node.as<long double>(), 1.79769e+307L));
        EXPECT_TRUE(is_near(scalar_node.as<long double>(4.0L), 1.79769e+307L));
    }

    // Out of range tests
    {
        scalar_node.at(0) = "1.18973e+4933";
        EXPECT_TRUE(is_near(scalar_node.as<long double>(), 0.0L));
        EXPECT_TRUE(is_near(scalar_node.as<long double>(4.0L), 4.0L));
    }
}

TEST(dom_scalar_node, as_string)
{
    using char_type = char;
    auto node = yaml::dom::node<char_type>::create_scalar();
    auto& scalar_node = node.as_scalar();

    scalar_node.push_back("");
    scalar_node.push_back("");
    scalar_node.push_back("first");
    scalar_node.push_back("second");
    scalar_node.push_back("");
    scalar_node.push_back("third \\\"");
    scalar_node.push_back("");
    scalar_node.push_back("");
    scalar_node.push_back("fourth \"");
    scalar_node.push_back("fifth ''");
    scalar_node.push_back("");
    scalar_node.push_back("");

    // Style: None
    {
        // Clip
        scalar_node.style(yaml::scalar_style_type::none);
        scalar_node.chomping(yaml::chomping_type::clip);
        EXPECT_STREQ(scalar_node.as<std::string>().c_str(), "first second\nthird \\\"\n\nfourth \" fifth ''");

        // Keep
        scalar_node.style(yaml::scalar_style_type::none);
        scalar_node.chomping(yaml::chomping_type::keep);
        EXPECT_STREQ(scalar_node.as<std::string>().c_str(), "first second\nthird \\\"\n\nfourth \" fifth ''");

        // Strip
        scalar_node.style(yaml::scalar_style_type::none);
        scalar_node.chomping(yaml::chomping_type::strip);
        EXPECT_STREQ(scalar_node.as<std::string>().c_str(), "first second\nthird \\\"\n\nfourth \" fifth ''");

    }
    // Style: Literal
    {
        // Clip
        scalar_node.style(yaml::scalar_style_type::literal);
        scalar_node.chomping(yaml::chomping_type::clip);
        EXPECT_STREQ(scalar_node.as<std::string>().c_str(), "\n\nfirst\nsecond\n\nthird \\\"\n\n\nfourth \"\nfifth ''\n");

        // Keep
        scalar_node.style(yaml::scalar_style_type::literal);
        scalar_node.chomping(yaml::chomping_type::keep);
        EXPECT_STREQ(scalar_node.as<std::string>().c_str(), "\n\nfirst\nsecond\n\nthird \\\"\n\n\nfourth \"\nfifth ''\n\n\n");

        // Strip
        scalar_node.style(yaml::scalar_style_type::literal);
        scalar_node.chomping(yaml::chomping_type::strip);
        EXPECT_STREQ(scalar_node.as<std::string>().c_str(), "\n\nfirst\nsecond\n\nthird \\\"\n\n\nfourth \"\nfifth ''");
    }
    // Style: Folded 
    {
        // Clip
        scalar_node.style(yaml::scalar_style_type::folded);
        scalar_node.chomping(yaml::chomping_type::clip);
        EXPECT_STREQ(scalar_node.as<std::string>().c_str(), "\n\nfirst second\nthird \\\"\n\nfourth \" fifth ''\n");

        // Keep
        scalar_node.style(yaml::scalar_style_type::folded);
        scalar_node.chomping(yaml::chomping_type::keep);
        EXPECT_STREQ(scalar_node.as<std::string>().c_str(), "\n\nfirst second\nthird \\\"\n\nfourth \" fifth ''\n\n\n");

        // Strip
        scalar_node.style(yaml::scalar_style_type::folded);
        scalar_node.chomping(yaml::chomping_type::strip);
        EXPECT_STREQ(scalar_node.as<std::string>().c_str(), "\n\nfirst second\nthird \\\"\n\nfourth \" fifth ''");
    }
    // Style: Double quoted
    {
        const auto double_quoted_string_0_newline = std::string{ "first second\nthird \"\n\nfourth \" fifth ''" };
        const auto double_quoted_string_1_newline = std::string{ " first second\nthird \"\n\nfourth \" fifth '' " };
        const auto double_quoted_string_2_newline = std::string{ "\nfirst second\nthird \"\n\nfourth \" fifth ''\n" };

        {
            scalar_node.style(yaml::scalar_style_type::double_quoted);

            scalar_node.pop_front();
            scalar_node.pop_front();
            scalar_node.pop_back();
            scalar_node.pop_back();

            scalar_node.chomping(yaml::chomping_type::clip);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_0_newline.c_str());
            scalar_node.chomping(yaml::chomping_type::keep);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_0_newline.c_str());
            scalar_node.chomping(yaml::chomping_type::strip);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_0_newline.c_str());

            scalar_node.push_front("");
            scalar_node.push_back("");

            scalar_node.chomping(yaml::chomping_type::clip);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_1_newline.c_str());
            scalar_node.chomping(yaml::chomping_type::keep);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_1_newline.c_str());
            scalar_node.chomping(yaml::chomping_type::strip);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_1_newline.c_str());

            scalar_node.push_front("");
            scalar_node.push_back("");

            scalar_node.chomping(yaml::chomping_type::clip);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_2_newline.c_str());
            scalar_node.chomping(yaml::chomping_type::keep);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_2_newline.c_str());
            scalar_node.chomping(yaml::chomping_type::strip);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_2_newline.c_str());
        }
    }
    // Style: Single quoted
    {
        const auto double_quoted_string_0_newline = std::string{ "first second\nthird \\\"\n\nfourth \" fifth '" };
        const auto double_quoted_string_1_newline = std::string{ " first second\nthird \\\"\n\nfourth \" fifth ' " };
        const auto double_quoted_string_2_newline = std::string{ "\nfirst second\nthird \\\"\n\nfourth \" fifth '\n" };

        {
            scalar_node.style(yaml::scalar_style_type::single_quoted);

            scalar_node.pop_front();
            scalar_node.pop_front();
            scalar_node.pop_back();
            scalar_node.pop_back();

            scalar_node.chomping(yaml::chomping_type::clip);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_0_newline.c_str());
            scalar_node.chomping(yaml::chomping_type::keep);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_0_newline.c_str());
            scalar_node.chomping(yaml::chomping_type::strip);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_0_newline.c_str());

            scalar_node.push_front("");
            scalar_node.push_back("");

            scalar_node.chomping(yaml::chomping_type::clip);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_1_newline.c_str());
            scalar_node.chomping(yaml::chomping_type::keep);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_1_newline.c_str());
            scalar_node.chomping(yaml::chomping_type::strip);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_1_newline.c_str());

            scalar_node.push_front("");
            scalar_node.push_back("");

            scalar_node.chomping(yaml::chomping_type::clip);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_2_newline.c_str());
            scalar_node.chomping(yaml::chomping_type::keep);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_2_newline.c_str());
            scalar_node.chomping(yaml::chomping_type::strip);
            EXPECT_STREQ(scalar_node.as<std::string>().c_str(), double_quoted_string_2_newline.c_str());
        }
    }
}

TEST(dom_scalar_node, ok_read)
{
    const std::string input =
        "This is a scalar\n"
        "with multiple lines.";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
    ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
    auto& scalar_node = node.as_scalar();

    EXPECT_EQ(scalar_node.style(), yaml::scalar_style_type::none);
    EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::strip);

    EXPECT_STREQ(scalar_node.as<std::string>().c_str(), "This is a scalar with multiple lines.");
}

TEST(dom_scalar_node, ok_read_with_gaps)
{
    const std::string input =
        "first\n"
        "second\n"
        "\n"
        "third\n"
        "\n"
        "\n"
        "fourth\n"
        "\n"
        "\n";

    auto read_result = yaml::dom::read_document(input);
    ASSERT_EQ(read_result.result_code, yaml::read_result_code::success);

    auto node = std::move(read_result.root_node);
    ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
    ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
    auto& scalar_node = node.as_scalar();

    EXPECT_EQ(scalar_node.style(), yaml::scalar_style_type::none);
    EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::strip);

    EXPECT_EQ(scalar_node.size(), size_t{ 7 });

    EXPECT_STREQ(scalar_node.as<std::string>().c_str(), "first second\nthird\n\nfourth");

    // Add removed trailing empty lines + front empty line.
    scalar_node.insert(scalar_node.begin(), "");
    scalar_node.push_back("");
    scalar_node.push_back("");
    EXPECT_EQ(scalar_node.size(), size_t{ 10 });

    EXPECT_STREQ(scalar_node.as<std::string>().c_str(), "first second\nthird\n\nfourth");
}

TEST(dom_scalar_node, ok_scalar)
{
    using char_type = char;

    auto node = yaml::dom::node<char_type>::create_scalar();

    ASSERT_EQ(node.type(), yaml::dom::node_type::scalar);
    ASSERT_NO_THROW_IGNORE_NODISCARD(node.as_scalar());
    EXPECT_ANY_THROW_IGNORE_NODISCARD(node.as_object());
    EXPECT_ANY_THROW_IGNORE_NODISCARD(node.as_array());
    EXPECT_FALSE(node.is_null());
    EXPECT_TRUE(node.is_scalar());
    EXPECT_FALSE(node.is_object());
    EXPECT_FALSE(node.is_array());

    auto& scalar_node = node.as_scalar();
    EXPECT_EQ(&scalar_node.overlying_node(), &node);

    // Block style tests.
    EXPECT_EQ(scalar_node.style(), yaml::scalar_style_type::none);
    EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::strip);

    scalar_node.style(yaml::scalar_style_type::literal);
    EXPECT_EQ(scalar_node.style(), yaml::scalar_style_type::literal);

    scalar_node.chomping(yaml::chomping_type::keep);
    EXPECT_EQ(scalar_node.chomping(), yaml::chomping_type::keep);

    // Empty tests.
    ASSERT_TRUE(scalar_node.empty());
    ASSERT_EQ(scalar_node.size(), size_t{ 0 });

    EXPECT_EQ(scalar_node.begin(), scalar_node.end());
    EXPECT_EQ(scalar_node.cbegin(), scalar_node.cend());
    EXPECT_EQ(scalar_node.crbegin(), scalar_node.crend());

    // Insert tests.
    {
        scalar_node.push_back("First line of scalar.");

        ASSERT_FALSE(scalar_node.empty());
        ASSERT_EQ(scalar_node.size(), size_t{ 1 });

        ASSERT_NE(scalar_node.begin(), scalar_node.end());
        ASSERT_NE(scalar_node.cbegin(), scalar_node.cend());
        ASSERT_NE(scalar_node.crbegin(), scalar_node.crend());

        EXPECT_STREQ(scalar_node.begin()->c_str(), "First line of scalar.");

        scalar_node.push_back("Second line of scalar.");

        ASSERT_EQ(scalar_node.size(), size_t{ 2 });
    }
    // Loop
    {
        std::vector<std::string> lines = {
            "First line of scalar.",
            "Second line of scalar."
        };

        std::vector<std::string> lines_reverse = {
            "Second line of scalar.",
            "First line of scalar."
        };

        const auto& const_scalar_node = scalar_node;

        ok_scalar_loop_test(scalar_node.begin(), scalar_node.end(), false, lines);
        ok_scalar_loop_test(const_scalar_node.begin(), const_scalar_node.end(), true, lines);
        ok_scalar_loop_test(scalar_node.cbegin(), scalar_node.cend(), true, lines);
        ok_scalar_loop_test(scalar_node.rbegin(), scalar_node.rend(), false, lines_reverse);
        ok_scalar_loop_test(const_scalar_node.rbegin(), const_scalar_node.rend(), true, lines_reverse);
        ok_scalar_loop_test(scalar_node.crbegin(), scalar_node.crend(), true, lines_reverse);
    }

    // as_string() tests.
    {
        scalar_node.style(yaml::scalar_style_type::none);
        scalar_node.chomping(yaml::chomping_type::strip);
        EXPECT_STREQ(scalar_node.as<std::string>().c_str(), "First line of scalar. Second line of scalar.");
    }
}