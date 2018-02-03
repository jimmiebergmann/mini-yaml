/*
* MIT License
*
* Copyright(c) 2018 Jimmie Bergmann
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

#include <Yaml.hpp>

using namespace Yaml;

static void Test_1();
static void Test_2();


int main()
{

    Test_1();
    Test_2();

	return 0;
}


void Test_1()
{
    const std::string data =
		"data1 : \t | \t \n"
		"  Hello1\n"
		"   world1.\n"
		"data2 : \t > \t \n"
		"  Hello2\n"
		"   world2.\n"
		"data3: \t |- \t \n"
		"  Hello3\n"
		"   world3.\n"
		"data4 : \t >- \t \n"
		"  Hello4\n"
		"   world4.\n"
		"data5: |\n"
		"   hello: world5\n"
		"    foo: bar5.\n"
		"data6: 123\n"
		"data7: 123.6\n";

    Node root;
    Reader reader;
    try
    {
        reader.Parse(root, data);
    }
    catch (const Exception e)
    {
        std::cout << "Exception " << e.Type() << ": " << e.what() << std::endl;
        std::cin.get();
    }

    std::cout << root["data1"].As<std::string>() << std::endl;
    std::cout << root["data2"].As<std::string>() << std::endl;
    std::cout << root["data3"].As<std::string>() << std::endl;
    std::cout << root["data4"].As<std::string>() << std::endl;
    std::cout << root["data5"].As<std::string>() << std::endl;
    std::cout << root["data6"].As<int>() << std::endl;
    std::cout << root["data7"].As<float>() << std::endl;
}

void Test_2()
{
    const std::string data =
		" - Hello world\n"
		" - 123\n"
		" - 123.4\n";

    Node root;
    Reader reader;
    try
    {
        reader.Parse(root, data);
        if(root.IsSequence() == false)
        {
            throw InternalException("Test: Root is not a sequence.");
        }
    }
    catch (const Exception e)
    {
        std::cout << "Exception " << e.Type() << ": " << e.what() << std::endl;
        std::cin.get();
    }

    std::cout << root[0].As<std::string>() << std::endl;
    std::cout << root[1].As<int>() << std::endl;
    std::cout << root[2].As<float>() << std::endl;
}

