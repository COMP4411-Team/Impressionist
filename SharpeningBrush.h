//
// SharpeningBrush.h
//

#pragma once
#ifndef SHARPENING_BRUSH_H
#define SHARPENING_BRUSH_H

#include "ImpBrush.h"

class SharpeningBrush : public ImpBrush
{
public:
	SharpeningBrush( ImpressionistDoc* pDoc = NULL, char* name = NULL );

	void BrushBegin( const Point source, const Point target );
	void BrushMove( const Point source, const Point target );
	void BrushEnd( const Point source, const Point target );
	char* BrushName( void );

	void SetColor(const Point source) override;

private:
	static int laplacian[3][3];
	GLubyte clamp(int a, int b);		// return the clamped sum
};

#endif

