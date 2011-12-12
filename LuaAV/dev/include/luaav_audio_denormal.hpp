namespace denormal {

void test_and_kill (float & val)
{
	// Requires 32-bit int
	const int32_t x = *reinterpret_cast <const int *> (&val);
	const int32_t abs_mantissa = x & 0x007FFFFF;
	const int32_t biased_exponent = x & 0x7F800000;
	if (biased_exponent == 0 && abs_mantissa != 0) {
		val = 0;
	}
}

void kill_by_quantization (float &val)
{
	static const float anti_denormal = 1e-18;
	val += anti_denormal;
	val -= anti_denormal;
}

static int32_t rand_state = 1; 
void prevent_by_computed_noise (float &val) {
	rand_state = rand_state * 1234567UL + 890123UL;
	int32_t mantissa = rand_state & 0x807F0000; // Keep only most significant bits
	int32_t flt_rnd = mantissa | 0x1E000000; // Set exponent
	val += *reinterpret_cast <const float *> (&flt_rnd);
}


static float noise_buffer[512];
static int32_t noise_buffer_index = 0; 

void init_buffered_noise() {
	for (int i=0; i<512; i++) {
		noise_buffer[i] = 0.f;
		prevent_by_computed_noise(noise_buffer[i]);
	}
}

void prevent_by_buffered_noise (float &val) {
	noise_buffer_index = (noise_buffer_index + 1) & 511;
	val += noise_buffer[noise_buffer_index];
}

void prevent_by_adding_dc (float & val) {
	static const float anti_denormal = 1e-20;
	val += anti_denormal;
}

void prevent_by_block_pulses(float * buffer, int blocksize) {
	static const float anti_denormal = 1e-20;
	// put a few dc pulses into the buffer:
	buffer[blocksize/2] += anti_denormal;
	buffer[blocksize/4] += anti_denormal;
	buffer[blocksize/8] += anti_denormal;
}

} // denormal ::