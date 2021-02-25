// 
// impressionistDoc.h
//
// header file for Doc 
//

#ifndef ImpressionistDoc_h
#define ImpressionistDoc_h

#include "Impressionist.h"
#include "Bitmap.h"
#include "KdTree.h"

enum class StrokeDirection
{
	SLIDER = 0,
	GRADIENT,
	BRUSH_DIR
};

class ImpressionistUI;

class ImpressionistDoc
{
public:
	ImpressionistDoc();

	void	setUI(ImpressionistUI* ui);		// Assign the UI to use

	int		loadImage(char* iname);			// called by the UI to load image
	int		saveImage(char* iname);			// called by the UI to save image

	int dissolveImage(char* name);
	int loadMuralImage(char* name);
	int loadAlphaMap(char* name);

	int     clearCanvas();                  // called by the UI to clear the drawing canvas
	void	setBrushType(int type);			// called by the UI to set the brushType
	void setStrokeDirection(int type);
	int		getSize();						// get the UI size
	void	setSize(int size);				// set the UI size
	int getWidth();
	int getAngle();
	double getAlpha();
	char* getImageName();					// get the current image name
	void constructEdgeImage(unsigned char* Gx, unsigned char* Gy);//draw edge image

// Attributes
public:
	// Dimensions of original window.
	int				m_nWidth,
		m_nHeight;
	// Dimensions of the paint window.
	int				m_nPaintWidth,
		m_nPaintHeight;
	//Dimensions of the paint for another Gradient
	int				m_GPaintWidth,
		m_GPaintHeight;
	//Dimensions of the paint for edge painting
	int				m_EPaintWidth,
		m_EPaintHeight;
	// Bitmaps for original image and painting.
	unsigned char* m_ucBitmap;
	unsigned char* m_ucPainting;
	unsigned char* m_background{ nullptr };
	unsigned char* m_GPainting{ nullptr };// painting of another Gradient;
	unsigned char* m_EPainting{ nullptr };//edge painting;
	unsigned char* smooth{ nullptr };//The smoothed painting when find edge.
	// Stroke direction
	StrokeDirection m_strokeDirection{ StrokeDirection::SLIDER };

	// The current active brush.
	ImpBrush* m_pCurrentBrush;
	// Size of the brush.
	int m_nSize;

	ImpressionistUI* m_pUI;

	int spacing;   //spacing for autopainting.
	bool sizeRand = FALSE; //whether use random size for autopainting.

	bool ableAnotherGradient = FALSE;//whether use another gradient.
	
	bool enableEdgeClip = FALSE;//whether use edge clipping.

	bool enableFindEdge = FALSE;

// Operations
public:
	// Get the color of the original picture at the specified coord
	GLubyte* GetOriginalPixel(int x, int y);
	// Get the color of the original picture at the specified point	
	GLubyte* GetOriginalPixel(const Point p);
	// Get the color of the Gradient Picture
	GLubyte* GetGradientPixel(int x, int y);
	// Get the color of the Gradient Picture	
	GLubyte* GetGradientPixel(const Point p);
	// Get the color of the Edge Picture
	GLubyte* GetEdgePixel(int x, int y);
	// Get the color of the Edge Picture	
	GLubyte* GetEdgePixel(const Point p);
	//Get the color of the smoothed Picture
	GLubyte* GetSmoothPixel(int x, int y);
	// Get the color of the smoothed Picture	
	GLubyte* GetSmoothPixel(const Point p);

	GLubyte* getCanvasPixel(int x, int y);

	// Draw a pixel on the canvas
	void writePixel(int x, int y, GLubyte* pixel);

	void swapCanvasAndOrigin();

	void initBackground();

	void autoPaint();

	int loadGradientImage(char* name);//Load the image for another Gradient
	int loadEdgeImage(char* name); //Load the image for edge painting.

	// Generate kd tree from thumbnails
	void constructKDTree();

private:
	char			m_imageName[256];
	bool pixelsEqual(unsigned char* a, unsigned char* b);

};

extern void MessageBox(char *message);

#endif
