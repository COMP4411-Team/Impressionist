//
// impressionistUI.h
//
// The user interface part for the program.
//


#include <FL/fl_ask.h>

#include <math.h>

#include "impressionistUI.h"
#include "impressionistDoc.h"
#include "BayesianMatting.h"
#include "KnnMatting.h"
#include <vector>
#include <string>

using Preset = Painterly::Preset;

/*
//------------------------------ Widget Examples -------------------------------------------------
Here is some example code for all of the widgets that you may need to add to the 
project.  You can copy and paste these into your code and then change them to 
make them look how you want.  Descriptions for all of the widgets here can be found 
in links on the fltk help session page.

//---------Window/Dialog and Menubar-----------------------------------
	
	//----To install a window--------------------------
	Fl_Window* myWindow = new Fl_Window(600, 300, "MyWindow");
		myWindow->user_data((void*)(this));	// record self to be used by static callback functions
		
		// install menu bar
		myMenubar = new Fl_Menu_Bar(0, 0, 600, 25);
		Fl_Menu_Item ImpressionistUI::myMenuItems[] = {
			{ "&File",		0, 0, 0, FL_SUBMENU },
				{ "&Load...",	FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_load },
				{ "&Save...",	FL_ALT + 's', (Fl_Callback *)ImpressionistUI::cb_save }.
				{ "&Quit",			FL_ALT + 'q', (Fl_Callback *)ImpressionistUI::cb_exit },
				{ 0 },
			{ "&Edit",		0, 0, 0, FL_SUBMENU },
				{ "&Copy",FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_copy, (void *)COPY },
				{ "&Cut",	FL_ALT + 'x', (Fl_Callback *)ImpressionistUI::cb_cut, (void *)CUT },
				{ "&Paste",	FL_ALT + 'v', (Fl_Callback *)ImpressionistUI::cb_paste, (void *)PASTE },
				{ 0 },
			{ "&Help",		0, 0, 0, FL_SUBMENU },
				{ "&About",	FL_ALT + 'a', (Fl_Callback *)ImpressionistUI::cb_about },
				{ 0 },
			{ 0 }
		};
		myMenubar->menu(myMenuItems);
    m_mainWindow->end();

	//----The window callback--------------------------
	// One of the callbacks
	void ImpressionistUI::cb_load(Fl_Menu_* o, void* v) 
	{	
		ImpressionistDoc *pDoc=whoami(o)->getDocument();

		char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName() );
		if (newfile != NULL) {
			pDoc->loadImage(newfile);
		}
	}


//------------Slider---------------------------------------

	//----To install a slider--------------------------
	Fl_Value_Slider * mySlider = new Fl_Value_Slider(10, 80, 300, 20, "My Value");
	mySlider->user_data((void*)(this));	// record self to be used by static callback functions
	mySlider->type(FL_HOR_NICE_SLIDER);
    mySlider->labelfont(FL_COURIER);
    mySlider->labelsize(12);
	mySlider->minimum(1);
	mySlider->maximum(40);
	mySlider->step(1);
	mySlider->value(m_nMyValue);
	mySlider->align(FL_ALIGN_RIGHT);
	mySlider->callback(cb_MyValueSlides);

	//----The slider callback--------------------------
	void ImpressionistUI::cb_MyValueSlides(Fl_Widget* o, void* v)
	{
		((ImpressionistUI*)(o->user_data()))->m_nMyValue=int( ((Fl_Slider *)o)->value() ) ;
	}
	

//------------Choice---------------------------------------
	
	//----To install a choice--------------------------
	Fl_Choice * myChoice = new Fl_Choice(50,10,150,25,"&myChoiceLabel");
	myChoice->user_data((void*)(this));	 // record self to be used by static callback functions
	Fl_Menu_Item ImpressionistUI::myChoiceMenu[3+1] = {
	  {"one",FL_ALT+'p', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)ONE},
	  {"two",FL_ALT+'l', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)TWO},
	  {"three",FL_ALT+'c', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)THREE},
	  {0}
	};
	myChoice->menu(myChoiceMenu);
	myChoice->callback(cb_myChoice);
	
	//-----The choice callback-------------------------
	void ImpressionistUI::cb_myChoice(Fl_Widget* o, void* v)
	{
		ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
		ImpressionistDoc* pDoc=pUI->getDocument();

		int type=(int)v;

		pDoc->setMyType(type);
	}


//------------Button---------------------------------------

	//---To install a button---------------------------
	Fl_Button* myButton = new Fl_Button(330,220,50,20,"&myButtonLabel");
	myButton->user_data((void*)(this));   // record self to be used by static callback functions
	myButton->callback(cb_myButton);

	//---The button callback---------------------------
	void ImpressionistUI::cb_myButton(Fl_Widget* o, void* v)
	{
		ImpressionistUI* pUI=((ImpressionistUI*)(o->user_data()));
		ImpressionistDoc* pDoc = pUI->getDocument();
		pDoc->startPainting();
	}


//---------Light Button------------------------------------
	
	//---To install a light button---------------------
	Fl_Light_Button* myLightButton = new Fl_Light_Button(240,10,150,25,"&myLightButtonLabel");
	myLightButton->user_data((void*)(this));   // record self to be used by static callback functions
	myLightButton->callback(cb_myLightButton);

	//---The light button callback---------------------
	void ImpressionistUI::cb_myLightButton(Fl_Widget* o, void* v)
	{
		ImpressionistUI *pUI=((ImpressionistUI*)(o->user_data()));

		if (pUI->myBool==TRUE) pUI->myBool=FALSE;
		else pUI->myBool=TRUE;
	}

//----------Int Input--------------------------------------

    //---To install an int input-----------------------
	Fl_Int_Input* myInput = new Fl_Int_Input(200, 50, 5, 5, "&My Input");
	myInput->user_data((void*)(this));   // record self to be used by static callback functions
	myInput->callback(cb_myInput);

	//---The int input callback------------------------
	void ImpressionistUI::cb_myInput(Fl_Widget* o, void* v)
	{
		((ImpressionistUI*)(o->user_data()))->m_nMyInputValue=int( ((Fl_Int_Input *)o)->value() );
	}

//------------------------------------------------------------------------------------------------
*/

//------------------------------------- Help Functions --------------------------------------------

Fl_Slider* ImpressionistUI::makeSlider(int x, int y, int w, int h, const char* l, double min, double max, double step)
{
	auto slider = new Fl_Value_Slider(x, y, w, h, l);
	slider->user_data((void*)(this));
	slider->type(FL_HOR_NICE_SLIDER);
    slider->labelfont(FL_COURIER);
    slider->labelsize(12);
	slider->minimum(min);
	slider->maximum(max);
	slider->step(step);
	slider->align(FL_ALIGN_RIGHT);
	
	return slider;
}

//------------------------------------------------------------
// This returns the UI, given the menu item.  It provides a
// link from the menu items to the UI
//------------------------------------------------------------
ImpressionistUI* ImpressionistUI::whoami(Fl_Menu_* o)	
{
	return ( (ImpressionistUI*)(o->parent()->user_data()) );
}

ImpressionistUI* ImpressionistUI::whoami(Fl_Widget* o)
{
	return ( (ImpressionistUI*)(o->user_data()) );
}


//--------------------------------- Callback Functions --------------------------------------------

//------------------------------------------------------------------
// Brings up a file chooser and then loads the chosen image
// This is called by the UI when the load image menu item is chosen
//------------------------------------------------------------------
void ImpressionistUI::cb_load_image(Fl_Menu_* o, void* v) 
{
	ImpressionistDoc *pDoc=whoami(o)->getDocument();

	char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName() );
	if (newfile != NULL) {
		pDoc->loadImage(newfile);
	}
}


//------------------------------------------------------------------
// Brings up a file chooser and then saves the painted image
// This is called by the UI when the save image menu item is chosen
//------------------------------------------------------------------
void ImpressionistUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	ImpressionistDoc *pDoc=whoami(o)->getDocument();

	char* newfile = fl_file_chooser("Save File?", "*.bmp", "save.bmp" );
	if (newfile != NULL) {
		pDoc->saveImage(newfile);
	}
}

//-------------------------------------------------------------
// Brings up the paint dialog
// This is called by the UI when the brushes menu item
// is chosen
//-------------------------------------------------------------
void ImpressionistUI::cb_brushes(Fl_Menu_* o, void* v) 
{
	whoami(o)->m_brushDialog->show();
}

//------------------------------------------------------------
// Clears the paintview canvas.
// Called by the UI when the clear canvas menu item is chosen
//------------------------------------------------------------
void ImpressionistUI::cb_clear_canvas(Fl_Menu_* o, void* v)
{
	ImpressionistDoc* pDoc=whoami(o)->getDocument();

	pDoc->clearCanvas();
}

//------------------------------------------------------------
// Causes the Impressionist program to exit
// Called by the UI when the quit menu item is chosen
//------------------------------------------------------------
void ImpressionistUI::cb_exit(Fl_Menu_* o, void* v) 
{
	whoami(o)->m_mainWindow->hide();
	whoami(o)->m_brushDialog->hide();

}



//-----------------------------------------------------------
// Brings up an about dialog box
// Called by the UI when the about menu item is chosen
//-----------------------------------------------------------
void ImpressionistUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("Impressionist FLTK version for CS341, Spring 2002");
}

//------- UI should keep track of the current for all the controls for answering the query from Doc ---------
//-------------------------------------------------------------
// Sets the type of brush to use to the one chosen in the brush 
// choice.  
// Called by the UI when a brush is chosen in the brush choice
//-------------------------------------------------------------
void ImpressionistUI::cb_brushChoice(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
	ImpressionistDoc* pDoc=pUI->getDocument();

	int type=(int)v;

	if (type != BRUSH_LINES && type != BRUSH_SCATTERED_LINES)
	{
		pUI->m_BrushWidthSlider->deactivate();
		pUI->m_BrushAngleSlider->deactivate();
		pUI->m_StrokeDirectionChoice->deactivate();
		pUI->m_anotherGradient->deactivate();
		pUI->m_edgeClip->deactivate();
	}
	else
	{
		pUI->m_BrushWidthSlider->activate();
		pUI->m_BrushAngleSlider->activate();
		pUI->m_StrokeDirectionChoice->activate();
		pUI->m_anotherGradient->activate();
		pUI->m_edgeClip->activate();
	}

	pDoc->setBrushType(type);
}

void ImpressionistUI::cb_strokeChoice(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI = ((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc* pDoc = pUI->getDocument();

	pDoc->setStrokeDirection(int(v));
}

//------------------------------------------------------------
// Clears the paintview canvas.
// Called by the UI when the clear canvas button is pushed
//------------------------------------------------------------
void ImpressionistUI::cb_clear_canvas_button(Fl_Widget* o, void* v)
{
	ImpressionistDoc * pDoc = ((ImpressionistUI*)(o->user_data()))->getDocument();

	pDoc->clearCanvas();
}


//-----------------------------------------------------------
// Updates the brush size to use from the value of the size
// slider
// Called by the UI when the size slider is moved
//-----------------------------------------------------------
void ImpressionistUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
}

void ImpressionistUI::cb_widthSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_nWidth = int(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_angleSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_nAngle = int(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_alphaSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_nAlpha = double(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_transSlides(Fl_Widget* o, void* v)
{
	whoami(o)->m_transparency = double(((Fl_Slider*)o)->value());
	whoami(o)->m_paintView->setBackground();
}

void ImpressionistUI::cb_swapCanvasAndOrigin(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_pDoc->swapCanvasAndOrigin();
}

void ImpressionistUI::cb_undo(Fl_Widget* o, void* v)
{
	whoami(o)->m_paintView->undo();
}

void ImpressionistUI::cb_space(Fl_Widget* o, void* v) {
	((ImpressionistUI*)(o->user_data()))->m_nSpacing = int(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_sizeRand(Fl_Widget* o, void* v) {
	ImpressionistUI* pUI = ((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc* pDoc = pUI->getDocument();

	if (pDoc->sizeRand == TRUE) pDoc->sizeRand = FALSE;
	else pDoc->sizeRand = TRUE;
}

void ImpressionistUI::cb_autoPaint(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI = ((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc* pDoc = pUI->getDocument();
	pDoc->spacing = pUI->m_nSpacing;
	pDoc->autoPaint();
}

void ImpressionistUI::cb_anotherGradient(Fl_Widget* o, void* v) {
	ImpressionistUI* pUI = ((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc* pDoc = pUI->getDocument();

	if (pDoc->ableAnotherGradient == TRUE) pDoc->ableAnotherGradient = FALSE;
	else pDoc->ableAnotherGradient = TRUE;

	if (pDoc->ableAnotherGradient&&pDoc->m_GPainting==nullptr) {
		char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName());
		if (newfile != NULL) {
			pDoc->loadGradientImage(newfile);
		}
	}
	
}

void ImpressionistUI::cb_edgeClip(Fl_Widget* o, void* v) {
	ImpressionistUI* pUI = ((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc* pDoc = pUI->getDocument();

	if (pDoc->enableEdgeClip == TRUE) pDoc->enableEdgeClip = FALSE;
	else pDoc->enableEdgeClip = TRUE;

	if (pDoc->enableEdgeClip&&pDoc->m_EPainting==nullptr) {
		char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName());
		if (newfile != NULL) {
			pDoc->loadEdgeImage(newfile);
		}
	}

}

void ImpressionistUI::cb_edgeThreshold(Fl_Widget* o, void* v) {
	((ImpressionistUI*)(o->user_data()))->edgeThreshold = int(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_findEdge(Fl_Widget* o, void* v) {
	auto* ui = whoami(o);
	ui->m_pDoc->enableFindEdge = TRUE;
	ui->m_pDoc->m_EPaintWidth = ui->m_pDoc->m_nWidth;
	ui->m_pDoc->m_EPaintHeight = ui->m_pDoc->m_nHeight;
	ui->m_pDoc->G = new float[ui->m_pDoc->m_EPaintWidth * ui->m_pDoc->m_EPaintHeight * 3];
	delete ui->customFilter;
	ui->customFilter = new CustomFilter(ui->getDocument(), 3);
	ui->m_pDoc->smooth = nullptr;
	
	float* temp = new float[9]{ 1.0f/16.0f,2.0f/16.0f,1.0f/16.0f,2.0f/16.0f,4.0f/16.0f,2.0f/16.0f,1.0f/16.0f,2.0f/16.0f,1.0f/16.0f };//gaussian fliter 
	ui->customFilter->parseWeights(temp);
	ui->customFilter->applyFilter();
	ui->m_pDoc->smooth = ui->m_pDoc->m_EPainting;
	ui->m_pDoc->m_EPainting = nullptr;
	
	delete[] temp;
	
	ui->m_pDoc->drawGmaps = TRUE;
	temp = new float[9]{ -1.0f,0,1.0f,-2.0f,0,2.0f,-1.0f,0,1.0f };//Sx for Sobel operator
	ui->customFilter->parseWeights(temp);
	ui->customFilter->applyFilter();
	float* Gx = ui->m_pDoc->G;
	ui->m_pDoc->G = nullptr;
	ui->m_pDoc->G = new float[ui->m_pDoc->m_EPaintWidth * ui->m_pDoc->m_EPaintHeight * 3];

	delete[] temp;
	temp = new float[9]{1.0f,2.0f,1.0f,0,0,0,-1.0f,-2.0f,-1.0f};//Sy for Sobel operator
	ui->customFilter->parseWeights(temp);
	ui->customFilter->applyFilter();
	float* Gy = ui->m_pDoc->G;
	ui->m_pDoc->G = nullptr;

	ui->m_pDoc->constructEdgeImage(Gx, Gy);
	delete[] Gx;
	delete[] Gy;
	delete[] temp;
	delete[] ui->m_pDoc->smooth;
	ui->m_pDoc->smooth = nullptr;
	ui->m_pDoc->drawGmaps = FALSE;
	ui->m_pDoc->enableFindEdge = FALSE;
}

void ImpressionistUI::cb_displayOri(Fl_Menu_* o, void* v) {
	auto* ui = whoami(o);
	if (ui->m_pDoc->m_ucBitmap == nullptr) return;
	ui->m_origView->setDisplay(0);
	ui->m_origView->refresh();
}

void ImpressionistUI::cb_displayEdge(Fl_Menu_* o, void* v) {
	auto* ui = whoami(o);
	if (ui->m_pDoc->m_EPainting == nullptr) return;
	ui->m_origView->setDisplay(1);
	ui->m_origView->refresh();
}

void ImpressionistUI::cb_displayGra(Fl_Menu_* o, void* v) {
	auto* ui = whoami(o);
	if (ui->m_pDoc->m_GPainting == nullptr) return;
	ui->m_origView->setDisplay(2);
	ui->m_origView->refresh();
}

void ImpressionistUI::cb_colors(Fl_Menu_* o, void* v)
{
	whoami(o)->m_colorDialog->show();
}

void ImpressionistUI::cb_colorChooser(Fl_Widget* o, void* v)
{
	double* rgbScale = whoami(o)->getRgbScale();
	Fl_Color_Chooser* chooser = (Fl_Color_Chooser*)o;
	rgbScale[0] = chooser->r();
	rgbScale[1] = chooser->g();
	rgbScale[2] = chooser->b();
}

void ImpressionistUI::cb_dissolve(Fl_Menu_* o, void* v)
{
	
	ImpressionistDoc *pDoc=whoami(o)->getDocument();
	
	char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName() );
	if (newfile != NULL) {
		pDoc->dissolveImage(newfile);
	}
	
}

void ImpressionistUI::cb_setDissolve(Fl_Menu_* o, void* v) {
	whoami(o)->m_dissolveSet->show();
}

void ImpressionistUI::cb_dissolveAlpha(Fl_Widget* o, void* v) {
	((ImpressionistUI*)(o->user_data()))->m_dissolveAlpha = double(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_newMural(Fl_Menu_* o, void* v)
{
	ImpressionistUI* ui = whoami(o);
	ImpressionistDoc *pDoc=whoami(o)->getDocument();

	char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName() );
	if (newfile != NULL) {
		// double alpha = ui->m_dissolveAlpha;
		// ui->m_dissolveAlpha = 1.0;
		pDoc->loadMuralImage(newfile);
		// ui->m_dissolveAlpha = alpha;
	}
}

void ImpressionistUI::cb_loadAlphaMap(Fl_Menu_* o, void* v)
{
	ImpressionistDoc *pDoc=whoami(o)->getDocument();

	char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName() );
	if (newfile != NULL) {
		pDoc->loadAlphaMap(newfile);
	}
}

void ImpressionistUI::cb_loadGradientMap(Fl_Menu_* o, void* v)
{
	ImpressionistDoc* pDoc = whoami(o)->getDocument();

	char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName());
	if (newfile != NULL) {
		pDoc->loadGradientImage(newfile);
	}
}

void ImpressionistUI::cb_loadEdgeMap(Fl_Menu_* o, void* v)
{
	ImpressionistDoc* pDoc = whoami(o)->getDocument();

	char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName());
	if (newfile != NULL) {
		pDoc->loadEdgeImage(newfile);
	}
}

void ImpressionistUI::cb_painterly(Fl_Menu_* o, void* v)
{
	whoami(o)->m_painterly->show();
}

void ImpressionistUI::cb_painterlyPresets(Fl_Widget* o, void* v)
{
	ImpressionistUI* ui = whoami(o);
	Painterly* painterly = (ui->painterly);
	Preset preset = static_cast<Preset>(int(v));
	painterly->applyPreset(preset);

	ui->pThreshold = painterly->threshold;
	ui->pLayer = painterly->layer;
	ui->pMinSize = painterly->brushMinSize;
	ui->pCurvature = painterly->curvature;
	ui->pOpacity = painterly->alpha;
	ui->pBlur = painterly->blurFactor;
	ui->pMinLen = painterly->minLen;
	ui->pMaxLen = painterly->maxLen;
	ui->pGridSize = painterly->gridSizeFactor;
	ui->pJitterR = painterly->jitterR;
	ui->pJitterG = painterly->jitterG;
	ui->pJitterB = painterly->jitterB;
	ui->pJitterH = painterly->jitterH;
	ui->pJitterS = painterly->jitterS;
	ui->pJitterV = painterly->jitterV;
}

void ImpressionistUI::cb_runPainterly(Fl_Widget* o, void* v)
{
	auto* ui = whoami(o);
	auto* painterly = (ui->painterly);

	painterly->threshold = ui->pThreshold;
	painterly->layer = ui->pLayer;
	painterly->brushMinSize = ui->pMinSize;
	painterly->curvature = ui->pCurvature;
	painterly->alpha = ui->pOpacity;
	painterly->blurFactor = ui->pBlur;
	painterly->minLen = ui->pMinLen;
	painterly->maxLen = ui->pMaxLen;
	painterly->gridSizeFactor = ui->pGridSize;
	painterly->jitterR = ui->pJitterR;
	painterly->jitterG = ui->pJitterG;
	painterly->jitterB = ui->pJitterB;
	painterly->jitterH = ui->pJitterH;
	painterly->jitterS = ui->pJitterS;
	painterly->jitterV = ui->pJitterV;
	
	ui->painterly->paint();
	ui->m_paintView->refresh();
}


//---------------------------------- per instance functions --------------------------------------

//------------------------------------------------
// Return the ImpressionistDoc used
//------------------------------------------------
ImpressionistDoc* ImpressionistUI::getDocument()
{
	return m_pDoc;
}

//------------------------------------------------
// Draw the main window
//------------------------------------------------
void ImpressionistUI::show() {
	m_mainWindow->show();
	m_paintView->show();
	m_origView->show();
}

//------------------------------------------------
// Change the paint and original window sizes to 
// w by h
//------------------------------------------------
void ImpressionistUI::resize_windows(int w, int h) {
	m_paintView->size(w,h);
	m_origView->size(w,h);
}

//------------------------------------------------ 
// Set the ImpressionistDoc used by the UI to 
// communicate with the brushes 
//------------------------------------------------
void ImpressionistUI::setDocument(ImpressionistDoc* doc)
{
	m_pDoc = doc;

	m_origView->m_pDoc = doc;
	m_paintView->m_pDoc = doc;

	if (painterly)	delete painterly;
	painterly = new Painterly(doc);
}

//------------------------------------------------
// Return the brush size
//------------------------------------------------
int ImpressionistUI::getSize()
{
	return m_nSize;
}

//-------------------------------------------------
// Set the brush size
//-------------------------------------------------
void ImpressionistUI::setSize( int size )
{
	m_nSize=size;

	if (size<=40) 
		m_BrushSizeSlider->value(m_nSize);
}

// Get and set width
int ImpressionistUI::getWidth()
{
	return m_nWidth;
}

void ImpressionistUI::setWidth(int width)
{
	m_nWidth = width;

	if (width <= 40)	
		m_BrushWidthSlider->value(m_nWidth);
}

// Get and set angle
int ImpressionistUI::getAngle()
{
	return m_nAngle;
}

void ImpressionistUI::setAngle(int angle)
{
	m_nAngle = angle;

	if (angle <= 359)
		m_BrushAngleSlider->value(m_nAngle);
}

double ImpressionistUI::getAlpha()
{
	return m_nAlpha;
}

void ImpressionistUI::setAlpha(double alpha)
{
	m_nAlpha = alpha;
	if (alpha <= 1)
		m_BrushAlphaSlider->value(m_nAlpha);
}

double* ImpressionistUI::getRgbScale()
{
	return m_rgbScale;
}

void ImpressionistUI::setRgbScale(float rscale,float gscale, float bscale)
{
	m_rgbScale[0] = rscale;
	m_rgbScale[1] = gscale;
	m_rgbScale[2] = bscale;
}

double ImpressionistUI::getTransparency()
{
	return m_transparency;
}

double ImpressionistUI::getDissolveAlpha()
{
	return m_dissolveAlpha;
}

// Main menu definition
Fl_Menu_Item ImpressionistUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Image...",	FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_load_image },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)ImpressionistUI::cb_save_image },
		{ "&Brushes...",	FL_ALT + 'b', (Fl_Callback *)ImpressionistUI::cb_brushes }, 
		{ "&Clear Canvas", FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_clear_canvas, 0, FL_MENU_DIVIDER },

		{"&Colors", FL_ALT+'r', (Fl_Callback *)ImpressionistUI::cb_colors, nullptr, FL_MENU_DIVIDER},
		{"&Dissolve", FL_ALT+'d', (Fl_Callback *)ImpressionistUI::cb_dissolve},
		{"&Dissolve Setting", FL_ALT + 'd', (Fl_Callback*)ImpressionistUI::cb_setDissolve},
		{"&New Mural Image", FL_ALT+'n', (Fl_Callback *)ImpressionistUI::cb_newMural},
		{"&Custom Filter", 0, (Fl_Callback *)ImpressionistUI::cb_showCustomFilter},
		{"&Load Alpha Map", 0, (Fl_Callback *)ImpressionistUI::cb_loadAlphaMap, nullptr, FL_MENU_DIVIDER},

		{"&Load Gradient Map", 0,(Fl_Callback*)ImpressionistUI::cb_loadGradientMap},
		{"&Load Edge Map", 0,(Fl_Callback*)ImpressionistUI::cb_loadEdgeMap},

		{"Mosaic", 0, (Fl_Callback *)ImpressionistUI::cb_showMosaicDialog},
		{"&Painterly", FL_ALT+'p', (Fl_Callback *)ImpressionistUI::cb_painterly},
		{"&Matting", FL_ALT+'m', (Fl_Callback *)ImpressionistUI::cb_showMattingDialog, nullptr, FL_MENU_DIVIDER},
	
		{ "&Quit",			FL_ALT + 'q', (Fl_Callback *)ImpressionistUI::cb_exit },
		{ 0 },
	{"&Display",0, 0, 0, FL_SUBMENU},
		{"&Qriginal Image",FL_ALT+'o',(Fl_Callback*)ImpressionistUI::cb_displayOri},
		{"&Edge Image",FL_ALT + 'e',(Fl_Callback*)ImpressionistUI::cb_displayEdge},
		{"&Gradient Image",FL_ALT + 'g',(Fl_Callback*)ImpressionistUI::cb_displayGra},
		{0},

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)ImpressionistUI::cb_about },
		{ 0 },

	{ 0 }
};

// Brush choice menu definition
Fl_Menu_Item ImpressionistUI::brushTypeMenu[NUM_BRUSH_TYPE+1] = {
  {"Points",			FL_ALT+'p', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_POINTS},
  {"Lines",				FL_ALT+'l', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_LINES},
  {"Circles",			FL_ALT+'c', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_CIRCLES},
  {"Scattered Points",	FL_ALT+'q', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_POINTS},
  {"Scattered Lines",	FL_ALT+'m', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_LINES},
  {"Scattered Circles",	FL_ALT+'d', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_CIRCLES},
	{"Sharpening", FL_ALT+'s', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SHARPENING},
	{"Alpha Map", FL_ALT+'a', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_ALPHA_MAP},
	{"Warp Brush", FL_ALT+'w', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_WARP},
  {0}
};

Fl_Menu_Item ImpressionistUI::strokeDirectionMenu[4] =
{
    {"Slider/Right Mouse", FL_ALT+'s', (Fl_Callback *)ImpressionistUI::cb_strokeChoice, (void *)StrokeDirection::SLIDER},
	{"Gradient", FL_ALT+'g', (Fl_Callback *)ImpressionistUI::cb_strokeChoice, (void *)StrokeDirection::GRADIENT},
	{"Brush Direction", FL_ALT+'b', (Fl_Callback *)ImpressionistUI::cb_strokeChoice, (void *)StrokeDirection::BRUSH_DIR},
	{0}
};

Fl_Menu_Item ImpressionistUI::painterlyPresets[5] =
{
	{"Impressionist", 0, (Fl_Callback*)ImpressionistUI::cb_painterlyPresets, (void*)Preset::IMPRESSIONIST},
	{"Expressionist", 0, (Fl_Callback*)ImpressionistUI::cb_painterlyPresets, (void*)Preset::EXPRESSIONIST},
	{"Colorist Wash", 0, (Fl_Callback*)ImpressionistUI::cb_painterlyPresets, (void*)Preset::COLORIST_WASH},
	{"Pointillist", 0, (Fl_Callback*)ImpressionistUI::cb_painterlyPresets, (void*)Preset::POINTILLIST},
	{0}
};


//----------------------------------------------------
// Constructor.  Creates all of the widgets.
// Add new widgets here
//----------------------------------------------------
ImpressionistUI::ImpressionistUI() {
	
	// Create the main window
	m_mainWindow = new Fl_Window(600, 300, "Impressionist");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 600, 25);
		m_menubar->menu(menuitems);

		// Create a group that will hold two sub windows inside the main
		// window
		Fl_Group* group = new Fl_Group(0, 25, 600, 275);

			// install paint view window
			m_paintView = new PaintView(300, 25, 300, 275, "This is the paint view");//0jon
			m_paintView->box(FL_DOWN_FRAME);

			// install original view window
			m_origView = new OriginalView(0, 25, 300, 275, "This is the orig view");//300jon
			m_origView->box(FL_DOWN_FRAME);
			m_origView->deactivate();

		group->end();
		Fl_Group::current()->resizable(group);
    m_mainWindow->end();

	// init values

	m_nSize = 10;
	m_nWidth = 1;
	m_nAngle = 0;
	m_nAlpha = 1.0;
	m_nSpacing = 1;
	

	// brush dialog definition
	m_brushDialog = new Fl_Window(400, 440, "Brush Dialog");
		// Add a brush type choice to the dialog
		m_BrushTypeChoice = new Fl_Choice(50,10,150,25,"&Brush");
		m_BrushTypeChoice->user_data((void*)(this));	// record self to be used by static callback functions
		m_BrushTypeChoice->menu(brushTypeMenu);
		m_BrushTypeChoice->callback(cb_brushChoice);

		m_StrokeDirectionChoice = new Fl_Choice(113, 45, 150, 25, "&Stroke Direction");
		m_StrokeDirectionChoice->user_data((void*)(this));
		m_StrokeDirectionChoice->menu(strokeDirectionMenu);
		m_StrokeDirectionChoice->callback(cb_strokeChoice);
		m_StrokeDirectionChoice->deactivate();

		m_ClearCanvasButton = new Fl_Button(240,10,150,25,"&Clear Canvas");
		m_ClearCanvasButton->user_data((void*)(this));
		m_ClearCanvasButton->callback(cb_clear_canvas_button);


		// Add brush size slider to the dialog 
		m_BrushSizeSlider = new Fl_Value_Slider(10, 80, 300, 20, "Size");
		m_BrushSizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_BrushSizeSlider->type(FL_HOR_NICE_SLIDER);
        m_BrushSizeSlider->labelfont(FL_COURIER);
        m_BrushSizeSlider->labelsize(12);
		m_BrushSizeSlider->minimum(1);
		m_BrushSizeSlider->maximum(40);
		m_BrushSizeSlider->step(1);
		m_BrushSizeSlider->value(m_nSize);
		m_BrushSizeSlider->align(FL_ALIGN_RIGHT);
		m_BrushSizeSlider->callback(cb_sizeSlides);

		// Add brush line width slider to the dialog 
		m_BrushWidthSlider = new Fl_Value_Slider(10, 120, 300, 20, "Line Width");
		m_BrushWidthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_BrushWidthSlider->type(FL_HOR_NICE_SLIDER);
        m_BrushWidthSlider->labelfont(FL_COURIER);
        m_BrushWidthSlider->labelsize(12);
		m_BrushWidthSlider->minimum(1);
		m_BrushWidthSlider->maximum(40);
		m_BrushWidthSlider->step(1);
		m_BrushWidthSlider->value(m_nWidth);
		m_BrushWidthSlider->align(FL_ALIGN_RIGHT);
		m_BrushWidthSlider->callback(cb_widthSlides);
		m_BrushWidthSlider->deactivate();

		// Add brush line angle slider to the dialog 
		m_BrushAngleSlider = new Fl_Value_Slider(10, 160, 300, 20, "Line Angle");
		m_BrushAngleSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_BrushAngleSlider->type(FL_HOR_NICE_SLIDER);
        m_BrushAngleSlider->labelfont(FL_COURIER);
        m_BrushAngleSlider->labelsize(12);
		m_BrushAngleSlider->minimum(0);
		m_BrushAngleSlider->maximum(359);
		m_BrushAngleSlider->step(1);
		m_BrushAngleSlider->value(m_nAngle);
		m_BrushAngleSlider->align(FL_ALIGN_RIGHT);
		m_BrushAngleSlider->callback(cb_angleSlides);
		m_BrushAngleSlider->deactivate();

		// Add brush line alpha slider to the dialog 
		m_BrushAlphaSlider = new Fl_Value_Slider(10, 200, 300, 20, "Alpha");
		m_BrushAlphaSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_BrushAlphaSlider->type(FL_HOR_NICE_SLIDER);
        m_BrushAlphaSlider->labelfont(FL_COURIER);
        m_BrushAlphaSlider->labelsize(12);
		m_BrushAlphaSlider->minimum(0.0);
		m_BrushAlphaSlider->maximum(1.0);
		m_BrushAlphaSlider->step(0.01);
		m_BrushAlphaSlider->value(m_nAlpha);
		m_BrushAlphaSlider->align(FL_ALIGN_RIGHT);
		m_BrushAlphaSlider->callback(cb_alphaSlides);

		m_transparencySlider = new Fl_Value_Slider(10, 280, 150, 20, "Transparency");
		m_transparencySlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_transparencySlider->type(FL_HOR_NICE_SLIDER);
        m_transparencySlider->labelfont(FL_COURIER);
        m_transparencySlider->labelsize(12);
		m_transparencySlider->minimum(0.0);
		m_transparencySlider->maximum(0.5);
		m_transparencySlider->step(0.01);
		m_transparencySlider->value(m_transparency);
		m_transparencySlider->align(FL_ALIGN_RIGHT);
		m_transparencySlider->callback(cb_transSlides);

		m_SwapCanvasAndOrigin = new Fl_Button(10, 240, 175, 25, "Swap Origin and Canvas");
		m_SwapCanvasAndOrigin->user_data((void*)(this));
		m_SwapCanvasAndOrigin->callback(cb_swapCanvasAndOrigin);

		m_undo = new Fl_Button(200, 240, 40, 25, "Undo");
		m_undo->user_data((void*)this);
		m_undo->callback(cb_undo);

		m_anotherGradient = new Fl_Light_Button(200, 320, 150, 20, "Another Gradient");
		m_anotherGradient->user_data((void*)(this));   // record self to be used by static callback functions
		m_anotherGradient->callback(cb_anotherGradient);
		m_anotherGradient->deactivate();

		m_edgeClip = new Fl_Light_Button(10, 320, 140, 20, "Edge Clipping");
		m_edgeClip->user_data((void*)(this));   // record self to be used by static callback functions
		m_edgeClip->callback(cb_edgeClip);
		m_edgeClip->deactivate();

		m_spacingSlider = new Fl_Value_Slider(10, 360, 150, 20, "Spacing");
		m_spacingSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_spacingSlider->type(FL_HOR_NICE_SLIDER);
		m_spacingSlider->labelfont(FL_COURIER);
		m_spacingSlider->labelsize(12);
		m_spacingSlider->minimum(1);
		m_spacingSlider->maximum(16);
		m_spacingSlider->step(1);
		m_spacingSlider->value(m_nSpacing);
		m_spacingSlider->align(FL_ALIGN_RIGHT);
		m_spacingSlider->callback(cb_space);

		m_sizeRand = new Fl_Light_Button(220, 360, 90, 20, "Size Rand");
		m_sizeRand->user_data((void*)(this));   // record self to be used by static callback functions
		m_sizeRand->callback(cb_sizeRand);

		m_autoPaint = new Fl_Button(330, 360, 50, 20, "Paint");
		m_autoPaint->user_data((void*)this);
		m_autoPaint->callback(cb_autoPaint);

		m_edgeThresholdSlider = makeSlider(10, 400, 150, 20, "edge Threshold", 0, 500, 1);
		m_edgeThresholdSlider->value(edgeThreshold);
		m_edgeThresholdSlider->callback(cb_edgeThreshold);

		m_findEdge = new Fl_Button(300, 400, 80, 20, "Find Edge");
		m_findEdge->user_data((void*)this);
		m_findEdge->callback(cb_findEdge);

    m_brushDialog->end();

	//dissolveSet
	m_dissolveSet = new Fl_Window(300, 40, "Dissolve Set");

	m_alphaDissolve = makeSlider(10, 10, 150, 20, "dissolve Alpha", 0.0, 1.0, 0.01);
	m_alphaDissolve->value(m_dissolveAlpha);
	m_alphaDissolve->callback(cb_dissolveAlpha);

	m_dissolveSet->end();


	// Color chooser for color blending
	m_colorDialog = new Fl_Window(200, 240, "Colors");
	
	m_colorChooser = new Fl_Color_Chooser(0, 0, 200, 240, "Color Blending");
	m_colorChooser->rgb(1.0, 1.0, 1.0);
	m_colorChooser->user_data((void*)this);
	m_colorChooser->callback(cb_colorChooser);
	
	m_colorDialog->end();

	// Painterly
	m_painterly = new Fl_Window(640, 360, "Painterly");

	m_pPresetChoice = new Fl_Choice(80,10,150,25,"Presets");
	m_pPresetChoice->user_data((void*)(this));	// record self to be used by static callback functions
	m_pPresetChoice->menu(painterlyPresets);
	m_pPresetChoice->callback(cb_painterlyPresets);

	
	m_pBrushChoice = new Fl_Choice(300, 10, 200, 25, "Stroke");
	m_pBrushChoice->user_data((void*)(this));
	m_pBrushChoice->menu(strokeDirectionMenu);
	m_pBrushChoice->callback(cb_strokeChoice);
	m_pBrushChoice->deactivate();
	

	runPainterly = new Fl_Button(540,10,50,25,"Run");
	runPainterly->user_data((void*)(this));
	runPainterly->callback(cb_runPainterly);

	// Painterly sliders
	p_thresholdSlider = makeSlider(10, 40, 200, 20, "Threshold", 0, 300, 3);
	p_thresholdSlider->value(pThreshold);
	p_thresholdSlider->callback(cb_pThreshold);

	p_layerSlider = makeSlider(10, 70, 200, 20, "Layer", 1, 5, 1);
	p_layerSlider->value(pLayer);
	p_layerSlider->callback(cb_pLayer);

	p_minSizeSlider = makeSlider(10, 100, 200, 20, "Min Radius", 1, 4, 1);
	p_minSizeSlider->value(pMinSize);
	p_minSizeSlider->callback(cb_pMinSize);

	p_curvatureSlider = makeSlider(10, 130, 200, 20, "Curvature", 0, 1, 0.01);
	p_curvatureSlider->value(pCurvature);
	p_curvatureSlider->callback(cb_pCurvature);

	p_opacitySlider = makeSlider(10, 160, 200, 20, "Opacity", 0, 1, 0.01);
	p_opacitySlider->value(pOpacity);
	p_opacitySlider->callback(cb_pOpacity);

	p_minLenSlider = makeSlider(10, 190, 200, 20, "Min Stroke Len", 1, 32, 1);
	p_minLenSlider->value(pMinLen);
	p_minLenSlider->callback(cb_pMinLen);

	p_maxLenSlider = makeSlider(10, 220, 200, 20, "Max Stroke Len", 1, 32, 1);
	p_maxLenSlider->value(pMaxLen);
	p_maxLenSlider->callback(cb_pMaxLen);

	p_gridSizeSlider = makeSlider(10, 250, 200, 20, "Grid Size", 0, 1, 0.01);
	p_gridSizeSlider->value(pGridSize);
	p_gridSizeSlider->callback(cb_pGridSize);

	p_BlurSlider = makeSlider(10, 280, 200, 20, "Blur", 0, 1, 0.01);
	p_BlurSlider->value(pBlur);
	p_BlurSlider->callback(cb_pBlur);

	p_jitterRSlider = makeSlider(330, 40, 200, 20, "Jitter R", 0, 1, 0.01);
	p_jitterRSlider->value(pJitterR);
	p_jitterRSlider->callback(cb_pJitterR);

	p_jitterGSlider = makeSlider(330, 70, 200, 20, "Jitter G", 0, 1, 0.01);
	p_jitterGSlider->value(pJitterG);
	p_jitterGSlider->callback(cb_pJitterG);

	p_jitterBSlider = makeSlider(330, 100, 200, 20, "Jitter B", 0, 1, 0.01);
	p_jitterBSlider->value(pJitterB);
	p_jitterBSlider->callback(cb_pJitterB);

	p_jitterHSlider = makeSlider(330, 130, 200, 20, "Jitter H", 0, 1, 0.01);
	p_jitterHSlider->value(pJitterH);
	p_jitterHSlider->callback(cb_pJitterH);

	p_jitterSSlider = makeSlider(330, 160, 200, 20, "Jitter S", 0, 1, 0.01);
	p_jitterSSlider->value(pJitterS);
	p_jitterSSlider->callback(cb_pJitterS);

	p_jitterVSlider = makeSlider(330, 190, 200, 20, "Jitter V", 0, 1, 0.01);
	p_jitterVSlider->value(pJitterV);
	p_jitterVSlider->callback(cb_pJitterV);

	m_painterly->end();


	// Bayesian matting
	mattingDialog = new Fl_Window(500, 300, "Matting");
	mattingDialog->user_data(this);

	fgBrush = new Fl_Button(30, 100, 150, 25, "Annotate Foreground");
	fgBrush->user_data(this);
	fgBrush->callback(cb_fgBrush);

	bgBrush = new Fl_Button(200, 100, 150, 25, "Annotate Background");
	bgBrush->user_data(this);
	bgBrush->callback(cb_bgBrush);

	runBayessianMatting = new Fl_Button(30, 190, 150, 25, "Run Bayesian Matting");
	runBayessianMatting->user_data(this);
	runBayessianMatting->callback(cb_runBayessianMatting);

	runKnnMatting = new Fl_Button(30, 225, 150, 25, "Run KNN Matting");
	runKnnMatting->user_data(this);
	runKnnMatting->callback(cb_runKnnMatting);

	mattingProgress = new Fl_Progress(200, 190, 280, 25, "Progress");
	mattingProgress->maximum(100.f);
	mattingProgress->minimum(0.f);
	mattingProgress->value(0.f);

	mattingDialog->end();

	// Custom filter
	filterDialog = new Fl_Window(600, 400, "Custom Filter");

	filterInput = new Fl_Multiline_Input(20, 70, 270, 250, "Filter Weights");
	filterInput->value("1\t2\t1\n0\t0\t0\n-1\t-2\t-1");
	filterInput->align(FL_ALIGN_TOP);

	filterView = new Fl_Multiline_Output(310, 70, 270, 250, "Weights Preview");
	filterView->align(FL_ALIGN_TOP);
	
	filterNormalize = new Fl_Button(140, 340, 100, 25, "Normalize");
	filterNormalize->user_data(this);
	filterNormalize->callback(cb_filterNormalize);
	
	applyFilter = new Fl_Button(260, 340, 100, 25, "Apply");
	applyFilter->user_data(this);
	applyFilter->callback(cb_applyFilter);

	setFilter = new Fl_Button(20, 340, 100, 25, "Set Weights");
	setFilter->user_data(this);
	setFilter->callback(cb_setFilter);
	
	filterSizeInput = new Fl_Value_Input(100, 15, 100, 25, "Filter Size");
	filterSizeInput->value(3);
	filterSizeInput->minimum(1);
	filterSizeInput->step(1);

	filterDialog->end();
	
	// Mosaic
	mosaicDialog = new Fl_Window(320, 250, "Mosaic");

	loadThumbnails = new Fl_Button(30, 30, 120, 25, "Load Thumbnails");
	loadThumbnails->user_data(this);
	loadThumbnails->callback(cb_loadThumbnails);

	buildMosaic = new Fl_Button(170, 30, 120, 25, "Build Mosaic");
	buildMosaic->user_data(this);
	buildMosaic->callback(cb_buildMosaic);

	mosaicDialog->end();
}

void ImpressionistUI::cb_pThreshold(Fl_Widget* o, void* v)
{
	whoami(o)->pThreshold = int( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_pLayer(Fl_Widget* o, void* v)
{
	whoami(o)->pLayer = int( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_pMinSize(Fl_Widget* o, void* v)
{
	whoami(o)->pMinSize = int( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_pCurvature(Fl_Widget* o, void* v)
{
	whoami(o)->pCurvature = double( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_pOpacity(Fl_Widget* o, void* v)
{
	whoami(o)->pOpacity = double( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_pMinLen(Fl_Widget* o, void* v)
{
	whoami(o)->pMinLen = int( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_pMaxLen(Fl_Widget* o, void* v)
{
	whoami(o)->pMaxLen = int( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_pGridSize(Fl_Widget* o, void* v)
{
	whoami(o)->pGridSize = double( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_pBlur(Fl_Widget* o, void* v)
{
	whoami(o)->pBlur = double( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_pJitterR(Fl_Widget* o, void* v)
{
	whoami(o)->pJitterR = double( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_pJitterG(Fl_Widget* o, void* v)
{
	whoami(o)->pJitterG = double( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_pJitterB(Fl_Widget* o, void* v)
{
	whoami(o)->pJitterB = double( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_pJitterH(Fl_Widget* o, void* v)
{
	whoami(o)->pJitterH = double( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_pJitterS(Fl_Widget* o, void* v)
{
	whoami(o)->pJitterS = double( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_pJitterV(Fl_Widget* o, void* v)
{
	whoami(o)->pJitterV = double( ((Fl_Slider *)o)->value() );
}

void ImpressionistUI::cb_showMattingDialog(Fl_Menu_* o, void* v)
{
	whoami(o)->mattingDialog->show();
}

void ImpressionistUI::cb_fgBrush(Fl_Widget* o, void* v)
{
	auto* doc = whoami(o)->m_pDoc;
	doc->setBrushType(BRUSH_CIRCLES);
	doc->m_pUI->m_nSize = 25;
	doc->m_pCurrentBrush->useOverrideColor = true;
	
	auto* color = doc->m_pCurrentBrush->overrideColor;
	color[0] = color[1] = color[2] = 255;
}

void ImpressionistUI::cb_bgBrush(Fl_Widget* o, void* v)
{
	auto* doc = whoami(o)->m_pDoc;
	doc->setBrushType(BRUSH_CIRCLES);
	doc->m_pUI->m_nSize = 25;
	doc->m_pCurrentBrush->useOverrideColor = true;
	
	auto* color = doc->m_pCurrentBrush->overrideColor;
	color[0] = color[1] = color[2] = 0;
}

void ImpressionistUI::cb_runBayessianMatting(Fl_Widget* o, void* v)
{
	BayesianMatting predictor(whoami(o)->getDocument());
	predictor.predict();
}

void ImpressionistUI::cb_runKnnMatting(Fl_Widget* o, void* v)
{
	KnnMatting predictor(whoami(o)->getDocument());
	predictor.runMatting(10, 100);
}

void ImpressionistUI::cb_exitMatting(Fl_Window* o, void* v)
{
	auto* ui = (ImpressionistUI*)(o->user_data());
	ui->m_pDoc->m_pCurrentBrush->useOverrideColor = false;
	Fl_Window::default_callback(o, v);
}

void ImpressionistUI::cb_showCustomFilter(Fl_Menu_* o, void* v)
{
	whoami(o)->filterDialog->show();
}

void ImpressionistUI::cb_filterNormalize(Fl_Widget* o, void* v)
{
	auto* ui = whoami(o);
	if (ui->customFilter == nullptr)
		return;
	ui->customFilter->normalize();
	ui->filterView->value(ui->customFilter->getParsedWeights().c_str());
}

void ImpressionistUI::cb_applyFilter(Fl_Widget* o, void* v)
{
	auto* ui = whoami(o);
	if (ui->customFilter == nullptr)
		return;
	ui->customFilter->applyFilter();
	ui->m_paintView->refresh();
}

void ImpressionistUI::cb_setFilter(Fl_Widget* o, void* v)
{
	auto* ui = whoami(o);
	delete ui->customFilter;
	ui->customFilter = new CustomFilter(ui->getDocument(), int(ui->filterSizeInput->value()));
	bool success = ui->customFilter->parseWeights(ui->filterInput->value());
	if (!success)
	{
		ui->filterView->value("Invalid input.");
		return;
	}
	ui->filterView->value(ui->customFilter->getParsedWeights().c_str());
}

void ImpressionistUI::cb_showMosaicDialog(Fl_Menu_* o, void* v)
{
	whoami(o)->mosaicDialog->show();
}

void ImpressionistUI::cb_loadThumbnails(Fl_Widget* o, void* v)
{
	auto* ui = whoami(o);
	if (ui->mosaicMaker == nullptr)
		ui->mosaicMaker = new MosaicMaker(ui->getDocument());

	auto* str = fl_dir_chooser("Choose multiple bmp files", ".", 1);
	if (str == nullptr) return;
	if (!ui->mosaicMaker->loadThumbnails(std::string(str)))
		fl_alert("Failed to load thumbnails");
}

void ImpressionistUI::cb_buildMosaic(Fl_Widget* o, void* v)
{
	auto* ui = whoami(o);
	if (!ui->mosaicMaker->generate(0.7))
		fl_alert("Please load >= 100 thumbnails, samples are under image/thumbnails, only 10x10px size will be accepted");
	ui->m_paintView->refresh();
}

