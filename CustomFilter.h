//
// Implement a user specified custom filter
//

#pragma once

#include <string>

class ImpressionistDoc;

class CustomFilter
{
public:
	CustomFilter(ImpressionistDoc* doc, int size);
	~CustomFilter();
	
	bool parseWeights(std::string raw);
	void normalize();
	std::string getParsedWeights();

	void applyFilter();

private:
	void convolution(int x, int y);
	void clamp(float* color);
	
	ImpressionistDoc* doc;

	int size;
	int height, width;
	float** filter;
};

