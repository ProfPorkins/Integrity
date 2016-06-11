#ifndef _MANDELBROT_HPP_
#define _MANDELBROT_HPP_

#include "Shared/Messages//MandelFinishedResult.hpp"
#include "Shared/Messages//MandelResult.hpp"

#include <windows.h>

#include <array>
#include <atomic>
#include <cstdint>
#include <memory>

// -----------------------------------------------------------------
//
// @details This class is used to manage the data, computation, and
// updates to the Mandelbrot set.
//
// -----------------------------------------------------------------
class Mandelbrot
{
public:
	Mandelbrot(HWND hwnd, uint16_t sizeX, uint16_t sizeY);

	void update();
	void processMandelResult(Messages::MandelResult& taskResult);
	void processMandelFinishedResult(Messages::MandelFinishedResult& taskResult);

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
	void copyPixels(Messages::MandelResult& taskResult);
	void prepareBitmap();
};

#endif // _MANDELBROT_HPP_
