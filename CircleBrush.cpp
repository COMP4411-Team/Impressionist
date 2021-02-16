//
// CircleBrush.cpp
//
// The implementation of Point Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "CircleBrush.h"

#define _USE_MATH_DEFINES
#include <math.h>


extern float frand();

CircleBrush::CircleBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}

void CircleBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;


	BrushMove(source, target);
}

void CircleBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();

	if (pDoc == NULL) {
		printf("CircleBrush::BrushMove  document is NULL\n");
		return;
	}

	glBegin(GL_POLYGON);
	SetColor(source);

	
	for (int i = 0; i < 361; i++) {
		glVertex2d(target.x + size / 2 * cos(i), target.y + size / 2 * sin(i));
		glVertex2d(target.x + size / 2 * cos(i+1), target.y + size / 2 * sin(i+1));
	}

	glEnd();
	glFlush();
}

void CircleBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}

