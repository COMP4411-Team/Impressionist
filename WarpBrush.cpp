#include "WarpBrush.h"
#include "ImpressionistUI.h"
#include "ImpressionistDoc.h"
#include <cmath>

WarpBrush::WarpBrush(ImpressionistDoc* pDoc, char* name): ImpBrush(pDoc, name) { }

void WarpBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();

	pixels.clear();
	pixels.resize(pDoc->getSize() * 3);
	for (auto& row: pixels)
	{
		row.clear();
		row.resize(pDoc->getSize() * 3);
	}
	count = 0;
	storeColor(target);

	BrushMove(source, target);
}

void WarpBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if ( pDoc == NULL ) {
		printf( "WarpBrush::BrushMove  document is NULL\n" );
		return;
	}

	++count;
	int size = pixels.size();
	if (count > size)	return;

	glPointSize(1.f);
	glBegin(GL_POINTS);
	
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
		{
			int offsetX = j - size / 2;
			int offsetY = i - size / 2;

			double dist = std::sqrt(offsetX * offsetX + offsetY * offsetY);
			if (dist > size / 2)
				continue;

			double threshold = size / 2 - std::sqrt(count);
			if (dist > threshold)
				continue;

			int x = target.x + j - size/2, y = target.y + i - size/2;
			SetColor({j, i});
			glVertex2d(x, y);
		}
	
	glEnd();
	updateColor(target);
}

void WarpBrush::BrushEnd(const Point source, const Point target)
{
	glPointSize(GetDocument()->getSize());
}

void WarpBrush::storeColor(const Point& source)
{
	auto* doc = GetDocument();
	int size = pixels.size();
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
		{
			int row = source.y + i - size / 2;
			int col = source.x + j - size / 2;
			pixels[i][j] = doc->getCanvasPixel(col, row);	
		}
}

void WarpBrush::updateColor(const Point& source)
{
	auto* doc = GetDocument();
	int size = pixels.size();
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
		{
			int offsetX = j - size / 2;
			int offsetY = i - size / 2;
			// if (offsetX * offsetX + offsetY * offsetY < count)
			// 	continue;
			
			int x = source.x + j - size/2;
			int y = source.y + i - size/2;
			pixels[i][j] = doc->getCanvasPixel(x, y);
		}
}

void WarpBrush::SetColor (const Point source)
{
	ImpressionistDoc* pDoc = GetDocument();

	if (useOverrideColor)
	{
		glColor3ubv(overrideColor);
		return;
	}

	GLubyte color[4];
	const double* rgbScale = pDoc->m_pUI->getRgbScale();

	memcpy ( color, pixels[source.y][source.x], 3 );
	color[3] = (GLubyte)(pDoc->getAlpha() * 255);

	// Color blending, scale each channel by the factor
	for (int i = 0; i < 3; ++i)
		color[i] *= rgbScale[i];
	
	glColor4ubv( color );
}
