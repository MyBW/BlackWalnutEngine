#include "BWCommon.h"

size_t HelperFunction::GetMaxMipmaps(size_t width, size_t height, size_t depth)
{
	size_t count = 0;
	do {
		if (width>1)		width = width / 2;
		if (height>1)	height = height / 2;
		if (depth>1)		depth = depth / 2;
		/*
		NOT needed, compressed formats will have mipmaps up to 1x1
		if(PixelUtil::isValidExtent(width, height, depth, format))
		count ++;
		else
		break;
		*/

		count++;
	} while (!(width == 1 && height == 1 && depth == 1));

	return count;
}
