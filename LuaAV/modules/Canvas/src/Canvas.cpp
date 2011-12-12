#include "Canvas.h"

Canvas :: Canvas(CanvasImpl *impl, const char *filename, Rect rect) 
:	mImpl(impl),
	mFilename(filename),
	mRect(rect)
{}

Canvas :: ~Canvas() {
	freeImpl();
}