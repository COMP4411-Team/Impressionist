//
// impressionistUI.h
//
// The header file for the UI part
//

#ifndef ImpressionistUI_h
#define ImpressionistUI_h

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_File_Chooser.H>		// FLTK file chooser
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Native_File_Chooser.H>

#include "Impressionist.h"
#include "OriginalView.h"
#include "PaintView.h"

#include "ImpBrush.h"
#include "Painterly.h"
#include "CustomFilter.h"
#include "MosaicMaker.h"

class ImpressionistUI {
public:
	ImpressionistUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;
								
	PaintView*			m_paintView;
	OriginalView*		m_origView;

// for brush dialog
	Fl_Window*			m_brushDialog;
	Fl_Choice*			m_BrushTypeChoice;
	Fl_Choice* m_StrokeDirectionChoice;

	Fl_Slider*			m_BrushSizeSlider;
	Fl_Slider* m_BrushWidthSlider;
	Fl_Slider* m_BrushAngleSlider;
	Fl_Slider* m_BrushAlphaSlider;
	Fl_Slider* m_transparencySlider;

	// automatically painting
	Fl_Slider*			m_spacingSlider;
	Fl_Light_Button*	m_sizeRand;
	Fl_Button*			m_autoPaint;

	//Another gradient
	Fl_Light_Button*	m_anotherGradient;

	//Edge clipping
	Fl_Light_Button*	m_edgeClip;

	//Find edge
	Fl_Slider*			m_edgeThresholdSlider;
	Fl_Button*			m_findEdge;

	// Painterly sliders
	Fl_Slider* p_thresholdSlider;
	Fl_Slider* p_layerSlider;
	Fl_Slider* p_minSizeSlider;
	Fl_Slider* p_curvatureSlider;
	Fl_Slider* p_opacitySlider;
	Fl_Slider* p_minLenSlider;
	Fl_Slider* p_maxLenSlider;
	Fl_Slider* p_gridSizeSlider;
	Fl_Slider* p_BlurSlider;

	Fl_Slider* p_jitterRSlider;
	Fl_Slider* p_jitterGSlider;
	Fl_Slider* p_jitterBSlider;
	Fl_Slider* p_jitterHSlider;
	Fl_Slider* p_jitterSSlider;
	Fl_Slider* p_jitterVSlider;

	Fl_Button*          m_ClearCanvasButton;
	Fl_Button* m_SwapCanvasAndOrigin;
	Fl_Button* m_undo;

	// Color chooser
	Fl_Window* m_colorDialog;
	Fl_Color_Chooser* m_colorChooser;
	Fl_Window* m_painterly;

	//window for dissolve transparance setting.
	Fl_Window* m_dissolveSet;
	Fl_Slider* m_alphaDissolve;

	// Painterly
	Fl_Choice* m_pPresetChoice;
	Fl_Choice* m_pBrushChoice;
	Fl_Button* runPainterly;

	// Bayesian matting
	Fl_Window* mattingDialog;
	Fl_Button* clearCanvas;
	Fl_Button* fgBrush;
	Fl_Button* bgBrush;
	Fl_Button* runBayessianMatting;
	Fl_Button* runKnnMatting;
	Fl_Progress* mattingProgress;

	// Custom filter
	Fl_Window* filterDialog;
	Fl_Multiline_Input* filterInput;
	Fl_Multiline_Output* filterView;
	Fl_Button* filterNormalize;
	Fl_Button* applyFilter;
	Fl_Button* setFilter;
	Fl_Value_Input* filterSizeInput;

	// Mosaic
	Fl_Window* mosaicDialog;
	Fl_Button* loadThumbnails;
	Fl_Button* buildMosaic;

	// Member functions
	void				setDocument(ImpressionistDoc* doc);
	ImpressionistDoc*	getDocument();

	void				show();
	void				resize_windows(int w, int h);

	// Interface to get attribute

	int					getSize();
	void				setSize(int size);

	int getWidth();
	void setWidth(int width);

	int getAngle();
	void setAngle(int angle);

	double getAlpha();
	void setAlpha(double alpha);

	double* getRgbScale();
	void setRgbScale(float rscale,float gscale, float bscale);

	double getTransparency();
	double getDissolveAlpha();
	
	// Painterly attributes
	int pThreshold{100};
	int pLayer{3};
	int pMinSize{2};
	double pCurvature{1.0};
	double pOpacity{1.0};
	double pBlur{0.5};
	int pMinLen{4}, pMaxLen{16};
	double pGridSize{1.0};
	double pJitterR{0.0}, pJitterG{0.0}, pJitterB{0.0};
	double pJitterH{0.0}, pJitterS{0.0}, pJitterV{0.0};

	int edgeThreshold{ 200 };

	Painterly* painterly{nullptr};

	CustomFilter* customFilter{nullptr};

	MosaicMaker* mosaicMaker{nullptr};

private:
	ImpressionistDoc*	m_pDoc;		// pointer to document to communicate with the document

	// All attributes here
	int		m_nSize;
	int m_nWidth;
	int m_nAngle;
	double m_nAlpha;
	int		m_nSpacing{ 4 };

	double m_rgbScale[3] = {1.0, 1.0, 1.0};	// for color blending, each between 0 and 1

	double m_transparency{0.3};	// transparency for the background

	double m_dissolveAlpha{0.5};		// alpha for dissolving image

	Fl_Slider* makeSlider(int x, int y, int w, int h, const char* l, double min, double max, double step);
	
	// Static class members
	static Fl_Menu_Item		menuitems[];
	static Fl_Menu_Item		brushTypeMenu[NUM_BRUSH_TYPE+1];
	static Fl_Menu_Item strokeDirectionMenu[4];
	static Fl_Menu_Item painterlyPresets[5];
	static Fl_Menu_Item painterlyBrush[3];

	static ImpressionistUI*	whoami(Fl_Menu_* o);
	static ImpressionistUI* whoami(Fl_Widget* o);

	// All callbacks here.  Callbacks are declared 
	// static
	static void	cb_load_image(Fl_Menu_* o, void* v);
	static void	cb_save_image(Fl_Menu_* o, void* v);
	static void	cb_brushes(Fl_Menu_* o, void* v);
	static void	cb_clear_canvas(Fl_Menu_* o, void* v);
	static void	cb_exit(Fl_Menu_* o, void* v);
	static void	cb_about(Fl_Menu_* o, void* v);
	static void	cb_brushChoice(Fl_Widget* o, void* v);
	static void cb_strokeChoice(Fl_Widget* o, void* v);
	static void	cb_clear_canvas_button(Fl_Widget* o, void* v);
	static void	cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_widthSlides(Fl_Widget* o, void* v);
	static void cb_angleSlides(Fl_Widget* o, void* v);
	static void cb_alphaSlides(Fl_Widget* o, void* v);
	static void cb_transSlides(Fl_Widget* o, void* v);
	static void cb_swapCanvasAndOrigin(Fl_Widget* o, void* v);
	static void cb_undo(Fl_Widget* o, void* v);

	static void	cb_displayOri(Fl_Menu_* o, void* v);
	static void	cb_displayEdge(Fl_Menu_* o, void* v);
	static void	cb_displayGra(Fl_Menu_* o, void* v);

	static void cb_space(Fl_Widget* o, void* v);
	static void cb_sizeRand(Fl_Widget* o, void* v);
	static void cb_autoPaint(Fl_Widget* o, void* v);

	static void cb_loadGradientMap(Fl_Menu_* o, void* v);
	static void cb_anotherGradient(Fl_Widget* o, void* v);

	static void cb_loadEdgeMap(Fl_Menu_* o, void* v);
	static void cb_edgeClip(Fl_Widget* o, void* v);
	
	static void cb_edgeThreshold(Fl_Widget* o, void* v);
	static void cb_findEdge(Fl_Widget* o, void* v);
	
	static void cb_colors(Fl_Menu_* o, void* v);
	static void cb_colorChooser(Fl_Widget* o, void* v);

	static void cb_dissolve(Fl_Menu_* o, void* v);
	static void cb_setDissolve(Fl_Menu_* o, void* v);
	static void cb_dissolveAlpha(Fl_Widget* o, void* v);

	static void cb_newMural(Fl_Menu_* o, void* v);
	static void cb_loadAlphaMap(Fl_Menu_* o, void* v);

	// Painterly
	static void cb_painterly(Fl_Menu_* o, void* v);
	static void cb_painterlyPresets(Fl_Widget* o, void* v);
	static void cb_runPainterly(Fl_Widget* o, void* v);

	static void cb_pThreshold(Fl_Widget* o, void* v);
	static void cb_pLayer(Fl_Widget* o, void* v);
	static void cb_pMinSize(Fl_Widget* o, void* v);
	static void cb_pCurvature(Fl_Widget* o, void* v);
	static void cb_pOpacity(Fl_Widget* o, void* v);
	static void cb_pMinLen(Fl_Widget* o, void* v);
	static void cb_pMaxLen(Fl_Widget* o, void* v);
	static void cb_pGridSize(Fl_Widget* o, void* v);
	static void cb_pBlur(Fl_Widget* o, void* v);
	static void cb_pJitterR(Fl_Widget* o, void* v);
	static void cb_pJitterG(Fl_Widget* o, void* v);
	static void cb_pJitterB(Fl_Widget* o, void* v);
	static void cb_pJitterH(Fl_Widget* o, void* v);
	static void cb_pJitterS(Fl_Widget* o, void* v);
	static void cb_pJitterV(Fl_Widget* o, void* v);

	// Bayesian matting
	static void cb_showMattingDialog(Fl_Menu_* o, void* v);
	static void cb_fgBrush(Fl_Widget* o, void* v);
	static void cb_bgBrush(Fl_Widget* o, void* v);
	static void cb_runBayessianMatting(Fl_Widget* o, void* v);
	static void cb_runKnnMatting(Fl_Widget* o, void* v);
	static void cb_exitMatting(Fl_Window* o, void* v);

	// Custom filter
	static void cb_showCustomFilter(Fl_Menu_* o, void* v);
	static void cb_filterNormalize(Fl_Widget* o, void* v);
	static void cb_applyFilter(Fl_Widget* o, void* v);
	static void cb_setFilter(Fl_Widget* o, void* v);

	// Mosaic
	static void cb_showMosaicDialog(Fl_Menu_* o, void* v);
	static void cb_loadThumbnails(Fl_Widget* o, void* v);
	static void cb_buildMosaic(Fl_Widget* o, void* v);
};

#endif
