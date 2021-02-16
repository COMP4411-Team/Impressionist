//
// ScatterPointBrush.cpp
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "ScatterPointBrush.h"

extern float frand();

ScatterPointBrush::ScatterPointBrush( ImpressionistDoc* pDoc, char* name ) : ImpBrush(pDoc,name) { }

void ScatterPointBrush::BrushBegin( const Point source, const Point target )
{
    ImpressionistDoc* pDoc = GetDocument();
    ImpressionistUI* dlg=pDoc->m_pUI;

    glPointSize( (float)pointSize );

    BrushMove( source, target );
}

void ScatterPointBrush::BrushMove( const Point source, const Point target )
{
    ImpressionistDoc* pDoc = GetDocument();
    ImpressionistUI* dlg=pDoc->m_pUI;

    if ( pDoc == NULL ) {
        printf( "PointBrush::BrushMove  document is NULL\n" );
        return;
    }

    glBegin( GL_POINTS );

        for (int i = 0; i < numPoints; ++i)
        {
            double x = target.x + (frand() - 0.5f) * pDoc->getSize();
            double y = target.y + (frand() - 0.5f) * pDoc->getSize();
            if (!dlg->m_paintView->isInCanvas(Point(x, y)))	continue;
            SetColor(Point(x, y));
            glVertex2d(x, y);
        }

    glEnd();
}

void ScatterPointBrush::BrushEnd( const Point source, const Point target )
{
    // do nothing so far
}