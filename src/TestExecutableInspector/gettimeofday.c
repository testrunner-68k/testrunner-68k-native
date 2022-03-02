#ifdef __m68k__

// Bebbo's amiga-gcc seems to be missing _gettimeofday
//  so we're adding a dummy implementation just to make it work

struct timeval;

int _gettimeofday(struct timeval *__tp, void *__tzp)
{
	return -1;
}

#endif