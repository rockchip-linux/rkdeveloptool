/*
 * (C) Copyright 2017 Fuzhou Rockchip Electronics Co., Ltd
 * Seth Liu 2017.03.01
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include "DefineHeader.h"
UINT gTable_Crc32[256] =
{//crc32 factor 0x04C10DB7
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
#define tole(x)		(x)
/*factor is 0xedb88320*/
unsigned int crc32table_le[] = {
tole(0x00000000L), tole(0x77073096L), tole(0xee0e612cL), tole(0x990951baL),
tole(0x076dc419L), tole(0x706af48fL), tole(0xe963a535L), tole(0x9e6495a3L),
tole(0x0edb8832L), tole(0x79dcb8a4L), tole(0xe0d5e91eL), tole(0x97d2d988L),
tole(0x09b64c2bL), tole(0x7eb17cbdL), tole(0xe7b82d07L), tole(0x90bf1d91L),
tole(0x1db71064L), tole(0x6ab020f2L), tole(0xf3b97148L), tole(0x84be41deL),
tole(0x1adad47dL), tole(0x6ddde4ebL), tole(0xf4d4b551L), tole(0x83d385c7L),
tole(0x136c9856L), tole(0x646ba8c0L), tole(0xfd62f97aL), tole(0x8a65c9ecL),
tole(0x14015c4fL), tole(0x63066cd9L), tole(0xfa0f3d63L), tole(0x8d080df5L),
tole(0x3b6e20c8L), tole(0x4c69105eL), tole(0xd56041e4L), tole(0xa2677172L),
tole(0x3c03e4d1L), tole(0x4b04d447L), tole(0xd20d85fdL), tole(0xa50ab56bL),
tole(0x35b5a8faL), tole(0x42b2986cL), tole(0xdbbbc9d6L), tole(0xacbcf940L),
tole(0x32d86ce3L), tole(0x45df5c75L), tole(0xdcd60dcfL), tole(0xabd13d59L),
tole(0x26d930acL), tole(0x51de003aL), tole(0xc8d75180L), tole(0xbfd06116L),
tole(0x21b4f4b5L), tole(0x56b3c423L), tole(0xcfba9599L), tole(0xb8bda50fL),
tole(0x2802b89eL), tole(0x5f058808L), tole(0xc60cd9b2L), tole(0xb10be924L),
tole(0x2f6f7c87L), tole(0x58684c11L), tole(0xc1611dabL), tole(0xb6662d3dL),
tole(0x76dc4190L), tole(0x01db7106L), tole(0x98d220bcL), tole(0xefd5102aL),
tole(0x71b18589L), tole(0x06b6b51fL), tole(0x9fbfe4a5L), tole(0xe8b8d433L),
tole(0x7807c9a2L), tole(0x0f00f934L), tole(0x9609a88eL), tole(0xe10e9818L),
tole(0x7f6a0dbbL), tole(0x086d3d2dL), tole(0x91646c97L), tole(0xe6635c01L),
tole(0x6b6b51f4L), tole(0x1c6c6162L), tole(0x856530d8L), tole(0xf262004eL),
tole(0x6c0695edL), tole(0x1b01a57bL), tole(0x8208f4c1L), tole(0xf50fc457L),
tole(0x65b0d9c6L), tole(0x12b7e950L), tole(0x8bbeb8eaL), tole(0xfcb9887cL),
tole(0x62dd1ddfL), tole(0x15da2d49L), tole(0x8cd37cf3L), tole(0xfbd44c65L),
tole(0x4db26158L), tole(0x3ab551ceL), tole(0xa3bc0074L), tole(0xd4bb30e2L),
tole(0x4adfa541L), tole(0x3dd895d7L), tole(0xa4d1c46dL), tole(0xd3d6f4fbL),
tole(0x4369e96aL), tole(0x346ed9fcL), tole(0xad678846L), tole(0xda60b8d0L),
tole(0x44042d73L), tole(0x33031de5L), tole(0xaa0a4c5fL), tole(0xdd0d7cc9L),
tole(0x5005713cL), tole(0x270241aaL), tole(0xbe0b1010L), tole(0xc90c2086L),
tole(0x5768b525L), tole(0x206f85b3L), tole(0xb966d409L), tole(0xce61e49fL),
tole(0x5edef90eL), tole(0x29d9c998L), tole(0xb0d09822L), tole(0xc7d7a8b4L),
tole(0x59b33d17L), tole(0x2eb40d81L), tole(0xb7bd5c3bL), tole(0xc0ba6cadL),
tole(0xedb88320L), tole(0x9abfb3b6L), tole(0x03b6e20cL), tole(0x74b1d29aL),
tole(0xead54739L), tole(0x9dd277afL), tole(0x04db2615L), tole(0x73dc1683L),
tole(0xe3630b12L), tole(0x94643b84L), tole(0x0d6d6a3eL), tole(0x7a6a5aa8L),
tole(0xe40ecf0bL), tole(0x9309ff9dL), tole(0x0a00ae27L), tole(0x7d079eb1L),
tole(0xf00f9344L), tole(0x8708a3d2L), tole(0x1e01f268L), tole(0x6906c2feL),
tole(0xf762575dL), tole(0x806567cbL), tole(0x196c3671L), tole(0x6e6b06e7L),
tole(0xfed41b76L), tole(0x89d32be0L), tole(0x10da7a5aL), tole(0x67dd4accL),
tole(0xf9b9df6fL), tole(0x8ebeeff9L), tole(0x17b7be43L), tole(0x60b08ed5L),
tole(0xd6d6a3e8L), tole(0xa1d1937eL), tole(0x38d8c2c4L), tole(0x4fdff252L),
tole(0xd1bb67f1L), tole(0xa6bc5767L), tole(0x3fb506ddL), tole(0x48b2364bL),
tole(0xd80d2bdaL), tole(0xaf0a1b4cL), tole(0x36034af6L), tole(0x41047a60L),
tole(0xdf60efc3L), tole(0xa867df55L), tole(0x316e8eefL), tole(0x4669be79L),
tole(0xcb61b38cL), tole(0xbc66831aL), tole(0x256fd2a0L), tole(0x5268e236L),
tole(0xcc0c7795L), tole(0xbb0b4703L), tole(0x220216b9L), tole(0x5505262fL),
tole(0xc5ba3bbeL), tole(0xb2bd0b28L), tole(0x2bb45a92L), tole(0x5cb36a04L),
tole(0xc2d7ffa7L), tole(0xb5d0cf31L), tole(0x2cd99e8bL), tole(0x5bdeae1dL),
tole(0x9b64c2b0L), tole(0xec63f226L), tole(0x756aa39cL), tole(0x026d930aL),
tole(0x9c0906a9L), tole(0xeb0e363fL), tole(0x72076785L), tole(0x05005713L),
tole(0x95bf4a82L), tole(0xe2b87a14L), tole(0x7bb12baeL), tole(0x0cb61b38L),
tole(0x92d28e9bL), tole(0xe5d5be0dL), tole(0x7cdcefb7L), tole(0x0bdbdf21L),
tole(0x86d3d2d4L), tole(0xf1d4e242L), tole(0x68ddb3f8L), tole(0x1fda836eL),
tole(0x81be16cdL), tole(0xf6b9265bL), tole(0x6fb077e1L), tole(0x18b74777L),
tole(0x88085ae6L), tole(0xff0f6a70L), tole(0x66063bcaL), tole(0x11010b5cL),
tole(0x8f659effL), tole(0xf862ae69L), tole(0x616bffd3L), tole(0x166ccf45L),
tole(0xa00ae278L), tole(0xd70dd2eeL), tole(0x4e048354L), tole(0x3903b3c2L),
tole(0xa7672661L), tole(0xd06016f7L), tole(0x4969474dL), tole(0x3e6e77dbL),
tole(0xaed16a4aL), tole(0xd9d65adcL), tole(0x40df0b66L), tole(0x37d83bf0L),
tole(0xa9bcae53L), tole(0xdebb9ec5L), tole(0x47b2cf7fL), tole(0x30b5ffe9L),
tole(0xbdbdf21cL), tole(0xcabac28aL), tole(0x53b39330L), tole(0x24b4a3a6L),
tole(0xbad03605L), tole(0xcdd70693L), tole(0x54de5729L), tole(0x23d967bfL),
tole(0xb3667a2eL), tole(0xc4614ab8L), tole(0x5d681b02L), tole(0x2a6f2b94L),
tole(0xb40bbe37L), tole(0xc30c8ea1L), tole(0x5a05df1bL), tole(0x2d02ef8dL)
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

    for ( i = 0; i < ulSize; i++)
        nAccum = (nAccum << 8) ^ gTable_Crc32[(nAccum >> 24) ^ (*pData++)];
    return nAccum;
}
#define DO_CRC(x) crc = tab[ (crc ^ (x)) & 255 ] ^ (crc>>8)

unsigned int crc32_le(unsigned int crc, unsigned char *p, unsigned int len)
{
/*
	UINT i;
	UINT nAccum = crc;

	for ( i = 0; i < len; i++) {
		nAccum = (nAccum >> 8) ^ crc32table_le[(nAccum ^ (*p)) & 0xFF];
		p++;
	}
	return nAccum;
*/
	unsigned int      *b =(unsigned int *)p;
	unsigned int      *tab = crc32table_le;
	crc = crc ^ 0xFFFFFFFF;
	if((((uintptr_t)b)&3 && len)){
		do {
			unsigned char *p = (unsigned char *)b;
			DO_CRC(*p++);
			b = (unsigned int *)p;
		} while ((--len) && ((uintptr_t)b)&3 );
	}
	if((len >= 4)){
		unsigned int save_len = len & 3;
		len = len >> 2;
		--b;
		do {
			crc ^= *++b;
			DO_CRC(0);
			DO_CRC(0);
			DO_CRC(0);
			DO_CRC(0);
		} while (--len);
		b++;
		len = save_len;
	}
	if(len){
		do {
			unsigned char *p = (unsigned char *)b;
			DO_CRC(*p++);
			b = (unsigned int *)p;
		} while (--len);
	}
	crc = crc ^ 0xFFFFFFFF;
	return crc;

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
