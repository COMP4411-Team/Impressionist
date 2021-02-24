#pragma once

#include "ImpBrush.h"
#include <vector>

using std::vector;

class WarpBrush: public ImpBrush
{
public:
	WarpBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);
	
	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);

private:
	void storeColor(const Point& source);
	void SetColor(const Point source) override;
	void updateColor(const Point& source);
	
	vector<vector<unsigned char*>> pixels;
	int count{0};
};

