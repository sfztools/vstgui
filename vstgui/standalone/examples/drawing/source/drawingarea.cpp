// This file is part of VSTGUI. It is subject to the license terms 
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/vstgui/LICENSE

#include "drawingarea.h"
#include "vstgui/lib/cdrawcontext.h"
#include "vstgui/lib/cgraphicspath.h"
#include <cmath>

DrawingArea::DrawingArea (const CRect& size)
	: CView (size)
{
}

void DrawingArea::setDrawingOffset (CCoord offset)
{
	if (drawingOffset == offset)
		return;

	drawingOffset = offset;
	invalid ();
}

void DrawingArea::setDrawingIntegral (bool integral)
{
	if (drawingIntegral == integral)
		return;

	drawingIntegral = integral;
	invalid ();
}

void DrawingArea::setPathRotation (CCoord rotation)
{
	if (pathRotation == rotation)
		return;

	pathRotation = rotation;
	invalid ();
}

void DrawingArea::setLineWidth (CCoord width)
{
	if (lineWidth == width)
		return;

	lineWidth = width;
	invalid ();
}

void DrawingArea::draw (CDrawContext *pContext)
{
	const CRect bounds = getViewSize();

	int drawMode = 0;
	if (!drawingIntegral)
		drawMode |= kNonIntegralMode;
	pContext->setDrawMode (drawMode);

	pContext->setLineWidth (lineWidth);

	pContext->setFillColor (bgColor);
	pContext->drawRect (bounds, kDrawFilled);

	drawDotGrid (pContext);

	pContext->setFontColor (textColor);
	pContext->setFrameColor (shapeFrameColor);
	pContext->setFillColor (shapeFillColor);

	int row = 0;

	drawTextInCell (pContext, 0, row, "Shapes");
	drawTextInCell (pContext, 1, row, "Stroked");
	drawTextInCell (pContext, 2, row, "Filled");
	drawTextInCell (pContext, 3, row, "FilledAndStroked");
	++row;

	drawTextInCell (pContext, 0, row, "Square");
	drawSquareExample (pContext, 1, row, rectShapeSize, kDrawStroked);
	drawSquareExample (pContext, 2, row, rectShapeSize, kDrawFilled);
	drawSquareExample (pContext, 3, row, rectShapeSize, kDrawFilledAndStroked);
	++row;

	drawTextInCell (pContext, 0, row, "Lines");
	drawLinesExample (pContext, 1, row, rectShapeSize, kDrawStroked);
	drawLinesExample (pContext, 2, row, rectShapeSize, kDrawFilled);
	drawLinesExample (pContext, 3, row, rectShapeSize, kDrawFilledAndStroked);
	++row;

	++row;

	drawTextInCell (pContext, 0, row, "Paths");
	drawTextInCell (pContext, 1, row, "Stroked");
	drawTextInCell (pContext, 2, row, "Filled");
	drawTextInCell (pContext, 3, row, "FilledEvenOdd");
	++row;

	drawTextInCell (pContext, 0, row, "Square path");
	drawSquarePathExample (pContext, 1, row, rectShapeSize, CDrawContext::kPathStroked);
	drawSquarePathExample (pContext, 2, row, rectShapeSize, CDrawContext::kPathFilled);
	drawSquarePathExample (pContext, 3, row, rectShapeSize, CDrawContext::kPathFilledEvenOdd);
	++row;

	drawTextInCell (pContext, 0, row, "Rounded square");
	drawRoundedRectExample (pContext, 1, row, rectShapeSize, CDrawContext::kPathStroked);
	drawRoundedRectExample (pContext, 2, row, rectShapeSize, CDrawContext::kPathFilled);
	drawRoundedRectExample (pContext, 3, row, rectShapeSize, CDrawContext::kPathFilledEvenOdd);
	++row;

	drawTextInCell (pContext, 0, row, "Lines path");
	drawLinesPathExample (pContext, 1, row, rectShapeSize, CDrawContext::kPathStroked);
	drawLinesPathExample (pContext, 2, row, rectShapeSize, CDrawContext::kPathFilled);
	drawLinesPathExample (pContext, 3, row, rectShapeSize, CDrawContext::kPathFilledEvenOdd);
	++row;
}

void DrawingArea::drawDotGrid (CDrawContext *pContext)
{
	const CRect bounds = getViewSize();

	const int w = (int) std::ceil (bounds.getWidth());
	const int h = (int) std::ceil (bounds.getHeight());

	for (int x = 0; x < w; x += dotSpacing) {
		for (int y = 0; y < h; y += dotSpacing)
			pContext->drawPoint(CPoint(x, y), bgDotColor);
	}
}

void DrawingArea::drawSquareExample (CDrawContext *pContext, int col, int row, CCoord size, CDrawStyle drawStyle)
{
	const CRect bounds = getGridCell (col, row);
	const CRect rect = CRect (0, 0, size, size).centerInside (bounds);
	pContext->drawRect (rect, drawStyle);
}

void DrawingArea::drawLinesExample (CDrawContext *pContext, int col, int row, CCoord size, CDrawStyle drawStyle)
{
	const CRect bounds = getGridCell (col, row);
	const CRect rect = CRect (0, 0, size, size).centerInside (bounds);
	pContext->drawLine (rect.getTopLeft (), rect.getBottomRight ());
	pContext->drawLine (rect.getTopRight (), rect.getBottomLeft ());
}

void DrawingArea::drawSquarePathExample (CDrawContext *pContext, int col, int row, CCoord size, CDrawContext::PathDrawMode drawMode)
{
	const CRect bounds = getGridCell (col, row);
	const CRect rect = CRect (0, 0, size, size).centerInside (bounds);
	auto path = owned (pContext->createGraphicsPath ());
	path->addRect (rect);
	CGraphicsTransform tm = getGridTransform (col, row);
	pContext->drawGraphicsPath (path, drawMode, &tm);
}

void DrawingArea::drawRoundedRectExample (CDrawContext *pContext, int col, int row, CCoord size, CDrawContext::PathDrawMode drawMode)
{
	const CRect bounds = getGridCell (col, row);
	const CRect rect = CRect (0, 0, size, size).centerInside (bounds);
	auto path = owned (pContext->createRoundRectGraphicsPath (rect, roundedRectShapeRadius));
	CGraphicsTransform tm = getGridTransform (col, row);
	pContext->drawGraphicsPath (path, drawMode, &tm);
}

void DrawingArea::drawLinesPathExample (CDrawContext *pContext, int col, int row, CCoord size, CDrawContext::PathDrawMode drawMode)
{
	const CRect bounds = getGridCell (col, row);
	const CRect rect = CRect (0, 0, size, size).centerInside (bounds);
	auto path = owned (pContext->createGraphicsPath ());
	path->beginSubpath (rect.getTopLeft ());
	path->addLine (rect.getBottomRight ());
	path->beginSubpath (rect.getTopRight ());
	path->addLine (rect.getBottomLeft ());
	CGraphicsTransform tm = getGridTransform (col, row);
	pContext->drawGraphicsPath (path, drawMode, &tm);
}

void DrawingArea::drawTextInCell (CDrawContext *pContext, int col, int row, UTF8StringPtr text)
{
	const CRect bounds = getGridCell (col, row);

	const CRect textRect = CRect(0, 0, pContext->getStringWidth (text), pContext->getFont ()->getSize ()).centerInside (bounds);
	const CColor fillColor = pContext->getFillColor ();
	pContext->setFillColor (bgColor);
	pContext->drawRect (textRect, kDrawFilled);
	pContext->setFillColor (fillColor);

	pContext->drawString (text, bounds);
}

CRect DrawingArea::getGridCell (int col, int row) const
{
	CCoord x = col * gridCellSize + drawingOffset;
	CCoord y = row * gridCellSize + drawingOffset;
	return CRect (x, y, x + gridCellSize, y + gridCellSize);
}

CGraphicsTransform DrawingArea::getGridTransform (int col, int row) const
{
	const CRect bounds = getGridCell (col, row);
	CPoint c = bounds.getCenter();
	CGraphicsTransform tm;
	if (pathRotation != 0) {
		tm.translate (-c);
		tm.rotate (pathRotation);
		tm.translate (c);
	}
	return tm;
}
