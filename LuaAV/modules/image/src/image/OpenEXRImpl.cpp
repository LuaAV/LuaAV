#include "OpenEXRImpl.h"

namespace image {

OpenEXRImpl :: OpenEXRImpl()
:	ImageImpl(ImageImpl::OPENEXR)
{}

OpenEXRImpl :: ~OpenEXRImpl() {
	destroy();
}

void OpenEXRImpl :: destroy() {
	mImage.freeData();
}

MuroError OpenEXRImpl :: load(const char *filename, AlloArray &mat) {
	ImageError err = IMAGE_ERROR_NONE;

	Imf::InputFile file(filename);
	char channelNames[] = {0, 0, 0, 0};

	const Imf::ChannelList &channels = file.header().channels();

	//doesn't account for chromaticities yet
	if( channels.findChannel("Y") ) {
		channelNames[0] = 'Y';

		if( channels.findChannel("A") ) {
			channelNames[1] = 'A';
		}
	}
	else if(channels.findChannel("R") &&
			channels.findChannel("G") &&
			channels.findChannel("B") )
	{
		channelNames[0] = 'R';
		channelNames[1] = 'G';
		channelNames[2] = 'B';

		if( channels.findChannel("A") ) {
			channelNames[3] = 'A';
		}
	}

	int numChannels = 0;
	const char *channel = channelNames;
	while(*channel && numChannels < 4) {
		numChannels++;
		channel++;
	}

	if(numChannels <= 0) {
		//no standard channels, so take them arbitrarily
		for(Imf::ChannelList::ConstIterator it=channels.begin();
			it != channels.end();
			++it)
		{
			channelNames[numChannels] = it.name()[0];
			numChannels++;

			if(numChannels >= 4) break;	//max out at 4
		}
	}

	loadChannels(file, mImage, numChannels, channelNames);
	
	mat.fromMatrix(&mImage);
	
	return err;
}

ImageError OpenEXRImpl :: save(const char *filename, AlloArray &mat) {
	ImageError err = IMAGE_ERROR_NONE;
	
	mImage.fromMatrix(&mat);

	Imf::Header header(mImage.header.dim[0], mImage.header.dim[1]);
	Imf::PixelType pt = pixelTypeForMatrixType(mImage.header.type);

	int planes = mImage.header.planes;
	int rowsize;
	mImage.getRowSize(rowsize);

	char channelNames[] = {0, 0, 0, 0};

	switch(planes) {
		case 1:
			channelNames[0] = 'Y';
			break;

		case 2:
			channelNames[0] = 'Y';
			channelNames[1] = 'A';
			break;

		case 3:
			channelNames[0] = 'R';
			channelNames[1] = 'G';
			channelNames[2] = 'B';
			break;

		case 4:
			channelNames[0] = 'R';
			channelNames[1] = 'G';
			channelNames[2] = 'B';
			channelNames[3] = 'A';
			break;
	}

	for(int i=0; i < planes; i++) {
		char c[] = {channelNames[i], '\0'};
		header.channels().insert(c, Imf::Channel(pt));
	}


	Imf::OutputFile file(filename, header);
	Imf::FrameBuffer frameBuffer;

	for(int i=0; i < planes; i++) {
		char c[] = {channelNames[i], '\0'};
		frameBuffer.insert (c,
						Imf::Slice(pt,
							   (char *)(mImage.data.fl + i),
							   sizeof(float)*planes,
							   rowsize));
	}

	file.setFrameBuffer(frameBuffer);
	file.writePixels(mImage.header.dim[1]);
	
	return err;
}

void OpenEXRImpl :: load_channels(Imf::InputFile &file, Matrix& mat, int numChannels, const char *channelNames) {
	Imath::Box2i dw = file.header().dataWindow();
	int width = dw.max.x - dw.min.x + 1;
	int height = dw.max.y - dw.min.y + 1;

	mat.create(numChannels, Matrix::FLOAT32, width, height);

	Imf::FrameBuffer frameBuffer;
	for(int i=0; i < numChannels; i++) {
		char c[2];
		c[0] = channelNames[i];
		c[1] = '\0';
		frameBuffer.insert(c,
				Imf::Slice(Imf::FLOAT,
					(char *)(mat.data.fl + i),
					sizeof(float)*numChannels,
					sizeof(float)*numChannels*width));
	}

	file.setFrameBuffer(frameBuffer);
	file.readPixels(dw.min.y, dw.max.y);
}

char * OpenEXRImpl :: getData() {
	return NULL;
}

Imf::PixelType OpenEXRImpl :: pixel_type_for_matrix_type(Matrix::Type type) {
	switch(type) {
		case Matrix::UINT:	return Imf::UINT;
	//	case Matrix::HALF:	return Imf::HALF;
		case Matrix::FLOAT32:
		default:			return Imf::FLOAT;
	}
}

}	// image::
