#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <GdiPlus.h>
#include "ReadWrite.h"
#include "Colors.h"

class Font
{
public:
	Font() :
		key(BLACK),
		charWidth(0),
		charHeight(0),
		nCharsPerRow(0)
	{}
	Font(string filename, char* fontName, const UINT cWidth, const UINT cHeight, const UINT nCPR, const RGB32 k)
	:	key(k),
		name(fontName),
		charWidth(cWidth),
		charHeight(cHeight),
		nCharsPerRow(nCPR)
	{
		Gdiplus::Bitmap bmp(StrToWstr(filename).c_str());
		try {
			if (bmp.GetLastStatus() == Gdiplus::Ok) {

				Gdiplus::Color pixel;

				height = bmp.GetHeight();
				width = bmp.GetWidth();

				RGB32* pixels = new RGB32[height * width];

				for (UINT32 y = 0; y < height; y++)
				{
					for (UINT32 x = 0; x <  width; x++)
					{
						bmp.GetPixel(x, y, &pixel);
						pixels[x + (y *  width)] =
							CREATE_RGB32(pixel.GetR(), pixel.GetG(), pixel.GetB(), pixel.GetA());
					}
				}

				this->surface = pixels;

			} else {
				throw 60;
			}
		} catch (int ecode) {
			string emsg = ErrorCodeToStr(ecode)+filename;
			HandleFatalError(ecode, emsg);
		}
	}
	~Font()
	{
		delete[] surface;
	}
	void SetSurface(RGB32* surf, RGB32 k)
	{
		surface = surf;
		key = k;
	}
	RGB32* GetSurface() const
	{
		return surface;
	}
	RGB32 GetPixel(UINT32 x, UINT32 y) const
	{
		return surface[x + y * width];
	}
public:
	UINT32 width, height;
	UINT32 charWidth;
	UINT32 charHeight;
	UINT32 nCharsPerRow;
	RGB32 key;
	char* name;
private:
	RGB32* surface;
};

class FontSet
{
public:
	FontSet() :
		fonts(NULL),
		fontCount(0)
	{}
	FontSet(Font* fontArray, UINT32 nCount) :
		fonts(fontArray),
		fontCount(nCount)
	{}
	~FontSet()
	{
		delete[] fonts;
	}
	Font* GetFontByID(UINT32 index) const
	{
		return &fonts[index];
	}
	Font* GetFontByName(const char* fontName) const
	{
		for (UINT32 index = 0; index < fontCount; index++)
		{
			if (fonts[index].name == fontName)
			{
				return &fonts[index];
			}
		}
		return NULL;
	}
	void SetFonts(Font* fontArray, UINT32 nCount)
	{
		fontCount = nCount;
		fonts = fontArray;
	}
	UINT32 Count() const
	{
		return fontCount;
	}
private:
	Font* fonts;
	UINT32 fontCount;
};