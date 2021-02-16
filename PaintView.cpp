//
// paintview.cpp
//
// The code maintaining the painting view of the input images
//

#include <cmath>

#include "impressionist.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "paintview.h"
#include "ImpBrush.h"


#define LEFT_MOUSE_DOWN		1
#define LEFT_MOUSE_DRAG		2
#define LEFT_MOUSE_UP		3
#define RIGHT_MOUSE_DOWN	4
#define RIGHT_MOUSE_DRAG	5
#define RIGHT_MOUSE_UP		6

#ifndef WIN32
#define min(a, b)	( ( (a)<(b) ) ? (a) : (b) )
#define max(a, b)	( ( (a)>(b) ) ? (a) : (b) )
#endif

const double PI = std::acos(-1);

static int		eventToDo;
static int		isAnEvent=0;
static Point	coord;

PaintView::PaintView(int			x, 
					 int			y, 
					 int			w, 
					 int			h, 
					 const char*	l)
						: Fl_Gl_Window(x,y,w,h,l)
{
	m_nWindowWidth	= w;
	m_nWindowHeight	= h;
}

PaintView::~PaintView()
{
	delete [] (GLubyte*) m_pPrevBitstart;
}

void PaintView::draw()
{
	#ifndef MESA
	// To avoid flicker on some machines.
	glDrawBuffer(GL_FRONT_AND_BACK);
	#endif // !MESA

	if(!valid())
	{
		// Enable color blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		
		glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

		// We're only using 2-D, so turn off depth 
		glDisable( GL_DEPTH_TEST );

		ortho();

		glClear( GL_COLOR_BUFFER_BIT );
	}

	Point scrollpos;// = GetScrollPosition();
	scrollpos.x = 0;
	scrollpos.y	= 0;

	m_nWindowWidth	= w();
	m_nWindowHeight	= h();

	int drawWidth, drawHeight;
	drawWidth = min( m_nWindowWidth, m_pDoc->m_nPaintWidth );
	drawHeight = min( m_nWindowHeight, m_pDoc->m_nPaintHeight );

	int startrow = m_pDoc->m_nPaintHeight - (scrollpos.y + drawHeight);
	if ( startrow < 0 ) startrow = 0;

	m_pPaintBitstart = m_pDoc->m_ucPainting + 
		3 * ((m_pDoc->m_nPaintWidth * startrow) + scrollpos.x);	// 4 since alpha channel added

	m_nDrawWidth	= drawWidth;
	m_nDrawHeight	= drawHeight;

	m_nStartRow		= startrow;
	m_nEndRow		= startrow + drawHeight;
	m_nStartCol		= scrollpos.x;
	m_nEndCol		= m_nStartCol + drawWidth;
	
	
	if ( m_pDoc->m_ucPainting && !isAnEvent) 
	{
		RestoreContent();
		// setBackground();	
		// SaveCurrentContent();

	}

	if ( m_pDoc->m_ucPainting && isAnEvent) 
	{
		// Clear it after processing.
		isAnEvent	= 0;	

		Point source( coord.x + m_nStartCol, m_nEndRow - coord.y );
		Point target( coord.x, m_nWindowHeight - coord.y );

		// This is the event handler
		switch (eventToDo) 
		{
		case LEFT_MOUSE_DOWN:
			if (!isInCanvas(target))	break;
			backupCanvas();
			m_previousSource = source;
			m_pDoc->m_pCurrentBrush->BrushBegin( source, target );
			break;
		case LEFT_MOUSE_DRAG:
			if (!isInCanvas(target))	break;
			if (m_pDoc->m_strokeDirection == StrokeDirection::BRUSH_DIR)
				setAngleFromBrushDir(m_previousSource, source);
			m_pDoc->m_pCurrentBrush->BrushMove( source, target );
			m_previousSource = source;
			break;
		case LEFT_MOUSE_UP:
			m_pDoc->m_pCurrentBrush->BrushEnd( source, target );

			SaveCurrentContent();
			RestoreContent();
			break;
		case RIGHT_MOUSE_DOWN:
			if (!isInCanvas(target))
			{
				m_targetStart.x = -1;
				break;
			}
			SaveCurrentContent();
			m_sourceStart = source;
			m_targetStart = target;
			break;
		case RIGHT_MOUSE_DRAG:
			if (!isInCanvas(target))	break;
			if (m_targetStart.x == -1)
			{
				m_sourceStart = source;
				m_targetStart = target;
			}
			RestoreContent();
			drawDirection(m_targetStart, target);
			break;
		case RIGHT_MOUSE_UP:
			RestoreContent();
			setStrokeDirection(m_sourceStart, source);
			break;

		default:
			printf("Unknown event!!\n");		
			break;
		}
	}

	glFlush();

	#ifndef MESA
	// To avoid flicker on some machines.
	glDrawBuffer(GL_BACK);
	#endif // !MESA

}


int PaintView::handle(int event)
{
	switch(event)
	{
	case FL_ENTER:
	    redraw();
		break;
	case FL_PUSH:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button()>1)
			eventToDo=RIGHT_MOUSE_DOWN;
		else
			eventToDo=LEFT_MOUSE_DOWN;
		isAnEvent=1;
		redraw();
		break;
	case FL_DRAG:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button()>1)
			eventToDo=RIGHT_MOUSE_DRAG;
		else
			eventToDo=LEFT_MOUSE_DRAG;
		isAnEvent=1;
		redraw();
		break;
	case FL_RELEASE:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button()>1)
			eventToDo=RIGHT_MOUSE_UP;
		else
			eventToDo=LEFT_MOUSE_UP;
		isAnEvent=1;
		redraw();
		break;
	case FL_MOVE:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		break;
	default:
		return 0;
		break;

	}

	m_pDoc->m_pUI->m_origView->setIndicator(Point(coord.x, m_nWindowHeight - coord.y));
	m_pDoc->m_pUI->m_origView->refresh();

	return 1;
}

void PaintView::refresh()
{
	redraw();
}

void PaintView::resizeWindow(int width, int height)
{
	resize(x(), y(), width, height);
}

void PaintView::SaveCurrentContent()
{
	// Tell openGL to read from the front buffer when capturing
	// out paint strokes
	glReadBuffer(GL_FRONT);

	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_PACK_ROW_LENGTH, m_pDoc->m_nPaintWidth );
	
	glReadPixels( 0, 
				  m_nWindowHeight - m_nDrawHeight, 
				  m_nDrawWidth, 
				  m_nDrawHeight, 
				  GL_RGB, 
				  GL_UNSIGNED_BYTE, 
				  m_pPaintBitstart );
}


void PaintView::RestoreContent()
{
	glDrawBuffer(GL_BACK);

	glClear( GL_COLOR_BUFFER_BIT );

	glRasterPos2i( 0, m_nWindowHeight - m_nDrawHeight );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, m_pDoc->m_nPaintWidth );
	glDrawPixels( m_nDrawWidth, 
				  m_nDrawHeight, 
				  GL_RGB, 
				  GL_UNSIGNED_BYTE, 
				  m_pPaintBitstart);

//	glDrawBuffer(GL_FRONT);
}

void PaintView::undo()
{
	if (m_pPrevBitstart == nullptr)	return;
	memcpy((GLubyte*)m_pPaintBitstart, (GLubyte*)m_pPrevBitstart, m_nDrawHeight * m_nDrawWidth * 4);
	RestoreContent();
	refresh();
}

// Use a translucent original image as the background
void PaintView::setBackground()
{
	int height = m_pDoc->m_nPaintHeight, width = m_pDoc->m_nPaintWidth;
	double alpha = m_pDoc->m_pUI->getTransparency();
	GLubyte* paint = m_pDoc->m_ucPainting, *bg = m_pDoc->m_background, *ori = m_pDoc->m_ucBitmap;
	
	for (int i = 0; i < height * width; ++i)
	{
		if (pixelEqual(paint + i * 3, bg + i * 3))	
		{
			paint[i * 3] = ori[i * 3] * alpha;
			paint[i * 3 + 1] = ori[i * 3 + 1] * alpha;
			paint[i * 3 + 2] = ori[i * 3 + 2] * alpha;
		}

		bg[i * 3] = ori[i * 3] * alpha;
		bg[i * 3 + 1] = ori[i * 3 + 1] * alpha;
		bg[i * 3 + 2] = ori[i * 3 + 2] * alpha;
	}

	refresh();
}


void PaintView::setStrokeDirection(Point start, Point end)
{
	double deltaX = end.x - start.x, deltaY = end.y - start.y;
	m_pDoc->m_pUI->setSize(std::sqrt(deltaX * deltaX + deltaY * deltaY));
	if (m_pDoc->m_strokeDirection == StrokeDirection::SLIDER)
		m_pDoc->m_pUI->setAngle(std::atan(deltaY / deltaX) / PI * 180);
}

void PaintView::drawDirection(Point start, Point end)
{
	glLineWidth(1.0f);
	glBegin(GL_LINES);

	glColor3ub(255, 0, 0);
	glVertex2d(start.x, start.y);
	glVertex2d(end.x, end.y);

	glEnd();
}

void PaintView::setAngleFromBrushDir(Point prev, Point cur)
{
	m_pDoc->m_pUI->setAngle(std::atan(double(cur.y - prev.y) / double(cur.x - prev.x)) / PI * 180.0);
}

void PaintView::backupCanvas()
{
	delete [] (GLubyte*)m_pPrevBitstart;
	m_pPrevBitstart = new GLubyte[m_nDrawHeight * m_nDrawWidth * 3];
	memcpy((GLubyte*)m_pPrevBitstart, (GLubyte*)m_pPaintBitstart, m_nDrawHeight * m_nDrawWidth * 3);
}

bool PaintView::pixelEqual(GLubyte* a, GLubyte* b)
{
	return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
}

bool PaintView::isInCanvas(const Point& target)
{
	return (target.x < m_nDrawWidth && target.x >= 0 
		&& target.y >= m_nWindowHeight - m_nDrawHeight && target.y < m_nWindowHeight);
}

int PaintView::getCanvasHeight()
{
	return m_nDrawHeight;
}

int PaintView::getCanvasWidth()
{
	return m_nDrawWidth;
}

int PaintView::getWindowHeight()
{
	return m_nWindowHeight;
}

int PaintView::getWindowWidth()
{
	return m_nWindowWidth;
}

