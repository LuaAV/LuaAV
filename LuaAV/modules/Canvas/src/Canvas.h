#ifndef CANVAS_H
#define CANVAS_H 1

#include <string>

using std::string;
class CanvasImpl;

class Canvas {
public:

	struct Point{
		Point(float x, float y)
		:	x(x), y(y)
		{}
		
		float x;
		float y;
	};

	struct Rect{
		Rect(float x, float y, float width, float height)
		:	x(x), y(y), width(width), height(height)
		{}
		
		
		float x;
		float y;
		float width;
		float height;
	};

protected:
	Canvas(CanvasImpl *impl, const char *filename, Rect rect);
	void freeImpl();
	
	void enable();
	void disable();
	
public:	
	~Canvas();
	
	static Canvas * create(const char *filename, Rect rect);
	
	void close();
	
	void clear(float r, float g, float b, float a);
	
	void line_width(float v) {mLineWidth = v;}
	float get_line_width() {return mLineWidth;}
	
	void stroke_color(float r, float g, float b, float a);
	void get_stroke_color(float &r, float &g, float &b, float &a);
	void fill_color(float r, float g, float b, float a);
	void get_fill_color(float &r, float &g, float &b, float &a);

	void color(float r, float g, float b, float a) {
		stroke_color(r, g, b, a);
		fill_color(r, g, b, a);
	}
	
	void stroke();
	void fill();
	void move_to(Point p);
	void line_to(Point p);
	
	void rect(Rect r);
	void oval(Rect r);
	
	void set_font(const char *name, float size);
	void show_text(Point p, const char *text);
	Point text_size(const char *text);
	
protected:
	CanvasImpl *mImpl;
	string mFilename;
	Rect mRect;
	float mLineWidth;
};

#endif // CANVAS_H