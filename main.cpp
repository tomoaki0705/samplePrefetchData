#include <iostream>
#include <iomanip>
#include <chrono>
#include <stdint.h>
#if defined(__ARM_NEON__) || (defined (__ARM_NEON) && defined(__aarch64))
#include <arm_neon.h>
#endif
#if defined _MSC_VER || defined __BORLANDC__
   typedef __int64 int64;
   typedef unsigned __int64 uint64;
#else
   typedef int64_t int64;
   typedef uint64_t uint64;
#endif
#define USE_PREFETCH 1
const uint64 initState = 0x12345678;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef int duration;
const int cSize = 256;

class RNG
{
public:
	uint64 state;
	RNG()                        { state = 0xffffffff; };
	RNG(uint64 _state)           { state = _state ? _state : 0xffffffff; };
	operator unsigned char()     { return (unsigned char)next(); };
	operator char()              { return (char)next(); };
	operator unsigned short()    { return (unsigned short)next(); };
	operator short()             { return (short)next(); };
	operator int()               { return (int)next(); };
	operator unsigned()          { return next(); };

	inline unsigned next()
	{
	    state = (uint64)(unsigned)state* 4164903690U + (unsigned)(state >> 32);
	    return (unsigned)state;
	}
};

template <typename T>
void initializeArray(RNG& rng, T* array, const int size)
{
	for(int i = 0;i < size;i++)
	{
		array[i] = (T)rng;
	}
}

template <typename T>
void dumpArray(const T *array, int size)
{
	using namespace std;
	for(int i = 0;i < size;i++)
	{
		cout << (int)array[i] << '\t';
		if((i & 0x7) == 7 && i)
		{
			cout << endl;
		}
	}
	cout << endl;
}

duration diffArray_verify(const unsigned char* image1, const unsigned char* image2, int cLength, unsigned char& result)
{
	auto startCopy = std::chrono::system_clock::now();
	char x = 0;
	for(int i = 0;i < cLength;i++)
	{
		int a = (int)image1[i] - (int)image2[i];
		if(a < 0)
		{
			a = -a;
		}
		x = x ^ ((char)a);
	}
	result = x;
	auto endCopy = std::chrono::system_clock::now();
	duration msec = std::chrono::duration_cast<std::chrono::milliseconds>(endCopy - startCopy).count();
	return msec;
}

duration diffArray_prefetch(const uint8_t* image1, const uint8_t* image2, int cLength, unsigned char& result)
{
	auto startCopy = std::chrono::system_clock::now();
	const int vectorLength = 16;
	uint8x16_t v_xor = vdupq_n_u8(0);
	__builtin_prefetch(image1);
	__builtin_prefetch(image2);
	for(int i = 0;i <= cLength - (vectorLength*2);i += vectorLength)
	{
		__builtin_prefetch(image1 + i + vectorLength);
		__builtin_prefetch(image2 + i + vectorLength);
		uint8x16_t v_img1 = vld1q_u8(image1 + i);
		uint8x16_t v_img2 = vld1q_u8(image2 + i);
		uint8x16_t v_absdiff = vabdq_u8(v_img1, v_img2);
		v_xor = veorq_u8(v_xor, v_absdiff);
	}
	uint8x16_t v_img1 = vld1q_u8(image1 + cLength - vectorLength);
	uint8x16_t v_img2 = vld1q_u8(image2 + cLength - vectorLength);
	uint8x16_t v_absdiff = vabdq_u8(v_img1, v_img2);
	v_xor = veorq_u8(v_xor, v_absdiff);
	unsigned char x = 0;
	x = x ^ vgetq_lane_u8(v_xor, 0);
	x = x ^ vgetq_lane_u8(v_xor, 1);
	x = x ^ vgetq_lane_u8(v_xor, 2);
	x = x ^ vgetq_lane_u8(v_xor, 3);
	x = x ^ vgetq_lane_u8(v_xor, 4);
	x = x ^ vgetq_lane_u8(v_xor, 5);
	x = x ^ vgetq_lane_u8(v_xor, 6);
	x = x ^ vgetq_lane_u8(v_xor, 7);
	x = x ^ vgetq_lane_u8(v_xor, 8);
	x = x ^ vgetq_lane_u8(v_xor, 9);
	x = x ^ vgetq_lane_u8(v_xor, 10);
	x = x ^ vgetq_lane_u8(v_xor, 11);
	x = x ^ vgetq_lane_u8(v_xor, 12);
	x = x ^ vgetq_lane_u8(v_xor, 13);
	x = x ^ vgetq_lane_u8(v_xor, 14);
	x = x ^ vgetq_lane_u8(v_xor, 15);
	result = x;
	auto endCopy = std::chrono::system_clock::now();
	duration msec = std::chrono::duration_cast<std::chrono::milliseconds>(endCopy - startCopy).count();
	return msec;
}

duration diffArray(const uint8_t* image1, const uint8_t* image2, int cLength, unsigned char& result)
{
	auto startCopy = std::chrono::system_clock::now();
	const int vectorLength = 16;
	uint8x16_t v_xor = vdupq_n_u8(0);
	for(int i = 0;i <= cLength - vectorLength;i += vectorLength)
	{
		uint8x16_t v_img1 = vld1q_u8(image1 + i);
		uint8x16_t v_img2 = vld1q_u8(image2 + i);
		uint8x16_t v_absdiff = vabdq_u8(v_img1, v_img2);
		v_xor = veorq_u8(v_xor, v_absdiff);
	}
	unsigned char x = 0;
	x = x ^ vgetq_lane_u8(v_xor, 0);
	x = x ^ vgetq_lane_u8(v_xor, 1);
	x = x ^ vgetq_lane_u8(v_xor, 2);
	x = x ^ vgetq_lane_u8(v_xor, 3);
	x = x ^ vgetq_lane_u8(v_xor, 4);
	x = x ^ vgetq_lane_u8(v_xor, 5);
	x = x ^ vgetq_lane_u8(v_xor, 6);
	x = x ^ vgetq_lane_u8(v_xor, 7);
	x = x ^ vgetq_lane_u8(v_xor, 8);
	x = x ^ vgetq_lane_u8(v_xor, 9);
	x = x ^ vgetq_lane_u8(v_xor, 10);
	x = x ^ vgetq_lane_u8(v_xor, 11);
	x = x ^ vgetq_lane_u8(v_xor, 12);
	x = x ^ vgetq_lane_u8(v_xor, 13);
	x = x ^ vgetq_lane_u8(v_xor, 14);
	x = x ^ vgetq_lane_u8(v_xor, 15);
	result = x;
	auto endCopy = std::chrono::system_clock::now();
	duration msec = std::chrono::duration_cast<std::chrono::milliseconds>(endCopy - startCopy).count();
	return msec;
}

void measureXorOperation(const int cLength, bool usePrefetch)
{
	RNG rng(initState);
	unsigned char *image1, *image2;
	image1 = new unsigned char[cLength];
	image2 = new unsigned char[cLength];
	initializeArray(rng, image1, cLength);
	initializeArray(rng, image2, cLength);
	unsigned char result;
	duration elapsedTime;
	if(usePrefetch)
	{
		elapsedTime = diffArray_prefetch((uint8_t*)image1, (uint8_t*)image2, cLength, result);
		std::cout << "----using prefetch-----" << std::endl;
	}
	else
	{
		elapsedTime = diffArray((uint8_t*)image1, (uint8_t*)image2, cLength, result);
		std::cout << "----normal process-----" << std::endl;
	}
	std::cout << "result      :" << (int)result << std::endl;
	std::cout << "elapsed time:" << elapsedTime << "[ms]" << std::endl;
	delete [] image1;
	delete [] image2;
}

int main(int argc, char ** argv)
{
	measureXorOperation(cSize, false);
	measureXorOperation(cSize, true);
	
	return 0;
}

