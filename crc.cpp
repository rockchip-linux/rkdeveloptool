/*
 * (C) Copyright 2017 Fuzhou Rockchip Electronics Co., Ltd
 * Seth Liu 2017.03.01
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include "DefineHeader.h"
UINT gTable_Crc32[256] =
{//crfc32 factor 0x04C10DB7
		0x00000000, 0x04c10db7, 0x09821b6e, 0x0d4316d9,
		0x130436dc, 0x17c53b6b, 0x1a862db2, 0x1e472005,
		0x26086db8, 0x22c9600f, 0x2f8a76d6, 0x2b4b7b61,
		0x350c5b64, 0x31cd56d3, 0x3c8e400a, 0x384f4dbd,
		0x4c10db70, 0x48d1d6c7, 0x4592c01e, 0x4153cda9,
		0x5f14edac, 0x5bd5e01b, 0x5696f6c2, 0x5257fb75,
		0x6a18b6c8, 0x6ed9bb7f, 0x639aada6, 0x675ba011,
		0x791c8014, 0x7ddd8da3, 0x709e9b7a, 0x745f96cd,
		0x9821b6e0, 0x9ce0bb57, 0x91a3ad8e, 0x9562a039,
		0x8b25803c, 0x8fe48d8b, 0x82a79b52, 0x866696e5,
		0xbe29db58, 0xbae8d6ef, 0xb7abc036, 0xb36acd81,
		0xad2ded84, 0xa9ece033, 0xa4aff6ea, 0xa06efb5d,
		0xd4316d90, 0xd0f06027, 0xddb376fe, 0xd9727b49,
		0xc7355b4c, 0xc3f456fb, 0xceb74022, 0xca764d95,
		0xf2390028, 0xf6f80d9f, 0xfbbb1b46, 0xff7a16f1,
		0xe13d36f4, 0xe5fc3b43, 0xe8bf2d9a, 0xec7e202d,
		0x34826077, 0x30436dc0, 0x3d007b19, 0x39c176ae,
		0x278656ab, 0x23475b1c, 0x2e044dc5, 0x2ac54072,
		0x128a0dcf, 0x164b0078, 0x1b0816a1, 0x1fc91b16,
		0x018e3b13, 0x054f36a4, 0x080c207d, 0x0ccd2dca,
		0x7892bb07, 0x7c53b6b0, 0x7110a069, 0x75d1adde,
		0x6b968ddb, 0x6f57806c, 0x621496b5, 0x66d59b02,
		0x5e9ad6bf, 0x5a5bdb08, 0x5718cdd1, 0x53d9c066,
		0x4d9ee063, 0x495fedd4, 0x441cfb0d, 0x40ddf6ba,
		0xaca3d697, 0xa862db20, 0xa521cdf9, 0xa1e0c04e,
		0xbfa7e04b, 0xbb66edfc, 0xb625fb25, 0xb2e4f692,
		0x8aabbb2f, 0x8e6ab698, 0x8329a041, 0x87e8adf6,
		0x99af8df3, 0x9d6e8044, 0x902d969d, 0x94ec9b2a,
		0xe0b30de7, 0xe4720050, 0xe9311689, 0xedf01b3e,
		0xf3b73b3b, 0xf776368c, 0xfa352055, 0xfef42de2,
		0xc6bb605f, 0xc27a6de8, 0xcf397b31, 0xcbf87686,
		0xd5bf5683, 0xd17e5b34, 0xdc3d4ded, 0xd8fc405a,
		0x6904c0ee, 0x6dc5cd59, 0x6086db80, 0x6447d637,
		0x7a00f632, 0x7ec1fb85, 0x7382ed5c, 0x7743e0eb,
		0x4f0cad56, 0x4bcda0e1, 0x468eb638, 0x424fbb8f,
		0x5c089b8a, 0x58c9963d, 0x558a80e4, 0x514b8d53,
		0x25141b9e, 0x21d51629, 0x2c9600f0, 0x28570d47,
		0x36102d42, 0x32d120f5, 0x3f92362c, 0x3b533b9b,
		0x031c7626, 0x07dd7b91, 0x0a9e6d48, 0x0e5f60ff,
		0x101840fa, 0x14d94d4d, 0x199a5b94, 0x1d5b5623,
		0xf125760e, 0xf5e47bb9, 0xf8a76d60, 0xfc6660d7,
		0xe22140d2, 0xe6e04d65, 0xeba35bbc, 0xef62560b,
		0xd72d1bb6, 0xd3ec1601, 0xdeaf00d8, 0xda6e0d6f,
		0xc4292d6a, 0xc0e820dd, 0xcdab3604, 0xc96a3bb3,
		0xbd35ad7e, 0xb9f4a0c9, 0xb4b7b610, 0xb076bba7,
		0xae319ba2, 0xaaf09615, 0xa7b380cc, 0xa3728d7b,
		0x9b3dc0c6, 0x9ffccd71, 0x92bfdba8, 0x967ed61f,
		0x8839f61a, 0x8cf8fbad, 0x81bbed74, 0x857ae0c3,
		0x5d86a099, 0x5947ad2e, 0x5404bbf7, 0x50c5b640,
		0x4e829645, 0x4a439bf2, 0x47008d2b, 0x43c1809c,
		0x7b8ecd21, 0x7f4fc096, 0x720cd64f, 0x76cddbf8,
		0x688afbfd, 0x6c4bf64a, 0x6108e093, 0x65c9ed24,
		0x11967be9, 0x1557765e, 0x18146087, 0x1cd56d30,
		0x02924d35, 0x06534082, 0x0b10565b, 0x0fd15bec,
		0x379e1651, 0x335f1be6, 0x3e1c0d3f, 0x3add0088,
		0x249a208d, 0x205b2d3a, 0x2d183be3, 0x29d93654,
		0xc5a71679, 0xc1661bce, 0xcc250d17, 0xc8e400a0,
		0xd6a320a5, 0xd2622d12, 0xdf213bcb, 0xdbe0367c,
		0xe3af7bc1, 0xe76e7676, 0xea2d60af, 0xeeec6d18,
		0xf0ab4d1d, 0xf46a40aa, 0xf9295673, 0xfde85bc4,
		0x89b7cd09, 0x8d76c0be, 0x8035d667, 0x84f4dbd0,
		0x9ab3fbd5, 0x9e72f662, 0x9331e0bb, 0x97f0ed0c,
		0xafbfa0b1, 0xab7ead06, 0xa63dbbdf, 0xa2fcb668,
		0xbcbb966d, 0xb87a9bda, 0xb5398d03, 0xb1f880b4,
};
#define rr_max  104	/* Number of parity checks, rr = deg[g(x)] */
#define parallel 8 //bit count
#define mm 13//limit count
#define nn 8191//code size
#define kk 4120//info length
#define tt 8//correct count

#define tt2 2*tt
UINT s[tt2+1]; // Syndrome values

UINT rr;//redundant length		// BCH code parameters


UINT p[mm + 1];
UINT alpha_to[nn+1], index_of[nn+1] ;	// Galois field
UINT gg[rr_max+1] ;		// Generator polynomial

UINT ggx1=0;
UINT ggx2=0;
UINT ggx3=0;
UINT ggx4=0;
// get crc32 value
UINT CRC_32(unsigned char* pData, UINT ulSize)
{
    UINT i;
    UINT nAccum = 0;

    for ( i=0; i<ulSize; i++)
        nAccum = (nAccum<<8)^gTable_Crc32[(nAccum>>24)^(*pData++)];
    return nAccum;
}
#define CRC16_CCITT         0x1021  //CRC operator
void CRCBuildTable16(unsigned short aPoly , unsigned short *crcTable)
{
    unsigned short i, j;
    unsigned short nData;
    unsigned short nAccum;

    for (i = 0; i < 256; i++)
    {
        nData = (unsigned short)(i << 8);
        nAccum = 0;
        for (j = 0; j < 8; j++)
        {
            if ((nData ^ nAccum) & 0x8000)
                nAccum = (nAccum << 1) ^ aPoly;
            else
                nAccum <<= 1;
            nData <<= 1;
        }
        crcTable[i] = nAccum;
    }
}

unsigned short CRC_16(unsigned char* aData, UINT aSize)
{
    UINT i;
    unsigned short nAccum = 0;
    unsigned short crcTable[256];

    CRCBuildTable16(CRC16_CCITT , crcTable);
    for (i = 0; i < aSize; i++)
        nAccum = (nAccum << 8) ^ crcTable[(nAccum >> 8) ^ *aData++];

    return nAccum;
}

void P_RC4(unsigned char* buf, unsigned short len)
{
	unsigned char S[256],K[256],temp;
	unsigned short i,j,t,x;
	unsigned char key[16]={124,78,3,4,85,5,9,7,45,44,123,56,23,13,23,17};

	j = 0;
	for(i=0; i<256; i++){
		S[i] = (unsigned char)i;
		j&=0x0f;
		K[i] = key[j];
		j++;
	}

	j = 0;
	for(i=0; i<256; i++){
		j = (j + S[i] + K[i]) % 256;
		temp = S[i];
		S[i] = S[j];
		S[j] = temp;
	}

	i = j = 0;
	for(x=0; x<len; x++){
		i = (i+1) % 256;
		j = (j + S[i]) % 256;
		temp = S[i];
		S[i] = S[j];
		S[j] = temp;
		t = (S[i] + (S[j] % 256)) % 256;
		buf[x] = buf[x] ^ S[t];
	}
}

void bch_encode(unsigned char* encode_in, unsigned char* encode_out)
{
	UINT i,j;
	bool feed_back;
	UINT bch1=0;
	UINT bch2=0;
	UINT bch3=0;
	UINT bch4=0;

	for (i=0;i<515;i++)
	{
		for (j=0;j<8;j++)
		{
			feed_back = (bch1&1) ^ ((encode_in[i]>>j) & 1);
			bch1=((bch1>>1)|((bch2&1)*0x80000000))^(ggx1*feed_back);
			bch2=((bch2>>1)|((bch3&1)*0x80000000))^(ggx2*feed_back);
			bch3=((bch3>>1)|((bch4&1)*0x80000000))^(ggx3*feed_back);
			bch4=(((bch4>>1)^(ggx4*feed_back))) | (feed_back*0x80);
		}
	}

	//********Handle FF***********************
	bch1 = ~(bch1 ^ 0xad6273b1);
	bch2 = ~(bch2 ^ 0x348393d2);
	bch3 = ~(bch3 ^ 0xe6ebed3c);
	bch4 = ~(bch4 ^ 0xc8);
	//*********************************************

	for (i=0;i<515;i++)
		encode_out[i] = encode_in[i];
	encode_out[515] = bch1&0x000000ff;
	encode_out[516] = (bch1&0x0000ff00)>>8;
	encode_out[517] = (bch1&0x00ff0000)>>16;
	encode_out[518] = (bch1&0xff000000)>>24;
	encode_out[519] = bch2&0x000000ff;
	encode_out[520] = (bch2&0x0000ff00)>>8;
	encode_out[521] = (bch2&0x00ff0000)>>16;
	encode_out[522] = (bch2&0xff000000)>>24;
	encode_out[523] = bch3&0x000000ff;
	encode_out[524] = (bch3&0x0000ff00)>>8;
	encode_out[525] = (bch3&0x00ff0000)>>16;
	encode_out[526] = (bch3&0xff000000)>>24;
	encode_out[527] = bch4&0x000000ff;
}

#define poly16_CCITT	0x1021          /* crc-ccitt mask */

unsigned short CRC_Calculate(unsigned short crc, unsigned char ch)
{
	UINT i;
	for(i=0x80; i!=0; i>>=1)
	{
		if((crc & 0x8000) != 0)
		{
			crc <<= 1;
			crc ^= poly16_CCITT;
		}
		else
			crc <<= 1;
		if((ch & i)!=0)
			crc ^= poly16_CCITT;
	}
	return crc;
}
unsigned short CRC_CCITT(unsigned char* p, UINT CalculateNumber)
{
    unsigned short crc = 0xffff;
    while(CalculateNumber--)
	{
		crc = CRC_Calculate(crc, *p);
		p++;
    }
    return crc;
}
void gen_poly()
{
	UINT gen_roots[nn + 1], gen_roots_true[nn + 1] ; 	// Roots of generator polynomial
	UINT i, j, Temp ;

// Initialization of gen_roots
	for (i = 0; i <= nn; i++)
	{	gen_roots_true[i] = 0;
		gen_roots[i] = 0;
	}

// Cyclotomic cosets of gen_roots
	for (i = 1; i <= 2*tt ; i++)
	{
		for (j = 0; j < mm; j++)
		{
			Temp = ((1<<j)*i)%nn;
			gen_roots_true[Temp] = 1;
		}
	}
	rr = 0;		// Count thenumber of parity check bits
	for (i = 0; i < nn; i++)
	{
		if (gen_roots_true[i] == 1)
		{
			rr++;
			gen_roots[rr] = i;
		}
	}
// Compute generator polynomial based on its roots
	gg[0] = 2 ;	// g(x) = (X + alpha) initially
	gg[1] = 1 ;
	for (i = 2; i <= rr; i++)
	{
		gg[i] = 1 ;
		for (j = i - 1; j > 0; j--)
			if (gg[j] != 0)
				gg[j] = gg[j-1]^ alpha_to[(index_of[gg[j]] + index_of[alpha_to[gen_roots[i]]]) % nn] ;
			else
				gg[j] = gg[j-1] ;
		gg[0] = alpha_to[(index_of[gg[0]] + index_of[alpha_to[gen_roots[i]]]) % nn] ;
	}

ggx1 = gg[103] | (gg[102]<<1) | (gg[101]<<2) | (gg[100]<<3) | (gg[99]<<4) |(gg[98]<<5)| (gg[97]<<6)|(gg[96]<<7)
	| (gg[95]<<8) | (gg[94]<<9) | (gg[93]<<10) | (gg[92]<<11) |(gg[91]<<12)| (gg[90]<<13)|(gg[89]<<14) |(gg[88]<<15)
	| (gg[87]<<16) | (gg[86]<<17) | (gg[85]<<18) | (gg[84]<<19) | (gg[83]<<20) |(gg[82]<<21)| (gg[81]<<22)|(gg[80]<<23)
	| (gg[79]<<24) | (gg[78]<<25) | (gg[77]<<26) | (gg[76]<<27) |(gg[75]<<28)| (gg[74]<<29)|(gg[73]<<30) |(gg[72]<<31);
ggx2 = gg[71] | (gg[70]<<1) | (gg[69]<<2) | (gg[68]<<3) | (gg[67]<<4) |(gg[66]<<5)| (gg[65]<<6)|(gg[64]<<7)
	| (gg[63]<<8) | (gg[62]<<9) | (gg[61]<<10) | (gg[60]<<11) |(gg[59]<<12)| (gg[58]<<13)|(gg[57]<<14) |(gg[56]<<15)
	| (gg[55]<<16) | (gg[54]<<17) | (gg[53]<<18) | (gg[52]<<19) | (gg[51]<<20) |(gg[50]<<21)| (gg[49]<<22)|(gg[48]<<23)
	| (gg[47]<<24) | (gg[46]<<25) | (gg[45]<<26) | (gg[44]<<27) |(gg[43]<<28)| (gg[42]<<29)|(gg[41]<<30) |(gg[40]<<31);
ggx3 = gg[39] | (gg[38]<<1) | (gg[37]<<2) | (gg[36]<<3) | (gg[35]<<4) |(gg[34]<<5)| (gg[33]<<6)|(gg[32]<<7)
	| (gg[31]<<8) | (gg[30]<<9) | (gg[29]<<10) | (gg[28]<<11) |(gg[27]<<12)| (gg[26]<<13)|(gg[25]<<14) |(gg[24]<<15)
	| (gg[23]<<16) | (gg[22]<<17) | (gg[21]<<18) | (gg[20]<<19) | (gg[19]<<20) |(gg[18]<<21)| (gg[17]<<22)|(gg[16]<<23)
	| (gg[15]<<24) | (gg[14]<<25) | (gg[13]<<26) | (gg[12]<<27) |(gg[11]<<28)| (gg[10]<<29)|(gg[9]<<30) |(gg[8]<<31);
ggx4 = gg[7] | (gg[6]<<1) | (gg[5]<<2) | (gg[4]<<3) | (gg[3]<<4) |(gg[2]<<5)| (gg[1]<<6);

}

void generate_gf()
{
	UINT i;
	UINT mask ;	// Register states

	// Primitive polynomials
	for (i = 1; i < mm; i++)
		p[i] = 0;
	p[0] = p[mm] = 1;
	if (mm == 2)		p[1] = 1;
	else if (mm == 3)	p[1] = 1;
	else if (mm == 4)	p[1] = 1;
	else if (mm == 5)	p[2] = 1;
	else if (mm == 6)	p[1] = 1;
	else if (mm == 7)	p[1] = 1;
	else if (mm == 8)	p[4] = p[5] = p[6] = 1;
	else if (mm == 9)	p[4] = 1;
	else if (mm == 10)	p[3] = 1;
	else if (mm == 11)	p[2] = 1;
	else if (mm == 12)	p[3] = p[4] = p[7] = 1;
	else if (mm == 13)	p[1] = p[2] = p[3] = p[5] = p[7] = p[8] = p[10] = 1;	// 25AF
	else if (mm == 14)	p[2] = p[4] = p[6] = p[7] = p[8] = 1;	// 41D5
	else if (mm == 15)	p[1] = 1;
	else if (mm == 16)	p[2] = p[3] = p[5] = 1;
	else if (mm == 17)	p[3] = 1;
	else if (mm == 18)	p[7] = 1;
	else if (mm == 19)	p[1] = p[5] = p[6] = 1;
	else if (mm == 20)	p[3] = 1;
	// Galois field implementation with shift registers
	// Ref: L&C, Chapter 6.7, pp. 217
	mask = 1 ;
	alpha_to[mm] = 0 ;
	for (i = 0; i < mm; i++)
	{
		alpha_to[i] = mask ;
		index_of[alpha_to[i]] = i ;
		if (p[i] != 0)
			alpha_to[mm] ^= mask ;
		mask <<= 1 ;
	}

	index_of[alpha_to[mm]] = mm ;
	mask >>= 1 ;
	for (i = mm + 1; i < nn; i++)
	{
		if (alpha_to[i-1] >= mask)
			alpha_to[i] = alpha_to[mm] ^ ((alpha_to[i-1] ^ mask) << 1) ;
		else
			alpha_to[i] = alpha_to[i-1] << 1 ;

		index_of[alpha_to[i]] = i ;
	}
	index_of[0] = -1 ;
}