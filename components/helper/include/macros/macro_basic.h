#ifndef MACRO_BASIC_H__
#define MACRO_BASIC_H__

#define BETWEEN(value,less,great)				(((value) >= (less) && (value) <= (great)) ? true : false)
#define BETWEEN_MARGIN(value, comp, margin)		BETWEEN(value,comp-margin,comp+margin)

#define EXTERN(value,less,great)				(!BETWEEN(value,less,great))
#define EXTERN_MARGIN(value,comp,margin)		(!BETWEEN_MARGIN(value,comp,margin))

#define xstr(s) 								STR(s)
#define STR(s) 									#s

#define CONCAT_BYTES(msb, lsb)            (((uint16_t)msb << 8) | (uint16_t)lsb)

#ifndef maximo
#define maximo(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#define SWAP(a, b)      do{a ^= b; b ^= a; a ^= b; }while(0)

#define SWAP_16(temp)		((((temp) << 8) & 0xFF00) | (((temp) >> 8) & 0x00FF))

#define MSB_16(data)		((uint8_t)(((data) >> 8) & 0x00FF))
#define LSB_16(data)		((uint8_t)((data) & 0x00FF))

#endif /* MACRO_BASIC_H__ */
