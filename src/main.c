#include "freetype/ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "lodepng/lodepng.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct
{
	FT_Library lib;
	FT_Face face;
}ftInfo;

int paddingSpace = 25;
int paddingHeight = 60;

int main(int argc, char **argv)
{
	ftInfo ft;

	int i = 0;
	int j = 0;
	int k = 0;

	int accumWidth = 0;
	int fontHeight = 50;
	int maxHei = 0;
	int biggestBottom = 0;
	int bestMiddle = 0;
	int averageTop = 0;
	int averageBottom = 0;
	int maxTop = 0;
	int minTop = 0;
	int xInc = 0;
	unsigned int* outputBuffer;
	FT_GlyphSlot g;

	if (FT_Init_FreeType(&ft.lib))
	{
		printf("Failed to initialize freetype");
	}

	if (FT_New_Face(ft.lib, argv[1], 0, &ft.face))
	{
		printf("Problem loading fontfile %s", argv[1]);
	}

	FT_Set_Pixel_Sizes(ft.face, 0, fontHeight);
	g = ft.face->glyph;

	for (i = 32; i < 128; ++i)
	{
		FT_Load_Char(ft.face, i, FT_LOAD_RENDER);
		accumWidth += g->bitmap.width + paddingSpace;
		
		if ((g->bitmap.rows - g->bitmap_top) + g->bitmap_top > maxTop)
		{
			maxTop = (g->bitmap.rows - g->bitmap_top) + g->bitmap_top;
		}
		
		if (g->bitmap_top < 0)
		{
			if ((g->bitmap_top + (-g->bitmap.rows) ) < minTop) 
			{
				minTop = g->bitmap_top + (-g->bitmap.rows);
			}
		}
	}

	bestMiddle = (maxTop + minTop) / 2;
	maxHei = (maxTop - minTop) + paddingHeight;

	outputBuffer = malloc(accumWidth*maxHei*4);
	memset(outputBuffer, 0x00, accumWidth*maxHei * 4);
	//accumWidth = 0;
	for (i = 32; i < 128; ++i)
	{
		FT_BBox  bbox;
		FT_Glyph gly;
		FT_Load_Char(ft.face, i, FT_LOAD_RENDER);
		
		printf("%c = %i \n", (char)i, g->bitmap.rows);


		
		for (j = 0; j < g->bitmap.rows; ++j)
		{		
			for (k = 0; k < g->bitmap.width; ++k)
			{
				unsigned int bytes = 0xFFFFFFFF;
				bytes = (unsigned int)g->bitmap.buffer[j*g->bitmap.width + k];
				bytes = (bytes << 24) | 0xFFFFFF;
				outputBuffer[(((j + (maxHei / 2)) - g->bitmap_top + bestMiddle)*accumWidth) + k + xInc] = bytes;
			}				
		}

		xInc += g->bitmap.width + paddingSpace;

		FT_Get_Glyph(g, &gly);
		FT_Glyph_Get_CBox(gly, FT_GLYPH_BBOX_PIXELS, &bbox);

		//	outputBuffer[xInc + (g->advance.x >> 6)] = 0xFF000000;
		outputBuffer[xInc + bbox.xMin] = 0xFF000000;

	}

	lodepng_encode32_file("harbage.png", (char*)outputBuffer, accumWidth, maxHei);

	system("PAUSE");
}