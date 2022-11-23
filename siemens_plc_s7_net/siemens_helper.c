#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "siemens_helper.h"
#include "socket.h"

#define BUFFER_SIZE 1024

// �ӵ�ַ������ı���
byte_array_info build_read_byte_command(siemens_s7_address_data address)
{
	const ushort command_len = 19 + 12;	// head + block
	byte* command = (byte*)malloc(command_len);

	command[0] = 0x03;												// ����ͷ -> Head
	command[1] = 0x00;
	command[2] = (byte)(command_len / 256);							// ���� -> Length
	command[3] = (byte)(command_len % 256);
	command[4] = 0x02;												// �̶� -> Fixed
	command[5] = 0xF0;
	command[6] = 0x80;
	command[7] = 0x32;                                              // Э���ʶ -> Protocol identification
	command[8] = 0x01;                                              // ����� -> Command: Send
	command[9] = 0x00;                                              // redundancy identification (reserved): 0x0000;
	command[10] = 0x00;
	command[11] = 0x00;                                             // protocol data unit reference; it��s increased by request event;
	command[12] = 0x01;
	command[13] = (byte)((command_len - 17) / 256);					// �������������ܳ��� -> Parameter command Data total length
	command[14] = (byte)((command_len - 17) % 256);
	command[15] = 0x00;                                              // ��ȡ�ڲ�����ʱΪ00����ȡCPU�ͺ�ΪData���ݳ��� -> Read internal data is 00, read CPU model is data length
	command[16] = 0x00;
	// =====================================================================================
	command[17] = 0x04;                                              // ��дָ�04����05д -> Read-write instruction, 04 read, 05 Write
	command[18] = 0x01;												 // ��ȡ���ݿ���� -> Number of data blocks read

	//===========================================================================================
	// ָ����Чֵ���� -> Specify a valid value type
	command[19] = 0x12;
	// ���������ε�ַ���ʳ��� -> The next time the address access length
	command[20] = 0x0A;
	// �﷨��ǣ�ANY -> Syntax tag, any
	command[21] = 0x10;
	// ����Ϊ��λ -> by word
	if (address.data_code == 0x1E || address.data_code == 0x1F)
	{
		command[22] = address.data_code;
		// �������ݵĸ��� -> Number of Access data
		command[23] = (byte)(address.length / 2 / 256);
		command[24] = (byte)(address.length / 2 % 256);
	}
	else
	{
		if (address.data_code == 0x06 || address.data_code == 0x07)
		{
			// �������ݵĸ��� -> Number of Access data
			command[22] = 0x04;
			command[23] = (byte)(address.length / 2 / 256);
			command[24] = (byte)(address.length / 2 % 256);
		}
		else
		{
			command[22] = 0x02;
			// �������ݵĸ��� -> Number of Access data
			command[23] = (byte)(address.length / 256);
			command[24] = (byte)(address.length % 256);
		}
	}
	// DB���ţ�������ʵ���DB��Ļ� -> DB block number, if you are accessing a DB block
	command[25] = (byte)(address.db_block / 256);
	command[26] = (byte)(address.db_block % 256);
	// ������������ -> Accessing data types
	command[27] = address.data_code;
	// ƫ��λ�� -> Offset position
	command[28] = (byte)(address.address_start / 256 / 256 % 256);
	command[29] = (byte)(address.address_start / 256 % 256);
	command[30] = (byte)(address.address_start % 256);

	byte_array_info ret = { 0 };
	ret.data = command;
	ret.length = command_len;
	return ret;
}

byte_array_info build_read_bit_command(siemens_s7_address_data address)
{
	const ushort command_len = 19 + 12;	// head + block
	byte* command = (byte*)malloc(command_len);

	command[0] = 0x03;
	command[1] = 0x00;
	// ���� -> Length
	command[2] = (byte)(command_len / 256);
	command[3] = (byte)(command_len % 256);
	// �̶� -> Fixed
	command[4] = 0x02;
	command[5] = 0xF0;
	command[6] = 0x80;
	command[7] = 0x32;
	// ����� -> command to send
	command[8] = 0x01;
	// ��ʶ���к�
	command[9] = 0x00;
	command[10] = 0x00;
	command[11] = 0x00;
	command[12] = 0x01;
	// ���������ܳ��� -> Identification serial Number
	command[13] = (byte)((command_len - 17) / 256);
	command[14] = (byte)((command_len - 17) % 256);

	command[15] = 0x00;
	command[16] = 0x00;

	// ������ʼ�� -> Command start character
	command[17] = 0x04;
	// ��ȡ���ݿ���� -> Number of data blocks read
	command[18] = 0x01;

	//===========================================================================================
	// ��ȡ��ַ��ǰ׺ -> Read the prefix of the address
	command[19] = 0x12;
	command[20] = 0x0A;
	command[21] = 0x10;
	// ��ȡ������ʱλ -> Data read-time bit
	command[22] = 0x01;
	// �������ݵĸ��� -> Number of Access data
	command[23] = 0x00;
	command[24] = 0x01;
	// DB���ţ�������ʵ���DB��Ļ� -> DB block number, if you are accessing a DB block
	command[25] = (byte)(address.db_block / 256);
	command[26] = (byte)(address.db_block % 256);
	// ������������ -> Types of reading data
	command[27] = address.data_code;
	// ƫ��λ�� -> Offset position
	command[28] = (byte)(address.address_start / 256 / 256 % 256);
	command[29] = (byte)(address.address_start / 256 % 256);
	command[30] = (byte)(address.address_start % 256);

	byte_array_info ret = { 0 };
	ret.data = command;
	ret.length = command_len;
	return ret;
}

byte_array_info build_write_byte_command(siemens_s7_address_data address, byte_array_info value)
{
	int val_len = 0;
	if (value.data != NULL)
		val_len = value.length;

	const ushort command_len = 35 + val_len;
	byte* command = (byte*)malloc(command_len);
	command[0] = 0x03;
	command[1] = 0x00;
	// ���� -> Length
	command[2] = (byte)((command_len) / 256);
	command[3] = (byte)((command_len) % 256);
	// �̶� -> Fixed
	command[4] = 0x02;
	command[5] = 0xF0;
	command[6] = 0x80;
	command[7] = 0x32;
	// ���� �� -> command to send
	command[8] = 0x01;
	// ��ʶ���к� -> Identification serial Number
	command[9] = 0x00;
	command[10] = 0x00;
	command[11] = 0x00;
	command[12] = 0x01;
	// �̶� -> Fixed
	command[13] = 0x00;
	command[14] = 0x0E;
	// д�볤��+4 -> Write Length +4
	command[15] = (byte)((4 + val_len) / 256);
	command[16] = (byte)((4 + val_len) % 256);
	// ��дָ�� -> Read and write instructions
	command[17] = 0x05;
	// д�����ݿ���� -> Number of data blocks written
	command[18] = 0x01;
	// �̶����������ݳ��� -> Fixed, return data length
	command[19] = 0x12;
	command[20] = 0x0A;
	command[21] = 0x10;
	if (address.data_code == 0x06 || address.data_code == 0x07)
	{
		// д�뷽ʽ��1�ǰ�λ��2�ǰ��� -> Write mode, 1 is bitwise, 2 is by byte, 4 is by word
		command[22] = 0x04;
		// д�����ݵĸ��� -> Number of Write Data
		command[23] = (byte)(val_len / 2 / 256);
		command[24] = (byte)(val_len / 2 % 256);
	}
	else
	{
		// д�뷽ʽ��1�ǰ�λ��2�ǰ��� -> Write mode, 1 is bitwise, 2 is by word
		command[22] = 0x02;
		// д�����ݵĸ��� -> Number of Write Data
		command[23] = (byte)(val_len / 256);
		command[24] = (byte)(val_len % 256);
	}
	// DB���ţ�������ʵ���DB��Ļ� -> DB block number, if you are accessing a DB block
	command[25] = (byte)(address.db_block / 256);
	command[26] = (byte)(address.db_block % 256);
	// д�����ݵ����� -> Types of writing data
	command[27] = address.data_code;
	// ƫ��λ�� -> Offset position
	command[28] = (byte)(address.address_start / 256 / 256 % 256); ;
	command[29] = (byte)(address.address_start / 256 % 256);
	command[30] = (byte)(address.address_start % 256);
	// ����д�� -> Write by Word
	command[31] = 0x00;
	command[32] = 0x04;
	// ��λ����ĳ��� -> The length of the bitwise calculation
	command[33] = (byte)(val_len * 8 / 256);
	command[34] = (byte)(val_len * 8 % 256);

	if (value.data != NULL)
	{
		memcpy(command + 35, value.data, val_len);
		free(value.data);
	}

	byte_array_info ret = { 0 };
	ret.data = command;
	ret.length = command_len;
	return ret;
}

byte_array_info build_write_bit_command(siemens_s7_address_data address, bool value)
{
	byte buffer[1] = { 0 };
	ushort buffer_len = sizeof(buffer);
	buffer[0] = value ? (byte)0x01 : (byte)0x00;

	const ushort command_len = 35 + buffer_len;
	byte* command = (byte*)malloc(command_len);

	command[0] = 0x03;
	command[1] = 0x00;
	// ���� -> length
	command[2] = (byte)((command_len) / 256);
	command[3] = (byte)((command_len) % 256);
	// �̶� -> fixed
	command[4] = 0x02;
	command[5] = 0xF0;
	command[6] = 0x80;
	command[7] = 0x32;
	// ���� �� -> command to send
	command[8] = 0x01;
	// ��ʶ���к� -> Identification serial Number
	command[9] = 0x00;
	command[10] = 0x00;
	command[11] = 0x00;
	command[12] = 0x01;
	// �̶� -> fixed
	command[13] = 0x00;
	command[14] = 0x0E;
	// д�볤��+4 -> Write Length +4
	command[15] = (byte)((4 + buffer_len) / 256);
	command[16] = (byte)((4 + buffer_len) % 256);
	// ������ʼ�� -> Command start character
	command[17] = 0x05;
	// д�����ݿ���� -> Number of data blocks written
	command[18] = 0x01;
	command[19] = 0x12;
	command[20] = 0x0A;
	command[21] = 0x10;
	// д�뷽ʽ��1�ǰ�λ��2�ǰ��� -> Write mode, 1 is bitwise, 2 is by word
	command[22] = 0x01;
	// д�����ݵĸ��� -> Number of Write Data
	command[23] = (byte)(buffer_len / 256);
	command[24] = (byte)(buffer_len % 256);
	// DB���ţ�������ʵ���DB��Ļ� -> DB block number, if you are accessing a DB block
	command[25] = (byte)(address.db_block / 256);
	command[26] = (byte)(address.db_block % 256);
	// д�����ݵ����� -> Types of writing data
	command[27] = address.data_code;
	// ƫ��λ�� -> Offset position
	command[28] = (byte)(address.address_start / 256 / 256);
	command[29] = (byte)(address.address_start / 256);
	command[30] = (byte)(address.address_start % 256);
	// ��λд�� -> Bitwise Write
	if (address.data_code == 0x1C)
	{
		command[31] = 0x00;
		command[32] = 0x09;
	}
	else
	{
		command[31] = 0x00;
		command[32] = 0x03;
	}
	// ��λ����ĳ��� -> The length of the bitwise calculation
	command[33] = (byte)(buffer_len / 256);
	command[34] = (byte)(buffer_len % 256);

	memcpy(command + 35, buffer, buffer_len);

	byte_array_info ret = { 0 };
	ret.data = command;
	ret.length = command_len;
	return ret;
}

/// <summary>
/// ��ȡBOOLʱ������S7Э��ķ��ر��ģ���ȷ��ȡ��ʵ�ʵ���������
/// </summary>
/// <param name="response"></param>
/// <param name="ret"></param>
/// <returns></returns>
s7_error_code_e s7_analysis_read_bit(byte_array_info response, byte_array_info* ret)
{
	s7_error_code_e ret_code = S7_ERROR_CODE_OK;
	if (response.length == 0)
		return S7_ERROR_CODE_FAILED;

	if (response.length >= 21 && response.data[20] == 1)
	{
		byte buffer[1] = { 0 };
		if (22 < response.length)
		{
			if (response.data[21] == 0xFF && response.data[22] == 0x03)
			{
				buffer[0] = response.data[25];
			}
			else if (response.data[21] == 0x05 && response.data[22] == 0x00)
			{
				ret_code = S7_ERROR_CODE_READ_LENGTH_OVER_PLC_ASSIGN;
			}
			else if (response.data[21] == 0x06 && response.data[22] == 0x00)
			{
				ret_code = S7_ERROR_CODE_ERROR_0006;
			}
			else if (response.data[21] == 0x0A && response.data[22] == 0x00)
			{
				ret_code = S7_ERROR_CODE_ERROR_000A;
			}
			else
			{
				ret_code = S7_ERROR_CODE_UNKOWN;
			}
		}

		ret->data = (byte*)malloc(1);
		memset(ret->data, 0, 1);
		memcpy(ret->data, buffer, 1);
		ret->length = 1;
	}
	else
	{
		ret_code = S7_ERROR_CODE_DATA_LENGTH_CHECK_FAILED;
	}

	return ret_code;
}

s7_error_code_e s7_analysis_read_byte(byte_array_info response, byte_array_info* ret)
{
	s7_error_code_e ret_code = S7_ERROR_CODE_OK;
	if (response.length == 0)
		return S7_ERROR_CODE_FAILED;

	int i = 0, j = 0;
	byte buffer[1024] = { 0 };
	int buffer_length = 0;
	if (response.length >= 21)
	{
		for (i = 21; i < response.length - 1; i++)
		{
			if (response.data[i] == 0xFF && response.data[i + 1] == 0x04)
			{
				int count = (response.data[i + 2] * 256 + response.data[i + 3]) / 8;
				memcpy(buffer + buffer_length, response.data + i + 4, count);
				buffer_length += count;

				i += count + 3;
			}
			else if (response.data[i] == 0xFF && response.data[i + 1] == 0x09)
			{
				int count = response.data[i + 2] * 256 + response.data[i + 3];
				if (count % 3 == 0)
				{
					for (j = 0; j < count / 3; j++)
					{
						memcpy(buffer + buffer_length, (void*)response.data[i + 5 + 3 * j], 2);
						buffer_length += 2;
					}
				}
				else
				{
					for (j = 0; j < count / 5; j++)
					{
						memcpy(buffer + buffer_length, (void*)response.data[i + 7 + 5 * j], 2);
						buffer_length += 2;
					}
				}
				i += count + 4;
			}
			else if (response.data[i] == 0x05 && response.data[i + 1] == 0x00)
				ret_code = S7_ERROR_CODE_READ_LENGTH_OVER_PLC_ASSIGN;
			else if (response.data[i] == 0x06 && response.data[i + 1] == 0x00)
				ret_code = S7_ERROR_CODE_ERROR_0006;
			else if (response.data[i] == 0x0A && response.data[i + 1] == 0x00)
				ret_code = S7_ERROR_CODE_ERROR_000A;
		}

		ret->data = (byte*)malloc(buffer_length);
		memset(ret->data, 0, buffer_length);
		memcpy(ret->data, buffer, buffer_length);
		ret->length = buffer_length;
	}
	else
	{
		ret_code = S7_ERROR_CODE_UNKOWN;
	}
	return ret_code;
}

s7_error_code_e s7_analysis_write(byte_array_info response)
{
	s7_error_code_e ret_code = S7_ERROR_CODE_OK;
	if (response.length == 0)
		return S7_ERROR_CODE_FAILED;

	byte buffer[1024] = { 0 };
	int buffer_length = 0;
	if (response.length >= 22)
	{
		byte code = response.data[21];
		if (code != 0xFF)
			ret_code = S7_ERROR_CODE_WRITE_ERROR;
	}
	else
	{
		ret_code = S7_ERROR_CODE_UNKOWN;
	}
	return ret_code;
}

bool read_data_from_core_server(int fd, byte_array_info send, byte_array_info* ret)
{
	bool is_ok = false;
	int need_send = send.length;
	int real_sends = socket_send_data(fd, send.data, need_send);
	if (real_sends == need_send)
	{
		byte temp[BUFFER_SIZE];
		memset(temp, 0, BUFFER_SIZE);

		int recv_size = socket_recv_data_one_loop(fd, temp, BUFFER_SIZE);
		int min = 21;
		if (recv_size >= min) //header size
		{
			ret->data = (byte*)malloc(recv_size);
			memset(ret->data, 0, recv_size);
			memcpy(ret->data, temp, recv_size);
			ret->length = recv_size;

			is_ok = true;
		}
	}
	return is_ok;
}

bool send_data_to_core_server(int fd, byte_array_info send)
{
	bool is_ok = false;
	int need_send = send.length;
	int real_sends = socket_send_data(fd, send.data, need_send);
	if (real_sends == need_send)
	{
		is_ok = true;
	}
	return is_ok;
}