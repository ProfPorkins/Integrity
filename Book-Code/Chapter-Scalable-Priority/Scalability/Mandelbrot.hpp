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

#ifndef _MANDELBROT_HPP_
#define _MANDELBROT_HPP_

#include <windows.h>

#include <atomic>
#include <array>
#include <cstdint>
#include <memory>

class Mandelbrot
{
public:
	Mandelbrot(HWND hwnd, uint16_t sizeX, uint16_t sizeY);

	void update();

	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void zoomIn();
	void zoomOut();

private:
	HWND m_hwnd;
	uint16_t m_sizeX;
	uint16_t m_sizeY;

	//
	// Bitmap stuff
	HBITMAP m_handleBMP;
	HBITMAP m_handleBMPWrite;
	HDC m_hdcBMP;
	BITMAP m_bmp;
	uint8_t* m_pixels;

	static const double MOVEMENT_RATE;
	static const uint16_t MAX_ITERATIONS = 1000;
	struct Color
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};
	std::array<Color, 768> m_colors;
	std::unique_ptr<uint16_t[]> m_image;

	std::atomic<bool> m_updateRequired;
	std::atomic<bool> m_inUpdate;

	//
	// Extents of the mandelbrot region being shown
	double m_mandelLeft;
	double m_mandelRight;
	double m_mandelTop;
	double m_mandelBottom;

	void startNewImage();
	void prepareColors();
	void copyToPixels();
	void prepareBitmap();

};

#endif // _MANDELBROT_HPP_
