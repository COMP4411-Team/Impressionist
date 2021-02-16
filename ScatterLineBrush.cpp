//
// ScatterLineBrush.cpp
//

#include "ScatterLineBrush.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include <math.h>
#include <algorithm>

extern float frand();

ScatterLineBrush::ScatterLineBrush(ImpressionistDoc *pDoc, char *name) : LineBrush(pDoc, name) { }

void ScatterLineBrush::BrushBegin(const Point source, const Point target)
{
    ImpressionistDoc* pDoc = GetDocument();

    BrushMove(source, target);
}

void ScatterLineBrush::BrushMove(const Point source, const Point target)
{
    ImpressionistDoc* pDoc = GetDocument();
    ImpressionistUI* dlg=pDoc->m_pUI;

    if ( pDoc == NULL ) {
        printf( "LineBrush::BrushMove  document is NULL\n" );
        return;
    }

    for (int i = 0; i < numLines; ++i)
    {
        double len = max(frand(), 0.5f) * pDoc->getSize();
        double x = (frand() - 0.5f) * pDoc->getSize(), y = (frand() - 0.5f) * pDoc->getSize();

        if (pDoc->m_strokeDirection == StrokeDirection::GRADIENT)    
            setAngleToGradient(pDoc, Point(source.x + x, source.y + y));
        drawLine(Point(source.x + x, source.y + y), Point(target.x + x, target.y + y),
                 len, pDoc->getWidth(), pDoc->getAngel());
    }
}

void ScatterLineBrush::BrushEnd(const Point source, const Point target)
{

}