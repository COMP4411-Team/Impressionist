#pragma once
#include "ImpBrush.h"
#include "Painterly.h"
class CurvedBrush : public ImpBrush
{
public:
	CurvedBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);
	~CurvedBrush();
	
	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);

private:
	void renderStrokes(Painterly::Stroke* stroke);
	void renderCircles(int x, int y, Painterly::Stroke& stroke);
	float norm(float a, float b);
	void alphaBlending(unsigned char* source, unsigned char* target);
	Painterly* painterly;
};

