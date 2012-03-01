#import "QtKitVideoCameraImpl.h"
#import <Cocoa/Cocoa.h>
#import <QTKit/QTKit.h>

@interface QtKitVideoCamera : NSObject {
	QTCaptureSession					*capture_session;
	QTCaptureDeviceInput				*capture_device_input;
	QTCaptureDecompressedVideoOutput	*capture_decompressed_video_output;
	CVImageBufferRef					current_frame;
	BOOL								has_new_frame;
	NSInteger							width;
	NSInteger							height;
	NSInteger							format;
}

- (id) init;
- (id) initWithDeviceID:(NSString *)uid;
- (void) openWithWidth:(NSInteger)_width height:(NSInteger)_height pixelFormat:(NSInteger)_format;
- (void) close;
- (void) captureOutput:(QTCaptureOutput *)captureOutput 
			didOutputVideoFrame:(CVImageBufferRef)videoFrame 
			withSampleBuffer:(QTSampleBuffer *)sampleBuffer 
			fromConnection:(QTCaptureConnection *)connection;
- (void) getFrame:(AlloArray *)mat;
+ (void) listDevices:(vector<video::Device> *)devices;

@end


@implementation QtKitVideoCamera

- (id) init
{
	if(self = [super init]) {
		NSError *error = nil;

		capture_session = [[QTCaptureSession alloc] init];

		QTCaptureDevice *device = [QTCaptureDevice defaultInputDeviceWithMediaType:QTMediaTypeVideo];
		if(! [device open: &error]) {
			fprintf(stderr, "Error opening default video capture device\n");
		}

		capture_device_input = [[QTCaptureDeviceInput alloc] initWithDevice:device];
		if(! [capture_session addInput:capture_device_input error:&error]) {
			fprintf(stderr, "Error adding input to capture session\n");
		}
		
		capture_decompressed_video_output = [[QTCaptureDecompressedVideoOutput alloc] init];
		[capture_decompressed_video_output setDelegate:self];
		if(! [capture_session addOutput:capture_decompressed_video_output error:&error]) {
			fprintf(stderr, "Error adding output to capture session\n");
		}
		
		current_frame = 0;
		has_new_frame = NO;
		width = 0;
		height = 0;
		format = kCVPixelFormatType_24RGB;
	}
	
	return self;
}

- (id) initWithDeviceID:(NSString *)uid
{
	if(self = [super init]) {
		NSError *error = nil;

		capture_session = [[QTCaptureSession alloc] init];

		QTCaptureDevice *device = [QTCaptureDevice deviceWithUniqueID:uid];
		if(! [device open: &error]) {
			fprintf(stderr, "Error opening specified video capture device, trying default\n");
			device = [QTCaptureDevice defaultInputDeviceWithMediaType:QTMediaTypeVideo];
			if(! [device open: &error]) {
				fprintf(stderr, "Error opening default video capture device\n");
			}
		}

		capture_device_input = [[QTCaptureDeviceInput alloc] initWithDevice:device];
		if(! [capture_session addInput:capture_device_input error:&error]) {
			fprintf(stderr, "Error adding input to capture session\n");
		}
		
		capture_decompressed_video_output = [[QTCaptureDecompressedVideoOutput alloc] init];
		[capture_decompressed_video_output setDelegate:self];
		if(! [capture_session addOutput:capture_decompressed_video_output error:&error]) {
			fprintf(stderr, "Error adding output to capture session\n");
		}
		
		current_frame = 0;
		has_new_frame = NO;
		width = 0;
		height = 0;
		format = kCVPixelFormatType_24RGB;
	}
	
	return self;
}


// kCVPixelFormatType_24RGB
// kCVPixelFormatType_32ARGB
NSInteger qt_format_from_format(video::PixelFormat fmt) {
	switch(fmt) {
		case video::PIX_FMT_RGB:
			return kCVPixelFormatType_24RGB;
			
		case video::PIX_FMT_RGBA:
			return kCVPixelFormatType_32ARGB;
	}
	return kCVPixelFormatType_24RGB;
}

int components_for_qt_format(NSInteger fmt) {
	switch(fmt) {
		case kCVPixelFormatType_24RGB: return 3;
		case kCVPixelFormatType_32ARGB: return 4;
	}
	return 3;
}


- (void) openWithWidth:(NSInteger)_width height:(NSInteger)_height pixelFormat:(NSInteger)_format
{
	if(_width < 0 || _height < 0) {
		NSArray *formats = [[capture_device_input device] formatDescriptions];
		for(int i=0; i < [formats count]; i++) {
			QTFormatDescription *desc = (QTFormatDescription *)[formats objectAtIndex:i];
			NSSize size = [[desc attributeForKey:QTFormatDescriptionVideoEncodedPixelsSizeAttribute] sizeValue];
			_width = size.width;
			_height = size.height;
			break;
		}
	}
	else {
		width = _width;
		height = _height;
	}
	format = _format;
	
	[capture_decompressed_video_output 
		setPixelBufferAttributes:
			[NSDictionary dictionaryWithObjectsAndKeys: 
			 [NSNumber numberWithInt: format], kCVPixelBufferPixelFormatTypeKey,
			 [NSNumber numberWithInt:width], kCVPixelBufferWidthKey, 
			 [NSNumber numberWithInt:height], kCVPixelBufferHeightKey, 
			// [NSNumber numberWithBool:YES], kCVPixelBufferOpenGLCompatibilityKey,
			nil]
	];
	[capture_session startRunning];
}

- (void) close
{
	[capture_session stopRunning];
}

- (void) captureOutput:(QTCaptureOutput *)captureOutput 
			didOutputVideoFrame:(CVImageBufferRef)videoFrame 
			withSampleBuffer:(QTSampleBuffer *)sampleBuffer 
			fromConnection:(QTCaptureConnection *)connection
{
	CVImageBufferRef last_frame;
    CVBufferRetain(videoFrame);
 
    @synchronized (self) {
        last_frame = current_frame;
        current_frame = videoFrame;
    }
 
    CVBufferRelease(last_frame);
	has_new_frame = YES;
}

- (void) getFrame:(AlloArray *)mat
{
	AlloArrayHeader h;
	h.components = components_for_qt_format(format);
	h.type = AlloUInt8Ty;
	h.dimcount = 2;
	h.dim[0] = (uint32_t)width;
	h.dim[1] = (uint32_t)height;
	allo_array_setstride(&h, 1);
	allo_array_adapt(mat, &h);


	@synchronized(self) {
		if(has_new_frame) {
			CVPixelBufferLockBaseAddress(current_frame, 0);
			
			unsigned char *data = (unsigned char *)CVPixelBufferGetBaseAddress(current_frame);
			size_t rowstride = CVPixelBufferGetBytesPerRow(current_frame);
			
			if(h.components == 4) {
				for(uint32_t j=0; j < h.dim[1]; j++) {
					unsigned char *src = data + j*rowstride;
					unsigned char *dst = (unsigned char *)(mat->data.ptr + j*mat->header.stride[1]);
					
					for(uint32_t i=0; i < h.dim[0]; i++) {
						dst[0] = src[1];
						dst[1] = src[2];
						dst[2] = src[3];
						dst[3] = src[0];
						
						dst += 4;
						src += 4;
					}
				}
			}
			else if(h.components == 3) {
				for(uint32_t j=0; j < h.dim[1]; j++) {
					unsigned char *src = data + j*rowstride;
					unsigned char *dst = (unsigned char *)(mat->data.ptr + j*mat->header.stride[1]);
					
					for(uint32_t i=0; i < h.dim[0]; i++) {
						dst[0] = src[0];
						dst[1] = src[1];
						dst[2] = src[2];
						
						dst += 3;
						src += 3;
					}
				}
			}
			
			CVPixelBufferUnlockBaseAddress(current_frame, 0);
			has_new_frame = NO;
		}
	}
}

+ (void) listDevices:(vector<video::Device> *)devices
{
	NSArray *devs = [
		[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo] 
			arrayByAddingObjectsFromArray:[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeMuxed]
	];
	for(int i=0; i < [devs count]; i++) {
		QTCaptureDevice *dev = [devs objectAtIndex:i];
		NSString *name = [dev localizedDisplayName];
		NSString *uid = [dev uniqueID];
		video::Device D;
		D.name = [name cStringUsingEncoding:NSASCIIStringEncoding];
		D.uid = [uid cStringUsingEncoding:NSASCIIStringEncoding];
		devices->push_back(D);
		/*printf("%s unique ID: %s\n", 
			[name cStringUsingEncoding:NSASCIIStringEncoding],
			[uid cStringUsingEncoding:NSASCIIStringEncoding]
		);
		*/
	}
}

@end


namespace video {

QtKitVideoCameraImpl :: QtKitVideoCameraImpl()
:	VideoCameraImpl(QTKIT),
	mVideoGrabber(0),
	mOpen(false)
{
	mVideoGrabber = (void *)[[QtKitVideoCamera alloc] init];
}

QtKitVideoCameraImpl :: QtKitVideoCameraImpl(const char *uid)
:	VideoCameraImpl(QTKIT)
{
	NSString *nsuid = [NSString stringWithCString:uid encoding:NSASCIIStringEncoding];
	mVideoGrabber = (void *)[[QtKitVideoCamera alloc] initWithDeviceID:nsuid];
}

QtKitVideoCameraImpl :: ~QtKitVideoCameraImpl() {
	[(QtKitVideoCamera *)mVideoGrabber close];
	[(QtKitVideoCamera *)mVideoGrabber release];
}

VideoError QtKitVideoCameraImpl :: open(int w, int h, PixelFormat fmt) {
	VideoError err = VIDEO_ERROR_NONE;
	if(! mOpen) {
		[(QtKitVideoCamera *)mVideoGrabber 
				openWithWidth:(NSInteger)w 
				height:(NSInteger)h
				pixelFormat:qt_format_from_format(fmt)
			];
		mOpen = true;
	}
	return err;
}

VideoError QtKitVideoCameraImpl :: close() {
	VideoError err = VIDEO_ERROR_NONE;
	[(QtKitVideoCamera *)mVideoGrabber close];
	mOpen = false;
	return err;
}

VideoError QtKitVideoCameraImpl :: settings() {
	VideoError err = VIDEO_ERROR_NONE;
	return err;
}

VideoError QtKitVideoCameraImpl :: next_frame(AlloArray *mat) {
	VideoError err = VIDEO_ERROR_NONE;
	[(QtKitVideoCamera *)mVideoGrabber getFrame:mat];
	return err;
}

void QtKitVideoCameraImpl :: list_devices(vector<Device> &devices) {
	[QtKitVideoCamera listDevices:&devices];
}
	
} // video::