﻿#include "xxlib_sb_utils.h"
#include <stddef.h>                 // ptrdiff_t
#include <assert.h>
#include <cmath>                    // std::isnan
#include <string.h>                 // memxxx
#ifdef _WIN32
#include <intrin.h>
#endif
#include <stdlib.h>                 // strtod


#define UINT64_C2(h, l) ((static_cast<unsigned long long>(h) << 32) | static_cast<unsigned long long>(l))

namespace xxlib
{
    namespace Utils
    {
        DiyFp::DiyFp()
        {
        }

        DiyFp::DiyFp(unsigned long long f, int e) : f(f), e(e)
        {
        }

        DiyFp::DiyFp(double d)
        {
            union
            {
                double d;
                unsigned long long u64;
            } u = { d };

            //<
            int biased_e = int((u.u64 & kDpExponentMask) >> kDpSignificandSize);
            //>
            unsigned long long significand = (u.u64 & kDpSignificandMask);
            if (biased_e != 0)
            {
                f = significand + kDpHiddenBit;
                e = biased_e - kDpExponentBias;
            }
            else
            {
                f = significand;
                e = kDpMinExponent + 1;
            }
        }

        DiyFp DiyFp::operator-(const DiyFp& rhs) const
        {
            assert(e == rhs.e);
            assert(f >= rhs.f);
            return DiyFp(f - rhs.f, e);
        }

        DiyFp DiyFp::operator*(const DiyFp& rhs) const
        {
#if defined(_MSC_VER) && defined(_M_AMD64)
            unsigned long long h;
            unsigned long long l = _umul128(f, rhs.f, &h);
            if (l & (unsigned long long(1) << 63)) // rounding
                h++;
            return DiyFp(h, e + rhs.e + 64);
#elif (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) && defined(__x86_64__)
            unsigned __int128 p = static_cast<unsigned __int128>(f) * static_cast<unsigned __int128>(rhs.f);
            unsigned long long h = p >> 64;
            unsigned long long l = static_cast<unsigned long long>(p);
            if (l & ((unsigned long long)1 << 63)) // rounding
                h++;
            return DiyFp(h, e + rhs.e + 64);
#else
            const unsigned long long M32 = 0xFFFFFFFF;
            const unsigned long long a = f >> 32;
            const unsigned long long b = f & M32;
            const unsigned long long c = rhs.f >> 32;
            const unsigned long long d = rhs.f & M32;
            const unsigned long long ac = a * c;
            const unsigned long long bc = b * c;
            const unsigned long long ad = a * d;
            const unsigned long long bd = b * d;
            unsigned long long tmp = (bd >> 32) + (ad & M32) + (bc & M32);
            tmp += 1U << 31;  /// mult_round
            return DiyFp(ac + (ad >> 32) + (bc >> 32) + (tmp >> 32), e + rhs.e + 64);
#endif
        }

        DiyFp DiyFp::Normalize() const
        {
#if defined(_MSC_VER) && defined(_M_AMD64)
            unsigned long index;
            _BitScanReverse64(&index, f);
            return DiyFp(f << (63 - index), e - (63 - index));
#elif defined(__GNUC__)
            int s = __builtin_clzll(f);
            return DiyFp(f << s, e - s);
#else
            DiyFp res = *this;
            while (!(res.f & kDpHiddenBit))
            {
                res.f <<= 1;
                res.e--;
            }
            res.f <<= (kDiySignificandSize - kDpSignificandSize - 1);
            res.e = res.e - (kDiySignificandSize - kDpSignificandSize - 1);
            return res;
#endif
        }

        DiyFp DiyFp::NormalizeBoundary() const
        {
#if defined(_MSC_VER) && defined(_M_AMD64)
            unsigned long index;
            _BitScanReverse64(&index, f);
            return DiyFp(f << (63 - index), e - (63 - index));
#else
            DiyFp res = *this;
            while (!(res.f & (kDpHiddenBit << 1)))
            {
                res.f <<= 1;
                res.e--;
            }
            res.f <<= (kDiySignificandSize - kDpSignificandSize - 2);
            res.e = res.e - (kDiySignificandSize - kDpSignificandSize - 2);
            return res;
#endif
        }

        void DiyFp::NormalizedBoundaries(DiyFp* minus, DiyFp* plus) const
        {
            DiyFp pl = DiyFp((f << 1) + 1, e - 1).NormalizeBoundary();
            DiyFp mi = (f == kDpHiddenBit) ? DiyFp((f << 2) - 1, e - 2) : DiyFp((f << 1) - 1, e - 1);
            mi.f <<= mi.e - pl.e;
            mi.e = pl.e;
            *plus = pl;
            *minus = mi;
        }

        const int DiyFp::kDiySignificandSize = 64;
        const int DiyFp::kDpSignificandSize = 52;
        const int DiyFp::kDpExponentBias = 0x3FF + kDpSignificandSize;
        const int DiyFp::kDpMinExponent = -kDpExponentBias;
        const unsigned long long DiyFp::kDpExponentMask = UINT64_C2(0x7FF00000, 0x00000000);
        const unsigned long long DiyFp::kDpSignificandMask = UINT64_C2(0x000FFFFF, 0xFFFFFFFF);
        const unsigned long long DiyFp::kDpHiddenBit = UINT64_C2(0x00100000, 0x00000000);


        DiyFp DiyFp::GetCachedPower(int e, int* K)
        {
            // 10^-348, 10^-340,..., 10^340
            static const unsigned long long kCachedPowers_F[] = {
                UINT64_C2(0xfa8fd5a0, 0x081c0288), UINT64_C2(0xbaaee17f, 0xa23ebf76),
                UINT64_C2(0x8b16fb20, 0x3055ac76), UINT64_C2(0xcf42894a, 0x5dce35ea),
                UINT64_C2(0x9a6bb0aa, 0x55653b2d), UINT64_C2(0xe61acf03, 0x3d1a45df),
                UINT64_C2(0xab70fe17, 0xc79ac6ca), UINT64_C2(0xff77b1fc, 0xbebcdc4f),
                UINT64_C2(0xbe5691ef, 0x416bd60c), UINT64_C2(0x8dd01fad, 0x907ffc3c),
                UINT64_C2(0xd3515c28, 0x31559a83), UINT64_C2(0x9d71ac8f, 0xada6c9b5),
                UINT64_C2(0xea9c2277, 0x23ee8bcb), UINT64_C2(0xaecc4991, 0x4078536d),
                UINT64_C2(0x823c1279, 0x5db6ce57), UINT64_C2(0xc2109436, 0x4dfb5637),
                UINT64_C2(0x9096ea6f, 0x3848984f), UINT64_C2(0xd77485cb, 0x25823ac7),
                UINT64_C2(0xa086cfcd, 0x97bf97f4), UINT64_C2(0xef340a98, 0x172aace5),
                UINT64_C2(0xb23867fb, 0x2a35b28e), UINT64_C2(0x84c8d4df, 0xd2c63f3b),
                UINT64_C2(0xc5dd4427, 0x1ad3cdba), UINT64_C2(0x936b9fce, 0xbb25c996),
                UINT64_C2(0xdbac6c24, 0x7d62a584), UINT64_C2(0xa3ab6658, 0x0d5fdaf6),
                UINT64_C2(0xf3e2f893, 0xdec3f126), UINT64_C2(0xb5b5ada8, 0xaaff80b8),
                UINT64_C2(0x87625f05, 0x6c7c4a8b), UINT64_C2(0xc9bcff60, 0x34c13053),
                UINT64_C2(0x964e858c, 0x91ba2655), UINT64_C2(0xdff97724, 0x70297ebd),
                UINT64_C2(0xa6dfbd9f, 0xb8e5b88f), UINT64_C2(0xf8a95fcf, 0x88747d94),
                UINT64_C2(0xb9447093, 0x8fa89bcf), UINT64_C2(0x8a08f0f8, 0xbf0f156b),
                UINT64_C2(0xcdb02555, 0x653131b6), UINT64_C2(0x993fe2c6, 0xd07b7fac),
                UINT64_C2(0xe45c10c4, 0x2a2b3b06), UINT64_C2(0xaa242499, 0x697392d3),
                UINT64_C2(0xfd87b5f2, 0x8300ca0e), UINT64_C2(0xbce50864, 0x92111aeb),
                UINT64_C2(0x8cbccc09, 0x6f5088cc), UINT64_C2(0xd1b71758, 0xe219652c),
                UINT64_C2(0x9c400000, 0x00000000), UINT64_C2(0xe8d4a510, 0x00000000),
                UINT64_C2(0xad78ebc5, 0xac620000), UINT64_C2(0x813f3978, 0xf8940984),
                UINT64_C2(0xc097ce7b, 0xc90715b3), UINT64_C2(0x8f7e32ce, 0x7bea5c70),
                UINT64_C2(0xd5d238a4, 0xabe98068), UINT64_C2(0x9f4f2726, 0x179a2245),
                UINT64_C2(0xed63a231, 0xd4c4fb27), UINT64_C2(0xb0de6538, 0x8cc8ada8),
                UINT64_C2(0x83c7088e, 0x1aab65db), UINT64_C2(0xc45d1df9, 0x42711d9a),
                UINT64_C2(0x924d692c, 0xa61be758), UINT64_C2(0xda01ee64, 0x1a708dea),
                UINT64_C2(0xa26da399, 0x9aef774a), UINT64_C2(0xf209787b, 0xb47d6b85),
                UINT64_C2(0xb454e4a1, 0x79dd1877), UINT64_C2(0x865b8692, 0x5b9bc5c2),
                UINT64_C2(0xc83553c5, 0xc8965d3d), UINT64_C2(0x952ab45c, 0xfa97a0b3),
                UINT64_C2(0xde469fbd, 0x99a05fe3), UINT64_C2(0xa59bc234, 0xdb398c25),
                UINT64_C2(0xf6c69a72, 0xa3989f5c), UINT64_C2(0xb7dcbf53, 0x54e9bece),
                UINT64_C2(0x88fcf317, 0xf22241e2), UINT64_C2(0xcc20ce9b, 0xd35c78a5),
                UINT64_C2(0x98165af3, 0x7b2153df), UINT64_C2(0xe2a0b5dc, 0x971f303a),
                UINT64_C2(0xa8d9d153, 0x5ce3b396), UINT64_C2(0xfb9b7cd9, 0xa4a7443c),
                UINT64_C2(0xbb764c4c, 0xa7a44410), UINT64_C2(0x8bab8eef, 0xb6409c1a),
                UINT64_C2(0xd01fef10, 0xa657842c), UINT64_C2(0x9b10a4e5, 0xe9913129),
                UINT64_C2(0xe7109bfb, 0xa19c0c9d), UINT64_C2(0xac2820d9, 0x623bf429),
                UINT64_C2(0x80444b5e, 0x7aa7cf85), UINT64_C2(0xbf21e440, 0x03acdd2d),
                UINT64_C2(0x8e679c2f, 0x5e44ff8f), UINT64_C2(0xd433179d, 0x9c8cb841),
                UINT64_C2(0x9e19db92, 0xb4e31ba9), UINT64_C2(0xeb96bf6e, 0xbadf77d9),
                UINT64_C2(0xaf87023b, 0x9bf0ee6b)
            };
            static const short kCachedPowers_E[] = {
                -1220, -1193, -1166, -1140, -1113, -1087, -1060, -1034, -1007, -980,
                -954, -927, -901, -874, -847, -821, -794, -768, -741, -715,
                -688, -661, -635, -608, -582, -555, -529, -502, -475, -449,
                -422, -396, -369, -343, -316, -289, -263, -236, -210, -183,
                -157, -130, -103, -77, -50, -24, 3, 30, 56, 83,
                109, 136, 162, 189, 216, 242, 269, 295, 322, 348,
                375, 402, 428, 455, 481, 508, 534, 561, 588, 614,
                641, 667, 694, 720, 747, 774, 800, 827, 853, 880,
                907, 933, 960, 986, 1013, 1039, 1066
            };

            //int k = static_cast<int>(ceil((-61 - e) * 0.30102999566398114)) + 374;
            double dk = (-61 - e) * 0.30102999566398114 + 347;	// dk must be positive, so can do ceiling in positive
            int k = static_cast<int>(dk);
            if (k != dk)
                k++;

            unsigned index = static_cast<unsigned>((k >> 3) + 1);
            *K = -(-348 + static_cast<int>(index << 3));	// decimal exponent no need lookup table

            assert(index < sizeof(kCachedPowers_F) / sizeof(kCachedPowers_F[0]));
            return DiyFp(kCachedPowers_F[index], kCachedPowers_E[index]);
        }

        void DiyFp::GrisuRound(char* buffer, int len, unsigned long long delta, unsigned long long rest, unsigned long long ten_kappa, unsigned long long wp_w)
        {
            while (rest < wp_w && delta - rest >= ten_kappa &&
                (rest + ten_kappa < wp_w ||  /// closer
                    wp_w - rest > rest + ten_kappa - wp_w))
            {
                buffer[len - 1]--;
                rest += ten_kappa;
            }
        }

        unsigned DiyFp::CountDecimalDigit32(unsigned int n)
        {
            // Simple pure C++ implementation was faster than __builtin_clz version in this situation.
            if (n < 10) return 1;
            if (n < 100) return 2;
            if (n < 1000) return 3;
            if (n < 10000) return 4;
            if (n < 100000) return 5;
            if (n < 1000000) return 6;
            if (n < 10000000) return 7;
            if (n < 100000000) return 8;
            if (n < 1000000000) return 9;
            return 10;
        }

        void DiyFp::DigitGen(const DiyFp& W, const DiyFp& Mp, unsigned long long delta, char* buffer, int* len, int* K)
        {
            static const unsigned int kPow10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
            const DiyFp one((unsigned long long)1 << -Mp.e, Mp.e);
            const DiyFp wp_w = Mp - W;
            unsigned int p1 = static_cast<unsigned int>(Mp.f >> -one.e);
            unsigned long long p2 = Mp.f & (one.f - 1);
            int kappa = static_cast<int>(CountDecimalDigit32(p1));
            *len = 0;

            while (kappa > 0)
            {
                unsigned int d;
                switch (kappa)
                {
                case 10: d = p1 / 1000000000; p1 %= 1000000000; break;
                case  9: d = p1 / 100000000; p1 %= 100000000; break;
                case  8: d = p1 / 10000000; p1 %= 10000000; break;
                case  7: d = p1 / 1000000; p1 %= 1000000; break;
                case  6: d = p1 / 100000; p1 %= 100000; break;
                case  5: d = p1 / 10000; p1 %= 10000; break;
                case  4: d = p1 / 1000; p1 %= 1000; break;
                case  3: d = p1 / 100; p1 %= 100; break;
                case  2: d = p1 / 10; p1 %= 10; break;
                case  1: d = p1;              p1 = 0; break;
                default:
#if defined(_MSC_VER)
                    __assume(0);
#elif __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
                    __builtin_unreachable();
#else
                    d = 0;
#endif
                }
                if (d || *len)
                    buffer[(*len)++] = '0' + static_cast<char>(d);
                kappa--;
                unsigned long long tmp = (static_cast<unsigned long long>(p1) << -one.e) + p2;
                if (tmp <= delta)
                {
                    *K += kappa;
                    GrisuRound(buffer, *len, delta, tmp, static_cast<unsigned long long>(kPow10[kappa]) << -one.e, wp_w.f);
                    return;
                }
            }

            // kappa = 0
            for (;;)
            {
                p2 *= 10;
                delta *= 10;
                char d = static_cast<char>(p2 >> -one.e);
                if (d || *len)
                    buffer[(*len)++] = '0' + d;
                p2 &= one.f - 1;
                kappa--;
                if (p2 < delta)
                {
                    *K += kappa;
                    GrisuRound(buffer, *len, delta, p2, one.f, wp_w.f * kPow10[-kappa]);
                    return;
                }
            }
        }

        void DiyFp::Grisu2(double value, char* buffer, int* length, int* K)
        {
            const DiyFp v(value);
            DiyFp w_m, w_p;
            v.NormalizedBoundaries(&w_m, &w_p);

            const DiyFp c_mk = GetCachedPower(w_p.e, K);
            const DiyFp W = v.Normalize() * c_mk;
            DiyFp Wp = w_p * c_mk;
            DiyFp Wm = w_m * c_mk;
            Wm.f++;
            Wp.f--;
            DigitGen(W, Wp, Wp.f - Wm.f, buffer, length, K);
        }

        const char* DiyFp::GetDigitsLut()
        {
            static const char cDigitsLut[200] = {
                '0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9',
                '1', '0', '1', '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8', '1', '9',
                '2', '0', '2', '1', '2', '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
                '3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3', '7', '3', '8', '3', '9',
                '4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5', '4', '6', '4', '7', '4', '8', '4', '9',
                '5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
                '6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8', '6', '9',
                '7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9',
                '8', '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9',
                '9', '0', '9', '1', '9', '2', '9', '3', '9', '4', '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'
            };
            return cDigitsLut;
        }

        int DiyFp::WriteExponent(int K, char* buffer)
        {
            //<
            auto bak = buffer;
            //>
            if (K < 0)
            {
                *buffer++ = '-';
                K = -K;
            }

            if (K >= 100)
            {
                *buffer++ = '0' + static_cast<char>(K / 100);
                K %= 100;
                const char* d = GetDigitsLut() + K * 2;
                *buffer++ = d[0];
                *buffer++ = d[1];
            }
            else if (K >= 10)
            {
                const char* d = GetDigitsLut() + K * 2;
                *buffer++ = d[0];
                *buffer++ = d[1];
            }
            else
                *buffer++ = '0' + static_cast<char>(K);

            *buffer = '\0';
            //<
            return int(buffer - bak);
            //>
        }

        int DiyFp::Prettify(char* buffer, int length, int k)
        {
            const int kk = length + k;	// 10^(kk-1) <= v < 10^kk

            if (length <= kk && kk <= 21)
            {
                // 1234e7 -> 12340000000
                for (int i = length; i < kk; i++)
                    buffer[i] = '0';
                buffer[kk] = '.';
                buffer[kk + 1] = '0';
                buffer[kk + 2] = '\0';
                //<
                return kk + 2;
                //>
            }
            else if (0 < kk && kk <= 21)
            {
                // 1234e-2 -> 12.34
                memmove(&buffer[kk + 1], &buffer[kk], length - kk);
                buffer[kk] = '.';
                buffer[length + 1] = '\0';
                //<
                return length + 1;
                //>
            }
            else if (-6 < kk && kk <= 0)
            {
                // 1234e-6 -> 0.001234
                const int offset = 2 - kk;
                memmove(&buffer[offset], &buffer[0], length);
                buffer[0] = '0';
                buffer[1] = '.';
                for (int i = 2; i < offset; i++)
                    buffer[i] = '0';
                buffer[length + offset] = '\0';
                //<
                return length + offset;
                //>
            }
            else if (length == 1)
            {
                // 1e30
                buffer[1] = 'e';
                //<
                auto tmp = WriteExponent(kk - 1, &buffer[2]);
                return 2 + tmp;
                //>
            }
            else
            {
                // 1234e30 -> 1.234e33
                memmove(&buffer[2], &buffer[1], length - 1);
                buffer[1] = '.';
                buffer[length + 1] = 'e';
                //<
                auto tmp = WriteExponent(kk - 1, &buffer[0 + length + 2]);
                return 0 + length + 2 + tmp;
                //>
            }
        }

        int DiyFp::dtoa_milo(double value, char* buffer)
        {
            // Not handling NaN and inf
            //<
            assert(!std::isnan(value));
            assert(!std::isinf(value));
            //>

            if (value == 0)
            {
                buffer[0] = '0';
                buffer[1] = '.';
                buffer[2] = '0';
                buffer[3] = '\0';
                //<
                return 3;
                //>
            }
            else
            {
                //<
                bool isNegative = value < 0;
                //>
                if (value < 0)
                {
                    *buffer++ = '-';
                    value = -value;
                }
                int length, K;
                Grisu2(value, buffer, &length, &K);
                return Prettify(buffer, length, K) + (isNegative ? 1 : 0);
            }
        }















        // the following code is copy from
        // https://github.com/miloyip/itoa-benchmark/tree/master/src

        const char gDigitsLut[200] = {
            '0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9',
            '1', '0', '1', '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8', '1', '9',
            '2', '0', '2', '1', '2', '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
            '3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3', '7', '3', '8', '3', '9',
            '4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5', '4', '6', '4', '7', '4', '8', '4', '9',
            '5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
            '6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8', '6', '9',
            '7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9',
            '8', '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9',
            '9', '0', '9', '1', '9', '2', '9', '3', '9', '4', '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'
        };

        // Branching for different cases (forward)
        // Use lookup table of two digits

        size_t u32toa_branchlut(unsigned int value, char* buffer)
        {
            auto bak = buffer;
            if (value < 10000)
            {
                const unsigned int d1 = (value / 100) << 1;
                const unsigned int d2 = (value % 100) << 1;

                if (value >= 1000)
                    *buffer++ = gDigitsLut[d1];
                if (value >= 100)
                    *buffer++ = gDigitsLut[d1 + 1];
                if (value >= 10)
                    *buffer++ = gDigitsLut[d2];
                *buffer++ = gDigitsLut[d2 + 1];
            }
            else if (value < 100000000)
            {
                // value = bbbbcccc
                const unsigned int b = value / 10000;
                const unsigned int c = value % 10000;

                const unsigned int d1 = (b / 100) << 1;
                const unsigned int d2 = (b % 100) << 1;

                const unsigned int d3 = (c / 100) << 1;
                const unsigned int d4 = (c % 100) << 1;

                if (value >= 10000000)
                    *buffer++ = gDigitsLut[d1];
                if (value >= 1000000)
                    *buffer++ = gDigitsLut[d1 + 1];
                if (value >= 100000)
                    *buffer++ = gDigitsLut[d2];
                *buffer++ = gDigitsLut[d2 + 1];

                *buffer++ = gDigitsLut[d3];
                *buffer++ = gDigitsLut[d3 + 1];
                *buffer++ = gDigitsLut[d4];
                *buffer++ = gDigitsLut[d4 + 1];
            }
            else
            {
                // value = aabbbbcccc in decimal

                const unsigned int a = value / 100000000; // 1 to 42
                value %= 100000000;

                if (a >= 10)
                {
                    const unsigned i = a << 1;
                    *buffer++ = gDigitsLut[i];
                    *buffer++ = gDigitsLut[i + 1];
                }
                else
                    *buffer++ = '0' + static_cast<char>(a);

                const unsigned int b = value / 10000; // 0 to 9999
                const unsigned int c = value % 10000; // 0 to 9999

                const unsigned int d1 = (b / 100) << 1;
                const unsigned int d2 = (b % 100) << 1;

                const unsigned int d3 = (c / 100) << 1;
                const unsigned int d4 = (c % 100) << 1;

                *buffer++ = gDigitsLut[d1];
                *buffer++ = gDigitsLut[d1 + 1];
                *buffer++ = gDigitsLut[d2];
                *buffer++ = gDigitsLut[d2 + 1];
                *buffer++ = gDigitsLut[d3];
                *buffer++ = gDigitsLut[d3 + 1];
                *buffer++ = gDigitsLut[d4];
                *buffer++ = gDigitsLut[d4 + 1];
            }
            //*buffer++ = '\0';
            return size_t(buffer - bak);
        }

        size_t i32toa_branchlut(int value, char* buffer)
        {
            unsigned int u = static_cast<unsigned int>(value);
            if (value < 0)
            {
                *buffer++ = '-';
                u = ~u + 1;
                return u32toa_branchlut(u, buffer) + 1;
            }
            return u32toa_branchlut(u, buffer);
        }

        size_t u64toa_branchlut(unsigned long long value, char* buffer)
        {
            auto bak = buffer;
            if (value < 100000000)
            {
                unsigned int v = static_cast<unsigned int>(value);
                if (v < 10000)
                {
                    const unsigned int d1 = (v / 100) << 1;
                    const unsigned int d2 = (v % 100) << 1;

                    if (v >= 1000)
                        *buffer++ = gDigitsLut[d1];
                    if (v >= 100)
                        *buffer++ = gDigitsLut[d1 + 1];
                    if (v >= 10)
                        *buffer++ = gDigitsLut[d2];
                    *buffer++ = gDigitsLut[d2 + 1];
                }
                else
                {
                    // value = bbbbcccc
                    const unsigned int b = v / 10000;
                    const unsigned int c = v % 10000;

                    const unsigned int d1 = (b / 100) << 1;
                    const unsigned int d2 = (b % 100) << 1;

                    const unsigned int d3 = (c / 100) << 1;
                    const unsigned int d4 = (c % 100) << 1;

                    if (value >= 10000000)
                        *buffer++ = gDigitsLut[d1];
                    if (value >= 1000000)
                        *buffer++ = gDigitsLut[d1 + 1];
                    if (value >= 100000)
                        *buffer++ = gDigitsLut[d2];
                    *buffer++ = gDigitsLut[d2 + 1];

                    *buffer++ = gDigitsLut[d3];
                    *buffer++ = gDigitsLut[d3 + 1];
                    *buffer++ = gDigitsLut[d4];
                    *buffer++ = gDigitsLut[d4 + 1];
                }
            }
            else if (value < 10000000000000000)
            {
                const unsigned int v0 = static_cast<unsigned int>(value / 100000000);
                const unsigned int v1 = static_cast<unsigned int>(value % 100000000);

                const unsigned int b0 = v0 / 10000;
                const unsigned int c0 = v0 % 10000;

                const unsigned int d1 = (b0 / 100) << 1;
                const unsigned int d2 = (b0 % 100) << 1;

                const unsigned int d3 = (c0 / 100) << 1;
                const unsigned int d4 = (c0 % 100) << 1;

                const unsigned int b1 = v1 / 10000;
                const unsigned int c1 = v1 % 10000;

                const unsigned int d5 = (b1 / 100) << 1;
                const unsigned int d6 = (b1 % 100) << 1;

                const unsigned int d7 = (c1 / 100) << 1;
                const unsigned int d8 = (c1 % 100) << 1;

                if (value >= 1000000000000000)
                    *buffer++ = gDigitsLut[d1];
                if (value >= 100000000000000)
                    *buffer++ = gDigitsLut[d1 + 1];
                if (value >= 10000000000000)
                    *buffer++ = gDigitsLut[d2];
                if (value >= 1000000000000)
                    *buffer++ = gDigitsLut[d2 + 1];
                if (value >= 100000000000)
                    *buffer++ = gDigitsLut[d3];
                if (value >= 10000000000)
                    *buffer++ = gDigitsLut[d3 + 1];
                if (value >= 1000000000)
                    *buffer++ = gDigitsLut[d4];
                if (value >= 100000000)
                    *buffer++ = gDigitsLut[d4 + 1];

                *buffer++ = gDigitsLut[d5];
                *buffer++ = gDigitsLut[d5 + 1];
                *buffer++ = gDigitsLut[d6];
                *buffer++ = gDigitsLut[d6 + 1];
                *buffer++ = gDigitsLut[d7];
                *buffer++ = gDigitsLut[d7 + 1];
                *buffer++ = gDigitsLut[d8];
                *buffer++ = gDigitsLut[d8 + 1];
            }
            else
            {
                const unsigned int a = static_cast<unsigned int>(value / 10000000000000000); // 1 to 1844
                value %= 10000000000000000;

                if (a < 10)
                    *buffer++ = '0' + static_cast<char>(a);
                else if (a < 100)
                {
                    const unsigned int i = a << 1;
                    *buffer++ = gDigitsLut[i];
                    *buffer++ = gDigitsLut[i + 1];
                }
                else if (a < 1000)
                {
                    *buffer++ = '0' + static_cast<char>(a / 100);

                    const unsigned int i = (a % 100) << 1;
                    *buffer++ = gDigitsLut[i];
                    *buffer++ = gDigitsLut[i + 1];
                }
                else
                {
                    const unsigned int i = (a / 100) << 1;
                    const unsigned int j = (a % 100) << 1;
                    *buffer++ = gDigitsLut[i];
                    *buffer++ = gDigitsLut[i + 1];
                    *buffer++ = gDigitsLut[j];
                    *buffer++ = gDigitsLut[j + 1];
                }

                const unsigned int v0 = static_cast<unsigned int>(value / 100000000);
                const unsigned int v1 = static_cast<unsigned int>(value % 100000000);

                const unsigned int b0 = v0 / 10000;
                const unsigned int c0 = v0 % 10000;

                const unsigned int d1 = (b0 / 100) << 1;
                const unsigned int d2 = (b0 % 100) << 1;

                const unsigned int d3 = (c0 / 100) << 1;
                const unsigned int d4 = (c0 % 100) << 1;

                const unsigned int b1 = v1 / 10000;
                const unsigned int c1 = v1 % 10000;

                const unsigned int d5 = (b1 / 100) << 1;
                const unsigned int d6 = (b1 % 100) << 1;

                const unsigned int d7 = (c1 / 100) << 1;
                const unsigned int d8 = (c1 % 100) << 1;

                *buffer++ = gDigitsLut[d1];
                *buffer++ = gDigitsLut[d1 + 1];
                *buffer++ = gDigitsLut[d2];
                *buffer++ = gDigitsLut[d2 + 1];
                *buffer++ = gDigitsLut[d3];
                *buffer++ = gDigitsLut[d3 + 1];
                *buffer++ = gDigitsLut[d4];
                *buffer++ = gDigitsLut[d4 + 1];
                *buffer++ = gDigitsLut[d5];
                *buffer++ = gDigitsLut[d5 + 1];
                *buffer++ = gDigitsLut[d6];
                *buffer++ = gDigitsLut[d6 + 1];
                *buffer++ = gDigitsLut[d7];
                *buffer++ = gDigitsLut[d7 + 1];
                *buffer++ = gDigitsLut[d8];
                *buffer++ = gDigitsLut[d8 + 1];
            }

            //*buffer = '\0';
            return size_t(buffer - bak);
        }

        size_t i64toa_branchlut(long long value, char* buffer)
        {
            unsigned long long u = static_cast<unsigned long long>(value);
            if (value < 0)
            {
                *buffer++ = '-';
                u = ~u + 1;
                return u64toa_branchlut(u, buffer) + 1;
            }
            return u64toa_branchlut(u, buffer);
        }












        // char* 转为各种长度的 有符号整数. Out 取值范围： int8~64
        template <typename OutType>
        void toInt(char const * in, OutType & out)
        {
            auto in_ = in;
            if (*in_ == '0')
            {
                out = 0;
                return;
            }
            bool b;
            if (*in_ == '-')
            {
                b = true;
                ++in_;
            }
            else b = false;
            OutType r = *in_ - '0';
            char c;
            while ((c = *(++in_))) r = r * 10 + (c - '0');
            out = b ? -r : r;
        }

        // char* (不能有减号打头) 转为各种长度的 无符号整数. Out 取值范围： uint8, uint16, uint32, uint64
        template <typename OutType>
        void toUInt(char const * in, OutType & out)
        {
            assert(in);
            auto in_ = in;
            if (*in_ == '0')
            {
                out = 0;
                return;
            }
            OutType r = *(in_)-'0';
            char c;
            while ((c = *(++in_))) r = r * 10 + (c - '0');
            out = r;
        }

        void FromString(unsigned char &out, char const *in)
        {
            toUInt(in, out);
        }
        void FromString(unsigned short &out, char const *in)
        {
            toUInt(in, out);
        }
        void FromString(unsigned int &out, char const *in)
        {
            toUInt(in, out);
        }
        void FromString(unsigned long long &out, char const *in)
        {
            toUInt(in, out);
        }
        void FromString(signed char &out, char const *in)
        {
            toInt(in, out);
        }
        void FromString(short &out, char const *in)
        {
            toInt(in, out);
        }
        void FromString(int &out, char const *in)
        {
            toInt(in, out);
        }
        void FromString(long long &out, char const *in)
        {
            toInt(in, out);
        }
        void FromString(double &out, char const *in)
        {
            out = strtod(in, nullptr);
        }
        void FromString(float &out, char const *in)
        {
            out = (float)strtod(in, nullptr);
        }
        void FromString(bool &out, char const *in)
        {
            out = (in[0] == '1' || in[0] == 'T' || in[0] == 't');
        }
        void FromString(long &out, char const *in)
        {
#ifdef _MSC_VER
            return FromString((int&)out, in);
#else
# ifdef XXLIB_64BIT
            return FromString((long long&)out, in);
# else
            return FromString((int&)out, in);
# endif
#endif

        }
        void FromString(unsigned long &out, char const *in)
        {
#ifdef _MSC_VER
            return FromString((unsigned int&)out, in);
#else
# ifdef XXLIB_64BIT
            return FromString((unsigned long long&)out, in);
# else
            return FromString((unsigned int&)out, in);
# endif
#endif
        }


    }
}
