#include "Canvas.h"
#include <Cocoa/Cocoa.h>
#include <CoreFoundation/CoreFoundation.h>

using std::string;

NSColor * createColor(CGFloat r, CGFloat g, CGFloat b, CGFloat a) {
	CGFloat components[] = {r, g, b, a};
	return [[NSColor 
		colorWithColorSpace: [NSColorSpace genericRGBColorSpace]
		components: components
		count: 4
	] retain];
}


class CanvasImpl {
public:
	CanvasImpl(const char *filename, Canvas::Rect rect);
	~CanvasImpl();

	
	NSGraphicsContext *ctx;
	NSBezierPath *path;
	NSColor *stroke_color;
	NSColor *fill_color;
	NSFont *font;
};


#define POINTS_TO_PIXELS(v) (v)


CanvasImpl :: CanvasImpl(const char *filename, Canvas::Rect rect)
:	ctx(0)
{
    CFStringRef filepath = CFStringCreateWithCString(
		NULL,
		filename,
		kCFStringEncodingUTF8
	);
	
    CFURLRef url = CFURLCreateWithFileSystemPath(
		NULL,
		filepath,
        kCFURLPOSIXPathStyle,
        false
	);
	
    if(url) {
		CGRect cgrect = CGRectMake(rect.x, rect.y, rect.width, rect.height);
		CGContextRef cgctx = CGPDFContextCreateWithURL(
			url,
			&cgrect,
			NULL
		);
		
		if(cgctx) {
			CGContextBeginPage(cgctx, &cgrect);
			ctx = [[NSGraphicsContext graphicsContextWithGraphicsPort:cgctx flipped:YES] retain];
			path = [[NSBezierPath bezierPath] retain];
			stroke_color = createColor(0, 0, 0, 1);
			fill_color = createColor(0, 0, 0, 1);
			font = nil;
		}
		
		CFRelease(url);
	}
	CFRelease(filepath);
}

CanvasImpl :: ~CanvasImpl() {
	if(ctx) {
		CGContextRef cgctx = (CGContextRef)[ctx graphicsPort];
		CGContextEndPage(cgctx);
		CGContextRelease(cgctx);
		
		[stroke_color release];
		[fill_color release];
		[ctx release];
//		[path release];
	}
}

void Canvas :: freeImpl() {
	delete mImpl;
	mImpl = NULL;
}

Canvas * Canvas :: create(const char *filename, Rect rect) {
	CanvasImpl *impl = new CanvasImpl(filename, rect);
	return new Canvas(impl, filename, rect);
}

void Canvas :: close() {
	freeImpl();
}


void Canvas :: enable() {
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:mImpl->ctx];
	
	NSAffineTransform *xform = [NSAffineTransform transform];
	[xform translateXBy:0 yBy:mRect.height];
	[xform scaleXBy:1. yBy:-1];
	[xform set];
}

void Canvas :: disable() {
	[NSGraphicsContext restoreGraphicsState];
}

void Canvas :: clear(float r, float g, float b, float a) {
	if(mImpl) {
		enable();
			NSColor *cc = createColor(r, g, b, a);
			[cc set];
			[[NSBezierPath bezierPathWithRect:
				NSMakeRect(mRect.x, mRect.y, mRect.width, mRect.height)] 
				fill
			];
			[cc release];
		disable();
	}
}

void Canvas :: stroke_color(float r, float g, float b, float a) {
	[mImpl->stroke_color release];
	mImpl->stroke_color = createColor(r, g, b, a);
}

void Canvas :: get_stroke_color(float &r, float &g, float &b, float &a) {
	float color[4];
	[mImpl->stroke_color getComponents:color];
	r = color[0];
	g = color[1];
	b = color[2];
	a = color[3];
}

void Canvas :: get_fill_color(float &r, float &g, float &b, float &a) {
	float color[4];
	[mImpl->fill_color getComponents:color];
	r = color[0];
	g = color[1];
	b = color[2];
	a = color[3];
}

void Canvas :: fill_color(float r, float g, float b, float a) {
	[mImpl->fill_color release];
	mImpl->fill_color = createColor(r, g, b, a);
}

void Canvas :: stroke() {
	if(mImpl) {
		enable();
			[mImpl->path setLineWidth:mLineWidth];
			[mImpl->stroke_color set];
			[mImpl->path stroke];
		disable();
		
		[mImpl->path removeAllPoints];
	}
}

void Canvas :: fill() {
	if(mImpl) {
		enable();
			[mImpl->fill_color set];
			[mImpl->path fill];
		disable();
		
		[mImpl->path removeAllPoints];
	}
}

void Canvas :: move_to(Point p) {
	if(mImpl) {
		[mImpl->path moveToPoint:NSMakePoint(p.x, p.y)];
	}
}

void Canvas :: line_to(Point p) {
	if(mImpl) {
		[mImpl->path lineToPoint:NSMakePoint(p.x, p.y)];
	}
}

void Canvas :: rect(Rect r) {
	if(mImpl) {
		[mImpl->path appendBezierPathWithRect:
			NSMakeRect(r.x, r.y, r.width, r.height)
		];
	}
}

void Canvas :: oval(Rect r) {
	if(mImpl) {
		[mImpl->path appendBezierPathWithOvalInRect:
			NSMakeRect(r.x, r.y, r.width, r.height)
		];
	}
}

void Canvas :: set_font(const char *name, float size) {
	if(mImpl) {
		enable();
		mImpl->font = [NSFont fontWithName:
			[NSString 
				stringWithCString:name 
				encoding:NSASCIIStringEncoding
			]
			size:size
		];
		[mImpl->font set];
		disable();
	}
}

void Canvas :: show_text(Point p, const char *text) {
	if(mImpl && mImpl->font) {
		NSDictionary *attrs = [NSDictionary
			dictionaryWithObjectsAndKeys:
				mImpl->fill_color, NSForegroundColorAttributeName,
				mImpl->font, NSFontAttributeName, 
				nil
		];
	
		enable();
		[[NSString 
			stringWithCString:text 
			encoding:NSASCIIStringEncoding
		] drawAtPoint:NSMakePoint(p.x, p.y) withAttributes:attrs];
		disable();
	}
}

Canvas::Point Canvas :: text_size(const char *text) {
	if(mImpl && mImpl->font) {
		NSDictionary *attrs = [NSDictionary
			dictionaryWithObjectsAndKeys:
				mImpl->fill_color, NSForegroundColorAttributeName,
				mImpl->font, NSFontAttributeName, 
				nil
		];
	
		enable();
		NSSize size = [[NSString 
			stringWithCString:text 
			encoding:NSASCIIStringEncoding
		] sizeWithAttributes:attrs];
		disable();
		
		return Point(size.width, size.height);
	}
	return Point(0, 0);
}