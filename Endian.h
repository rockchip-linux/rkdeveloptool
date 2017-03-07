#ifndef ENDIAN_HEADER
#define ENDIAN_HEADER

#define Endian16_Swap(value)	(((((unsigned short)value) << 8) & 0xFF00)|\
								((((unsigned short)value) >> 8) & 0x00FF))

#define Endian32_Swap(value)	(((((unsigned int)value) << 24) & 0xFF000000) |\
								((((unsigned int)value) << 8) & 0x00FF0000) |\
								((((unsigned int)value) >> 8) & 0x0000FF00) |\
								((((unsigned int)value) >> 24) & 0x000000FF))

#define EndianS16_LtoB(value)              	 ((short)Endian16_Swap(value))
#define EndianS16_BtoL(value)                ((short)Endian16_Swap(value))
#define EndianU16_LtoB(value)                ((unsigned short)Endian16_Swap(value))
#define EndianU16_BtoL(value)                ((unsigned short)Endian16_Swap(value))
#define EndianS32_LtoB(value)                ((int)Endian32_Swap(value))
#define EndianS32_BtoL(value)                ((int)Endian32_Swap(value))
#define EndianU32_LtoB(value)                ((unsigned int)Endian32_Swap(value))
#define EndianU32_BtoL(value)                ((unsigned int)Endian32_Swap(value))

#endif
