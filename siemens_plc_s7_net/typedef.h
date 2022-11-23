#ifndef __H_TYPEDEF_H__
#define __H_TYPEDEF_H__

#include <stdint.h>
#include <stdbool.h>

typedef unsigned char byte;
typedef unsigned short ushort;
typedef signed int	int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

typedef enum _tag_siemens_plc_type {
	S1200 = 1,		// 1200ϵ��
	S300 = 2,		// 300ϵ��
	S400 = 3,		// 400ϵ��
	S1500 = 4,		// 1500ϵ��PLC
	S200Smart = 5,	// 200��smartϵ��
	S200 = 6		// 200ϵͳ����Ҫ����������̫��ģ��
} siemens_plc_types_e;

typedef enum _tag_s7_error_code {
	S7_ERROR_CODE_OK = 0,							// �ɹ�
	S7_ERROR_CODE_FAILED = 1,						// ����
	S7_ERROR_CODE_FW_ERROR,							// �������쳣��������Ϣ����Fetch/WriteЭ���ĵ�
	S7_ERROR_CODE_ERROR_0006,						// ��ǰ�������������Ͳ�֧��
	S7_ERROR_CODE_ERROR_000A,						// ���Զ�ȡ�����ڵ�DB������
	S7_ERROR_CODE_WRITE_ERROR,						// д�������쳣
	S7_ERROR_CODE_DB_SIZE_TOO_LARGE,				// DB�������޷�����255
	S7_ERROR_CODE_READ_LENGTH_MAST_BE_EVEN,			// ��ȡ�����ݳ��ȱ���Ϊż��
	S7_ERROR_CODE_DATA_LENGTH_CHECK_FAILED,			// ���ݿ鳤��У��ʧ�ܣ������Ƿ���put/get�Լ��ر�db���Ż�
	S7_ERROR_CODE_READ_LENGTH_OVER_PLC_ASSIGN,		// ��ȡ�����ݷ�Χ������PLC���趨
	S7_ERROR_CODE_READ_LENGTH_CANNT_LARAGE_THAN_19,	// ��ȡ�������������������19
	S7_ERROR_CODE_UNKOWN = 99,						// δ֪����
} s7_error_code_e;

#endif // !__H_TYPEDEF_H__
