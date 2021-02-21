//
// Implement a custom filter
//

#include "CustomFilter.h"

#include <sstream>

#include "ImpressionistDoc.h"
#include "ImpressionistUI.h"

CustomFilter::CustomFilter(ImpressionistDoc* doc, int size): doc(doc), size(size)
{
	height = doc->m_nPaintHeight;
	width = doc->m_nPaintWidth;
	
	filter = new float* [size];
	for (int i = 0; i < size; ++i)
		filter[i] = new float [size];
}

CustomFilter::~CustomFilter()
{
	for (int i = 0; i < size; ++i)
		delete [] filter[i];

	delete [] filter;
}

bool CustomFilter::parseWeights(std::string raw)
{
	std::stringstream ss;
	ss.str(raw);

	for (int i = size - 1; i >= 0; --i)
		for (int j = 0; j < size; ++j)
		{
			ss >> filter[i][j];
			if (ss.fail())
				return false;
		}
	
	return true;
}

void CustomFilter::normalize()
{
	float sum = 0.f;
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			sum += filter[i][j];

	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			filter[i][j] /= sum;
}

std::string CustomFilter::getParsedWeights()
{
	std::stringstream ss;
	for (int i = size - 1; i >= 0; --i)
	{
		for (int j = 0; j < size; ++j)
			ss << filter[i][j] << "\t";
		ss << std::endl;
	}
	
	return ss.str();
}

void CustomFilter::applyFilter()
{
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
			convolution(j, i);
}

void CustomFilter::convolution(int x, int y)
{
	float sum[3] = {0.f, 0.f, 0.f};
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
		{
			auto* color = doc->GetOriginalPixel(j + x - size/2, i + y - size/2);
			sum[0] += color[0] * filter[i][j];
			sum[1] += color[1] * filter[i][j];
			sum[2] += color[2] * filter[i][j];
		}
	clamp(sum);
	auto* target = doc->m_ucPainting + (y * width + x) * 3;
	target[0] = static_cast<unsigned char>(sum[0]);
	target[1] = static_cast<unsigned char>(sum[1]);
	target[2] = static_cast<unsigned char>(sum[2]);
}

void CustomFilter::clamp(float* color)
{
	for (int i = 0; i < 3; ++i)
	{
		color[i] = min(color[i], 255.f);
		color[i] = max(color[i], 0.f);
	}
}
