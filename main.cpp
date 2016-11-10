#include <iostream>
#include <stdint.h>
#if defined _MSC_VER || defined __BORLANDC__
   typedef __int64 int64;
   typedef unsigned __int64 uint64;
#else
   typedef int64_t int64;
   typedef uint64_t uint64;
#endif
const uint64 initState = 0x12345678;
typedef unsigned char uchar;
typedef unsigned short ushort;

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

const int cSize = 100;
int main(int argc, char ** argv)
{
	char array[cSize];
	RNG rng(initState);
	initializeArray(rng, array, cSize);
	dumpArray(array, cSize);
	return 0;
}

