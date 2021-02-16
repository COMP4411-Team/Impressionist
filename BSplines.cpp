//
// BSplines.cpp
//

#include "BSplines.h"


BSplines::BSplines(const std::vector<Point>& controlPoints, int degree, int nSamples)
	: degree(degree), nSamples(nSamples), controlPoints(controlPoints), nControls(controlPoints.size())
{
	if (nControls < degree + 1)	
		this->degree = degree = nControls - 1;
	createKnots();

	float delta = (knots[nControls] - knots[degree - 1]) / (nSamples - 1);

	for (int i = 0; i < nSamples; ++i)
	{
		if (i == nSamples - 1)
			samples.push_back(controlPoints[controlPoints.size() - 1]);
		else
		{
			float t = knots[degree - 1] + i * delta;
			int index = getKnotIndex(t);
			if (index >= nControls)	continue;
			samples.push_back(deBoor(index, degree - 1, t));
		}
	}
}

BSplines::~BSplines()
{
	delete [] knots;
}


void BSplines::createKnots()
{
	int nKnots = nControls + degree;
	knots = new float[nKnots];

	for (int i = 0; i < nKnots; ++i)
	{
		// clamped B-splines
		if (i < degree)	knots[i] = 0;
		else if (i < nControls + 1)	knots[i] = knots[i - 1] + 1;
		else knots[i] = knots[i - 1];
	}
}


// P(i, j, t) = (1 - p) * P(i-1, j-1, t) + p * P(i, j-1, t)     for j > 0
// P(i, j, t) = P_i         for j = 0
// p = (t - t_i) / (t_{i+degree-j} - t_i)
Point BSplines::deBoor(int i, int j, float t)
{
    if (j <= 0) return controlPoints[i];

	float p = (t - knots[i]) / (knots[i + degree - j] - knots[i]);
	Point a = deBoor(i - 1, j - 1, t);
	Point b = deBoor(i, j - 1, t);
	
	return Point((1 - p) * a.x + p * b.x, (1 - p) * a.y + p * b.y);
}

int BSplines::getKnotIndex(float t)
{
	int nKnots = nControls + degree;
	if (t == knots[nKnots - 1])	return nKnots - 1;
	
	for (int i = 1; i < nKnots; ++i)
		if (t < knots[i])	return i - 1;
}
