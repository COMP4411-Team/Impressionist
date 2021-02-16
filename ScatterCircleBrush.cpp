//
// ScatterCircleBrush.cpp
//

#include "ScatterCircleBrush.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include <math.h>

extern float frand();

ScatterCircleBrush::ScatterCircleBrush(ImpressionistDoc *pDoc, char *name) : CircleBrush(pDoc, name) { }

void ScatterCircleBrush::BrushBegin(const Point source, const Point target)
{
    ImpressionistDoc* pDoc = GetDocument();

    BrushMove(source, target);
}

void ScatterCircleBrush::BrushMove(const Point source, const Point target)
{
    ImpressionistDoc* pDoc = GetDocument();
    ImpressionistUI* dlg = pDoc->m_pUI;

    if ( pDoc == NULL ) {
        printf( "CircleBrush::BrushMove  document is NULL\n" );
        return;
    }

    int size = pDoc->getSize();

    for (int i = 0; i < numCircles; ++i)
    {
        double x = (frand() - 0.5) * size, y = (frand() - 0.5) * size;
        drawCircle(Point(source.x - x, source.y - y), Point(target.x - x, target.y - y),
                   size / 2.0);
    }
}

void ScatterCircleBrush::BrushEnd(const Point source, const Point target)
{

}