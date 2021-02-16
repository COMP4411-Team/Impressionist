//
// SPointBrush.h
//
// The header file for Point Brush. 
//

#ifndef SLINEBRUSH_H
#define SLINEBRUSH_H

#include "ImpBrush.h"

class SLineBrush : public ImpBrush
{
public:
	SLineBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);
};

#endif
