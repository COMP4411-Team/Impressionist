//
// Painterly.cpp
//

#include "Painterly.h"
#include "ImpressionistDoc.h"
#include "ImpressionistUI.h"
#include <cmath>
#include <vector>

extern float frand();

const float EPISILON = 1e-4;

int Painterly::sobelOpDirY[3][3] = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1} };
int Painterly::sobelOpDirX[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };

Painterly::Painterly(ImpressionistDoc* doc) : doc(doc)
{
	applyPreset(Preset::IMPRESSIONIST);
}

void Painterly::paint()
{
	unsigned char* canvas = doc->m_ucPainting;
	int height = doc->m_nPaintHeight, width = doc->m_nPaintWidth;
	memset(canvas, 0, height * width * 3);

	int* radii = new int[layer];
	radii[0] = brushMinSize;
	for (int i = 1; i < layer; ++i)	radii[i] = radii[i - 1] * brushRatio;

	for (int i = layer - 1; i >= 0; --i)
	{
		unsigned char* ref = new unsigned char[width * height * 3];
		applyGaussianBlur(ref, radii[i]);
		paintLayer(ref, radii[i]);
	}

	delete [] radii;
}

void Painterly::paintLayer(unsigned char* ref, int size)
{
	int height = doc->m_nPaintHeight, width = doc->m_nPaintWidth;
	float* zBuffer = new float [height * width];

	std::vector<Stroke*> strokes;
	
	for (int i = 0; i < height * width; ++i)
		zBuffer[i] = 1.f;

	float* diff = calDifferenceImage(ref);

	int gridSize = size * gridSizeFactor;

	for (int i = 0; i < height; i += gridSize)
		for (int j = 0; j < width; j += gridSize)
		{
			int maxRow, maxCol;
			float error = sumError(j, i, gridSize, diff, maxRow, maxCol);
			if (error <= threshold)	continue;

			strokes.push_back(makeSplineStroke(ref, maxCol, maxRow, size));
		}

	renderStrokes(strokes, zBuffer);
	delete [] zBuffer;
	delete [] diff;
	for (auto* stroke : strokes)
		delete stroke;
}

Painterly::Stroke* Painterly::makeSplineStroke(unsigned char* ref, int x, int y, int size)
{
	Stroke* stroke = new Stroke;
	stroke->depth = frand();
	stroke->size = size;

	Point cur(x, y);
	double prevGradX = 0.0, prevGradY = 0.0;
	stroke->controlPoints.push_back(cur);

	int width = doc->m_nPaintWidth, height = doc->m_nPaintHeight;
	int index = (y * width + x) * 3;
	memcpy(stroke->color, ref + index, 3);

	for (int i = 0; i < maxLen; ++i)
	{
		index = (cur.y * width + cur.x) * 3;
		if (calDifference(ref + index, doc->m_ucPainting + index) < calDifference(ref + index, stroke->color))
		{
			if (i >= minLen)
				return stroke;
		}

		auto grad = calGradient(cur.x, cur.y, ref);
		float magnitude = norm(grad.first, grad.second);
		if (magnitude < EPISILON)	return stroke;	// vanishing gradient

		float gradX = -grad.second / magnitude, gradY = grad.first / magnitude;	// find perpendicular and unit vector

		if (gradX * prevGradX + gradY * prevGradY < 0)	// find direction with smaller angle
		{
			gradX = -gradX;
			gradY = -gradY;
		}

		gradX = curvature * gradX + (1 - curvature) * prevGradX;
		gradY = curvature * gradY + (1 - curvature) * prevGradY;

		magnitude = norm(gradX, gradY);
		prevGradX = gradX / magnitude;
		prevGradY = gradY / magnitude;

		cur.x += prevGradX * size;
		cur.y += prevGradY * size;
		cur.x = max(cur.x, 0);
		cur.x = min(width - 1, cur.x);
		cur.y = max(cur.y, 0);
		cur.y = min(height - 1, cur.y);
		stroke->controlPoints.push_back(cur);
	}

	return stroke;
}

void Painterly::renderStrokes(std::vector<Stroke*> strokes, float* zBuffer)
{
	for (auto* stroke : strokes)
	{
		BSplines bSplines(stroke->controlPoints, 3, stroke->size * sampleRate);
		auto& samples = bSplines.samples;

		addColorJitter(stroke->color);
		
		for (auto& sample : samples)
		{
			renderCircles(sample.x, sample.y, *stroke, zBuffer);
		}
	}
}


void Painterly::renderCircles(int x, int y, Stroke& stroke, float* zBuffer)
{
	int height = doc->m_nPaintHeight, width = doc->m_nPaintWidth, radius = stroke.size;
	unsigned char* canvas = doc->m_ucPainting;
	
	for (int i = y - radius; i < y + radius; ++i)
		for (int j = x - radius; j < x + radius; ++j)
		{
			if (norm(i - y, j - x) > radius)	continue;
			if (i < 0 || j < 0 || i >= height || j >= width)
				continue;
			
			int index = i * width + j;
			if (zBuffer[index] < stroke.depth)	continue;
			zBuffer[index] = stroke.depth;

			alphaBlending(stroke.color, canvas + index * 3);
		}
}



// Do convolution with center at (x, y)
void Painterly::convolution(int x, int y, unsigned char* target, double* kernel, int height, int width)
{
	unsigned char* color, sum[3];
	
	int curX = x - width / 2, curY = y - height / 2;
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			color = doc->GetOriginalPixel(curX + j, curY + i);
			for (int k = 0; k < 3; ++k)
				sum[k] += color[k] * kernel[i * width + j];
		}

	memcpy(target + (y * doc->m_nPaintWidth + x) * 3, color, 3);
}

double* Painterly::calGaussianFilter(int height, int width, double sigma2)
{
	double* kernel = new double [height * width];
    double sum = 0.0;
    
    for (int i = 0; i < height; i++) 
        for (int j = 0; j < width; j++) 
		{
        		int x = j - width / 2, y = i - height / 2;
            kernel[i * width + j] = std::exp(-(x * x + y * y) / (2 * sigma2));
            sum += kernel[i * width + j];
		}
   
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
			kernel[i * width + j] /= sum;

	return kernel;
}

void Painterly::applyGaussianBlur(unsigned char* ref, int size)
{
	int height = doc->m_nPaintHeight, width = doc->m_nPaintWidth;
	double* kernel = calGaussianFilter(size, size, blurFactor * blurFactor * size * size);

	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
			convolution(j, i, ref, kernel, size, size);
}

float* Painterly::calDifferenceImage(unsigned char* ref)
{
	int height = doc->m_nPaintHeight, width = doc->m_nPaintWidth;
	float* diff = new float [width * height];
	
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			int index = (i * width + j) * 3;
			diff[i * width + j] = calDifference(ref + index, doc->m_ucPainting + index);
		}

	return diff;
}

float Painterly::calDifference(unsigned char* a, unsigned char* b)
{
	int deltaR = (int)a[0] - b[0], deltaG = (int)a[1] - b[1], deltaB = (int)a[2] - b[2];
	return std::sqrt(deltaR * deltaR + deltaG * deltaG + deltaB * deltaB);
}


float Painterly::sumError(int x, int y, int size, float* diff, int& maxRow, int& maxCol)
{
	int height = doc->m_nPaintHeight, width = doc->m_nPaintWidth;
	int startRow = max(0, y - size / 2), startCol = max(0, x - size / 2);
	int endRow = min(startRow + size, height), endCol = min(startCol + size, width);

	float sum = 0, maxError = 0;
	for (int i = startRow; i < endRow; ++i)
		for (int j = startCol; j < endCol; ++j)
		{
			float curError = diff[i * width + j];
			sum += curError;
			if (curError > maxError)
			{
				maxRow = i;
				maxCol = j;
			}
		}

	return sum / (size * size);
}

unsigned char* Painterly::getColor(int x, int y, unsigned char* src)
{
	x = max(x, 0);
	x = min(x, doc->m_nPaintWidth - 1);
	y = max(y, 0);
	y = min(y, doc->m_nPaintHeight - 1);

	return src + (y * doc->m_nPaintWidth + x) * 3;
}

float Painterly::rgb2Grey(unsigned char* src)
{
	return 0.30f * src[0] + 0.59f * src[1] + 0.11f * src[2];
}

std::pair<float, float> Painterly::calGradient(int x, int y, unsigned char* src)
{
	float gradX = 0.f, gradY = 0.f;
	for (int i = 0; i < 3; ++i)			// row
		for (int j = 0; j < 3; ++j)		// col
		{
			float greyScale = rgb2Grey(getColor(x + j - 1, y + 2 - i, src));
			gradX += greyScale * sobelOpDirX[i][j];
			gradY += greyScale * sobelOpDirY[i][j];
		}
	return std::pair<float, float>(gradX, gradY);
}

float Painterly::norm(float a, float b)
{
	return std::sqrt(a * a + b * b);
}

void Painterly::alphaBlending(unsigned char* source, unsigned char* target)
{
	target[0] = source[0] * alpha + target[0] * (1 - alpha);
	target[1] = source[1] * alpha + target[1] * (1 - alpha);
	target[2] = source[2] * alpha + target[2] * (1 - alpha);
}

void Painterly::addColorJitter(unsigned char* color)
{
	if (jitterH > 0.0 || jitterS > 0.0 || jitterV > 0.0)
	{
		Hsv hsv = rgb2Hsv(color);
		hsv.h = (1 - jitterH * jitterFactor) * hsv.h + jitterH * jitterFactor * frand() * 360;
		hsv.s = (1 - jitterS * jitterFactor) * hsv.s + jitterS * jitterFactor * frand() * 1;
		hsv.v = (1 - jitterV * jitterFactor) * hsv.v + jitterV * jitterFactor * frand() * 1;
		hsv2Rgb(hsv, color);
	}
	
	color[0] = (1 - jitterR * jitterFactor) * color[0] + jitterR * jitterFactor * frand() * 255;
	color[1] = (1 - jitterG * jitterFactor) * color[1] + jitterG * jitterFactor * frand() * 255;
	color[2] = (1 - jitterB * jitterFactor) * color[2] + jitterB * jitterFactor * frand() * 255;
}

void Painterly::applyPreset(Preset preset)
{
	switch (preset)
	{
	case Preset::IMPRESSIONIST:
		threshold = 100;
		layer = 3;
		brushRatio = 2.0;
		brushMinSize = 2;
		curvature = 1.0;
		blurFactor = 0.5;
		alpha = 1.0;
		gridSizeFactor = 1.0;
		minLen = 4;
		maxLen = 16;
		jitterR = jitterG = jitterB = 0.0;
		jitterH = jitterS = jitterV = 0.0;
		break;
	case Preset::EXPRESSIONIST:
		threshold = 50;
		layer = 3;
		brushRatio = 2.0;
		brushMinSize = 2;
		curvature = 0.25;
		blurFactor = 0.5;
		alpha = 0.7;
		gridSizeFactor = 1.0;
		minLen = 10;
		maxLen = 16;
		jitterV = 0.5;
		jitterR = jitterG = jitterB = 0.0;
		jitterH = jitterS = 0.0;
		break;
	case Preset::COLORIST_WASH:
		threshold = 120;
		layer = 3;
		brushRatio = 2.0;
		brushMinSize = 2;
		curvature = 1.0;
		blurFactor = 0.5;
		alpha = 0.5;
		gridSizeFactor = 1.0;
		minLen = 4;
		maxLen = 16;
		jitterG = jitterB = jitterB = 0.3;
		jitterH = jitterS = jitterV = 0.0;
		break;
	case Preset::POINTILLIST:
		threshold = 100;
		layer = 2;
		brushRatio = 2.0;
		brushMinSize = 2;
		curvature = 1.0;
		blurFactor = 0.5;
		alpha = 1.0;
		gridSizeFactor = 0.5;
		minLen = 1;		// use 1 instead of 0 in the paper to avoid handling special cases
		maxLen = 1;
		jitterV = 1.0;
		jitterH = 0.3;
		jitterS = 0.0;
		jitterR = jitterG = jitterB = 0.0;
		break;
	}
	
	sampleRate = 4;
	jitterFactor = 0.2;
}


// From https://stackoverflow.com/a/6930407
Painterly::Hsv Painterly::rgb2Hsv(unsigned char* color)
{
	double r = color[0] / 255.0, g = color[1] / 255.0, b = color[2] / 255.0;
	double min, max, delta;

	min = r < g ? r : g;
	min = min < b ? min : b;

	max = r > g ? r : g;
	max = max > b ? max : b;

	Hsv out(0.0, 0.0, max);
	delta = max - min;
	if (delta < 0.00001)
		return out;

	if (max > 0.0)
		out.s = (delta / max);
	else
	{
		out.h = NAN;
		return out;
	}

	if (r >= max)
		out.h = (g - b) / delta; // between yellow & magenta
	else if (g >= max)
		out.h = 2.0 + (b - r) / delta; // between cyan & yellow
	else
		out.h = 4.0 + (r - g) / delta; // between magenta & cyan

	out.h *= 60.0; // degrees

	if (out.h < 0.0)
		out.h += 360.0;

	return out;
}


// From https://stackoverflow.com/a/6930407
void Painterly::hsv2Rgb(Hsv in, unsigned char* out)
{
	double hh, p, q, t, ff;
	long i;

	if (in.s <= 0.0)
	{
		out[0] = in.v * 255;
		out[1] = in.v * 255;
		out[2] = in.v * 255;
		return;
	}

	hh = in.h;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = in.v * (1.0 - in.s);
	q = in.v * (1.0 - (in.s * ff));
	t = in.v * (1.0 - (in.s * (1.0 - ff)));

	in.v *= 255; // convert to [0, 255]
	t *= 255;
	p *= 255;
	q *= 255;

	switch (i)
	{
	case 0:
		out[0] = in.v;
		out[1] = t;
		out[2] = p;
		break;
	case 1:
		out[0] = q;
		out[1] = in.v;
		out[2] = p;
		break;
	case 2:
		out[0] = p;
		out[1] = in.v;
		out[2] = t;
		break;
	case 3:
		out[0] = p;
		out[1] = q;
		out[2] = in.v;
		break;
	case 4:
		out[0] = t;
		out[1] = p;
		out[2] = in.v;
		break;
	case 5:
	default:
		out[0] = in.v;
		out[1] = p;
		out[2] = q;
		break;
	}    
}
