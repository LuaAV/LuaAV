#ifndef INCLUDE_LUAAV_AUDIO_REVERB_H
#define INCLUDE_LUAAV_AUDIO_REVERB_H 1

extern "C" void	*calloc(size_t, size_t);
extern "C" void free(void *);

/// Plate reverberator

/// Design from:
/// Dattorro, J. (1997). Effect design: Part 1: Reverberator and other filters. 
/// Journal of the Audio Engineering Society, 45(9):660–684.
/// https://ccrma.stanford.edu/~dattorro/EffectDesignPart1.pdf
class Reverb{
public:

	class DelayLine {
	public:
		double * buffer;
		int mPos;
		int mSize;
	
		DelayLine(int size): mPos(0), mSize(size) {
			buffer = (double *)calloc(size, sizeof(double));
		}
		
		~DelayLine() {
			free(buffer);
		}
		
		/// Get absolute index of write tap
		int pos() const { return mPos; }
		
		/// Write value to delay
		void write(const double& v){
			buffer[pos()] = v;
			++mPos; 
			if(mPos >= size()) mPos=0;
		}

		/// Read value at delay i
		const double& read(int i) const {
			int ind = pos()-i;
			if(ind < 0) ind += size();
			//else if(ind >= size()) ind -= size();
			return buffer[ind];
		}
		
		/// Write new value and return oldest value
		double operator()(const double& v){
			double r = buffer[pos()];
			write(v);
			return r;
		}
		
		const double& back() const { return buffer[indexBack()]; }
		
		double comb(const double& v, const double& ffd, const double& fbk){
			double d = buffer[pos()];
			double r = v + d*fbk;
			write(r);
			return d + r*ffd;
		}
		
		int size() const { return mSize; }

		int indexBack() const {
			int i = pos()+1;
			return (i < size()) ? i : 0;
		}

	};
	
	

	class OnePole{
	public:
		OnePole(): mO1(0), mA0(1), mB1(0){}
		void damping(const double& v){ mB1=v; mA0=1.-v; }
		double operator()(const double& i0){ return mO1 = mO1*mB1 + i0*mA0; }
	protected:
		double mO1;
		double mA0, mB1;
	};
	
	
	

	Reverb()
	:	mPreDelay(10),
		mAPIn1(142), mAPIn2(107), mAPIn3(379), mAPIn4(277),
		mAPDecay11(672), mAPDecay12(1800), mDly11(4453), mDly12(3720),
		mAPDecay21(908), mAPDecay22(2656), mDly21(4217), mDly22(3163)
	{
//		bandwidth(0.9995);
//		decay(0.5);
//		damping(0.0005);
//		diffusion(0.75, 0.625, 0.7, 0.5);
		bandwidth(0.9995);
		decay(0.85);
		damping(0.4);
		diffusion(0.76, 0.666, 0.707, 0.571);
	}

	/// Set input signal bandwidth, [0,1]
	Reverb& bandwidth(double v){ mOPIn.damping((1.)-v); return *this; }

	/// Set high-frequency damping amount, [0, 1]
	Reverb& damping(double v){ mOP1.damping(v); mOP2.damping(v); return *this; }

	/// Set decay rate, [0, 1)
	Reverb& decay(double v){ mDecay=v; return *this; }

	/// Set diffusion amounts
	
	/// The recommended range of these coefficients is from 0.0 to 0.9999999
	///
	Reverb& diffusion(double in1, double in2, double decay1, double decay2){
		mDfIn1=in1;	mDfIn2=in2; mDfDcy1=decay1; mDfDcy2=decay2;
		return *this;
	}
	
	/// Set input diffusion 1 amount, [0,1)
	Reverb& diffusionIn1(const double& v){ mDfIn1=v; return *this; }
	
	/// Set input diffusion 2 amount, [0,1)
	Reverb& diffusionIn2(const double& v){ mDfIn2=v; return *this; }
	
	/// Set tank decay diffusion 1 amount, [0,1)
	Reverb& diffusionDecay1(const double& v){ mDfDcy1=v; return *this; }
	
	/// Set tank decay diffusion 2 amount, [0,1)
	Reverb& diffusionDecay2(const double& v){ mDfDcy2=v; return *this; }

	/// Compute wet stereo output from dry input
	void operator()(const double& i0, double& o1, double& o2){
		double v = mPreDelay(i0 * 0.5);
		v = mOPIn(v);
		v = mAPIn1.comb(v, mDfIn1,-mDfIn1);
		v = mAPIn2.comb(v, mDfIn1,-mDfIn1);
		v = mAPIn3.comb(v, mDfIn2,-mDfIn2);
		v = mAPIn4.comb(v, mDfIn2,-mDfIn2);
		
		double a = v + mDly22.back() * mDecay;
		double b = v + mDly12.back() * mDecay;
		
		a = mAPDecay11.comb(a,-mDfDcy1, mDfDcy1);
		a = mDly11(a);
		a = mOP1(a) * mDecay;
		a = mAPDecay12.comb(a, mDfDcy2,-mDfDcy2);
		mDly12.write(a);

		b = mAPDecay21.comb(b,-mDfDcy1, mDfDcy1);
		b = mDly21(b);
		b = mOP2(b) * mDecay;
		b = mAPDecay22.comb(b, mDfDcy2,-mDfDcy2);
		mDly22.write(b);
		
		double m = (0.6);
		
		o1 = m*(  mDly21.read(266)
				+ mDly21.read(2974)
				- mAPDecay22.read(1913)
				+ mDly22.read(1996)
				- mDly11.read(1990)
				- mAPDecay12.read(187)
				- mDly12.read(1066));

		o2 = m*(  mDly11.read(353)
				+ mDly11.read(3627)
				- mAPDecay12.read(1228)
				+ mDly12.read(2673)
				- mDly21.read(2111)
				- mAPDecay22.read(335)
				- mDly22.read(121));
	}

protected:

	double mDfIn1, mDfIn2, mDfDcy1, mDfDcy2, mDecay;
	DelayLine mPreDelay;
	OnePole mOPIn;
	DelayLine mAPIn1, mAPIn2, mAPIn3, mAPIn4;
	DelayLine mAPDecay11, mAPDecay12, mDly11, mDly12;
	OnePole mOP1;
	DelayLine mAPDecay21, mAPDecay22, mDly21, mDly22;
	OnePole mOP2;
};

#endif /* include guard */
