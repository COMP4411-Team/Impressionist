//
// SPointBrush.h
//
// The header file for Point Brush. 
//

#ifndef SPOINTBRUSH_H
#define SPOINTBRUSH_H

#include "ImpBrush.h"

class SPointBrush : public ImpBrush
{
public:
	SPointBrush( ImpressionistDoc* pDoc = NULL, char* name = NULL );

	void BrushBegin( const Point source, const Point target );
	void BrushMove( const Point source, const Point target );
	void BrushEnd( const Point source, const Point target );
	char* BrushName( void );
};

#endif
