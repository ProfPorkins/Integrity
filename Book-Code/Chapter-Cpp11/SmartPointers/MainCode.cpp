/*
Copyright (c) 2015 James Dean Mathias

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <cstdint>
#include <iostream>
#include <memory>

void sharedPointer1()
{
	std::shared_ptr<uint32_t> myValue(new uint32_t(6));
	std::cout << *myValue << std::endl;
}

void sharedPointer2()
{
	std::shared_ptr<uint32_t> myValue = std::make_shared<uint32_t>(6);
	std::cout << *myValue << std::endl;
}

void sharedPointer3()
{
	std::shared_ptr<uint32_t> outer = nullptr;
	{
		std::shared_ptr<uint32_t> inner = std::make_shared<uint32_t>(6);
		std::cout << *inner << std::endl;
		outer = inner;
	}
	std::cout << *outer << std::endl;
}

void reportValue(const std::shared_ptr<uint32_t const>& value)
{
	std::cout << "The value is: " << *value << std::endl;
}

int main()
{
	sharedPointer1();
	sharedPointer2();
	sharedPointer3();

	std::shared_ptr<uint32_t> myValue = std::make_shared<uint32_t>(4);
	reportValue(myValue);

	return 0;
}
