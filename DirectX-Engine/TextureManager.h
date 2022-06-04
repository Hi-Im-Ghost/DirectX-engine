#pragma once
#include "ChiliWin.h"

class TextureManager
{
public:
	TextureManager();
	~TextureManager();
private:
	static ULONG_PTR token;
	static int refCount;
};