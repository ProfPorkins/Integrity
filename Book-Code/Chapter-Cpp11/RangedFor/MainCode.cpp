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
#include <vector>

uint32_t myAccumulate1(const std::vector<uint8_t>& source)
{
	uint32_t total = 0;
	for (std::vector<uint8_t>::const_iterator itr = source.begin(); itr != source.end(); ++itr)
	{
		total += *itr;
	}
	return total;
}

uint32_t myAccumulate2(const std::vector<uint8_t>& source)
{
	uint32_t total = 0;
	for (auto itr = source.begin(); itr != source.end(); ++itr)
	{
		total += *itr;
	}
	return total;
}

uint32_t myAccumulate3(const std::vector<uint8_t>& source)
{
	uint32_t total = 0;
	for (auto value : source)
	{
		total += value;
	}
	return total;
}

void myScale(std::vector<uint8_t>& source, uint32_t scalar)
{
	for (auto& value : source)
	{
		value *= scalar;
	}
}

int main()
{
	std::vector<uint8_t> values;
	values.push_back(1);
	values.push_back(2);
	values.push_back(3);
	values.push_back(4);
	values.push_back(5);

	std::cout << "Total-1 : " << myAccumulate1(values) << std::endl;
	std::cout << "Total-2 : " << myAccumulate2(values) << std::endl;
	std::cout << "Total-3 : " << myAccumulate3(values) << std::endl;

	myScale(values, 2);
	for (auto value : values)
	{
		std::cout << "Scaled : " << static_cast<uint16_t>(value) << std::endl;
	}

	return 0;
}
