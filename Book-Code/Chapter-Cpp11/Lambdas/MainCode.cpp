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

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

void simpleLambda()
{
	auto myLambda = 
		[] () 
		{
			std::cout << "My first lambda!" << std::endl; 
		};
	myLambda();
}

uint32_t initialize(uint32_t value)
{ 
	static uint32_t nextValue = 1;
	return nextValue++;
}

class Scale
{
public:
	Scale(uint8_t factor) : m_factor(factor) {}

	uint32_t operator()(uint32_t& value) const
	{
		return value * m_factor;
	}

private:
	uint8_t m_factor;
};

void report(const std::string& title, std::vector<uint32_t>& values)
{
	std::cout << title << std::endl;
	for (auto value : values)
	{
		std::cout << "value : " << value << std::endl;
	}
}

void functorExample()
{
	std::vector<uint32_t> myVector(10);

	std::transform(myVector.begin(), myVector.end(), myVector.begin(), initialize);
	report("Initialized Values...", myVector);

	std::transform(myVector.begin(), myVector.end(), myVector.begin(), Scale(4));
	report("Scaled Values...", myVector);
}

void lambdaExample()
{
	std::vector<uint32_t> myVector(10);

	std::transform(
		myVector.begin(), myVector.end(), myVector.begin(),
		[](uint32_t value)
		{
			static uint32_t nextValue = 1;
			return nextValue++;
		});
	report("Initialized Values...", myVector);

	std::transform(
		myVector.begin(), myVector.end(), myVector.begin(),
		[](uint32_t value)
		{ 
			return value * 4; 
		});
	report("Scaled Values...", myVector);
}

std::function<uint32_t ()> makeLambda(uint32_t value)
{
	uint32_t local = value;
	return [&] () { return local; };
}

void badLambdaCapture()
{
	auto badLambda = makeLambda(8);
	uint32_t result = badLambda();
	std::cout << result << std::endl;
}

int main()
{
	std::cout << "-- simpleLambda --" << std::endl;
	simpleLambda();

	std::cout << std::endl << "-- functorExample --" << std::endl;
	functorExample();

	std::cout << std::endl << "-- lambdaExample --" << std::endl;
	lambdaExample();

	std::cout << std::endl << "-- badLambdaCapture --" << std::endl;
	badLambdaCapture();

	return 0;
}
