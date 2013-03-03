#ifndef AV_H
#define AV_H

#ifdef __cplusplus
#include <stdint.h>
extern "C" {
#endif

void av_sleep(double seconds);
double av_time();

enum {
	// Standard ASCII non-printable characters 
	AV_KEY_ENTER		=3,		
	AV_KEY_BACKSPACE	=8,		
	AV_KEY_TAB			=9,
	AV_KEY_RETURN		=13,
	AV_KEY_ESCAPE		=27,
	AV_KEY_DELETE		=127,
		
	// Non-standard, but common keys
	AV_KEY_F1=256, 
	AV_KEY_F2, AV_KEY_F3, AV_KEY_F4, AV_KEY_F5, AV_KEY_F6, AV_KEY_F7, AV_KEY_F8, AV_KEY_F9, AV_KEY_F10, AV_KEY_F11, AV_KEY_F12,
	 
	AV_KEY_INSERT, 
	AV_KEY_LEFT, AV_KEY_UP, AV_KEY_RIGHT, AV_KEY_DOWN, 
	AV_KEY_PAGE_DOWN, AV_KEY_PAGE_UP, 
	AV_KEY_END, AV_KEY_HOME
};

typedef struct av_Window {
	int width, height;
	int is_fullscreen;
	int button;
	int shift, alt, ctrl;
	double fps;
	
	void (*oncreate)(struct av_Window * self);
	void (*onresize)(struct av_Window * self, int w, int h);
	void (*onvisible)(struct av_Window * self, int state);
	void (*ondraw)(struct av_Window * self);
	void (*onkey)(struct av_Window * self, int event, int key);
	void (*onmouse)(struct av_Window * self, int event, int button, int x, int y);
	
} av_Window;

av_Window * av_window_create();

void av_window_setfullscreen(av_Window * self, int b);
void av_window_settitle(av_Window * self, const char * name);
void av_window_setdim(av_Window * self, int x, int y);

#ifdef __cplusplus
}
#endif

#endif // AV_H