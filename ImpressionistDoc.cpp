// 
// impressionistDoc.cpp
//
// It basically maintain the bitmap for answering the color query from the brush.
// It also acts as the bridge between brushes and UI (including views)
//

#include <FL/fl_ask.H>

#include "impressionistDoc.h"
#include "impressionistUI.h"

#include "ImpBrush.h"

// Include individual brush headers here.
#include "PointBrush.h"
#include "LineBrush.h"
#include "CircleBrush.h"
#include "ScatterPointBrush.h"
#include "ScatterLineBrush.h"
#include "ScatterCircleBrush.h"
#include "SharpeningBrush.h"
#include "AlphaMappedBrush.h"
#include "WarpBrush.h"

#define DESTROY(p)	{  if ((p)!=NULL) {delete [] p; p=NULL; } }

extern float frand();

ImpressionistDoc::ImpressionistDoc() 
{
	// Set NULL image name as init. 
	m_imageName[0]	='\0';	

	m_nWidth		= -1;
	m_ucBitmap		= NULL;
	m_ucPainting	= NULL;

	//initialize the size of paint;
	m_nPaintWidth	= 0;
	m_nPaintHeight	= 0;


	// create one instance of each brush
	ImpBrush::c_nBrushCount	= NUM_BRUSH_TYPE;
	ImpBrush::c_pBrushes	= new ImpBrush* [ImpBrush::c_nBrushCount];

	ImpBrush::c_pBrushes[BRUSH_POINTS]	= new PointBrush( this, "Points" );

	// Note: You should implement these 5 brushes.  They are set the same (PointBrush) for now
	ImpBrush::c_pBrushes[BRUSH_LINES]				
		= new LineBrush( this, "Lines" );
	ImpBrush::c_pBrushes[BRUSH_CIRCLES]				
		= new CircleBrush( this, "Circles" );
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_POINTS]	
		= new ScatterPointBrush( this, "Scattered Points" );
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_LINES]		
		= new ScatterLineBrush( this, "Scattered Lines" );
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_CIRCLES]	
		= new ScatterCircleBrush( this, "Scattered Circles" );
	ImpBrush::c_pBrushes[BRUSH_SHARPENING] = new SharpeningBrush(this, "Sharpening");
	ImpBrush::c_pBrushes[BRUSH_ALPHA_MAP] = new AlphaMappedBrush(this, "Alpha Mapped Brush");
	ImpBrush::c_pBrushes[BRUSH_WARP] = new WarpBrush(this, "Warp Brush");

	// make one of the brushes current
	m_pCurrentBrush	= ImpBrush::c_pBrushes[0];

}


//---------------------------------------------------------
// Set the current UI 
//---------------------------------------------------------
void ImpressionistDoc::setUI(ImpressionistUI* ui) 
{
	m_pUI	= ui;
}

//---------------------------------------------------------
// Returns the active picture/painting name
//---------------------------------------------------------
char* ImpressionistDoc::getImageName() 
{
	return m_imageName;
}

//---------------------------------------------------------
// Called by the UI when the user changes the brush type.
// type: one of the defined brush types.
//---------------------------------------------------------
void ImpressionistDoc::setBrushType(int type)
{
	m_pCurrentBrush	= ImpBrush::c_pBrushes[type];
}

void ImpressionistDoc::setStrokeDirection(int type)
{
	m_strokeDirection = static_cast<StrokeDirection>(type);
}

//---------------------------------------------------------
// Returns the size of the brush.
//---------------------------------------------------------
int ImpressionistDoc::getSize()
{
	return m_pUI->getSize();
}

int ImpressionistDoc::getWidth()
{
    return m_pUI->getWidth();
}

int ImpressionistDoc::getAngle()
{
    return m_pUI->getAngle();
}

double ImpressionistDoc::getAlpha()
{
    return m_pUI->getAlpha();
}


//---------------------------------------------------------
// Load the specified image
// This is called by the UI when the load image button is 
// pressed.
//---------------------------------------------------------
int ImpressionistDoc::loadImage(char *iname) 
{
	// try to open the image to read
	unsigned char*	data;
	int				width, 
					height;

	if ( (data=readBMP(iname, width, height))==NULL ) 
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	// reflect the fact of loading the new image
	m_nWidth		= width;
	m_nPaintWidth	= width;
	m_nHeight		= height;
	m_nPaintHeight	= height;


	// release old storage
	if ( m_ucBitmap ) delete [] m_ucBitmap;
	if ( m_ucPainting ) delete [] m_ucPainting;
	if (m_EPainting) {
		delete[] m_EPainting;
		m_EPainting = nullptr;
	}
	if (m_GPainting) {
		delete[] m_GPainting;
		m_GPainting = nullptr;
	}

	m_ucBitmap		= data;

	// allocate space for draw view
	m_ucPainting	= new unsigned char [width*height*3];
	// memset(m_ucPainting, 0, width*height*3);
	initBackground();

	m_pUI->m_mainWindow->resize(m_pUI->m_mainWindow->x(), 
								m_pUI->m_mainWindow->y(), 
								width*2, 
								height+25);

	// display it on origView
	m_pUI->m_origView->resizeWindow(width, height);	
	m_pUI->m_origView->setDisplay(0);
	m_pUI->m_origView->refresh();

	// refresh paint view as well
	m_pUI->m_paintView->resizeWindow(width, height);
	m_pUI->m_paintView->refresh();


	return 1;
}

int ImpressionistDoc::loadMuralImage(char* name)
{
	unsigned char* paint = m_ucPainting, *background = m_background;		// backup the content
	int oriHeight = m_nPaintHeight, oriWidth = m_nPaintWidth;
	
	m_ucPainting = m_background = nullptr;
	
	loadImage(name);

	int startRow = oriHeight < m_nPaintHeight ? m_nPaintHeight - oriHeight : 0;
	int endCol = oriWidth < m_nPaintWidth ? oriWidth : m_nPaintWidth;
	for (int i = startRow; i < m_nPaintHeight; ++i)
		for (int j = 0; j < endCol; ++j)
		{
			int oriIndex = ((oriHeight - m_nPaintHeight + i) * oriWidth + j) * 3;
			int index = (i * m_nPaintWidth + j) * 3;
			
			if (pixelsEqual(paint + oriIndex, background + oriIndex))	continue;
			memcpy(m_ucPainting + index, paint + oriIndex, 3);
		}

	delete [] paint;
	delete [] background;
	return 1;
}

int ImpressionistDoc::loadAlphaMap(char* name)
{
	unsigned char*	data;
	int				width, 
					height;

	if ( (data=readBMP(name, width, height))==NULL ) 
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	
	float* map = new float [width * height*3];
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j)
		{
			int index = (i * width + j) * 3;
			map[index] = data[index] / 255.0f;
			map[index + 1] = data[index + 1] / 255.0f;
			map[index + 2] = data[index + 2] / 255.0f;
		}
	}
	

	((AlphaMappedBrush*)(ImpBrush::c_pBrushes[BRUSH_ALPHA_MAP]))->setAlphaMap(map, height, width);

	delete [] map;
	delete [] data;
	return 1;
}


// Maintain the dimension of the original image
// image with different dimension will be aligned to top-left corner
int ImpressionistDoc::dissolveImage(char *name) 
{
	// try to open the image to read
	unsigned char*	data;
	int				width, 
					height;
	double alpha = m_pUI->getDissolveAlpha();

	if ( (data=readBMP(name, width, height))==NULL ) 
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	int startRow = height < m_nPaintHeight ? m_nPaintHeight - height : 0;
	int endCol = width < m_nPaintWidth ? width : m_nPaintWidth;

	for (int i = startRow; i < m_nPaintHeight; ++i)
		for (int j = 0; j < endCol; ++j)
		{
			int oriIndex = (i * m_nPaintWidth + j) * 3, index = ((i - startRow) * width + j) * 3;
			m_ucBitmap[oriIndex] = data[index] * alpha + m_ucBitmap[oriIndex] * (1 - alpha);
			m_ucBitmap[oriIndex + 1] = data[index + 1] * alpha + m_ucBitmap[oriIndex + 1] * (1 - alpha);
			m_ucBitmap[oriIndex + 2] = data[index + 2] * alpha + m_ucBitmap[oriIndex + 2] * (1 - alpha);
		}

	delete [] data;

	m_pUI->m_origView->refresh();
m_pUI->m_paintView->setBackground();
m_pUI->m_paintView->refresh();

return 1;
}

int ImpressionistDoc::loadGradientImage(char* iname) {
	unsigned char*	data;
	int				width,
					height;

	if ((data = readBMP(iname, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	if ((m_nPaintHeight > height || m_nPaintWidth > width)) {
		fl_alert("The painting map is larger than the map of another Gradient");
		return 0;
	}

	// reflect the fact of loading the new image
	m_GPaintWidth = width;
	m_GPaintHeight = height;


	// release old storage
	if (m_GPainting!= nullptr) delete[] m_GPainting;

	m_GPainting = data;

	return 1;
}

int ImpressionistDoc::loadEdgeImage(char* iname) {
	unsigned char* data;
	int				width,
		height;

	if ((data = readBMP(iname, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	if ((m_nPaintHeight != height || m_nPaintWidth != width)) {
		fl_alert("The loading map is not the edge map of the mapping");
		return 0;
	}

	// reflect the fact of loading the new image
	m_EPaintWidth = width;
	m_EPaintHeight = height;


	// release old storage
	if (m_EPainting != nullptr) delete[] m_EPainting;

	m_EPainting = data;

	return 1;
}

//Construct the Edge Image.
void ImpressionistDoc::constructEdgeImage(float* Gx, float* Gy) {
	if (m_EPainting != nullptr) {
		delete[]m_EPainting;
	}
	m_EPainting = new unsigned char[m_EPaintWidth * m_EPaintHeight * 3];
	for (int i = 0; i < m_EPaintWidth; i++) {
		for (int j = 0; j < m_EPaintHeight; j++) {
			auto* colorx= Gx + 3 * (j * m_EPaintWidth + i);
			auto* colory = Gy + 3 * (j * m_EPaintWidth + i);
			auto greyx = 0.299 * colorx[0] + 0.587 * colorx[1] + 0.114 * colorx[2];
			auto greyy = 0.299 * colory[0] + 0.587 * colory[1] + 0.114 * colory[2];
			double magnitude = sqrt(greyx * greyx + greyy * greyy);
			GLubyte* Ecolor = nullptr;
			//colory[0] += 128;
			//colory[1] += 128;
			//colory[2] += 128;
			if (magnitude < m_pUI->edgeThreshold) Ecolor = new GLubyte[3]{ 0,0,0 };
			//else Ecolor = new GLubyte[3]{ 255,255,255 };
			else {
				auto c = (GLubyte)(min(magnitude, 255));
				Ecolor = new GLubyte[3]{ c,c,c };
			}
			memcpy(m_EPainting + (j * m_EPaintWidth + i) * 3,Ecolor, 3);
			delete[] Ecolor;
		}
	}
	m_pUI->m_origView->setDisplay(1);
	m_pUI->m_origView->refresh();
}


//----------------------------------------------------------------
// Save the specified image
// This is called by the UI when the save image menu button is 
// pressed.
//----------------------------------------------------------------
int ImpressionistDoc::saveImage(char* iname)
{

	writeBMP(iname, m_nPaintWidth, m_nPaintHeight, m_ucPainting);

	return 1;
}

//----------------------------------------------------------------
// Clear the drawing canvas
// This is called by the UI when the clear canvas menu item is 
// chosen
//-----------------------------------------------------------------
int ImpressionistDoc::clearCanvas()
{

	// Release old storage
	if (m_ucPainting)
	{
		delete[] m_ucPainting;

		// allocate space for draw view
		m_ucPainting = new unsigned char[m_nPaintWidth * m_nPaintHeight * 3];
		memset(m_ucPainting, 0, m_nPaintWidth * m_nPaintHeight * 3);

		initBackground();

		// refresh paint view as well	
		m_pUI->m_paintView->refresh();
	}

	return 0;
}

//------------------------------------------------------------------
// Get the color of the pixel in the original image at coord x and y
//------------------------------------------------------------------
GLubyte* ImpressionistDoc::GetOriginalPixel(int x, int y)
{

	if (x < 0)
		x = 0;
	else if (x >= m_nWidth)
		x = m_nWidth - 1;

	if (y < 0)
		y = 0;
	else if (y >= m_nHeight)
		y = m_nHeight - 1;

	return (GLubyte*)(m_ucBitmap + 3 * (y * m_nWidth + x));

}

//----------------------------------------------------------------
// Get the color of the pixel in the original image at point p
//----------------------------------------------------------------
GLubyte* ImpressionistDoc::GetOriginalPixel(const Point p)
{
	return GetOriginalPixel(p.x, p.y);
}

//------------------------------------------------------------------
// Get the color of the pixel in the Edge image at coord x and y
//------------------------------------------------------------------
GLubyte* ImpressionistDoc::GetEdgePixel(int x, int y) {
	if (x < 0)
		x = 0;
	else if (x >= m_EPaintWidth)
		x = m_EPaintWidth - 1;

	if (y < 0)
		y = 0;
	else if (y >= m_EPaintHeight)
		y = m_EPaintHeight - 1;

	return (GLubyte*)(m_EPainting + 3 * (y * m_EPaintWidth + x));
}
//----------------------------------------------------------------
// Get the color of the pixel in the Edge image at point p
//----------------------------------------------------------------
GLubyte* ImpressionistDoc::GetEdgePixel(const Point p)
{
	return GetEdgePixel(p.x, p.y);
}

GLubyte* ImpressionistDoc::getCanvasPixel(int x, int y)
{
	if (x < 0)
		x = 0;
	else if (x >= m_nPaintWidth)
		x = m_nPaintWidth - 1;

	if (y < 0)
		y = 0;
	else if (y >= m_nPaintHeight)
		y = m_nPaintHeight - 1;

	return (GLubyte*)(m_ucPainting + 3 * (y * m_nPaintWidth + x));
}

//------------------------------------------------------------------
// Get the color of the pixel in the gradient image at coord x and y
//------------------------------------------------------------------
GLubyte* ImpressionistDoc::GetGradientPixel(int x, int y) {
	if (x < 0)
		x = 0;
	else if (x >= m_GPaintWidth)
		x = m_GPaintWidth - 1;

	if (y < 0)
		y = 0;
	else if (y >= m_GPaintHeight)
		y = m_GPaintHeight - 1;

	return (GLubyte*)(m_GPainting + 3 * (y * m_GPaintWidth + x));
}
//----------------------------------------------------------------
// Get the color of the pixel in the gradient image at point p
//----------------------------------------------------------------
GLubyte* ImpressionistDoc::GetGradientPixel(const Point p)
{
	return GetGradientPixel(p.x, p.y);
}

//------------------------------------------------------------------
// Get the color of the pixel in the smooth image at coord x and y
//------------------------------------------------------------------
GLubyte* ImpressionistDoc::GetSmoothPixel(int x, int y) {
	if (x < 0)
		x = 0;
	else if (x >= m_nWidth)
		x = m_nWidth - 1;

	if (y < 0)
		y = 0;
	else if (y >= m_nHeight)
		y = m_nHeight - 1;

	return (GLubyte*)(smooth + 3 * (y * m_EPaintWidth + x));
}
//----------------------------------------------------------------
// Get the color of the pixel in the smooth image at point p
//----------------------------------------------------------------
GLubyte* ImpressionistDoc::GetSmoothPixel(const Point p)
{
	return GetSmoothPixel(p.x, p.y);
}

void ImpressionistDoc::writePixel(int x, int y, GLubyte* pixel)
{
	int index = (y * m_nPaintWidth + x) * 3;
	memcpy(m_ucPainting + index, pixel, 3);
}

void ImpressionistDoc::swapCanvasAndOrigin()
{
	unsigned char* tmp = m_ucBitmap;
	m_ucBitmap = m_ucPainting;
	m_ucPainting = tmp;
	m_pUI->m_origView->refresh();
	m_pUI->m_paintView->refresh();
}



void ImpressionistDoc::initBackground()
{
	if (m_background) delete[] m_background;
	m_background = new unsigned char[m_nHeight * m_nWidth * 3];

	memcpy(m_ucPainting, m_ucBitmap, m_nWidth * m_nHeight * 3);
	for (int i = 0; i < m_nWidth * m_nHeight * 3; ++i)
		m_ucPainting[i] *= m_pUI->getTransparency();

	memcpy(m_background, m_ucPainting, m_nWidth * m_nHeight * 3);
}

void ImpressionistDoc::autoPaint() {
	/*
	int Osize = getSize();
	Point tsource(0, 0);
	Point ttarget(0, 0);

	for(int i = 0; i < m_nWidth; i += spacing){
		for (int j = 0; j < m_nHeight; j += spacing) {
			if (sizeRand) {
				float temp = frand()  + 0.5;
				m_pUI->setSize(temp*Osize);
			}
			tsource.x = i;
			tsource.y = m_nPaintHeight - j; 
			ttarget.x = i;
			ttarget.y = m_nHeight - j;
			m_pCurrentBrush->BrushBegin(tsource, ttarget);
			glFlush();
			m_pUI->m_paintView->refresh();
		}
	}
	m_pUI->setSize(Osize);
	*/
	m_pUI->m_paintView->enableAutoPaint = true;
	m_pUI->m_paintView->spacing = spacing;
	m_pUI->m_paintView->sizeRand = sizeRand;
	m_pUI->m_paintView->refresh();
}

bool ImpressionistDoc::pixelsEqual(unsigned char* a, unsigned char* b)
{
	return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
}


