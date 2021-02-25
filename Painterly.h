/*
	@inproceedings{hertzmann1998painterly,
	  title={Painterly rendering with curved brush strokes of multiple sizes},
	  author={Hertzmann, Aaron},
	  booktitle={Proceedings of the 25th annual conference on Computer graphics and interactive techniques},
	  pages={453--460},
	  year={1998}
	}
 *
 *
 *  Painterly.h
 *	An implementation of the painterly rendering
 *
 * 
 */

#pragma once

#include <vector>
#include <utility>
#include "ImpBrush.h"
#include "BSplines.h"

class ImpressionistDoc;

class Painterly
{
	// Tired of adding mutator, just made attributes public
	friend class CurvedBrush;

public:
	enum class Preset
	{
		IMPRESSIONIST,
		EXPRESSIONIST,
		COLORIST_WASH,
		POINTILLIST
	};
	
	struct Stroke
	{
		int size;
		float depth;
		unsigned char color[3];
		std::vector<Point> controlPoints;
	};

	struct Hsv
	{
		Hsv(double h, double s, double v) : h(h), s(s), v(v) { }
		double h, s, v;
	};
	
	Painterly(ImpressionistDoc* doc);

	void applyPreset(Preset preset);
	
	void paint();
	void paintLayer(unsigned char* ref, int size);
	Stroke* makeSplineStroke(unsigned char* ref, int x, int y, int size);

	ImpressionistDoc* doc;

	int layer;				// number of layers
	int threshold;			// approximation threshold
	int brushMinSize;
	double brushRatio;		// ratio between two brush sizes
	double curvature;		// curvature of the stroke
	double blurFactor;		// size of kernel for blurring
	int minLen, maxLen;		// minimum and maximum length for a stroke
	double alpha;
	double gridSizeFactor;				// gridSizeFactor * brushSize = step for iteration
	double jitterR, jitterG, jitterB;	// factors for random jitter on RGB
	double jitterH, jitterS, jitterV;	// jitter on HSV, h in degrees, s and v in [0, 1]
	double jitterFactor;		// rescale the jitter
	int sampleRate;			// samples per size

private:
	void convolution(int x, int y, unsigned char* target, double* kernel, int height, int width);
	double* calGaussianFilter(int height, int width, double sigma2);
	void applyGaussianBlur(unsigned char* ref, int size);

	float* calDifferenceImage(unsigned char* ref);
	float calDifference(unsigned char* a, unsigned char* b);

	float sumError(int x, int y, int size, float* diff, int& maxRow, int& maxCol);

	float rgb2Grey(unsigned char* src);
	unsigned char* getColor(int x, int y, unsigned char* src);
	std::pair<float, float> calGradient(int x, int y, unsigned char* src);

	float norm(float a, float b);

	void renderStrokes(std::vector<Stroke*> strokes, float* zBuffer);
	void renderCircles(int x, int y, Stroke& stroke, float* zBuffer);
	void alphaBlending(unsigned char* source, unsigned char* target);
	void addColorJitter(unsigned char* color);

	static Hsv rgb2Hsv(unsigned char* color);
	static void hsv2Rgb(Hsv in, unsigned char* out);
	
	static int sobelOpDirX[3][3], sobelOpDirY[3][3];
};

