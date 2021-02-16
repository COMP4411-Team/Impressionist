// LineBrush.h
// 
// Header for class LineBrush

#ifndef LINEBRUSH_H
#define LINEBRUSH_H

#include "ImpBrush.h"

class LineBrush : public ImpBrush
{
public:
	LineBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);
	
	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);

protected:
	static int sobelOpDirY[3][3];
	static int sobelOpDirX[3][3];
    void drawLine(const Point source, const Point target, const int size,
                  const int width, const double angle);
	double rgbToGreyScale(ImpressionistDoc* pDoc, Point source);
	void setAngleToGradient(ImpressionistDoc* pDoc, Point source);
	void clipLine(const Point& start, Point& end);
};

#endif
