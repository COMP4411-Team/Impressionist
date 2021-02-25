//
// AlphaMappedBrush.cpp
//

#include "AlphaMappedBrush.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"

extern float frand();

AlphaMappedBrush::AlphaMappedBrush( ImpressionistDoc* pDoc, char* name ) : ImpBrush(pDoc,name)
{
	genMap(15, 15);
}

AlphaMappedBrush::~AlphaMappedBrush()
{
	delete [] alphaMap;
}


void AlphaMappedBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	glPointSize( 1.f );

	BrushMove( source, target );
}

void AlphaMappedBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if ( pDoc == NULL ) {
		printf( "PointBrush::BrushMove  document is NULL\n" );
		return;
	}

	if (!dlg->m_paintView->isInCanvas(target))	return;

	glBegin( GL_POINTS );

		
		//memcpy ( color, pDoc->GetOriginalPixel( source ), 3 );

		//for (int i = 0; i < 3; ++i)
			//color[i] *= rgbScale[i];

		int x = target.x - width / 2, y = target.y - height / 2;
		if (x < 0)	x = 0;
		if (y < 0)	y = 0;
		
		
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j)
			{
				memcpy(color, pDoc->GetOriginalPixel(source), 3);
				pDoc->m_pUI->setRgbScale(alphaMap[(i * width + j) * 3], alphaMap[(i * width + j) * 3 + 1], alphaMap[(i * width + j) * 3 + 2]);
				const double* rgbScale = pDoc->m_pUI->getRgbScale();

				for (int k = 0; k < 3; ++k) {
					color[k] *= rgbScale[k];
					if (color[k]) {
						int m = 0;
					}
				}

				color[3] = GLubyte(pDoc->getAlpha()*225);
				glColor4ubv(color);
				glVertex2d(x + j, y + i);
			}
		}
		pDoc->m_pUI->setRgbScale(1.0f, 1.0f, 1.0f);
	glEnd();
}

void AlphaMappedBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}


void AlphaMappedBrush::genMap(int height, int width)
{
	delete [] alphaMap;
	alphaMap = new float [height * width*3];

	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width*3; ++j)
			alphaMap[i * width*3 + j] = frand() * frand();

	this->height = height;
	this->width = width;
}


void AlphaMappedBrush::setAlphaMap(float* map, int height, int width)
{
	delete [] alphaMap;
	alphaMap = new float [height * width*3];
	memcpy(alphaMap, map, width * height * 3*sizeof(float));
	if (alphaMap[2]) {};
	this->height = height;
	this->width = width;
}
