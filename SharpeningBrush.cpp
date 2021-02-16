//
// SharpeningBrush.cpp
// Use Laplacian operator for image sharpening
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "SharpeningBrush.h"

extern float frand();

int SharpeningBrush::laplacian[3][3] = { {-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1} };

SharpeningBrush::SharpeningBrush( ImpressionistDoc* pDoc, char* name ) : ImpBrush(pDoc, name) { }

void SharpeningBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	glPointSize( 1.0f );

	BrushMove( source, target );
}

void SharpeningBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if ( pDoc == NULL ) {
		printf( "PointBrush::BrushMove  document is NULL\n" );
		return;
	}

	if (!dlg->m_paintView->isInCanvas(target))	return;

	int size = pDoc->getSize();

	glBegin( GL_POINTS );

		for (int i = -size; i < size; ++i)
			for (int j = -size; j < size; ++j)
			{
				SetColor( Point(source.x + i, source.y + j) );
				glVertex2d( target.x + i, target.y + j);
			}
	
	glEnd();
}

void SharpeningBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

void SharpeningBrush::SetColor(const Point source)
{
	ImpressionistDoc* pDoc = GetDocument();
	int rawColor[3];
	GLubyte color[4];
	const double* rgbScale = pDoc->m_pUI->getRgbScale();

	memset(rawColor, 0, sizeof(rawColor));
	for (int i = -1; i < 2; ++i)
		for (int j = -1; j < 2; ++j)
		{
			GLubyte* curColor = pDoc->GetOriginalPixel(source.x + i, source.y + j);
			rawColor[0] += laplacian[i + 1][j + 1] * curColor[0];
			rawColor[1] += laplacian[i + 1][j + 1] * curColor[1];
			rawColor[2] += laplacian[i + 1][j + 1] * curColor[2];
		}

	memcpy( color, pDoc->GetOriginalPixel( source ), 3 );
	color[3] = (GLubyte)(pDoc->getAlpha() * 255);

	color[0] = clamp(color[0], rawColor[0]);
	color[1] = clamp(color[1], rawColor[1]);
	color[2] = clamp(color[2], rawColor[2]);

	// Color blending, scale each channel by the factor
	for (int i = 0; i < 3; ++i)
		color[i] *= rgbScale[i];
	
	glColor4ubv( color );
}

GLubyte SharpeningBrush::clamp(int a, int b)
{
	if (a + b > 255)	return 255;
	if (a + b < 0)	return 0;
	return a + b;
}
