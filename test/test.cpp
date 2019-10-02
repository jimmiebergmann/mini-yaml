/*
* MIT License
*
* Copyright(c) 2019 Jimmie Bergmann
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


#include "gtest/gtest.h"
#include "../yaml/yaml.hpp"
#include <fstream>
#include <memory>

TEST(exceptions, throw)
{

    { // yaml::exception
        {
            bool throwed = false;
            try
            {
                throw yaml::exception("exception message: internal_error", yaml::exception_type::internal_error);
            }
            catch (const yaml::exception & e)
            {
                throwed = true;
                EXPECT_EQ(e.type(), yaml::exception_type::internal_error);
                EXPECT_STREQ(e.what(), "exception message: internal_error");
            }
            EXPECT_TRUE(throwed);
        }
        {
            bool throwed = false;
            try
            {
                throw yaml::exception("exception message: parsing_error", yaml::exception_type::parsing_error);
            }
            catch (const yaml::exception & e)
            {
                throwed = true;
                EXPECT_EQ(e.type(), yaml::exception_type::parsing_error);
                EXPECT_STREQ(e.what(), "exception message: parsing_error");
            }
            EXPECT_TRUE(throwed);
        }
        {
            bool throwed = false;
            try
            {
                throw yaml::exception("exception message: operation_error", yaml::exception_type::operation_error);
            }
            catch (const yaml::exception & e)
            {
                throwed = true;
                EXPECT_EQ(e.type(), yaml::exception_type::operation_error);
                EXPECT_STREQ(e.what(), "exception message: operation_error");
            }
            EXPECT_TRUE(throwed);
        }
    }
    { // yaml::error
        {
            bool throwed = false;
            try
            {
                throw yaml::error("exception message: internal_error", yaml::exception_type::internal_error);
            }
            catch (const yaml::error & e)
            {
                throwed = true;
                EXPECT_EQ(e.type(), yaml::exception_type::internal_error);
                EXPECT_STREQ(e.what(), "exception message: internal_error");
            }
            EXPECT_TRUE(throwed);
        }
        {
            bool throwed = false;
            try
            {
                throw yaml::error("exception message: parsing_error", yaml::exception_type::parsing_error);
            }
            catch (const yaml::error & e)
            {
                throwed = true;
                EXPECT_EQ(e.type(), yaml::exception_type::parsing_error);
                EXPECT_STREQ(e.what(), "exception message: parsing_error");
            }
            EXPECT_TRUE(throwed);
        }
        {
            bool throwed = false;
            try
            {
                throw yaml::error("exception message: operation_error", yaml::exception_type::operation_error);
            }
            catch (const yaml::error & e)
            {
                throwed = true;
                EXPECT_EQ(e.type(), yaml::exception_type::operation_error);
                EXPECT_STREQ(e.what(), "exception message: operation_error");
            }
            EXPECT_TRUE(throwed);
        }

        {
            bool throwed = false;
            try
            {
                throw yaml::error("throw error, catch exception", yaml::exception_type::operation_error);
            }
            catch (const yaml::exception & e)
            {
                throwed = true;
                EXPECT_EQ(e.type(), yaml::exception_type::operation_error);
                EXPECT_STREQ(e.what(), "throw error, catch exception");
            }
            EXPECT_TRUE(throwed);
        }
        {
            bool throwed = false;
            try
            {
                throw yaml::exception("throw exception, catch error", yaml::exception_type::operation_error);
            }
            catch (const yaml::error &)
            {
            }
            catch (const yaml::exception &)
            {
            }
            EXPECT_FALSE(throwed);
        }
    }
    { // yaml::internal_error
        bool throwed = false;
        try
        {
            throw yaml::internal_error("throwing internal error.");
        }
        catch (const yaml::internal_error & e)
        {
            throwed = true;
            EXPECT_EQ(e.type(), yaml::exception_type::internal_error);
            EXPECT_STREQ(e.what(), "throwing internal error.");
        }
        EXPECT_TRUE(throwed);
    }
    { // yaml::parsing_error
        bool throwed = false;
        try
        {
            throw yaml::parsing_error("throwing parsing error.");
        }
        catch (const yaml::parsing_error & e)
        {
            throwed = true;
            EXPECT_EQ(e.type(), yaml::exception_type::parsing_error);
            EXPECT_STREQ(e.what(), "throwing parsing error.");
        }
        EXPECT_TRUE(throwed);
    }
    { // yaml::operation_error
        bool throwed = false;
        try
        {
            throw yaml::operation_error("throwing operation error.");
        }
        catch (const yaml::operation_error & e)
        {
            throwed = true;
            EXPECT_EQ(e.type(), yaml::exception_type::operation_error);
            EXPECT_STREQ(e.what(), "throwing operation error.");
        }
        EXPECT_TRUE(throwed);
    }
}

TEST(priv, data_converter)
{
    {
        EXPECT_TRUE((yaml::priv::data_converter<std::string, bool>::get("yes")));
        EXPECT_TRUE((yaml::priv::data_converter<std::string, bool>::get("Yes")));
        EXPECT_TRUE((yaml::priv::data_converter<std::string, bool>::get("yeS")));
        EXPECT_TRUE((yaml::priv::data_converter<std::string, bool>::get("YES")));

        EXPECT_FALSE((yaml::priv::data_converter<std::string, bool>::get("no")));
        EXPECT_FALSE((yaml::priv::data_converter<std::string, bool>::get("foo")));
        EXPECT_FALSE((yaml::priv::data_converter<std::string, bool>::get("")));

        EXPECT_TRUE((yaml::priv::data_converter<std::string, bool>::get("yes", true)));
        EXPECT_TRUE((yaml::priv::data_converter<std::string, bool>::get("yes", false)));

        EXPECT_FALSE((yaml::priv::data_converter<std::string, bool>::get("no", true)));
        EXPECT_FALSE((yaml::priv::data_converter<std::string, bool>::get("no", false)));

        EXPECT_TRUE((yaml::priv::data_converter<std::string, bool>::get("", true)));
        EXPECT_FALSE((yaml::priv::data_converter<std::string, bool>::get("", false)));

        EXPECT_TRUE((yaml::priv::data_converter<std::string, bool>::get("foo", true)));
        EXPECT_FALSE((yaml::priv::data_converter<std::string, bool>::get("foo", false)));
    }
    {
        EXPECT_EQ((yaml::priv::data_converter<std::string, int32_t>::get("1")), int32_t(1));
        EXPECT_EQ((yaml::priv::data_converter<std::string, int32_t>::get("0")), int32_t(0));
        EXPECT_EQ((yaml::priv::data_converter<std::string, int32_t>::get("-1")), int32_t(-1));
        EXPECT_EQ((yaml::priv::data_converter<std::string, int32_t>::get("123456789")), int32_t(123456789));
        EXPECT_EQ((yaml::priv::data_converter<std::string, int32_t>::get("-123456789")), int32_t(-123456789));
    }
    {
        EXPECT_STREQ((yaml::priv::data_converter<std::string, std::string>::get("foo bar")).c_str(), "foo bar");
        EXPECT_STREQ((yaml::priv::data_converter<std::string, std::string>::get("foo", "")).c_str(), "foo");
        EXPECT_STREQ((yaml::priv::data_converter<std::string, std::string>::get("foo", "bar")).c_str(), "foo");
        EXPECT_STREQ((yaml::priv::data_converter<std::string, std::string>::get("", "bar")).c_str(), "bar");
        EXPECT_STREQ((yaml::priv::data_converter<std::string, std::string>::get("", "")).c_str(), "");
    }
}

TEST(priv, null_node_impl)
{
    {
        yaml::priv::null_node_impl impl;
        EXPECT_EQ(impl.data_type(), yaml::node_data_type::null);
    }
}

TEST(priv, map_node_impl)
{
    {
        yaml::priv::map_node_impl impl;
        EXPECT_EQ(impl.data_type(), yaml::node_data_type::null);
    }
}

TEST(priv, sequence_node_impl)
{
    {
        yaml::priv::sequence_node_impl impl;
        EXPECT_EQ(impl.data_type(), yaml::node_data_type::null);
    }
}

TEST(priv, scalar_node_impl)
{
    {
        yaml::priv::scalar_node_impl impl_1;
        EXPECT_EQ(impl_1.data_type(), yaml::node_data_type::null);

        yaml::priv::scalar_node_impl impl_2(yaml::node_data_type::null);
        EXPECT_EQ(impl_2.data_type(), yaml::node_data_type::null);

        yaml::priv::scalar_node_impl impl_3(yaml::node_data_type::boolean);
        EXPECT_EQ(impl_3.data_type(), yaml::node_data_type::boolean);

        yaml::priv::scalar_node_impl impl_4(yaml::node_data_type::float32);
        EXPECT_EQ(impl_4.data_type(), yaml::node_data_type::float32);

        yaml::priv::scalar_node_impl impl_5(yaml::node_data_type::float64);
        EXPECT_EQ(impl_5.data_type(), yaml::node_data_type::float64);

        yaml::priv::scalar_node_impl impl_6(yaml::node_data_type::int32);
        EXPECT_EQ(impl_6.data_type(), yaml::node_data_type::int32);

        yaml::priv::scalar_node_impl impl_7(yaml::node_data_type::int64);
        EXPECT_EQ(impl_7.data_type(), yaml::node_data_type::int64);

        yaml::priv::scalar_node_impl impl_8(yaml::node_data_type::string);
        EXPECT_EQ(impl_8.data_type(), yaml::node_data_type::string);
    }
    {
        {
            yaml::priv::scalar_node_impl impl(bool(false));
            EXPECT_EQ(impl.data_type(), yaml::node_data_type::boolean);
            EXPECT_EQ(impl.as<bool>(), bool(false));
            EXPECT_EQ(impl.as<float>(), float(0.0f));
            EXPECT_EQ(impl.as<double>(), double(0.0f));
            EXPECT_EQ(impl.as<int32_t>(), int32_t(0));
            EXPECT_EQ(impl.as<int64_t>(), int64_t(0));
            EXPECT_STREQ(impl.as<std::string>().c_str(), "false");
        }
        {
            yaml::priv::scalar_node_impl impl(bool(true));
            EXPECT_EQ(impl.data_type(), yaml::node_data_type::boolean);
            EXPECT_EQ(impl.as<bool>(), bool(true));
            EXPECT_EQ(impl.as<float>(), float(1.0f));
            EXPECT_EQ(impl.as<double>(), double(1.0f));
            EXPECT_EQ(impl.as<int32_t>(), int32_t(1));
            EXPECT_EQ(impl.as<int64_t>(), int64_t(1));
            EXPECT_STREQ(impl.as<std::string>().c_str(), "true");
        }
        {
            {
                yaml::priv::scalar_node_impl impl(float(3.0f));
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::float32);
                EXPECT_EQ(impl.as<bool>(), bool(true));
                EXPECT_EQ(impl.as<float>(), float(3.0f));
                EXPECT_EQ(impl.as<double>(), double(3.0f));
                EXPECT_EQ(impl.as<int32_t>(), int32_t(3));
                EXPECT_EQ(impl.as<int64_t>(), int64_t(3));
                EXPECT_STREQ(impl.as<std::string>().c_str(), "3");
            }
            {
                yaml::priv::scalar_node_impl impl(float(0.0f));
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::float32);
                EXPECT_EQ(impl.as<bool>(), bool(false));
            }
            {
                yaml::priv::scalar_node_impl impl(float(-1.0f));
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::float32);
                EXPECT_EQ(impl.as<bool>(), bool(true));
            }
        }
        {
            {
                yaml::priv::scalar_node_impl impl(double(3.0f));
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::float64);
                EXPECT_EQ(impl.as<bool>(), bool(true));
                EXPECT_EQ(impl.as<float>(), float(3.0f));
                EXPECT_EQ(impl.as<double>(), double(3.0f));
                EXPECT_EQ(impl.as<int32_t>(), int32_t(3));
                EXPECT_EQ(impl.as<int64_t>(), int64_t(3));
                EXPECT_STREQ(impl.as<std::string>().c_str(), "3");
            }
            {
                yaml::priv::scalar_node_impl impl(double(0.0f));
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::float64);
                EXPECT_EQ(impl.as<bool>(), bool(false));
            }
            {
                yaml::priv::scalar_node_impl impl(double(-1.0f));
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::float64);
                EXPECT_EQ(impl.as<bool>(), bool(true));
            }
        }
        {
            {
                yaml::priv::scalar_node_impl impl(int32_t(3));
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::int32);
                EXPECT_EQ(impl.as<bool>(), bool(true));
                EXPECT_EQ(impl.as<float>(), float(3.0f));
                EXPECT_EQ(impl.as<double>(), double(3.0f));
                EXPECT_EQ(impl.as<int32_t>(), int32_t(3));
                EXPECT_EQ(impl.as<int64_t>(), int64_t(3));
                EXPECT_STREQ(impl.as<std::string>().c_str(), "3");
            }
            {
                yaml::priv::scalar_node_impl impl(int32_t(0));
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::int32);
                EXPECT_EQ(impl.as<bool>(), bool(false));
            }
            {
                yaml::priv::scalar_node_impl impl(int32_t(-1));
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::int32);
                EXPECT_EQ(impl.as<bool>(), bool(true));
            }
        }
        {
            {
                yaml::priv::scalar_node_impl impl(int64_t(3));
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::int64);
                EXPECT_EQ(impl.as<bool>(), bool(true));
                EXPECT_EQ(impl.as<float>(), float(3.0f));
                EXPECT_EQ(impl.as<double>(), double(3.0f));
                EXPECT_EQ(impl.as<int32_t>(), int32_t(3));
                EXPECT_EQ(impl.as<int64_t>(), int64_t(3));
                EXPECT_STREQ(impl.as<std::string>().c_str(), "3");
            }
            {
                yaml::priv::scalar_node_impl impl(int64_t(0));
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::int64);
                EXPECT_EQ(impl.as<bool>(), bool(false));
            }
            {
                yaml::priv::scalar_node_impl impl(int64_t(-1));
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::int64);
                EXPECT_EQ(impl.as<bool>(), bool(true));
            }
        }
        {
            {
                yaml::priv::scalar_node_impl impl("foo bar");
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::string);
                EXPECT_EQ(impl.as<bool>(), bool(false));
                EXPECT_EQ(impl.as<float>(), float(0.0f));
                EXPECT_EQ(impl.as<double>(), double(0.0f));
                EXPECT_EQ(impl.as<int32_t>(), int32_t(0));
                EXPECT_EQ(impl.as<int64_t>(), int64_t(0));
                EXPECT_STREQ(impl.as<std::string>().c_str(), "foo bar");
            }
            {
                yaml::priv::scalar_node_impl impl("true");
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::string);
                EXPECT_EQ(impl.as<bool>(), bool(true));
                EXPECT_EQ(impl.as<float>(), float(0.0f));
                EXPECT_EQ(impl.as<double>(), double(0.0f));
                EXPECT_EQ(impl.as<int32_t>(), int32_t(0));
                EXPECT_EQ(impl.as<int64_t>(), int64_t(0));
                EXPECT_STREQ(impl.as<std::string>().c_str(), "true");
            }
            {
                yaml::priv::scalar_node_impl impl("false");
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::string);
                EXPECT_EQ(impl.as<bool>(), bool(false));
                EXPECT_EQ(impl.as<float>(), float(0.0f));
                EXPECT_EQ(impl.as<double>(), double(0.0f));
                EXPECT_EQ(impl.as<int32_t>(), int32_t(0));
                EXPECT_EQ(impl.as<int64_t>(), int64_t(0));
                EXPECT_STREQ(impl.as<std::string>().c_str(), "false");
            }
            {
                yaml::priv::scalar_node_impl impl("-128");
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::string);
                EXPECT_EQ(impl.as<bool>(), bool(false));
                EXPECT_EQ(impl.as<float>(), float(-128.0f));
                EXPECT_EQ(impl.as<double>(), double(-128.0f));
                EXPECT_EQ(impl.as<int32_t>(), int32_t(-128));
                EXPECT_EQ(impl.as<int64_t>(), int64_t(-128));
                EXPECT_STREQ(impl.as<std::string>().c_str(), "-128");
            }
            {
                yaml::priv::scalar_node_impl impl("64.5");
                EXPECT_EQ(impl.data_type(), yaml::node_data_type::string);
                EXPECT_EQ(impl.as<bool>(), bool(false));
                EXPECT_EQ(impl.as<float>(), float(64.5));
                EXPECT_EQ(impl.as<double>(), double(64.5f));
                EXPECT_EQ(impl.as<int32_t>(), int32_t(64));
                EXPECT_EQ(impl.as<int64_t>(), int64_t(64));
                EXPECT_STREQ(impl.as<std::string>().c_str(), "64.5");
            }
        }
    }
}

TEST(node, constructor)
{
    {
        yaml::node node;
        EXPECT_EQ(node.type(), yaml::node_type::null);
        EXPECT_TRUE(node.is_null());
        EXPECT_FALSE(node.is_scalar());
        EXPECT_FALSE(node.is_map());
        EXPECT_FALSE(node.is_sequence());
        EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
    }
}

TEST(node, constructor_scalar)
{
    {
        yaml::node node1(bool(true));
        EXPECT_EQ(node1.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::boolean);
        EXPECT_EQ(node1.as<bool>(), bool(true));

        yaml::node node2(bool(false));
        EXPECT_EQ(node2.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node2.is_scalar());
        EXPECT_FALSE(node2.is_null());
        EXPECT_FALSE(node2.is_map());
        EXPECT_FALSE(node2.is_sequence());
        EXPECT_EQ(node2.data_type(), yaml::node_data_type::boolean);
        EXPECT_EQ(node2.as<bool>(), bool(false));
    }
    {
        yaml::node node1(float(3.5f));
        EXPECT_EQ(node1.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::float32);
        EXPECT_EQ(node1.as<float>(), float(3.5f));

        yaml::node node2(float(-5.0f));
        EXPECT_EQ(node2.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node2.is_scalar());
        EXPECT_FALSE(node2.is_null());
        EXPECT_FALSE(node2.is_map());
        EXPECT_FALSE(node2.is_sequence());
        EXPECT_EQ(node2.data_type(), yaml::node_data_type::float32);
        EXPECT_EQ(node2.as<float>(), float(-5.0f));
    }
    {
        yaml::node node1(double(7.5f));
        EXPECT_EQ(node1.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::float64);
        EXPECT_EQ(node1.as<double>(), double(7.5f));

        yaml::node node2(double(-6.0f));
        EXPECT_EQ(node2.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node2.is_scalar());
        EXPECT_FALSE(node2.is_null());
        EXPECT_FALSE(node2.is_map());
        EXPECT_FALSE(node2.is_sequence());
        EXPECT_EQ(node2.data_type(), yaml::node_data_type::float64);
        EXPECT_EQ(node2.as<double>(), double(-6.0f));
    }
    {
        yaml::node node1(int32_t(12345));
        EXPECT_EQ(node1.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::int32);
        EXPECT_EQ(node1.as<int32_t>(), int32_t(12345));

        yaml::node node2(int64_t(-4345));
        EXPECT_EQ(node2.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node2.is_scalar());
        EXPECT_FALSE(node2.is_null());
        EXPECT_FALSE(node2.is_map());
        EXPECT_FALSE(node2.is_sequence());
        EXPECT_EQ(node2.data_type(), yaml::node_data_type::int64);
        EXPECT_EQ(node2.as<int64_t>(), int64_t(-4345));
    }
    {
        yaml::node node1(std::string("coolio"));
        EXPECT_EQ(node1.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::string);
        EXPECT_STREQ(node1.as<std::string>().c_str(), "coolio");

        yaml::node node2(std::string("o'rly?"));
        EXPECT_EQ(node2.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node2.is_scalar());
        EXPECT_FALSE(node2.is_null());
        EXPECT_FALSE(node2.is_map());
        EXPECT_FALSE(node2.is_sequence());
        EXPECT_EQ(node2.data_type(), yaml::node_data_type::string);
        EXPECT_STREQ(node2.as<std::string>().c_str(), "o'rly?");
    }
    {
        const char str_value_1[] = "test";
        const char str_value_2[] = "foo bar";

        yaml::node node1(str_value_1);
        EXPECT_EQ(node1.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::string);
        EXPECT_STREQ(node1.as<std::string>().c_str(), "test");

        yaml::node node2(str_value_2);
        EXPECT_EQ(node2.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node2.is_scalar());
        EXPECT_FALSE(node2.is_null());
        EXPECT_FALSE(node2.is_map());
        EXPECT_FALSE(node2.is_sequence());
        EXPECT_EQ(node2.data_type(), yaml::node_data_type::string);
        EXPECT_STREQ(node2.as<std::string>().c_str(), "foo bar");
    }

}

TEST(node, constructor_node_type)
{
    {
        yaml::node node1(yaml::node_type::null);
        EXPECT_EQ(node1.type(), yaml::node_type::null);
        EXPECT_FALSE(node1.is_scalar());
        EXPECT_TRUE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::null);
    }
    {
        yaml::node node1(yaml::node_type::map);
        EXPECT_EQ(node1.type(), yaml::node_type::map);
        EXPECT_FALSE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_TRUE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::null);
    }
    {
        yaml::node node1(yaml::node_type::scalar);
        EXPECT_EQ(node1.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::null);
    }
    {
        yaml::node node1(yaml::node_type::sequence);
        EXPECT_EQ(node1.type(), yaml::node_type::sequence);
        EXPECT_FALSE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_TRUE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::null);
    }
}

TEST(node, constructor_node_data_type)
{
    {
        yaml::node node1(yaml::node_data_type::null);
        EXPECT_EQ(node1.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::null);
    }
    {
        yaml::node node1(yaml::node_data_type::boolean);
        EXPECT_EQ(node1.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::boolean);
    }
    {
        yaml::node node1(yaml::node_data_type::float32);
        EXPECT_EQ(node1.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::float32);
    }
    {
        yaml::node node1(yaml::node_data_type::float64);
        EXPECT_EQ(node1.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::float64);
    }
    {
        yaml::node node1(yaml::node_data_type::int32);
        EXPECT_EQ(node1.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::int32);
    }
    {
        yaml::node node1(yaml::node_data_type::int64);
        EXPECT_EQ(node1.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::int64);
    }
    {
        yaml::node node1(yaml::node_data_type::string);
        EXPECT_EQ(node1.type(), yaml::node_type::scalar);
        EXPECT_TRUE(node1.is_scalar());
        EXPECT_FALSE(node1.is_null());
        EXPECT_FALSE(node1.is_map());
        EXPECT_FALSE(node1.is_sequence());
        EXPECT_EQ(node1.data_type(), yaml::node_data_type::string);
    }
}

TEST(node, assign_scalar_value)
{
    yaml::node node;

    EXPECT_FALSE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::null);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

    EXPECT_NO_THROW(node = bool(true));
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::boolean);
    EXPECT_EQ(node.as<bool>(), bool(true));
    EXPECT_STREQ(node.as<std::string>().c_str(), "true");

    EXPECT_NO_THROW(node = bool(false));
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::boolean);
    EXPECT_EQ(node.as<bool>(), bool(false));
    EXPECT_STREQ(node.as<std::string>().c_str(), "false");

    EXPECT_NO_THROW(node = float(-2.0f));
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::float32);
    EXPECT_EQ(node.as<float>(), float(-2.0f));

    EXPECT_NO_THROW(node = double(-2.0f));
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::float64);
    EXPECT_EQ(node.as<double>(), double(-2.0f));

    EXPECT_NO_THROW(node = int32_t(1234567));
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::int32);
    EXPECT_EQ(node.as<int32_t>(), int32_t(1234567));

    EXPECT_NO_THROW(node = int64_t(123456789012LL));
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::int64);
    EXPECT_EQ(node.as<int64_t>(), int64_t(123456789012LL));

    EXPECT_NO_THROW(node = "cool kid.");
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::string);
    EXPECT_STREQ(node.as<std::string>().c_str(), "cool kid.");
   
    EXPECT_NO_THROW(node.clear());
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.data_type(), yaml::node_data_type::string);
    EXPECT_STREQ(node.as<std::string>().c_str(), "");
    
    EXPECT_NO_THROW(node = std::string("cool kid."));
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::string);
    EXPECT_STREQ(node.as<std::string>().c_str(), "cool kid.");

    std::string my_string = "new string";
    EXPECT_NO_THROW(node = my_string);
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::string);
    EXPECT_STREQ(node.as<std::string>().c_str(), "new string");
}

TEST(node, assign_node_data_type)
{
    yaml::node node;

    EXPECT_NO_THROW(node = yaml::node_data_type::boolean);
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::boolean);

    EXPECT_NO_THROW(node = yaml::node_data_type::float32);
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::float32);

    EXPECT_NO_THROW(node = yaml::node_data_type::float64);
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::float64);

    EXPECT_NO_THROW(node = yaml::node_data_type::int32);
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::int32);

    EXPECT_NO_THROW(node = yaml::node_data_type::int64);
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::int64);

    EXPECT_NO_THROW(node = yaml::node_data_type::null);
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

    EXPECT_NO_THROW(node = yaml::node_data_type::string);
    EXPECT_TRUE(node.is_scalar());
    EXPECT_EQ(node.type(), yaml::node_type::scalar);
    EXPECT_EQ(node.data_type(), yaml::node_data_type::string);
}

TEST(node, as_default)
{
    {
        yaml::node node1(std::string("foo"));
        EXPECT_TRUE(node1.as<bool>(true));
        EXPECT_FALSE(node1.as<bool>(false));

        yaml::node node2(std::string("bar"));
        EXPECT_TRUE(node2.as<bool>(true));
        EXPECT_FALSE(node2.as<bool>(false));

        yaml::node node3(std::string("true"));
        EXPECT_TRUE(node3.as<bool>(true));
        EXPECT_TRUE(node3.as<bool>(false));

        yaml::node node4(std::string("0"));
        EXPECT_FALSE(node4.as<bool>(true));
        EXPECT_FALSE(node4.as<bool>(false));
    }
    {
        yaml::node node1(std::string("128.5"));
        EXPECT_EQ(node1.as<float>(64.0f), float(128.5f));
        EXPECT_EQ(node1.as<float>(32.0f), float(128.5f));

        yaml::node node2(std::string("cool"));
        EXPECT_EQ(node2.as<float>(100.0f), float(100.0f));
        EXPECT_EQ(node2.as<float>(200.0f), float(200.0f));
    }
    {
        yaml::node node1(std::string("64.5"));
        EXPECT_EQ(node1.as<double>(5.0f), double(64.5f));
        EXPECT_EQ(node1.as<double>(10.0f), double(64.5f));

        yaml::node node2(std::string("cool"));
        EXPECT_EQ(node2.as<double>(300.0f), double(300.0f));
        EXPECT_EQ(node2.as<double>(400.0f), double(400.0f));
    }
    {
        yaml::node node1(std::string("64.5"));
        EXPECT_EQ(node1.as<int32_t>(5), int32_t(64));
        EXPECT_EQ(node1.as<int32_t>(10), int32_t(64));

        yaml::node node2(std::string("cool"));
        EXPECT_EQ(node2.as<int32_t>(500), int32_t(500));
        EXPECT_EQ(node2.as<int32_t>(600), int32_t(600));

        yaml::node node3(std::string("87678"));
        EXPECT_EQ(node3.as<int32_t>(500), int32_t(87678));
        EXPECT_EQ(node3.as<int32_t>(400), int32_t(87678));
    }
    {
        yaml::node node1(std::string("512.5"));
        EXPECT_EQ(node1.as<int64_t>(50), int64_t(512));
        EXPECT_EQ(node1.as<int64_t>(100), int64_t(512));

        yaml::node node2(std::string("cool"));
        EXPECT_EQ(node2.as<int64_t>(500000), int64_t(500000));
        EXPECT_EQ(node2.as<int64_t>(600000), int64_t(600000));

        yaml::node node3(std::string("87678909876"));
        EXPECT_EQ(node3.as<int64_t>(500), int64_t(87678909876));
        EXPECT_EQ(node3.as<int64_t>(400), int64_t(87678909876));
    }
    {
        yaml::node node1(std::string("The quick brown fox jumps over the lazy dog"));
        EXPECT_STREQ(node1.as<std::string>("").c_str(), "The quick brown fox jumps over the lazy dog");
        EXPECT_STREQ(node1.as<std::string>("Some random text").c_str(), "The quick brown fox jumps over the lazy dog");

        yaml::node node2(std::string(""));
        EXPECT_STREQ(node2.as<std::string>("").c_str(), "");
        EXPECT_STREQ(node2.as<std::string>("Some random text").c_str(), "");
    }
}

TEST(node, exists)
{
    {
        {
            yaml::node node;
            EXPECT_TRUE(node.is_null());
            EXPECT_FALSE(node.exists(""));
            EXPECT_FALSE(node.exists("empty"));
            EXPECT_FALSE(node.exists("foo bar"));
            EXPECT_FALSE(node.exists("The quick brown fox jumps over the lazy dog"));
        }
        {
            yaml::node node(yaml::node_type::map);
            EXPECT_TRUE(node.is_map());
            EXPECT_FALSE(node.exists(""));
            EXPECT_FALSE(node.exists("empty"));
            EXPECT_FALSE(node.exists("foo bar"));
            EXPECT_FALSE(node.exists("The quick brown fox jumps over the lazy dog"));
        }
        {
            yaml::node node(yaml::node_type::scalar);
            EXPECT_TRUE(node.is_scalar());
            EXPECT_FALSE(node.exists(""));
            EXPECT_FALSE(node.exists("empty"));
            EXPECT_FALSE(node.exists("foo bar"));
            EXPECT_FALSE(node.exists("The quick brown fox jumps over the lazy dog"));
        }
        {
            yaml::node node(yaml::node_type::sequence);
            EXPECT_TRUE(node.is_sequence());
            EXPECT_FALSE(node.exists(""));
            EXPECT_FALSE(node.exists("empty"));
            EXPECT_FALSE(node.exists("foo bar"));
            EXPECT_FALSE(node.exists("The quick brown fox jumps over the lazy dog"));
        }
    }
    {
        yaml::node node(yaml::node_type::map);
        EXPECT_TRUE(node.is_map());
        
        EXPECT_FALSE(node.exists("my_key_1"));
        EXPECT_FALSE(node.exists("my_key_2"));
        EXPECT_FALSE(node.exists("my_key_3"));
        EXPECT_FALSE(node.exists("my_key_4"));

        EXPECT_NO_THROW(node.insert("my_key_1", yaml::node_type::sequence));
        EXPECT_TRUE(node.exists("my_key_1"));
        EXPECT_FALSE(node.exists("my_key_2"));
        EXPECT_FALSE(node.exists("my_key_3"));
        EXPECT_FALSE(node.exists("my_key_4"));

        EXPECT_NO_THROW(node.insert("my_key_2", yaml::node_data_type::string));
        EXPECT_TRUE(node.exists("my_key_1"));
        EXPECT_TRUE(node.exists("my_key_2"));
        EXPECT_FALSE(node.exists("my_key_3"));
        EXPECT_FALSE(node.exists("my_key_4"));

        EXPECT_NO_THROW(node.insert("my_key_3", int32_t(10)));
        EXPECT_TRUE(node.exists("my_key_1"));
        EXPECT_TRUE(node.exists("my_key_2"));
        EXPECT_TRUE(node.exists("my_key_3"));
        EXPECT_FALSE(node.exists("my_key_4"));
    }
}

TEST(node, insert)
{
    { // By scalar value.

        { // Update existing.     
            yaml::node node;
            
            EXPECT_FALSE(node.exists("my_key"));
            EXPECT_EQ(node.size(), size_t(0));

            EXPECT_NO_THROW(node.insert("my_key", yaml::node_type::scalar));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.exists("my_key"));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
            EXPECT_EQ(node.size(), size_t(1));
            auto & at_1 = node.at("my_key");
            EXPECT_TRUE(at_1.is_scalar());
            EXPECT_EQ(at_1.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("my_key", float(100.0f)));
            EXPECT_TRUE(node.exists("my_key"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
            auto & at_2 = node.at("my_key");
            EXPECT_TRUE(at_2.is_scalar());
            EXPECT_EQ(at_2.data_type(), yaml::node_data_type::float32);

            EXPECT_NO_THROW(node.insert("my_key", double(200.0f)));
            EXPECT_TRUE(node.exists("my_key"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
            auto & at_3 = node.at("my_key");
            EXPECT_TRUE(at_3.is_scalar());
            EXPECT_EQ(at_3.data_type(), yaml::node_data_type::float64);

            EXPECT_NO_THROW(node.insert("my_key", int32_t(1001)));
            EXPECT_TRUE(node.exists("my_key"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
            auto & at_4 = node.at("my_key");
            EXPECT_TRUE(at_4.is_scalar());
            EXPECT_EQ(at_4.data_type(), yaml::node_data_type::int32);

            EXPECT_NO_THROW(node.insert("my_key", int64_t(2001)));
            EXPECT_TRUE(node.exists("my_key"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
            auto & at_5 = node.at("my_key");
            EXPECT_TRUE(at_5.is_scalar());
            EXPECT_EQ(at_5.data_type(), yaml::node_data_type::int64);

            std::string tmp_string = "test";
            EXPECT_NO_THROW(node.insert("my_key", std::string("lol")));
            EXPECT_TRUE(node.exists("my_key"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
            auto & at_6 = node.at("my_key");
            EXPECT_TRUE(at_6.is_scalar());
            EXPECT_EQ(at_6.data_type(), yaml::node_data_type::string);

            EXPECT_NO_THROW(node.insert("my_key", tmp_string));
            EXPECT_TRUE(node.exists("my_key"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
            auto & at_7 = node.at("my_key");
            EXPECT_TRUE(at_7.is_scalar());
            EXPECT_EQ(at_7.data_type(), yaml::node_data_type::string);
        }
        { // Insert new.
            yaml::node node;

            EXPECT_FALSE(node.exists("my_key_1"));
            EXPECT_FALSE(node.exists("my_key_1"));
            EXPECT_FALSE(node.exists("my_key_2"));
            EXPECT_FALSE(node.exists("my_key_3"));
            EXPECT_FALSE(node.exists("my_key_4"));
            EXPECT_FALSE(node.exists("my_key_5"));
            EXPECT_FALSE(node.exists("my_key_6"));
            EXPECT_FALSE(node.exists("my_key_7"));
            EXPECT_EQ(node.size(), size_t(0));
            EXPECT_TRUE(node.at("my_key_1").is_null());
            EXPECT_TRUE(node.at("my_key_2").is_null());
            EXPECT_TRUE(node.at("my_key_3").is_null());
            EXPECT_TRUE(node.at("my_key_4").is_null());
            EXPECT_TRUE(node.at("my_key_5").is_null());
            EXPECT_TRUE(node.at("my_key_6").is_null());
            EXPECT_TRUE(node.at("my_key_7").is_null());

            EXPECT_NO_THROW(node.insert("my_key_1", yaml::node_type::scalar));     
            EXPECT_TRUE(node.exists("my_key_1"));
            EXPECT_FALSE(node.exists("my_key_2"));
            EXPECT_FALSE(node.exists("my_key_3"));
            EXPECT_FALSE(node.exists("my_key_4"));
            EXPECT_FALSE(node.exists("my_key_5"));
            EXPECT_FALSE(node.exists("my_key_6"));
            EXPECT_FALSE(node.exists("my_key_7"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
            EXPECT_EQ(node.size(), size_t(1));

            EXPECT_NO_THROW(node.insert("my_key_2", float(100.0f)));
            EXPECT_TRUE(node.exists("my_key_1"));
            EXPECT_TRUE(node.exists("my_key_2"));
            EXPECT_FALSE(node.exists("my_key_3"));
            EXPECT_FALSE(node.exists("my_key_4"));
            EXPECT_FALSE(node.exists("my_key_5"));
            EXPECT_FALSE(node.exists("my_key_6"));
            EXPECT_FALSE(node.exists("my_key_7"));
            EXPECT_EQ(node.size(), size_t(2));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("my_key_3", double(200.0f)));
            EXPECT_TRUE(node.exists("my_key_1"));
            EXPECT_TRUE(node.exists("my_key_2"));
            EXPECT_TRUE(node.exists("my_key_3"));
            EXPECT_FALSE(node.exists("my_key_4"));
            EXPECT_FALSE(node.exists("my_key_5"));
            EXPECT_FALSE(node.exists("my_key_6"));
            EXPECT_FALSE(node.exists("my_key_7"));
            EXPECT_EQ(node.size(), size_t(3));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("my_key_4", int32_t(1001)));
            EXPECT_TRUE(node.exists("my_key_1"));
            EXPECT_TRUE(node.exists("my_key_2"));
            EXPECT_TRUE(node.exists("my_key_3"));
            EXPECT_TRUE(node.exists("my_key_4"));
            EXPECT_FALSE(node.exists("my_key_5"));
            EXPECT_FALSE(node.exists("my_key_6"));
            EXPECT_FALSE(node.exists("my_key_7"));
            EXPECT_EQ(node.size(), size_t(4));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("my_key_5", int64_t(2001)));
            EXPECT_TRUE(node.exists("my_key_1"));
            EXPECT_TRUE(node.exists("my_key_2"));
            EXPECT_TRUE(node.exists("my_key_3"));
            EXPECT_TRUE(node.exists("my_key_4"));
            EXPECT_TRUE(node.exists("my_key_5"));
            EXPECT_FALSE(node.exists("my_key_6"));
            EXPECT_FALSE(node.exists("my_key_7"));
            EXPECT_EQ(node.size(), size_t(5));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            
            EXPECT_NO_THROW(node.insert("my_key_6", std::string("lol")));
            EXPECT_TRUE(node.exists("my_key_1"));
            EXPECT_TRUE(node.exists("my_key_2"));
            EXPECT_TRUE(node.exists("my_key_3"));
            EXPECT_TRUE(node.exists("my_key_4"));
            EXPECT_TRUE(node.exists("my_key_5"));
            EXPECT_TRUE(node.exists("my_key_6"));
            EXPECT_FALSE(node.exists("my_key_7"));
            EXPECT_EQ(node.size(), size_t(6));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            std::string tmp_string = "test_7";
            EXPECT_NO_THROW(node.insert("my_key_7", tmp_string));
            EXPECT_TRUE(node.exists("my_key_1"));
            EXPECT_TRUE(node.exists("my_key_2"));
            EXPECT_TRUE(node.exists("my_key_3"));
            EXPECT_TRUE(node.exists("my_key_4"));
            EXPECT_TRUE(node.exists("my_key_5"));
            EXPECT_TRUE(node.exists("my_key_6"));
            EXPECT_TRUE(node.exists("my_key_7"));
            EXPECT_EQ(node.size(), size_t(7));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_TRUE(node.at("my_key_1").is_scalar());
            EXPECT_TRUE(node.at("my_key_2").is_scalar());
            EXPECT_TRUE(node.at("my_key_3").is_scalar());
            EXPECT_TRUE(node.at("my_key_4").is_scalar());
            EXPECT_TRUE(node.at("my_key_5").is_scalar());
            EXPECT_TRUE(node.at("my_key_6").is_scalar());
            EXPECT_TRUE(node.at("my_key_7").is_scalar());

            EXPECT_EQ(node.at("my_key_1").data_type(), yaml::node_data_type::null);
            EXPECT_EQ(node.at("my_key_2").data_type(), yaml::node_data_type::float32);
            EXPECT_EQ(node.at("my_key_3").data_type(), yaml::node_data_type::float64);
            EXPECT_EQ(node.at("my_key_4").data_type(), yaml::node_data_type::int32);
            EXPECT_EQ(node.at("my_key_5").data_type(), yaml::node_data_type::int64);
            EXPECT_EQ(node.at("my_key_6").data_type(), yaml::node_data_type::string);
            EXPECT_EQ(node.at("my_key_7").data_type(), yaml::node_data_type::string);

            EXPECT_STREQ(node.at("my_key_1").as<std::string>().c_str(), "" );
            EXPECT_EQ(node.at("my_key_2").as<float>(), float(100.0f));
            EXPECT_EQ(node.at("my_key_3").as<double>(), double(200.0f));
            EXPECT_EQ(node.at("my_key_4").as<int32_t>(), int32_t(1001));
            EXPECT_EQ(node.at("my_key_5").as<int64_t>(), int64_t(2001));
            EXPECT_STREQ(node.at("my_key_6").as<std::string>().c_str(), "lol");
            EXPECT_STREQ(node.at("my_key_7").as<std::string>().c_str(), "test_7");
        }
    }
    { // By node type value.

        { // Update existing.  
            yaml::node node;

            EXPECT_FALSE(node.exists("foo_bar"));
            EXPECT_EQ(node.size(), size_t(0));

            EXPECT_NO_THROW(node.insert("foo_bar", yaml::node_type::null));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.exists("foo_bar"));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
            EXPECT_EQ(node.size(), size_t(1));

            EXPECT_NO_THROW(node.insert("foo_bar", yaml::node_type::map));
            EXPECT_TRUE(node.exists("foo_bar"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("foo_bar", yaml::node_type::scalar));
            EXPECT_TRUE(node.exists("foo_bar"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("foo_bar", yaml::node_type::sequence));
            EXPECT_TRUE(node.exists("foo_bar"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
        }
        { // Insert new.
            yaml::node node;

            EXPECT_FALSE(node.exists("foo_bar_1"));
            EXPECT_FALSE(node.exists("foo_bar_2"));
            EXPECT_FALSE(node.exists("foo_bar_3"));
            EXPECT_FALSE(node.exists("foo_bar_4"));
            EXPECT_EQ(node.size(), size_t(0));

            EXPECT_NO_THROW(node.insert("foo_bar_1", yaml::node_type::scalar));
            EXPECT_TRUE(node.exists("foo_bar_1"));
            EXPECT_FALSE(node.exists("foo_bar_2"));
            EXPECT_FALSE(node.exists("foo_bar_3"));
            EXPECT_FALSE(node.exists("foo_bar_4"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
            EXPECT_EQ(node.size(), size_t(1));

            EXPECT_NO_THROW(node.insert("foo_bar_2", yaml::node_type::scalar));
            EXPECT_TRUE(node.exists("foo_bar_1"));
            EXPECT_TRUE(node.exists("foo_bar_2"));
            EXPECT_FALSE(node.exists("foo_bar_3"));
            EXPECT_FALSE(node.exists("foo_bar_4"));
            EXPECT_EQ(node.size(), size_t(2));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("foo_bar_3", yaml::node_type::scalar));
            EXPECT_TRUE(node.exists("foo_bar_1"));
            EXPECT_TRUE(node.exists("foo_bar_2"));
            EXPECT_TRUE(node.exists("foo_bar_3"));
            EXPECT_FALSE(node.exists("foo_bar_4"));
            EXPECT_EQ(node.size(), size_t(3));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("foo_bar_4", yaml::node_type::scalar));
            EXPECT_TRUE(node.exists("foo_bar_1"));
            EXPECT_TRUE(node.exists("foo_bar_2"));
            EXPECT_TRUE(node.exists("foo_bar_3"));
            EXPECT_TRUE(node.exists("foo_bar_4"));
            EXPECT_EQ(node.size(), size_t(4));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
        }
    }
    { // By node data type value.

        { // Update existing.  
            yaml::node node;

            EXPECT_FALSE(node.exists("foo_bar"));
            EXPECT_EQ(node.size(), size_t(0));

            EXPECT_NO_THROW(node.insert("foo_bar", yaml::node_data_type::null));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.exists("foo_bar"));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
            EXPECT_EQ(node.size(), size_t(1));

            EXPECT_NO_THROW(node.insert("foo_bar", yaml::node_data_type::boolean));
            EXPECT_TRUE(node.exists("foo_bar"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("foo_bar", yaml::node_data_type::float32));
            EXPECT_TRUE(node.exists("foo_bar"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("foo_bar", yaml::node_data_type::float64));
            EXPECT_TRUE(node.exists("foo_bar"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("foo_bar", yaml::node_data_type::int32));
            EXPECT_TRUE(node.exists("foo_bar"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("foo_bar", yaml::node_data_type::int64));
            EXPECT_TRUE(node.exists("foo_bar"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("foo_bar", yaml::node_data_type::string));
            EXPECT_TRUE(node.exists("foo_bar"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
        }
        { // Insert new.
            yaml::node node;

            EXPECT_FALSE(node.exists("foo_bar_1"));
            EXPECT_FALSE(node.exists("foo_bar_2"));
            EXPECT_FALSE(node.exists("foo_bar_3"));
            EXPECT_FALSE(node.exists("foo_bar_4"));
            EXPECT_FALSE(node.exists("foo_bar_5"));
            EXPECT_FALSE(node.exists("foo_bar_6"));
            EXPECT_FALSE(node.exists("foo_bar_7"));
            EXPECT_EQ(node.size(), size_t(0));

            EXPECT_NO_THROW(node.insert("foo_bar_1", yaml::node_data_type::boolean));
            EXPECT_TRUE(node.exists("foo_bar_1"));
            EXPECT_FALSE(node.exists("foo_bar_2"));
            EXPECT_FALSE(node.exists("foo_bar_3"));
            EXPECT_FALSE(node.exists("foo_bar_4"));
            EXPECT_FALSE(node.exists("foo_bar_5"));
            EXPECT_FALSE(node.exists("foo_bar_6"));
            EXPECT_FALSE(node.exists("foo_bar_7"));
            EXPECT_EQ(node.size(), size_t(1));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
            EXPECT_EQ(node.size(), size_t(1));

            EXPECT_NO_THROW(node.insert("foo_bar_2", yaml::node_data_type::float32));
            EXPECT_TRUE(node.exists("foo_bar_1"));
            EXPECT_TRUE(node.exists("foo_bar_2"));
            EXPECT_FALSE(node.exists("foo_bar_3"));
            EXPECT_FALSE(node.exists("foo_bar_4"));
            EXPECT_FALSE(node.exists("foo_bar_5"));
            EXPECT_FALSE(node.exists("foo_bar_6"));
            EXPECT_FALSE(node.exists("foo_bar_7"));
            EXPECT_EQ(node.size(), size_t(2));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("foo_bar_3", yaml::node_data_type::float64));
            EXPECT_TRUE(node.exists("foo_bar_1"));
            EXPECT_TRUE(node.exists("foo_bar_2"));
            EXPECT_TRUE(node.exists("foo_bar_3"));
            EXPECT_FALSE(node.exists("foo_bar_4"));
            EXPECT_FALSE(node.exists("foo_bar_5"));
            EXPECT_FALSE(node.exists("foo_bar_6"));
            EXPECT_FALSE(node.exists("foo_bar_7"));
            EXPECT_EQ(node.size(), size_t(3));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("foo_bar_4", yaml::node_data_type::int32));
            EXPECT_TRUE(node.exists("foo_bar_1"));
            EXPECT_TRUE(node.exists("foo_bar_2"));
            EXPECT_TRUE(node.exists("foo_bar_3"));
            EXPECT_TRUE(node.exists("foo_bar_4"));
            EXPECT_FALSE(node.exists("foo_bar_5"));
            EXPECT_FALSE(node.exists("foo_bar_6"));
            EXPECT_FALSE(node.exists("foo_bar_7"));
            EXPECT_EQ(node.size(), size_t(4));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("foo_bar_5", yaml::node_data_type::int64));
            EXPECT_TRUE(node.exists("foo_bar_1"));
            EXPECT_TRUE(node.exists("foo_bar_2"));
            EXPECT_TRUE(node.exists("foo_bar_3"));
            EXPECT_TRUE(node.exists("foo_bar_4"));
            EXPECT_TRUE(node.exists("foo_bar_5"));
            EXPECT_FALSE(node.exists("foo_bar_6"));
            EXPECT_FALSE(node.exists("foo_bar_7"));
            EXPECT_EQ(node.size(), size_t(5));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("foo_bar_6", yaml::node_data_type::null));
            EXPECT_TRUE(node.exists("foo_bar_1"));
            EXPECT_TRUE(node.exists("foo_bar_2"));
            EXPECT_TRUE(node.exists("foo_bar_3"));
            EXPECT_TRUE(node.exists("foo_bar_4"));
            EXPECT_TRUE(node.exists("foo_bar_5"));
            EXPECT_TRUE(node.exists("foo_bar_6"));
            EXPECT_FALSE(node.exists("foo_bar_7"));
            EXPECT_EQ(node.size(), size_t(6));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);

            EXPECT_NO_THROW(node.insert("foo_bar_7", yaml::node_data_type::string));
            EXPECT_TRUE(node.exists("foo_bar_1"));
            EXPECT_TRUE(node.exists("foo_bar_2"));
            EXPECT_TRUE(node.exists("foo_bar_3"));
            EXPECT_TRUE(node.exists("foo_bar_4"));
            EXPECT_TRUE(node.exists("foo_bar_5"));
            EXPECT_TRUE(node.exists("foo_bar_6"));
            EXPECT_TRUE(node.exists("foo_bar_7"));
            EXPECT_EQ(node.size(), size_t(7));
            EXPECT_TRUE(node.is_map());
            EXPECT_EQ(node.data_type(), yaml::node_data_type::null);
        }
    }
    { // By copy constructor.

        { // Update existing.  

        }
        { // Insert new.

        }
    }
}

TEST(node, push_back)
{
    {
        yaml::node node;
        EXPECT_FALSE(node.is_sequence());
        EXPECT_EQ(node.size(), size_t(0));

        {
            EXPECT_NO_THROW(node.push_back(yaml::node_type::null));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(1));

            EXPECT_NO_THROW(node.push_back(yaml::node_type::map));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(2));

            EXPECT_NO_THROW(node.push_back(yaml::node_type::scalar));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(3));

            EXPECT_NO_THROW(node.push_back(yaml::node_type::sequence));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(4));
        }
        {
            EXPECT_NO_THROW(node.push_back(yaml::node_data_type::null));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(5));

            EXPECT_NO_THROW(node.push_back(yaml::node_data_type::boolean));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(6));

            EXPECT_NO_THROW(node.push_back(yaml::node_data_type::float32));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(7));

            EXPECT_NO_THROW(node.push_back(yaml::node_data_type::float64));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(8));

            EXPECT_NO_THROW(node.push_back(yaml::node_data_type::int32));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(9));

            EXPECT_NO_THROW(node.push_back(yaml::node_data_type::int64));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(10));

            EXPECT_NO_THROW(node.push_back(yaml::node_data_type::string));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(11));
        }
        {
            EXPECT_NO_THROW(node.push_back(bool(false)));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(12));

            EXPECT_NO_THROW(node.push_back(float(123.0f)));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(13));

            EXPECT_NO_THROW(node.push_back(double(456.0f)));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(14));

            EXPECT_NO_THROW(node.push_back(int32_t(123456)));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(15));

            EXPECT_NO_THROW(node.push_back(int64_t(987654321)));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(16));

            EXPECT_NO_THROW(node.push_back(std::string("test")));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(17));

            EXPECT_NO_THROW(node.push_back(std::string("test")));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(18));

            EXPECT_NO_THROW(node.push_back("hello world"));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(19));
        }
    }
}

TEST(node, push_front)
{
    {
        yaml::node node;
        EXPECT_FALSE(node.is_sequence());
        EXPECT_EQ(node.size(), size_t(0));

        {
            EXPECT_NO_THROW(node.push_front(yaml::node_type::null));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(1));

            EXPECT_NO_THROW(node.push_front(yaml::node_type::map));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(2));

            EXPECT_NO_THROW(node.push_front(yaml::node_type::scalar));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(3));

            EXPECT_NO_THROW(node.push_front(yaml::node_type::sequence));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(4));
        }
        {
            EXPECT_NO_THROW(node.push_front(yaml::node_data_type::null));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(5));

            EXPECT_NO_THROW(node.push_front(yaml::node_data_type::boolean));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(6));

            EXPECT_NO_THROW(node.push_front(yaml::node_data_type::float32));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(7));

            EXPECT_NO_THROW(node.push_front(yaml::node_data_type::float64));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(8));

            EXPECT_NO_THROW(node.push_front(yaml::node_data_type::int32));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(9));

            EXPECT_NO_THROW(node.push_front(yaml::node_data_type::int64));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(10));

            EXPECT_NO_THROW(node.push_front(yaml::node_data_type::string));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(11));
        }
        {
            EXPECT_NO_THROW(node.push_front(bool(false)));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(12));

            EXPECT_NO_THROW(node.push_front(float(123.0f)));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(13));

            EXPECT_NO_THROW(node.push_front(double(456.0f)));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(14));

            EXPECT_NO_THROW(node.push_front(int32_t(123456)));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(15));

            EXPECT_NO_THROW(node.push_front(int64_t(987654321)));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(16));

            EXPECT_NO_THROW(node.push_front(std::string("test")));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(17));

            EXPECT_NO_THROW(node.push_front(std::string("test")));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(18));

            EXPECT_NO_THROW(node.push_front("hello world"));
            EXPECT_TRUE(node.is_sequence());
            EXPECT_EQ(node.size(), size_t(19));
        }
    }
}

TEST(node, map_operator)
{
    {
        yaml::node node;

        node["foo"]["bar"] = int32_t(123);

        auto & foo = node.at("foo");
        EXPECT_TRUE(foo.is_map());
        EXPECT_EQ(foo.size(), size_t(1));

        auto & bar = foo.at("bar");
        EXPECT_TRUE(bar.is_scalar());
        EXPECT_EQ(bar.data_type(), yaml::node_data_type::int32);
        EXPECT_EQ(bar.as<int32_t>(), int32_t(123));
    }
    {
        yaml::node node;

        node["foo"]["bar"] = int32_t(123);
        node["foo"]["hello"]["world 1"] = "text here";
        node["foo"]["hello"]["world 2"] = float(128.0f);
        node["foo"]["hello"]["world 3"] = int64_t(1234567890);

        auto & foo = node.at("foo");
        EXPECT_TRUE(foo.is_map());
        EXPECT_EQ(foo.size(), size_t(2));

        auto & bar = foo.at("bar");
        EXPECT_TRUE(bar.is_scalar());
        EXPECT_EQ(bar.data_type(), yaml::node_data_type::int32);
        EXPECT_EQ(bar.as<int32_t>(), int32_t(123));

        auto & hello = foo.at("hello");
        EXPECT_TRUE(hello.is_map());
        EXPECT_EQ(hello.size(), size_t(3));

        auto & world_1 = hello.at("world 1");
        EXPECT_TRUE(world_1.is_scalar());
        EXPECT_EQ(world_1.data_type(), yaml::node_data_type::string);
        EXPECT_STREQ(world_1.as<std::string>().c_str(), "text here");

        auto & world_2 = hello.at("world 2");
        EXPECT_TRUE(world_2.is_scalar());
        EXPECT_EQ(world_2.data_type(), yaml::node_data_type::float32);
        EXPECT_EQ(world_2.as<float>(), float(128.0f));

        auto & world_3 = hello.at("world 3");
        EXPECT_TRUE(world_3.is_scalar());
        EXPECT_EQ(world_3.data_type(), yaml::node_data_type::int64);
        EXPECT_EQ(world_3.as<int64_t>(), int64_t(1234567890));
    }
}

TEST(node, map_find)
{
    yaml::node node;

    node["foo"]["bar"] = int32_t(123);
    node["foo"]["hello"]["world 1"] = "text here";
    node["foo"]["hello"]["world 2"] = float(128.0f);
    node["foo"]["hello"]["world 3"] = int64_t(1234567890);

    {
        auto foo_it = node.find("foo");
        EXPECT_EQ(foo_it.type(), yaml::node::iterator_type::map);
        EXPECT_EQ(foo_it, node.begin());
        EXPECT_NE(foo_it, node.end());
        auto & foo_node = (*foo_it).second;
        EXPECT_STREQ((*foo_it).first.c_str(), "foo");
        EXPECT_TRUE(foo_node.is_map());

        auto bar_it = foo_node.find("bar");
        EXPECT_EQ(bar_it.type(), yaml::node::iterator_type::map);
        auto & bar_node = (*bar_it).second;
        EXPECT_STREQ((*bar_it).first.c_str(), "bar");
        EXPECT_TRUE(bar_node.is_scalar());
        EXPECT_EQ(bar_node.data_type(), yaml::node_data_type::int32);

        auto hello_it = foo_node.find("hello");
        EXPECT_EQ(hello_it.type(), yaml::node::iterator_type::map);
        auto & hello_node = (*hello_it).second;
        EXPECT_STREQ((*hello_it).first.c_str(), "hello");
        EXPECT_TRUE(hello_node.is_map());
    }
    {
        auto what_it = node.find("what");
        EXPECT_EQ(what_it.type(), yaml::node::iterator_type::map);
        EXPECT_NE(what_it, node.begin());
        EXPECT_EQ(what_it, node.end());

    }
}

TEST(node, map_erase)
{
    // by string
    {
        yaml::node node;

        node["foo"]["bar"] = int32_t(123);
        node["foo"]["hello"]["world 1"] = "text here";
        node["foo"]["hello"]["world 2"] = float(128.0f);
        node["foo"]["hello"]["world 3"] = int64_t(1234567890);

        auto & foo_hello = node["foo"]["hello"];
        EXPECT_EQ(foo_hello.size(), size_t(3));
        foo_hello.erase("random");
        EXPECT_EQ(foo_hello.size(), size_t(3));
        foo_hello.erase("world 2");
        EXPECT_EQ(foo_hello.size(), size_t(2));
        foo_hello.erase("world 2");
        EXPECT_EQ(foo_hello.size(), size_t(2));
        foo_hello.erase("world 3");
        EXPECT_EQ(foo_hello.size(), size_t(1));
        foo_hello.erase("world 1");
        EXPECT_EQ(foo_hello.size(), size_t(0));

        auto & foo = node["foo"];
        EXPECT_EQ(foo.size(), size_t(2));
        foo.erase("hello");
        EXPECT_EQ(foo.size(), size_t(1));
        foo.erase("bar");
        EXPECT_EQ(foo.size(), size_t(0));

        EXPECT_EQ(node.size(), size_t(1));
        node.erase("foo");
        EXPECT_EQ(node.size(), size_t(0));
    }
    // by iterator.
    {
       /* yaml::node node;

        node["foo"]["bar"] = int32_t(123);
        node["foo"]["hello"]["world 1"] = "text here";
        node["foo"]["hello"]["world 2"] = float(128.0f);
        node["foo"]["hello"]["world 3"] = int64_t(1234567890);
        */

    }
}

template<typename T>
static void iterator_test_map_1(T & node)
{
    EXPECT_EQ(node.size(), size_t(4));

    bool ok[4] = { false, false, false, false };

    for (auto it = node.begin(); it != node.end(); it++)
    {
        auto & key = (*it).first;
        if (key == "foo")
        {
            ok[0] = true;
            EXPECT_STREQ((*it).first.c_str(), "foo");
            T & n = (*it).second;     
            EXPECT_TRUE(n.is_scalar());
            EXPECT_EQ(n.template as<int32_t>(), int32_t(100));
        }
        else if (key == "bar")
        {
            ok[1] = true;
            EXPECT_STREQ((*it).first.c_str(), "bar");
            T & n = (*it).second;  
            EXPECT_TRUE(n.is_scalar());           
            EXPECT_EQ(n.template as<int32_t>(), int32_t(200));
        }
        else if (key == "hello")
        {
            ok[2] = true;
            EXPECT_STREQ((*it).first.c_str(), "hello");
            T & n = (*it).second;  
            EXPECT_TRUE(n.is_scalar());
            EXPECT_EQ(n.template as<int32_t>(), int32_t(300));
        }
        else if (key == "world")
        {
            ok[3] = true;
            EXPECT_STREQ((*it).first.c_str(), "world");
            T & n = (*it).second;
            EXPECT_TRUE(n.is_scalar());
            EXPECT_EQ(n.template as<int32_t>(), int32_t(400));
        }
    }

    EXPECT_TRUE(ok[0]);
    EXPECT_TRUE(ok[1]);
    EXPECT_TRUE(ok[2]);
    EXPECT_TRUE(ok[3]);
}

template<typename T>
static void iterator_test_sequence_1(T & node)
{
    EXPECT_EQ(node.size(), size_t(100));

    int32_t index = 0;
    for (auto it = node.begin(); it != node.end(); it++)
    {
        EXPECT_STREQ((*it).first.c_str(), "");
        T & n = (*it).second;
        EXPECT_EQ(n.template as<int32_t>(), index * 100);
        index++;
    }
    EXPECT_EQ(index, int32_t(100));
}

TEST(node, iterator)
{
    {
        yaml::node node;
        node["foo"] = int32_t(100);
        node["bar"] = int32_t(200);
        node["hello"] = int32_t(300);
        node["world"] = int32_t(400);

        const yaml::node & const_node = node;
        iterator_test_map_1<yaml::node>(node);
        iterator_test_map_1<const yaml::node>(const_node);
    }
    {
        yaml::node node;

        for (int32_t i = 0; i < 100; i++)
        {
            node.push_back(i * 100);
        }

        const yaml::node & const_node = node;
        iterator_test_sequence_1<yaml::node>(node);
        iterator_test_sequence_1<const yaml::node>(const_node);
    }
}

TEST(dump, config)
{
    {
        yaml::dump_config config;
        EXPECT_EQ(config.indentation, size_t(2));
    }
    {
        yaml::dump_config config(3);
        EXPECT_EQ(config.indentation, size_t(3));
    }
    {
        yaml::dump_config config(7, 100);
        EXPECT_EQ(config.indentation, size_t(7));
        EXPECT_EQ(config.scalar_fold_length, size_t(100));
    }
    
}

TEST(dump, string)
{
    {
        yaml::node node;
        node["header"] = "Hello world.";
        node["key"]["another_key"] = int32_t(123);
        node["key"]["another_nested_map"]["key 1"] = "text here";
        node["key"]["another_nested_map"]["key 2"] = float(1.25f);
        node["key"]["another_nested_map"]["key 3"] = double(1.125f);
        node["key"]["another_nested_map"]["key 4"] = int32_t(123456);
        node["key"]["another_nested_map"]["key 5"] = int64_t(1234567890);
        node["key"]["another_nested_map"]["key 6"] = bool(true);
        node["key"]["another_nested_map"]["key 7"] = bool(false);
        node["key"]["another_nested_map"]["key 8"] = yaml::node_data_type::null;

        auto & people = node["people"];

        auto & jimmie = people.push_back(yaml::node_type::map);
        jimmie["name"] = "Jimmie";
        jimmie["age"] = 27;
        auto & bosse = people.push_back(yaml::node_type::map);
        bosse["name"] = "Bosse";
        bosse["age"] = 4;

        auto & places = node["places"];
        places.push_back("Sweden");
        places.push_back("Norway");
        places.push_back("Denmark");
        places.push_back("Iceland");

        const std::string expected_dump_indent_2 =
            "header: Hello world."  + std::string("\n") +
            "key:"                  + std::string("\n") +
            "  another_key: 123"    + std::string("\n") +
            "  another_nested_map:" + std::string("\n") +
            "    key 1: text here"  + std::string("\n") +
            "    key 2: 1.25"       + std::string("\n") +
            "    key 3: 1.125"      + std::string("\n") +
            "    key 4: 123456"     + std::string("\n") +
            "    key 5: 1234567890" + std::string("\n") +
            "    key 6: true"       + std::string("\n") +
            "    key 7: false"      + std::string("\n") +
            "    key 8: ~"          + std::string("\n") +
            "people:"               + std::string("\n") +
            "  -"                   + std::string("\n") +
            "    age: 27"           + std::string("\n") +
            "    name: Jimmie"      + std::string("\n") +
            "  -"                   + std::string("\n") +
            "    age: 4"            + std::string("\n") +
            "    name: Bosse"       + std::string("\n") +
            "places:"               + std::string("\n") +
            "  - Sweden"            + std::string("\n") +
            "  - Norway"            + std::string("\n") +
            "  - Denmark"           + std::string("\n") +
            "  - Iceland"           + std::string("\n");

        const std::string expected_dump_indent_5 =
            "header: Hello world." + std::string("\n") +
            "key:" + std::string("\n") +
            "     another_key: 123" + std::string("\n") +
            "     another_nested_map:" + std::string("\n") +
            "          key 1: text here" + std::string("\n") +
            "          key 2: 1.25" + std::string("\n") +
            "          key 3: 1.125" + std::string("\n") +
            "          key 4: 123456" + std::string("\n") +
            "          key 5: 1234567890" + std::string("\n") +
            "          key 6: true" + std::string("\n") +
            "          key 7: false" + std::string("\n") +
            "          key 8: ~" + std::string("\n") +
            "people:" + std::string("\n") +
            "     -" + std::string("\n") +
            "          age: 27" + std::string("\n") +
            "          name: Jimmie" + std::string("\n") +
            "     -" + std::string("\n") +
            "          age: 4" + std::string("\n") +
            "          name: Bosse" + std::string("\n") +
            "places:" + std::string("\n") +
            "     - Sweden" + std::string("\n") +
            "     - Norway" + std::string("\n") +
            "     - Denmark" + std::string("\n") +
            "     - Iceland" + std::string("\n");
   
        auto data = yaml::dump(node);
        EXPECT_STREQ(data.c_str(), expected_dump_indent_2.c_str());

        data = yaml::dump(node, { 5 });
        EXPECT_STREQ(data.c_str(), expected_dump_indent_5.c_str());

        data = yaml::dump(node, { 0 });
        EXPECT_STREQ(data.c_str(), expected_dump_indent_2.c_str());
    }
}

TEST(dump, file)
{
    {
        yaml::node node;
        node["header"] = "Hello world.";
        node["key"]["another_key"] = int32_t(123);
        node["key"]["another_nested_map"]["key 1"] = "text here";
        node["key"]["another_nested_map"]["key 2"] = float(1.25f);
        node["key"]["another_nested_map"]["key 3"] = double(1.125f);
        node["key"]["another_nested_map"]["key 4"] = int32_t(123456);
        node["key"]["another_nested_map"]["key 5"] = int64_t(1234567890);
        node["key"]["another_nested_map"]["key 6"] = bool(true);
        node["key"]["another_nested_map"]["key 7"] = bool(false);
        node["key"]["another_nested_map"]["key 8"] = yaml::node_data_type::null;

        auto & people = node["people"];

        auto & jimmie = people.push_back(yaml::node_type::map);
        jimmie["name"] = "Jimmie";
        jimmie["age"] = 27;
        auto & bosse = people.push_back(yaml::node_type::map);
        bosse["name"] = "Bosse";
        bosse["age"] = 4;

        auto & places = node["places"];
        places.push_back("Sweden");
        places.push_back("Norway");
        places.push_back("Denmark");
        places.push_back("Iceland");

        const std::string expected_dump_indent_2 =
            "header: Hello world." + std::string("\n") +
            "key:" + std::string("\n") +
            "  another_key: 123" + std::string("\n") +
            "  another_nested_map:" + std::string("\n") +
            "    key 1: text here" + std::string("\n") +
            "    key 2: 1.25" + std::string("\n") +
            "    key 3: 1.125" + std::string("\n") +
            "    key 4: 123456" + std::string("\n") +
            "    key 5: 1234567890" + std::string("\n") +
            "    key 6: true" + std::string("\n") +
            "    key 7: false" + std::string("\n") +
            "    key 8: ~" + std::string("\n") +
            "people:" + std::string("\n") +
            "  -" + std::string("\n") +
            "    age: 27" + std::string("\n") +
            "    name: Jimmie" + std::string("\n") +
            "  -" + std::string("\n") +
            "    age: 4" + std::string("\n") +
            "    name: Bosse" + std::string("\n") +
            "places:" + std::string("\n") +
            "  - Sweden" + std::string("\n") +
            "  - Norway" + std::string("\n") +
            "  - Denmark" + std::string("\n") +
            "  - Iceland" + std::string("\n");

        EXPECT_NO_THROW(yaml::dump_file(node, "test_dump_file.txt"));

        {
            std::ifstream fin("test_dump_file.txt", std::ifstream::binary);
            EXPECT_TRUE(fin.is_open());
            if (fin.is_open())
            {
                fin.seekg(0, fin.end);
                std::streampos pos = fin.tellg();
                EXPECT_GT(pos, std::streampos(0));
                fin.seekg(0, fin.beg);

                if (pos)
                {
                    size_t buffer_size = static_cast<size_t>(pos);
                    std::unique_ptr<char[]> buffer(new char[buffer_size + 1]);
                    buffer.get()[buffer_size] = 0;

                    fin.read(buffer.get(), pos);
                    EXPECT_STREQ(buffer.get(), expected_dump_indent_2.c_str());
                }             
            }
        }

    }
}



/*
Yaml 1.0 spec notes:

*   Multi-line scalars with no token"|(+-) <(+-)" has not newline in end or after each row(uses spaces).
|   token = 1 newline.
|-  token = 0 newlines.
|+  token = 2 newlines.
>   token, same as | but with spaces instead of newlines after each row.

* An escaped newline is ignored. \ = last character of line.

*/



/*
TEST(Exception, throw)
{
    {
        try
        {
            throw yaml::internal_exception("internal");
        }
        catch(const Yaml::Exception & e)
        {
            EXPECT_EQ(e.Type(), Yaml::Exception::InternalError);
            EXPECT_STREQ(e.what(), "internal");
            EXPECT_STREQ(e.Message(), "internal");
        }
    }
    {
        try
        {
            throw Yaml::ParsingException("parsing");
        }
        catch(const Yaml::Exception & e)
        {
            EXPECT_EQ(e.Type(), Yaml::Exception::ParsingError);
            EXPECT_STREQ(e.what(), "parsing");
            EXPECT_STREQ(e.Message(), "parsing");
        }
    }
    {
        try
        {
            throw Yaml::OperationException("operation");
        }
        catch(const Yaml::Exception & e)
        {
            EXPECT_EQ(e.Type(), Yaml::Exception::OperationError);
            EXPECT_STREQ(e.what(), "operation");
            EXPECT_STREQ(e.Message(), "operation");
        }
    }
}

TEST(Node, Type)
{
    {
        Yaml::Node node;
        EXPECT_EQ(node.Type(), Yaml::Node::None);
        EXPECT_TRUE(node.IsNone());
        EXPECT_FALSE(node.IsSequence());
        EXPECT_FALSE(node.IsMap());
        EXPECT_FALSE(node.IsScalar());
        node.Clear();
        EXPECT_EQ(node.Type(), Yaml::Node::None);
    }
    {
        Yaml::Node node = "test";
        EXPECT_EQ(node.Type(), Yaml::Node::ScalarType);
        EXPECT_FALSE(node.IsNone());
        EXPECT_FALSE(node.IsSequence());
        EXPECT_FALSE(node.IsMap());
        EXPECT_TRUE(node.IsScalar());
        node.Clear();
        EXPECT_EQ(node.Type(), Yaml::Node::None);
    }
    {
        Yaml::Node node;
        node.PushBack();
        EXPECT_EQ(node.Type(), Yaml::Node::SequenceType);
        EXPECT_FALSE(node.IsNone());
        EXPECT_TRUE(node.IsSequence());
        EXPECT_FALSE(node.IsMap());
        EXPECT_FALSE(node.IsScalar());
        node.Clear();
        EXPECT_EQ(node.Type(), Yaml::Node::None);
    }
    {
        Yaml::Node node;
        node["test"];
        EXPECT_EQ(node.Type(), Yaml::Node::MapType);
        EXPECT_FALSE(node.IsNone());
        EXPECT_FALSE(node.IsSequence());
        EXPECT_TRUE(node.IsMap());
        EXPECT_FALSE(node.IsScalar());
        node.Clear();
        EXPECT_EQ(node.Type(), Yaml::Node::None);
    }
}

TEST(Node, As)
{
    {
        Yaml::Node node = "Hello world!";
        EXPECT_EQ(node.As<std::string>(), "Hello world!");
        node = std::string("Foo bar.");
        EXPECT_EQ(node.As<std::string>(), "Foo bar.");
        node = std::string("");
        EXPECT_EQ(node.As<std::string>("empty"), "empty");
    }
    {
        Yaml::Node node = "123456";
        EXPECT_EQ(node.As<int>(), 123456);
        node = "-123456";
        EXPECT_EQ(node.As<int>(), -123456);
        node = "invalid";
        EXPECT_EQ(node.As<int>(123), 123);
    }
    {
        Yaml::Node node = "123.45";
        EXPECT_EQ(node.As<float>(), 123.45f);
        EXPECT_EQ(node.As<int>(), 123);
        node = "-123.45";
        EXPECT_EQ(node.As<float>(), -123.45f);
        EXPECT_EQ(node.As<int>(), -123);
        node = "invalid";
        EXPECT_EQ(node.As<float>(999.1f), 999.1f);
    }
}

TEST(Node, Size)
{
    {
        Yaml::Node node;
        EXPECT_EQ(node.Size(), 0);
        node.PushBack();
        EXPECT_EQ(node.Size(), 1);
        node.PushBack();
        EXPECT_EQ(node.Size(), 2);
        node.Erase(1);
        EXPECT_EQ(node.Size(), 1);
        node.Erase(0);
        EXPECT_EQ(node.Size(), 0);

        node.Insert(10);
        EXPECT_EQ(node.Size(), 1);
        node.Erase(1);
        EXPECT_EQ(node.Size(), 1);
        node.Erase(0);
        EXPECT_EQ(node.Size(), 0);

        node.PushBack();
        node.PushBack();
        node.Clear();
        EXPECT_EQ(node.Size(), 0);
    }
    {
        Yaml::Node node;
        EXPECT_EQ(node.Size(), 0);
        node["test"];
        EXPECT_EQ(node.Size(), 1);
        node["test"];

        EXPECT_EQ(node.Size(), 1);
        node["foo bar"];
        EXPECT_EQ(node.Size(), 2);

        node.Erase("hello world");
        EXPECT_EQ(node.Size(), 2);

        node.Erase("test");
        EXPECT_EQ(node.Size(), 1);
        node.Erase("foo bar");
        EXPECT_EQ(node.Size(), 0);

        node["foo bar"];
        EXPECT_EQ(node.Size(), 1);
        node.Clear();
        EXPECT_EQ(node.Size(), 0);
    }
    {
        Yaml::Node node = "test";
        EXPECT_EQ(node.Size(), 0);
    }
}

void Compare_Node_Copy(Yaml::Node & node)
{
    EXPECT_TRUE(node.IsSequence());
    EXPECT_EQ(node.Size(), 3);


    Yaml::Node & item_1 = node[0];
    EXPECT_TRUE(item_1.IsScalar());
    EXPECT_EQ(item_1.As<std::string>(), "item 1");

    {
        Yaml::Node & item_2 = node[1];
        EXPECT_TRUE(item_2.IsMap());
        EXPECT_EQ(item_2.Size(), 1);

        Yaml::Node & key = item_2["key"];
        EXPECT_TRUE(key.IsSequence());
        EXPECT_EQ(key.Size(), 2);

        Yaml::Node & item_2_1 = key[0];
        EXPECT_TRUE(item_2_1.IsScalar());
        EXPECT_EQ(item_2_1.As<std::string>(), "item 2.1");

        Yaml::Node & item_2_2 = key[1];
        EXPECT_TRUE(item_2_2.IsMap());
        EXPECT_EQ(item_2_2.Size(), 1);

        Yaml::Node & key_two = item_2_2["key two"];
        EXPECT_TRUE(key_two.IsScalar());
        EXPECT_EQ(key_two.As<std::string>(), "item 2.2");
    }

    Yaml::Node & item_3 = node[2];
    EXPECT_TRUE(item_3.IsScalar());
    EXPECT_EQ(item_3.As<std::string>(), "item 3");
}

TEST(Node, Copy_1)
{
    {
        Yaml::Node root;
        root["key 1"] = "value 1";
        root["key 2"] = "value 2";
        root["key 3"] = "value 3";

        {
            Yaml::Node copy(root);

            EXPECT_TRUE(copy.IsMap());

            Yaml::Node & key_1 = copy["key 1"];
            EXPECT_TRUE(key_1.IsScalar());
            EXPECT_EQ(key_1.As<std::string>(), "value 1");

            Yaml::Node & key_2 = copy["key 2"];
            EXPECT_TRUE(key_2.IsScalar());
            EXPECT_EQ(key_2.As<std::string>(), "value 2");

            Yaml::Node & key_3 = copy["key 3"];
            EXPECT_TRUE(key_3.IsScalar());
            EXPECT_EQ(key_3.As<std::string>(), "value 3");
        }
    }

    {
        Yaml::Node root;
        root.PushBack() = "item 1";
        root.PushBack() = "item 2";
        root.PushBack() = "item 3";

        {
            Yaml::Node copy(root);

            EXPECT_TRUE(copy.IsSequence());
            EXPECT_EQ(copy.Size(), 3);

            Yaml::Node & item_1 = copy[0];
            EXPECT_TRUE(item_1.IsScalar());
            EXPECT_EQ(item_1.As<std::string>(), "item 1");

            Yaml::Node & item_2 = copy[1];
            EXPECT_TRUE(item_2.IsScalar());
            EXPECT_EQ(item_2.As<std::string>(), "item 2");

            Yaml::Node & item_3 = copy[2];
            EXPECT_TRUE(item_3.IsScalar());
            EXPECT_EQ(item_3.As<std::string>(), "item 3");
        }
    }

    {
        Yaml::Node root;
        root.PushBack() = "item 1";
        Yaml::Node & map = root.PushBack()["key"];
        map.PushBack() = "item 2.1";
        map.PushBack()["key two"] = "item 2.2";
        root.PushBack() = "item 3";

        {
            Yaml::Node copy(root);
            Compare_Node_Copy(copy);
        }

        {
            Yaml::Node assign;
            assign = root;
            Compare_Node_Copy(assign);
        }
    }
}

void Parse_File_learnyaml(Yaml::Node & root)
{
    Yaml::Node & key = root["key"];
    EXPECT_TRUE(key.IsScalar());
    EXPECT_EQ(key.As<std::string>(), "value");

    Yaml::Node & another_key = root["another_key"];
    EXPECT_TRUE(another_key.IsScalar());
    EXPECT_EQ(another_key.As<std::string>(), "Another value goes here.");

    Yaml::Node & a_number_value = root["a_number_value"];
    EXPECT_TRUE(a_number_value.IsScalar());
    EXPECT_EQ(a_number_value.As<int>(), 100);

    Yaml::Node & scientific_notation = root["scientific_notation"];
    EXPECT_TRUE(scientific_notation.IsScalar());
    EXPECT_EQ(scientific_notation.As<std::string>(), "1e+12");

    Yaml::Node & boolean = root["boolean"];
    EXPECT_TRUE(boolean.IsScalar());
    EXPECT_EQ(boolean.As<std::string>(), "true");
    EXPECT_EQ(boolean.As<bool>(), true);

    // Null values are handled as strings.
    Yaml::Node & null_value = root["null_value"];
    EXPECT_TRUE(null_value.IsScalar());
    EXPECT_EQ(null_value.As<std::string>(), "null");

    Yaml::Node & key_with_spaces = root["key with spaces"];
    EXPECT_TRUE(key_with_spaces.IsScalar());
    EXPECT_EQ(key_with_spaces.As<std::string>(), "value");

    Yaml::Node & keys_can_be_quoted_too = root["Keys: can be \"quoted\" too."];
    EXPECT_TRUE(keys_can_be_quoted_too.IsScalar());
    EXPECT_EQ(keys_can_be_quoted_too.As<std::string>(), "Useful if you want to put a ':' in your key. Or use comment char(#).");

    Yaml::Node & scalar_block = root["scalar_block"];
    EXPECT_TRUE(scalar_block.IsScalar());
    EXPECT_EQ(scalar_block.As<std::string>(), "block of text will separate all lines with spaces.");

    Yaml::Node & literal_block = root["literal_block"];
    EXPECT_TRUE(literal_block.IsScalar());
    EXPECT_EQ(literal_block.As<std::string>(),  "This entire block of text will be the value of the 'literal_block' key,      \n"
                                                "with line breaks being preserved.\n\n"
                                                "The literal continues until de-dented, and the leading indentation is\n"
                                                "stripped.\n\n"
                                                "    Any lines that are 'more-indented' keep the rest of their indentation -\n"
                                                "    these lines will be indented by 4 spaces.\n");

    Yaml::Node & folded_style = root["folded_style"];
    EXPECT_TRUE(folded_style.IsScalar());
    EXPECT_EQ(folded_style.As<std::string>(),   "This entire block of text will be the value of 'folded_style', but this time, all newlines will be replaced with a single space.\n"
                                                "Blank lines, like above, are converted to a newline character.\n\n"
                                                "    'More-indented' lines keep their newlines, too -\n"
                                                "    this text will appear over two lines.\n");

    {
        Yaml::Node & a_nested_map = root["a_nested_map"];
        EXPECT_TRUE(a_nested_map.IsMap());
        EXPECT_EQ(a_nested_map.Size(), 3);

        Yaml::Node & key = a_nested_map["key"];
        EXPECT_TRUE(key.IsScalar());
        EXPECT_EQ(key.As<std::string>(), "value");

        Yaml::Node & another_key = a_nested_map["another_key"];
        EXPECT_TRUE(another_key.IsScalar());
        EXPECT_EQ(another_key.As<std::string>(), "Another Value");

        Yaml::Node & another_nested_map = a_nested_map["another_nested_map"];
        EXPECT_TRUE(another_nested_map.IsMap());
        EXPECT_EQ(another_nested_map.Size(), 1);

        Yaml::Node & hello = another_nested_map["hello"];
        EXPECT_TRUE(hello.IsScalar());
        EXPECT_EQ(hello.As<std::string>(), "hello");
    }

    // Not allowing floats as keys, read as string.
    Yaml::Node & point_twenty_five = root["0.25"];
    EXPECT_TRUE(point_twenty_five.IsScalar());
    EXPECT_EQ(point_twenty_five.As<std::string>(), "a float key");

    {
        Yaml::Node & a_sequence = root["a_sequence"];
        EXPECT_TRUE(a_sequence.IsSequence());
        EXPECT_EQ(a_sequence.Size(), 6);

        Yaml::Node & item_1 = a_sequence[0];
        EXPECT_TRUE(item_1.IsScalar());
        EXPECT_EQ(item_1.As<std::string>(), "Item 1");

        Yaml::Node & item_2 = a_sequence[1];
        EXPECT_TRUE(item_2.IsScalar());
        EXPECT_EQ(item_2.As<std::string>(), "Item 2");

        Yaml::Node & item_3 = a_sequence[2];
        EXPECT_TRUE(item_3.IsScalar());
        EXPECT_EQ(item_3.As<std::string>(), "0.5");
        EXPECT_EQ(item_3.As<float>(), 0.5f);

        Yaml::Node & item_4 = a_sequence[3];
        EXPECT_TRUE(item_4.IsScalar());
        EXPECT_EQ(item_4.As<std::string>(), "Item 4");

        Yaml::Node & item_5 = a_sequence[4];
        EXPECT_TRUE(item_5.IsMap());
        EXPECT_EQ(item_5.Size(), 2);

        Yaml::Node & key = item_5["key"];
        EXPECT_TRUE(key.IsScalar());
        EXPECT_EQ(key.As<std::string>(), "value");

        Yaml::Node & another_key = item_5["another_key"];
        EXPECT_TRUE(another_key.IsScalar());
        EXPECT_EQ(another_key.As<std::string>(), "another_value");

        Yaml::Node & item_6 = a_sequence[5];
        EXPECT_TRUE(item_6.IsSequence());
        EXPECT_EQ(item_6.Size(), 2);

        Yaml::Node & item_6_1 = item_6[0];
        EXPECT_TRUE(item_6_1.IsScalar());
        EXPECT_EQ(item_6_1.As<std::string>(), "This is a sequence");

        Yaml::Node & item_6_2 = item_6[1];
        EXPECT_TRUE(item_6_2.IsScalar());
        EXPECT_EQ(item_6_2.As<std::string>(), "inside another sequence");
    }

    Yaml::Node & datetime = root["datetime"];
    EXPECT_TRUE(datetime.IsScalar());
    EXPECT_EQ(datetime.As<std::string>(), "2001-12-15T02:59:43.1Z");

    Yaml::Node & datetime_with_spaces = root["datetime_with_spaces"];
    EXPECT_TRUE(datetime_with_spaces.IsScalar());
    EXPECT_EQ(datetime_with_spaces.As<std::string>(), "2001-12-14 21:59:43.10 -5");

    Yaml::Node & date = root["date"];
    EXPECT_TRUE(date.IsScalar());
    EXPECT_EQ(date.As<std::string>(), "2002-12-14");

    {
        Yaml::Node & set2 = root["set2"];
        EXPECT_TRUE(set2.IsMap());
        EXPECT_EQ(set2.Size(), 3);

        Yaml::Node & item1 = set2["item1"];
        EXPECT_TRUE(item1.IsScalar());
        EXPECT_EQ(item1.As<std::string>(), "null");

        Yaml::Node & item2 = set2["item2"];
        EXPECT_TRUE(item2.IsScalar());
        EXPECT_EQ(item2.As<std::string>(), "null");

        Yaml::Node & item3 = set2["item3"];
        EXPECT_TRUE(item3.IsScalar());
        EXPECT_EQ(item3.As<std::string>(), "null");
    }
}

TEST(Node, Copy_2)
{

    Yaml::Node root;
    EXPECT_NO_THROW(Yaml::Parse(root, "../test/learnyaml.yaml"));

    {
        Yaml::Node copy(root);
        Parse_File_learnyaml(copy);
    }
    {
        Yaml::Node copy = root;
        Parse_File_learnyaml(copy);
    }


}

TEST(Parse, File)
{
    {
        Yaml::Node root;
        EXPECT_THROW(Yaml::Parse(root, "bad_path_of_file.txt"), Yaml::OperationException);
    }
    {
        Yaml::Node root;
        EXPECT_NO_THROW(Yaml::Parse(root, "../.travis.yml"));
    }
    {
        Yaml::Node root;
        EXPECT_THROW(Yaml::Parse(root, "../yaml/Yaml.hpp"), Yaml::ParsingException);
    }
    {
        Yaml::Node root;
        EXPECT_NO_THROW(Yaml::Parse(root, "../test/learnyaml.yaml"));
    }
}

TEST(Parse, File_learnyaml)
{
    const std::string filename = "../test/learnyaml.yaml";

    {
        Yaml::Node root;
        EXPECT_NO_THROW(Yaml::Parse(root, filename.c_str()));

        Parse_File_learnyaml(root);
    }
    {
        std::ifstream fin(filename, std::ifstream::binary);
        EXPECT_TRUE(fin.is_open());
        if(fin.is_open())
        {
            fin.seekg(0, fin.end);
            size_t dataSize = static_cast<size_t>(fin.tellg());
            fin.seekg(0, fin.beg);
            char * pData = new char [dataSize];
            fin.read(pData, dataSize);
            fin.close();

            Yaml::Node root;
            EXPECT_NO_THROW(Yaml::Parse(root, pData, dataSize));
            delete [] pData;
            Parse_File_learnyaml(root);
        }
    }
    {
        std::ifstream fin(filename, std::ifstream::binary);
        EXPECT_TRUE(fin.is_open());
        if(fin.is_open())
        {
            fin.seekg(0, fin.end);
            size_t dataSize = static_cast<size_t>(fin.tellg());
            fin.seekg(0, fin.beg);
            char * pData = new char [dataSize];
            fin.read(pData, dataSize);
            fin.close();
            std::string data(pData, dataSize);
            delete [] pData;

            // String
            Yaml::Node root_string;
            EXPECT_NO_THROW(Yaml::Parse(root_string, data));
            Parse_File_learnyaml(root_string);

            // Stream
            std::stringstream stream(data);
            Yaml::Node root_stream;
            EXPECT_NO_THROW(Yaml::Parse(root_stream, stream));
            Parse_File_learnyaml(root_stream);
        }
    }
}

TEST(Parse, Invalid)
{
    std::ifstream fin("../test/invalid.yaml", std::ifstream::binary);
    EXPECT_TRUE(fin.is_open());
    if(fin.is_open())
    {
        fin.seekg(0, fin.end);
        size_t dataSize = static_cast<size_t>(fin.tellg());
        fin.seekg(0, fin.beg);
        char * pData = new char [dataSize];
        fin.read(pData, dataSize);
        fin.close();
        std::string data(pData, dataSize);
        delete [] pData;
        std::stringstream stream(data);
        Yaml::Node root_stream;

        const size_t tests = 8;
        size_t loops = 0;
        while(loops < tests)
        {
            loops++;

            //try
            //{
            //    Yaml::Parse(root_stream, stream);
            //}
            //catch(const Yaml::Exception & e)
            //{
            //    std::cout << "Exception(" << e.Type() <<  "): " << e.what() << std::endl;
            //}

            EXPECT_THROW(Yaml::Parse(root_stream, stream), Yaml::ParsingException);

        }
    }
}

TEST(Parse, Valid)
{
    std::ifstream fin("../test/valid.yaml", std::ifstream::binary);
    EXPECT_TRUE(fin.is_open());
    if(fin.is_open())
    {
        fin.seekg(0, fin.end);
        size_t dataSize = static_cast<size_t>(fin.tellg());
        fin.seekg(0, fin.beg);
        char * pData = new char [dataSize];
        fin.read(pData, dataSize);
        fin.close();
        std::string data(pData, dataSize);
        delete [] pData;
        std::stringstream stream(data);
        Yaml::Node root_stream;

        const size_t tests = 8;
        size_t loops = 0;
        while(loops < tests)
        {
            loops++;

            //try
            //{
            //    Yaml::Parse(root_stream, stream);
            //}
            //catch(const Yaml::Exception & e)
            //{
            //    std::cout << "Exception(" << e.Type() <<  "): " << e.what() << std::endl;
            //}

            EXPECT_NO_THROW(Yaml::Parse(root_stream, stream));

        }
    }
}

TEST(Iterator, Iterator)
{
    Yaml::Node root;
    root["key"] = "value";
    root["another_key"] = "Another Value";
    root["another_nested_map"]["key"] = "value";

    EXPECT_TRUE(root.IsMap());
    EXPECT_EQ(root.Size(), 3);

    size_t loops = 0;
    bool flags[3] = {false, false, false};
    for(Yaml::Iterator it = root.Begin(); it != root.End(); it++)
    {
        std::pair<std::string, Yaml::Node&> value = *it;
        const std::string & key = value.first;

        if(key == "key")
        {
            Yaml::Node & node = value.second;
            EXPECT_TRUE(node.IsScalar());
            EXPECT_EQ(node.As<std::string>(), "value");
            flags[0] = true;
        }
        else if(key == "another_key")
        {
            Yaml::Node & node = value.second;
            EXPECT_TRUE(node.IsScalar());
            EXPECT_EQ(node.As<std::string>(), "Another Value");
            flags[1] = true;
        }
        else if(key == "another_nested_map")
        {
            Yaml::Node & node = value.second;
            EXPECT_TRUE(node.IsMap());
            flags[2] = true;
        }

        loops++;
    }

    EXPECT_EQ(loops, 3);
    EXPECT_TRUE(flags[0]);
    EXPECT_TRUE(flags[1]);
    EXPECT_TRUE(flags[2]);

}

TEST(Iterator, ConstIterator)
{
    Yaml::Node root;
    root["key"] = "value";
    root["another_key"] = "Another Value";
    root["another_nested_map"]["key"] = "value";
    const Yaml::Node & constRoot = root;;


    EXPECT_TRUE(constRoot.IsMap());
    EXPECT_EQ(constRoot.Size(), 3);

    size_t loops = 0;
    bool flags[3] = {false, false, false};
    for(Yaml::ConstIterator it = constRoot.Begin(); it != constRoot.End(); it++)
    {
        std::pair<std::string, const Yaml::Node&> value = *it;
        const std::string & key = value.first;

        if(key == "key")
        {
            const Yaml::Node & node = value.second;
            EXPECT_TRUE(node.IsScalar());
            EXPECT_EQ(node.As<std::string>(), "value");
            flags[0] = true;
        }
        else if(key == "another_key")
        {
            const Yaml::Node & node = value.second;
            EXPECT_TRUE(node.IsScalar());
            EXPECT_EQ(node.As<std::string>(), "Another Value");
            flags[1] = true;
        }
        else if(key == "another_nested_map")
        {
            const Yaml::Node & node = value.second;
            EXPECT_TRUE(node.IsMap());
            flags[2] = true;
        }

        loops++;
    }

    EXPECT_EQ(loops, 3);
    EXPECT_TRUE(flags[0]);
    EXPECT_TRUE(flags[1]);
    EXPECT_TRUE(flags[2]);
}

TEST(Serialize, Serialize)
{
    Yaml::Node root;
    EXPECT_NO_THROW(Yaml::Parse(root, "../test/learnyaml.yaml"));

    {
        EXPECT_NO_THROW(Yaml::Serialize(root, "test_learnyaml.yaml"));

        Yaml::Node learn_yaml;
        EXPECT_NO_THROW(Yaml::Parse(learn_yaml, "test_learnyaml.yaml"));
        Parse_File_learnyaml(learn_yaml);
    }
    {
        std::string data = "";
        EXPECT_NO_THROW(Yaml::Serialize(root, data));

        Yaml::Node learn_yaml;
        EXPECT_NO_THROW(Yaml::Parse(learn_yaml, data));
        Parse_File_learnyaml(learn_yaml);
    }
    {
        std::stringstream data;
        EXPECT_NO_THROW(Yaml::Serialize(root, data));

        Yaml::Node learn_yaml;
        EXPECT_NO_THROW(Yaml::Parse(learn_yaml, data));
        Parse_File_learnyaml(learn_yaml);
    }
}*/


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
