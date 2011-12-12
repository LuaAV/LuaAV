#import "QtKitVideoImpl.h"
#import <Cocoa/Cocoa.h>
#import <QTKit/QTKit.h>


@interface QtKitVideo : NSObject {
	QTMovie *movie;
	float rate;
	BOOL paused;
	long nframes;
	int width;
	int height;
}

- (id) initWithFilename:(NSString *)filename;
- (QTTrack *)firstVideoTrack;
- (void) play;
- (void) pause;
- (void) stop;
- (void) close;
- (void) getFrame:(AlloArray *)mat;
- (float) getPosition;
- (void) setPosition:(float)pos;
- (int) getFrame;
- (void) setFrame:(int)frame;
- (float) getRate;
- (void) setRate:(float)v;
- (int) getNFrames;
- (int) getWidth;
- (int) getHeight;

@end

@implementation QtKitVideo


- (id) initWithFilename:(NSString *)filename;
{
	if(self = [super init]) {
		NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:
							(id)filename, QTMovieFileNameAttribute,
							//[NSNumber numberWithBool:YES], QTMovieOpenForPlaybackAttribute, 
							nil];
		movie = [[QTMovie alloc] initWithAttributes:attributes error:NULL];
		rate = 1.;
		paused = NO;
		
		
		QTTrack *track = [self firstVideoTrack];
		if(track) {
			nframes = [[track attributeForKey: QTMediaSampleCountAttribute] longValue];
		}
		else {
			nframes = 0;
			fprintf(stderr, "File as no video track\n");
		}
		
		NSSize contentSize = NSZeroSize;
		NSValue *contentSizeValue = [movie attributeForKey:QTMovieNaturalSizeAttribute];
		if(contentSizeValue) {
			contentSize = [contentSizeValue sizeValue];
		}
		width = (int)contentSize.width;
		height = (int)contentSize.height;
	}
		
	return self;
}

- (QTTrack *)firstVideoTrack
{
	QTTrack *track = nil;
	NSEnumerator *enumerator = [[movie tracks] objectEnumerator];
	while((track = [enumerator nextObject]) != nil) {
		if([track isEnabled]) {
			QTMedia *media = [track media];
			NSString *mediaType = [media attributeForKey:QTMediaTypeAttribute];
			if([mediaType isEqualToString:QTMediaTypeVideo] || [mediaType isEqualToString:QTMediaTypeMPEG]) {
				if([media hasCharacteristic:QTMediaCharacteristicHasVideoFrameRate]) {
					break; // found first video track
				}
			}
		}
	}

	return track;
}


- (void) play
{
	if(paused) {
		[movie setRate: rate];
	}
	else {
		[movie play];
	}
	paused = NO;
}

- (void) pause
{
	if(! paused) {
		[movie setRate:0];
		paused = YES;
	}
}

- (void) stop
{
	[movie stop];
	paused = NO;
}

- (void) close
{
	[movie stop];
	paused = NO;
}

- (void) getFrame:(AlloArray *)mat
{
	QTTime time = [movie currentTime];
	QTTime dur = [movie duration];
	printf("time: %d/%d\n", (int)time.timeValue, (int)dur.timeValue);

	NSImage *frame = [movie currentFrameImage];
	NSArray *reps = [frame representations];
	NSBitmapImageRep *bitmap = (NSBitmapImageRep *)[reps objectAtIndex:0];
	unsigned char *data = [bitmap bitmapData];
	NSInteger rowstride = [bitmap bytesPerRow];	
	
	AlloArrayHeader h;
	h.components = (uint8_t)[bitmap samplesPerPixel];
	h.type = AlloUInt8Ty;
	h.dimcount = 2;
	h.dim[0] = (uint32_t)[bitmap pixelsWide];
	h.dim[1] = (uint32_t)[bitmap pixelsHigh];
	allo_array_setstride(&h, 1);
	allo_array_adapt(mat, &h);
	
	for(int j=0; j < mat->header.dim[1]; j++) {
		unsigned char *src = data + j*rowstride;
		unsigned char *dst = (unsigned char *)(mat->data.ptr + j*mat->header.stride[1]);
	
		for(int i=0; i < mat->header.dim[0]*mat->header.components; i++) {
			*dst++ = *src++;
		}
	}
}

- (float) getPosition
{
	QTTime time = [movie currentTime];
	QTTime dur = [movie duration];
	return ((float)time.timeValue)/((float)dur.timeValue);
}

- (void) setPosition:(float)pos
{
	QTTime dur = [movie duration];
	QTTime time = QTMakeTime((long long)(pos * ((float)dur.timeValue)), dur.timeScale);
	[movie setCurrentTime:time];
}

- (int) getFrame
{
	QTTime time = [movie currentTime];
	QTTime dur = [movie duration];
	return (int) (((float)time.timeValue)/((float)dur.timeValue) * ((float)nframes));
}

- (void) setFrame:(int)frame
{
	QTTime dur = [movie duration];
	QTTime time = QTMakeTime((long long)(((float)frame)/((float)nframes)*((float)dur.timeValue)), dur.timeScale);
	[movie setCurrentTime:time];
}

- (float) getRate
{
	return rate;
}

- (void) setRate:(float)v
{
	[movie setRate:v];
}

- (int) getNFrames
{
	return nframes;
}

- (int) getWidth
{
	return width;
}

- (int) getHeight
{
	return height;
}

@end


namespace video {

QtKitVideoImpl :: QtKitVideoImpl()
:	VideoImpl(QTKIT),
	mVideo(0),
	mSpeed(1.)
{}

QtKitVideoImpl :: ~QtKitVideoImpl() {
	close();
}

VideoError QtKitVideoImpl :: open(const char *filename) {
	VideoError err = VIDEO_ERROR_NONE;
	if(mVideo) {
		err = close();
	}
	
	mVideo = [
		[QtKitVideo alloc] initWithFilename:
			[NSString stringWithCString:filename encoding:NSASCIIStringEncoding]
	];
	[(QtKitVideo *)mVideo setRate:mSpeed];
	
	return err;
}

VideoError QtKitVideoImpl :: close() {
	VideoError err = VIDEO_ERROR_NONE;
	if(mVideo) {
		[(QtKitVideo *)mVideo close];
		[(QtKitVideo *)mVideo release];
		mVideo = 0;
	}
	return err;
}

VideoError QtKitVideoImpl :: play() {
	VideoError err = VIDEO_ERROR_NONE;
	if(mVideo) {
		[(QtKitVideo *)mVideo play];
	}
	return err;
}

VideoError QtKitVideoImpl :: next_frame(AlloArray *mat) {
	VideoError err = VIDEO_ERROR_NONE;
	if(mVideo) {
		[(QtKitVideo *)mVideo getFrame:mat];
	}
	return err;
}

VideoError QtKitVideoImpl :: stop() {
	VideoError err = VIDEO_ERROR_NONE;
	if(mVideo) {
		[(QtKitVideo *)mVideo stop];
	}
	return err;
}

VideoError QtKitVideoImpl :: pause(bool b) {
	VideoError err = VIDEO_ERROR_NONE;
	if(mVideo) {
		[(QtKitVideo *)mVideo pause];
	}
	return err;
}

void QtKitVideoImpl :: set_position(float pct) {
	if(mVideo) {
		[(QtKitVideo *)mVideo setPosition:pct];
	}
}

void QtKitVideoImpl :: set_speed(float speed) {
	mSpeed = speed;
	if(mVideo) {
		[(QtKitVideo *)mVideo setRate:speed];
	}
}

void QtKitVideoImpl :: set_frame(int f) {
	if(mVideo) {
		[(QtKitVideo *)mVideo setFrame:f];
	}
}

float QtKitVideoImpl :: get_position() {
	if(mVideo) {
		return [(QtKitVideo *)mVideo getPosition];
	}
	else {
		return 0.;
	}
}

float QtKitVideoImpl :: get_speed() {
	return mSpeed;
}

int QtKitVideoImpl :: get_frame() {
	if(mVideo) {
		return [(QtKitVideo *)mVideo getFrame];
	}
	else {
		return 0;
	}
}

int QtKitVideoImpl :: get_nframes() {
	if(mVideo) {
		return [(QtKitVideo *)mVideo getNFrames];
	}
	else {
		return 0;
	}
}

float QtKitVideoImpl :: get_duration() {
	return 0;
}

void QtKitVideoImpl :: get_dim(int &w, int &h) {
	if(mVideo) {
		w = [(QtKitVideo *)mVideo getWidth];
		h = [(QtKitVideo *)mVideo getHeight];
	}
	else {
		w = 0;
		h = 0;
	}
}

}	// video::