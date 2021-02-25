#include "CurvedBrush.h"
#include "ImpressionistUI.h"
#include "ImpressionistDoc.h"

CurvedBrush::CurvedBrush(ImpressionistDoc* pDoc, char* name): ImpBrush(pDoc, name)
{
	painterly = new Painterly(GetDocument());
}

CurvedBrush::~CurvedBrush()
{
	delete painterly;
}

void CurvedBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();

	BrushMove(source, target);
}

void CurvedBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if ( pDoc == NULL ) {
		printf( "LineBrush::BrushMove  document is NULL\n" );
		return;
	}

	int size = pDoc->getSize(), height = pDoc->m_nPaintHeight, width = pDoc->m_nPaintWidth;

	if (size > 5)
		size = 5 + (size - 5) / 10;

	auto* stroke = painterly->makeSplineStroke(pDoc->m_ucBitmap, source.x, source.y, size);
	renderStrokes(stroke);
}

void CurvedBrush::BrushEnd(const Point source, const Point target)
{
	
}

void CurvedBrush::renderStrokes(Painterly::Stroke* stroke)
{
	int sampleRate = 4;
	BSplines bSplines(stroke->controlPoints, 3, stroke->size * sampleRate);
	auto& samples = bSplines.samples;

	for (auto& sample : samples)
	{
		renderCircles(sample.x, sample.y, *stroke);
	}
}

void CurvedBrush::renderCircles(int x, int y, Painterly::Stroke& stroke)
{
	auto* doc = GetDocument();
	int height = doc->m_nPaintHeight, width = doc->m_nPaintWidth, radius = doc->getSize();
	unsigned char* orig = doc->m_ucPainting;

	if (radius > 3) 
		radius = 3;

	glPointSize(1.f);
	glBegin(GL_POINTS);
	
	for (int i = y - radius; i < y + radius; ++i)
		for (int j = x - radius; j < x + radius; ++j)
		{
			if (norm(i - y, j - x) > radius)	continue;
			if (i < 0 || j < 0 || i >= height || j >= width)
				continue;
			
			int index = i * width + j;

			unsigned char color[3];
			memcpy(color, orig + index * 3, 3);
			alphaBlending(stroke.color, color);
			glColor3ubv(color);
			glVertex2d(j, i);
		}

	glEnd();
	glPointSize(doc->getSize());
}

float CurvedBrush::norm(float a, float b)
{
	return std::sqrt(a * a + b * b);
}

void CurvedBrush::alphaBlending(unsigned char* source, unsigned char* target)
{
	auto alpha = GetDocument()->getAlpha();
	target[0] = source[0] * alpha + target[0] * (1 - alpha);
	target[1] = source[1] * alpha + target[1] * (1 - alpha);
	target[2] = source[2] * alpha + target[2] * (1 - alpha);
}
