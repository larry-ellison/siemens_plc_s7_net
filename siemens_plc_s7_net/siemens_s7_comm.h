#ifndef __H_SIEMENS_S7_COMM_H__
#define __H_SIEMENS_S7_COMM_H__
#include "utill.h"

typedef struct _tag_siemens_s7_address_data {
	byte	data_code;			// ���͵Ĵ���ֵ
	ushort	db_block;			// ��ȡ������PLC��DB��������Ϣ
	int		address_start;		// ���ֵ���ʼ��ַ��Ҳ����ƫ�Ƶ�ַ
	int		length;				// ��ȡ�����ݳ���
}siemens_s7_address_data;

siemens_s7_address_data s7_analysis_address(const char* address, int length);

#endif//__H_SIEMENS_S7_COMM_H__