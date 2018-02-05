#include "gtest/gtest.h"
#include "../yaml/Yaml.hpp"
#include <iostream>


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

TEST(Parse, File)
{
    {
        Yaml::Node root;
        EXPECT_THROW(Parse(root, "bad_path_of_file.txt"), Yaml::OperationException);
    }
    {
        Yaml::Node root;
        EXPECT_NO_THROW(Parse(root, "../.travis.yml"));
    }
    {
        Yaml::Node root;
        EXPECT_THROW(Parse(root, "../yaml/Yaml.hpp"), Yaml::ParsingException);
    }
    {
        Yaml::Node root;
        EXPECT_NO_THROW(Parse(root, "../test/learnyaml.yaml"));
    }
}

TEST(Parse, File_learnyaml)
{
    Yaml::Node root;
    EXPECT_NO_THROW(Parse(root, "../test/learnyaml.yaml"));

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

}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
