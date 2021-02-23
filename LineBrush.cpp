// LineBrush.cpp

#include "LineBrush.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include <cmath>

const double PI = acos(-1);
const double EPSILON = 1e-5;

int LineBrush::sobelOpDirY[3][3] = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1} };
int LineBrush::sobelOpDirX[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };

LineBrush::LineBrush(ImpressionistDoc* pDoc, char* name):
	ImpBrush(pDoc, name) { }

void LineBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();

	glLineWidth(pDoc->getWidth());

	BrushMove(source, target);
}

void LineBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if ( pDoc == NULL ) {
		printf( "LineBrush::BrushMove  document is NULL\n" );
		return;
	}

	if (pDoc->m_strokeDirection == StrokeDirection::GRADIENT) setAngleToGradient(pDoc, source);
	
	drawLine(source, target, pDoc->getSize(), pDoc->getWidth(), pDoc->getAngle());
}

void LineBrush::BrushEnd(const Point source, const Point target)
{
	
}

void LineBrush::drawLine(const Point source, const Point target, const int size, const int width, const double angle)
{
    ImpressionistDoc* pDoc = GetDocument();

	double lsize = -size / 2.0, rsize = size / 2.0;

	if (pDoc->enableEdgeClip&&pDoc->m_EPainting!=nullptr) {
		GLubyte* Esource = pDoc->GetEdgePixel(source);
		for (int i = 0; i >= -size / 2.0; --i) {
			GLubyte* Epoint = pDoc->GetEdgePixel(source.x + i * cos(angle * PI / 180), source.y + i * sin(angle * PI / 180));
			if (Epoint[0] != Esource[0] || Esource[1] != Epoint[1] || Esource[2] != Epoint[2]) {
				lsize = i;
				break;
			}
		}
		for (int i = 0; i <= size / 2.0; ++i) {
			GLubyte* Epoint = pDoc->GetEdgePixel(source.x + i * cos(angle * PI / 180), source.y + i * sin(angle * PI / 180));
			if (Esource[0] != Epoint[0] || Esource[1] != Epoint[1] || Esource[2] != Epoint[2]) {
				rsize = i;
				break;
			}
		}
	}

    double ldeltaX = lsize  * cos(angle * PI / 180);
    double ldeltaY = lsize  * sin(angle * PI / 180);
	double rdeltaX = rsize  * cos(angle * PI / 180);
	double rdeltaY = rsize  * sin(angle * PI / 180);

	Point end1(target.x + rdeltaX, target.y + rdeltaY);
	Point end2(target.x + ldeltaX, target.y +ldeltaY);
	clipLine(target, end1);
	clipLine(target, end2);


    // double deltaX2 = width / 2.0 * cos((angle + 90) * PI / 180);
    // double deltaY2 = width / 2.0 * sin((angle + 90) * PI / 180);

	/*
	double deltaX2, deltaY2;
	if (abs(deltaX1) > abs(deltaY1)) deltaX2 = 0.0, deltaY2 = width / 2.0;
	else deltaX2 = width / 2.0, deltaY2 = 0.0;

	
    glBegin( GL_QUADS );

        SetColor( source );

        glVertex2d( target.x - deltaX1 + deltaX2, target.y - deltaY1 + deltaY2 );
        glVertex2d(target.x - deltaX1 - deltaX2, target.y - deltaY1 - deltaY2);

        glVertex2d(target.x + deltaX1 - deltaX2, target.y + deltaY1 - deltaY2);
        glVertex2d(target.x + deltaX1 + deltaX2, target.y + deltaY1 + deltaY2);

    glEnd();
    */

	glBegin(GL_LINES);

	SetColor(source);
	glVertex2d(end1.x, end1.y);
	glVertex2d(end2.x, end2.y);

	glEnd();
}

double LineBrush::rgbToGreyScale(ImpressionistDoc *pDoc, Point source)
{
	GLubyte* pixel;
	if (pDoc->ableAnotherGradient&&pDoc->m_GPainting!=nullptr)pixel = pDoc->GetGradientPixel(source);
	else pixel = pDoc->GetOriginalPixel(source);
	return 0.3 * pixel[0] * 0.59 * pixel[1] * 0.11 * pixel[2];
}

void LineBrush::setAngleToGradient(ImpressionistDoc *pDoc, Point source)
{
	double gradientX = 0.0, gradientY = 0.0;

	// Out of bound issue already been tackled in GetOriginalPixel()
	for (int i = 0; i < 3; ++i)			// row
		for (int j = 0; j < 3; ++j)		// col
		{
			double greyScale = rgbToGreyScale(pDoc, Point(source.x + j - 1, source.y + 2 - i));
			gradientX += greyScale * sobelOpDirX[i][j];
			gradientY += greyScale * sobelOpDirY[i][j];
		}

	double angle = atan(gradientY / gradientX) + PI / 2.0;
	pDoc->m_pUI->setAngle(angle / PI * 180.0);
}

void LineBrush::clipLine(const Point& start, Point& end)
{
	PaintView* view = GetDocument()->m_pUI->m_paintView;

	//start point is in the canvas
	if (view->isInCanvas(end)) return;

	double slope = (end.y - start.y) / ((end.x - start.x) + EPSILON), intercept = start.y - slope * start.x;
	int bottom = view->getWindowHeight() - view->getCanvasHeight();
	int top = view->getWindowHeight();
	if (end.x < 0)
	{
		end.x = 0;
		end.y = intercept;
	}
	if (end.y < bottom)
	{
		end.y = bottom;
		end.x = (end.y - intercept) / slope;
	}
	if (end.x > view->getCanvasWidth())
	{
		end.x = view->getCanvasWidth();
		end.y = slope * end.x + intercept;
	}
	if (end.y > top)
	{
		end.y = top;
		end.x = (end.y - intercept) / slope;
	}
}

