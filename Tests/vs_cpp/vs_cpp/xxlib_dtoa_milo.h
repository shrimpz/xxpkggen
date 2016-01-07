#pragma once
// the code is copy & refact from following URL:
// https://github.com/miloyip/itoa-benchmark/tree/master/src

struct DiyFp
{
	DiyFp();
	DiyFp( uint64 f, int e );
	DiyFp( double d );

	DiyFp operator-( const DiyFp& rhs ) const;
	DiyFp operator*( const DiyFp& rhs ) const;

	DiyFp Normalize() const;
	DiyFp NormalizeBoundary() const;
	void NormalizedBoundaries( DiyFp* minus, DiyFp* plus ) const;

	static const int kDiySignificandSize;
	static const int kDpSignificandSize;
	static const int kDpExponentBias;
	static const int kDpMinExponent;
	static const uint64 kDpExponentMask;
	static const uint64 kDpSignificandMask;
	static const uint64 kDpHiddenBit;

	uint64 f;
	int e;

	static DiyFp GetCachedPower( int e, int* K );
	static void GrisuRound( char* buffer, int len, uint64 delta, uint64 rest, uint64 ten_kappa, uint64 wp_w );
	static unsigned CountDecimalDigit32( uint n );
	static void DigitGen( const DiyFp& W, const DiyFp& Mp, uint64 delta, char* buffer, int* len, int* K );
	static void Grisu2( double value, char* buffer, int* length, int* K );
	static const char* GetDigitsLut();
	static int WriteExponent( int K, char* buffer );
	static int Prettify( char* buffer, int length, int k );
	static int dtoa_milo( double value, char* buffer );
};
