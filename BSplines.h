//
// BSplines.h
// Implementation of clamped B-splines
// Reference to https://zhuanlan.zhihu.com/p/50450278
//

#pragma once

#include <vector>
#include "ImpBrush.h"

class BSplines
{
public:
	BSplines(const std::vector<Point>& controlPoints, int degree, int nSamples);
	~BSplines();

	void createKnots();

	Point deBoor(int i, int j, float t);
	int getKnotIndex(float t);

	std::vector<Point> samples;
	const std::vector<Point>& controlPoints;
	float* knots;
	int nSamples;
	int nControls;
	int degree;
};

