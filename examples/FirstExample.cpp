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

int main()
{
	/*std::string data =
		"data1 : \t | \t \n"
		"  Hello1\n"
		"   world1.\n"
		"data2 : \t > \t \n"
		"  Hello2\n"
		"   world2.\n"
		"data3 : \t |- \t \n"
		"  Hello3\n"
		"   world3.\n"
		"data4 : \t >- \t \n"
		"  Hello4\n"
		"   world4.\n"
		"data5: |\n"
		"   hello: world5\n"
		"    foo: bar5.\n"
		"val_3: test\n";
		*/

	/*std::string data =
		"val_2: |\n"
		"   hello: world\n"
		"    foo: bar.\n"
		"val_3: test\n";
	*/
	
	
	std::string data =
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
		"val_3: test\n"
		"foo  :\n"
		"   -   bar   :  super1\n"
		"   -   bar1   :  super2\n"
		"   -   bar2  \t :  \n"
		"        bar3   :  super3\n"
		"        bar4   :  |\n"
		"          1\n"
		"          2\n"
		"          3\n"
		"foo1  :\n"
		"  foo2  :\n"
		"   foo3  :\n"
		"   foo4  : 123\n"
		"map1:\n"
		"   map2:\n"
		"    map3: 123\n"
		"map1:\n"
		"  map2  :  123\n";
	
	/*std::string data =
		" val : 100\n"
		" \n"
		"   \n";*/


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
	


	return 0;
}