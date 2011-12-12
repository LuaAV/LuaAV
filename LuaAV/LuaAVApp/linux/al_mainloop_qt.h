#ifndef INCLUDE_AL_MAINLOOP_LINUX_H
#define INCLUDE_AL_MAINLOOP_LINUX_H

#include "allocore/system/al_MainLoop.h"
#include <QTimer>

class OSClock :  public QObject {
	Q_OBJECT

public:
	OSClock(al_sec interval);
	~OSClock();

	void start();

private slots:
	void tick();
	
protected:
	QTimer *timer;
};

#endif	// INCLUDE_AL_MAINLOOP_LINUX_H

