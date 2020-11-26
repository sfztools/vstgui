// This file is part of VSTGUI. It is subject to the license terms 
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/vstgui/LICENSE

#pragma once
#include "vstgui/lib/cview.h"
#include "vstgui/lib/ccolor.h"
#include "vstgui/lib/cdrawcontext.h"
#include "vstgui/lib/cdrawdefs.h"

using namespace VSTGUI;

class DrawingArea : public CView
{
public:
	explicit DrawingArea (const CRect& size);
	void setDrawingOffset (CCoord offset);
	void setDrawingIntegral (bool integral);
	void setPathRotation (CCoord rotation);
	void setLineWidth (CCoord width);

protected:
	void draw (CDrawContext *pContext) override;
	void drawDotGrid (CDrawContext *pContext);

	void drawSquareExample (CDrawContext *pContext, int col, int row, CCoord size, CDrawStyle drawStyle);
	void drawLinesExample (CDrawContext *pContext, int col, int row, CCoord size, CDrawStyle drawStyle);
	void drawSquarePathExample (CDrawContext *pContext, int col, int row, CCoord size, CDrawContext::PathDrawMode drawMode);
	void drawRoundedRectExample (CDrawContext *pContext, int col, int row, CCoord size, CDrawContext::PathDrawMode drawMode);
	void drawLinesPathExample (CDrawContext *pContext, int col, int row, CCoord size, CDrawContext::PathDrawMode drawMode);
	void drawTextInCell (CDrawContext *pContext, int col, int row, UTF8StringPtr text);

	CRect getGridCell (int col, int row) const;
	CGraphicsTransform getGridTransform (int col, int row) const;

private:
	const CColor bgColor { 0xEB, 0xE8, 0xD7 };
	const CColor bgDotColor { 0x00, 0x00, 0x00 };
	const CColor textColor { 0x00, 0x00, 0x00 };
	const CColor shapeFrameColor { 0xEB, 0x76, 0x97 };
	const CColor shapeFillColor { 0x76, 0xBC, 0xEB };
	const int dotSpacing = 5;
	const int gridCellSize = 100;
	const int rectShapeSize = 50;
	const CCoord roundedRectShapeRadius = 10;
	CCoord drawingOffset = 0;
	bool drawingIntegral = false;
	CCoord pathRotation = 0;
	CCoord lineWidth = 1;
};
