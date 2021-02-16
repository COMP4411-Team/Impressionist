//
// AlphaMappedBrush.h
//

#pragma once
#include "ImpBrush.h"

class AlphaMappedBrush : public ImpBrush
{
public:
	AlphaMappedBrush( ImpressionistDoc* pDoc = NULL, char* name = NULL );
	~AlphaMappedBrush();

	void BrushBegin( const Point source, const Point target );
	void BrushMove( const Point source, const Point target );
	void BrushEnd( const Point source, const Point target );
	char* BrushName( void );

	void genMap(int height, int width);
	void setAlphaMap(float* map, int height, int width);

private:
	unsigned char color[4];
	float* alphaMap{nullptr};
	int height;
	int width;
};