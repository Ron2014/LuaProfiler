// Engine中数据类型的定义,平台相关
#ifndef	__BaseTypes_H__
#define	__BaseTypes_H__

#if !defined LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif

#define  INVALID_32BIT 0xffffffff

typedef	char				int8;
typedef	short				int16;
typedef	int					int32;
typedef	long long			int64;

typedef	unsigned char		uint8;
typedef	unsigned short		uint16;
typedef	unsigned int		uint32;
typedef	unsigned long long	uint64;


#define SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)

#endif	//#ifndef	__BaseTypes_H__

