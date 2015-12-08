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

#include <iostream>
#include <functional>
#include <string>
#include <algorithm>

void report(std::function<std::string(std::string)> update, std::string message)
{
	std::cout << "Your message: " << update(message) << std::endl;
}

std::string leetE(std::string message)
{
	std::replace(message.begin(), message.end(), 'e', '3');
	std::replace(message.begin(), message.end(), 'E', '3');
	return message;
}

std::string leetT(std::string message)
{
	std::replace(message.begin(), message.end(), 't', '7');
	std::replace(message.begin(), message.end(), 'T', '7');
	return message;
}

class Replace
{
public:
	std::string leetL(std::string message)
	{
		std::replace(message.begin(), message.end(), 'l', '1');
		std::replace(message.begin(), message.end(), 'L', '1');
		return message;
	}
};

int main()
{
	std::string myMessage = "This is a leet test";
	report(leetE, myMessage);
	report(leetT, myMessage);

	Replace myReplace;
	report(std::bind(&Replace::leetL, &myReplace, std::placeholders::_1), myMessage);

	return 0;
}
