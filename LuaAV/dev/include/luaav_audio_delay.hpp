#ifndef INCLUDE_LUAAV_AUDIO_DELAY_H
#define INCLUDE_LUAAV_AUDIO_DELAY_H 1

extern "C" void	*calloc(size_t, size_t);
extern "C" void free(void *);

class Delay{
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

		/// Read value at delay i
		const double& read_linear(double d) const {
			const double r = pos() - d;
			long ind1 = long(r);
			long ind2 = ind1+1;
			double a = r - (double)ind1;
			while(ind1 < 0) ind1 += size();
			while(ind1 > size()) ind1 -= size();
			while(ind2 < 0) ind2 += size();
			while(ind2 > size()) ind2 -= size();
			const double x1 = buffer[ind1];
			const double x2 = buffer[ind2];
			return x1+a*(x2-x1);
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
	
	
	

	Delay(int maxdelay=44100)
	:	mDelay(maxdelay)
	{}
	
	/// Compute wet stereo output from dry input
	inline double operator()(const double& i0, const double& d, const double& feedback){
		double o = mDelay.read_linear(d);
		mDelay.write(i0 + o*feedback);
		return o;
	}
	
protected:

	DelayLine mDelay;
};

#endif /* include guard */
