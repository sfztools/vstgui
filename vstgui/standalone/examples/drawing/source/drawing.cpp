// This file is part of VSTGUI. It is subject to the license terms 
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/vstgui/LICENSE

#include "drawing.h"
#include "drawingarea.h"
#include "vstgui/lib/cframe.h"
#include "vstgui/lib/controls/cbuttons.h"
#include "vstgui/lib/controls/ctextlabel.h"
#include "vstgui/lib/controls/coptionmenu.h"
#include "vstgui/lib/controls/icontrollistener.h"
#include "vstgui/standalone/include/iapplication.h"
#include "vstgui/standalone/include/iuidescwindow.h"
#include "vstgui/standalone/include/helpers/appdelegate.h"
#include "vstgui/standalone/include/helpers/windowlistener.h"

using namespace VSTGUI;
using namespace VSTGUI::Standalone;

class MyApplication : public Application::DelegateAdapter, public WindowListenerAdapter, public VSTGUIEditorInterface, public IControlListener
{
public:
	MyApplication ()
	: Application::DelegateAdapter (
	      {"drawing", "1.0.0", "vstgui.examples.drawing"})
	{
	}

	void finishLaunching () override
	{
		UIDesc::Config config;
		config.uiDescFileName = "Window.uidesc";
		config.viewName = "Window";
		config.windowConfig.title = "Drawing";
		config.windowConfig.autoSaveFrameName = "DrawingWindow";
		config.windowConfig.style.border ().close ().size ().centered ();
		if (auto window = UIDesc::makeWindow (config))
		{
			window->show ();
			window->registerWindowListener (this);

			SharedPointer<CFrame> frame = createFrame (*window);
			window->setContentView (frame);
		}
		else
		{
			IApplication::instance ().quit ();
		}
	}
	void onClosed (const IWindow& window) override
	{
		IApplication::instance ().quit ();
	}

	enum {
		kTagOffset,
		kTagDrawIntegral,
		kTagDrawRotation,
		kTagLineWidth,
	};

	enum {
		kOffsetMinusHalf,
		kOffsetZero,
		kOffsetPlusHalf,
	};

	enum {
		kPathRotation0,
		kPathRotation45,
		kPathRotation90,
	};

	enum {
		kLineWidth0_5,
		kLineWidth1,
		kLineWidth2,
	};

	SharedPointer<CFrame> createFrame (const IWindow& window)
	{
		const CPoint size { 800, 800 };

		SharedPointer<CFrame> frame = makeOwned<CFrame>(
			CRect(0, 0, size.x, size.y),
			this);

		SharedPointer<DrawingArea> area = makeOwned<DrawingArea>(CRect(0, 0, size.x, size.y));
		this->area = area;
		frame->addView(area);

		CCoord labelTop = 50;
		CCoord labelLeft = 500;
		CCoord labelRight = labelLeft + 80;
		CCoord labelHeight = kNormalFont->getSize () + 4;
		CCoord menuLeft = labelRight + 20;
		CCoord menuRight = menuLeft + 80;

		{
			SharedPointer<CTextLabel> label;
			SharedPointer<COptionMenu> menu;

			auto createLabel = [labelTop, labelLeft, labelRight, labelHeight]
				(int row, UTF8StringPtr text) -> SharedPointer<CTextLabel>
			{
				CCoord top = labelTop + row * labelHeight;
				auto label = makeOwned<CTextLabel> (CRect (labelLeft, top, labelRight, top + labelHeight), text);
				label->setFontColor (CColor (0x00, 0x00, 0x00));
				label->setFrameColor (CColor (0x00, 0x00, 0x00, 0x00));
				label->setBackColor (CColor (0xEB, 0xE8, 0xD7));
				return label;
			};

			auto createMenu = [this, labelTop, labelHeight, menuLeft, menuRight]
				(int row, int tag) -> SharedPointer<COptionMenu>
			{
				CCoord top = labelTop + row * labelHeight;
				auto menu = makeOwned<COptionMenu> (CRect (menuLeft, top, menuRight, top + labelHeight), this, tag);
				return menu;
			};

			int row = 0;

			label = createLabel (row, "Offset");
			frame->addView (label);
			menu = createMenu (row, kTagOffset);
			frame->addView (menu);
			menu->addEntry ("-0.5");
			menu->addEntry ("0");
			menu->addEntry ("+0.5");
			menu->setCurrent (1);
			++row;

			label = createLabel (row, "Integral");
			frame->addView (label);
			menu = createMenu (row, kTagDrawIntegral);
			frame->addView (menu);
			menu->addEntry ("off");
			menu->addEntry ("on");
			menu->setCurrent (0);
			++row;

			label = createLabel (row, "Rotation");
			frame->addView (label);
			menu = createMenu (row, kTagDrawRotation);
			frame->addView (menu);
			menu->addEntry ("0");
			menu->addEntry ("45");
			menu->addEntry ("90");
			menu->setCurrent (0);
			++row;

			label = createLabel (row, "Line width");
			frame->addView (label);
			menu = createMenu (row, kTagLineWidth);
			frame->addView (menu);
			menu->addEntry ("0.5");
			menu->addEntry ("1");
			menu->addEntry ("2");
			menu->setCurrent (1);
			++row;
		}

		return frame;
	}

	void valueChanged (CControl* pControl) override
	{
		switch (pControl->getTag ())
		{
		case kTagOffset:
			switch ((int) pControl->getValue())
			{
			case kOffsetMinusHalf:
				area->setDrawingOffset (-0.5);
				break;
			case kOffsetZero:
				area->setDrawingOffset (0.0);
				break;
			case kOffsetPlusHalf:
				area->setDrawingOffset (+0.5);
				break;
			}
			break;
		case kTagDrawIntegral:
			area->setDrawingIntegral (pControl->getValue() != 0);
			break;
		case kTagDrawRotation:
			switch ((int) pControl->getValue()) {
				case kPathRotation0:
					area->setPathRotation (0);
					break;
				case kPathRotation45:
					area->setPathRotation (45);
					break;
				case kPathRotation90:
					area->setPathRotation (90);
					break;
			}
			break;
		case kTagLineWidth:
			switch ((int) pControl->getValue()) {
				case kLineWidth0_5:
					area->setLineWidth (0.5);
					break;
				case kLineWidth1:
					area->setLineWidth (1);
					break;
				case kLineWidth2:
					area->setLineWidth (2);
					break;
			}
			break;
		}
	}

private:
	SharedPointer<DrawingArea> area;
};

static Application::Init gAppDelegate (std::make_unique<MyApplication> ());
