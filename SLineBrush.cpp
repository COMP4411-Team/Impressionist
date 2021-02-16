//
// SLineBrush.cpp
//
// The implementation of Point Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "SLineBrush.h"

extern float frand();

SLineBrush::SLineBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}

void SLineBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();



	for (int i = 0; i < 4; i++) {
		BrushMove(source, target);
	}
	BrushEnd(source, target);
}

void SLineBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;
	int size = pDoc->getSize();
	float temp1 = frand();
	float temp2 = frand();

	if (pDoc == NULL) {
		printf("SLineBrush::BrushMove  document is NULL\n");
		return;
	}
	glBegin(GL_LINE_STRIP);

	GLubyte color[3];

	memcpy(color, pDoc->GetOriginalPixel(source.x + (temp1 - 0.5) * size, source.y + (temp2 - 0.5) * size), 3);

	glColor3ubv(color);


	glVertex2d(target.x + (temp1 - 1) * size, target.y + (temp2 - 0.5) * size);
	glVertex2d(target.x + temp1 * size, target.y + (temp2 - 0.5) * size);

	glEnd();
}

void SLineBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}