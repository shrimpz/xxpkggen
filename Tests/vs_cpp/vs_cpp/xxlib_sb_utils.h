#pragma once
#include <stddef.h>     // size_t

// some code copy & edit from:
// https://github.com/miloyip/itoa-benchmark/tree/master/src

namespace xxlib
{
    namespace Utils
    {

        struct DiyFp
        {
            DiyFp();
            DiyFp(unsigned long long f, int e);
            DiyFp(double d);

            DiyFp operator-(const DiyFp& rhs) const;
            DiyFp operator*(const DiyFp& rhs) const;

            DiyFp Normalize() const;
            DiyFp NormalizeBoundary() const;
            void NormalizedBoundaries(DiyFp* minus, DiyFp* plus) const;

            static const int kDiySignificandSize;
            static const int kDpSignificandSize;
            static const int kDpExponentBias;
            static const int kDpMinExponent;
            static const unsigned long long kDpExponentMask;
            static const unsigned long long kDpSignificandMask;
            static const unsigned long long kDpHiddenBit;

            unsigned long long f;
            int e;

            static DiyFp GetCachedPower(int e, int* K);
            static void GrisuRound(char* buffer, int len, unsigned long long delta, unsigned long long rest, unsigned long long ten_kappa, unsigned long long wp_w);
            static unsigned CountDecimalDigit32(unsigned int n);
            static void DigitGen(const DiyFp& W, const DiyFp& Mp, unsigned long long delta, char* buffer, int* len, int* K);
            static void Grisu2(double value, char* buffer, int* length, int* K);
            static const char* GetDigitsLut();
            static int WriteExponent(int K, char* buffer);
            static int Prettify(char* buffer, int length, int k);
            static int dtoa_milo(double value, char* buffer);
        };

        size_t u32toa_branchlut(unsigned int value, char* buffer);
        size_t i32toa_branchlut(int value, char* buffer);
        size_t u64toa_branchlut(unsigned long long value, char* buffer);
        size_t i64toa_branchlut(long long value, char* buffer);


        void FromString(unsigned char &out, char const *in);
        void FromString(unsigned short &out, char const *in);
        void FromString(unsigned int &out, char const *in);
        void FromString(unsigned long long &out, char const *in);
        void FromString(signed char &out, char const *in);
        void FromString(short &out, char const *in);
        void FromString(int &out, char const *in);
        void FromString(long long &out, char const *in);
        void FromString(double &out, char const *in);
        void FromString(float &out, char const *in);
        void FromString(bool &out, char const *in);
        void FromString(long &out, char const *in);
        void FromString(unsigned long &out, char const *in);

    }
}
