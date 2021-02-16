//
// paintview.h
//
// The header file for painting view of the input images
//

#ifndef PAINTVIEW_H
#define PAINTVIEW_H

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>

class ImpressionistDoc;

class PaintView : public Fl_Gl_Window
{
public:
	PaintView(int x, int y, int w, int h, const char* l);
	~PaintView();
	
	void draw();
	int handle(int event);

	void refresh();
	
	void resizeWindow(int width, int height);

	void SaveCurrentContent();

	void RestoreContent();

	bool isInCanvas(const Point& target);

	void undo();

	// Use a dimmed original image as the background
	void setBackground();

	int getCanvasWidth();
	int getCanvasHeight();
	int getWindowWidth();
	int getWindowHeight();

	ImpressionistDoc *m_pDoc;

private:
	GLvoid* m_pPaintBitstart;
	GLvoid* m_pPrevBitstart{nullptr};
	
	int		m_nDrawWidth,
			m_nDrawHeight,
			m_nStartRow, 
			m_nEndRow,
			m_nStartCol, 
			m_nEndCol,
			m_nWindowWidth, 
			m_nWindowHeight;

	// Record the starting point for drawing the stroke direction
	Point m_sourceStart, m_targetStart;

	// Record the last coords for stroke direction
	Point m_previousSource;

	void setStrokeDirection(Point start, Point end);
	void drawDirection(Point source, Point target);
	void setAngleFromBrushDir(Point prev, Point cur);

	void backupCanvas();
	bool pixelEqual(GLubyte* a, GLubyte* b);
};

#endif