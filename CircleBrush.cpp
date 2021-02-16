//
// CircleBrush.cpp
//

#include "CircleBrush.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include <math.h>

const double PI = acos(-1);

CircleBrush::CircleBrush(ImpressionistDoc *pDoc, char *name) : ImpBrush(pDoc, name) { }

void CircleBrush::BrushBegin(const Point source, const Point target)
{
    ImpressionistDoc* pDoc = GetDocument();

    BrushMove(source, target);
}

void CircleBrush::BrushMove(const Point source, const Point target)
{
    ImpressionistDoc* pDoc = GetDocument();
    ImpressionistUI* dlg = pDoc->m_pUI;

    if ( pDoc == NULL ) {
        printf( "CircleBrush::BrushMove  document is NULL\n" );
        return;
    }

    drawCircle(source, target, pDoc->getSize() / 2.0);
}

void CircleBrush::BrushEnd(const Point source, const Point target)
{

}

void CircleBrush::drawCircle(const Point source, const Point target, double radius)
{
    glBegin(GL_POLYGON);

        SetColor(source);
        for (int i = 0; i < numEdges; ++i)
        {
            double angle = i * PI / 180.0;
            double x = target.x + radius * cos(angle), y = target.y + radius * sin(angle);
            glVertex2d(x, y);
        }

    glEnd();
}
