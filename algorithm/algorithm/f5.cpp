#include <iostream>
#include "img_factory.h"
#include "img_jpeg.h"
#include "img.h"
#include "f5.h"
using namespace std;

/*member function*/
void f5_algorithm::f5_embed(const char* infilePath, const char* outfilePath, int seed, int headLength, char* message)
{
	///������ز���
	const char *jpeg_inputpath = infilePath;
	const char *jpeg_outputpath = outfilePath;
	img *img_jpeg_ptr = img_factory::createImage(jpeg_inputpath, seed, headLength);

	///��ȡͼ���width�Լ�height
	int img_jpeg_width = img_jpeg_ptr->getWidth();
	int img_jpeg_height = img_jpeg_ptr->getHeight();

	///��ȡͼ���channel
	int channel = img_jpeg_ptr->getChannels();

	///��ȡͼ����ڸ�ͨ����coeff
	vector<mat2D<int>*> coeff = img_jpeg_ptr->getElementMatrix();

	///��ȡͼ�����Ч����
	int *valid_cover = img_jpeg_ptr->getValidCover();
	int * valid_cover_embed_ptr = valid_cover;

	//**f5��ЧǶ��
	///��ȡͼ����Ч��ĳ���
	int valid_cover_len = img_jpeg_ptr->getSize();

	///��ȡ��Ϣ����
	int message_len = strlen(message);

	///�������Ԫ��(1,n,k)
	int k = 1;
	int n = 1;
	double int_real_efiicient_rate = (message_len * 8.0 * 100.0) / (valid_cover_len);
	if (int_real_efiicient_rate <= 1.76)
	{
		k = 9;
		n = 511;
	}
	else if (int_real_efiicient_rate <= 3.14)
	{
		k = 8;
		n = 255;
	}
	else if (int_real_efiicient_rate <= 5.51)
	{
		k = 7;
		n = 127;
	}
	else if (int_real_efiicient_rate <= 9.52)
	{
		k = 6;
		n = 63;
	}
	else if (int_real_efiicient_rate <= 16.13)
	{
		k = 5;
		n = 31;
	}
	else if (int_real_efiicient_rate <= 26.67)
	{
		k = 4;
		n = 15;
	}
	else if (int_real_efiicient_rate <= 42.86)
	{
		k = 3;
		n = 7;
	}
	else if (int_real_efiicient_rate <= 66.67)
	{
		k = 2;
		n = 3;
	}
	else if (int_real_efiicient_rate <= 100.00)
	{
		k = 1;
		n = 1;
	}

	///����lsb�㷨Ƕ���ļ�����
	//�洢message�ĳ���
	int bit = 0;
	for (int i = 0; i < 32; i++)
	{
		bit = (message_len >> (31 - i)) & 0x00000001;
		if (0 == bit)
		{
			if (1 == *valid_cover_embed_ptr)
			{
				(*valid_cover_embed_ptr)++;
				valid_cover_embed_ptr++;
			}
			else if (1 == ((*valid_cover_embed_ptr % 2) + 2) % 2)
			{
				(*valid_cover_embed_ptr)--;
				valid_cover_embed_ptr++;
			}
			else
			{
				valid_cover_embed_ptr++;
			}
		}
		else
		{
			if (0 == ((*valid_cover_embed_ptr % 2) + 2) % 2)
			{
				(*valid_cover_embed_ptr)++;
				valid_cover_embed_ptr++;
			}
			else
			{
				valid_cover_embed_ptr++;
			}
		}
	}


	//f5�㷨:ȡ����Ч��n�����壬kλǶ����Ϣλ,��Ƕ��
	int change_num = 0;                                     //��Ч��Ƕ��->�������µ�0
	if (k != 1 && n != 1)
	{
		int *cover_n = new int[n];
		int *cover_n_begin = cover_n;                      //��¼�׵�ַ
		int *copy_back = valid_cover_embed_ptr;                //д�����ݵ�ַ
		int eight_bits_remain = 0;
		bool isend = false;
		int messagebits_k;                                       //���kbit��Ϣ
		int charbits;
		int charnextbitget;

		do
		{
			messagebits_k = 0;
			//��ȡkλǶ����Ϣλ
			for (int i = 0; i < k; i++)
			{
				if (eight_bits_remain == 0)
				{
					//��������ַ���β�˵Ļ�������ѭ��
					if (*message == '\0')
					{
						isend = true;
						break;
					}
					else
					{
						charbits = int(*message);
						message++;
						eight_bits_remain = 8;
					}
				}

				//k bits ��ȡ
				charnextbitget = charbits & 1;
				charbits >>= 1;
				eight_bits_remain--;
				messagebits_k |= charnextbitget << i;
			}

			//kbits��ϢǶ��
			cover_n = cover_n_begin;

			///��ʼ��ȡn��Ƕ��λ��
			for (int i = 0; i < n; i++)
			{
				*cover_n = *valid_cover_embed_ptr;
				int temp = *valid_cover_embed_ptr;
				cover_n++;
				valid_cover_embed_ptr++;
			}
			bool embed_success = false;
			do{
				//��ȡ�ı��λ��change_loc,����ɢ�к��� ��f(a) = (ai*i)^....(an*n)
				int change_loc;
				int hash = 0;
				cover_n = cover_n_begin;
				for (int i = 1; i <= n; i++)
				{
					if (((*cover_n % 2) + 2) % 2 == 1)
					{
						hash = hash ^ i;
					}
					cover_n++;
				}

				change_loc = hash ^ messagebits_k;
				//����ϵ�����޸�(��Ϣ��Ƕ��)
				cover_n = cover_n_begin;
				if (change_loc == 0) break;
				for (int i = 1; i <= change_loc; i++)
				{
					cover_n++;
				}
				//�����������1����
				cover_n--;
				if (*cover_n > 0)
				{
					if (1 == *cover_n)
					{
						(*cover_n)++;
						change_num++;
						embed_success = true;
					}
					else
					{
						(*cover_n)--;
						change_num++;
						embed_success = true;
					}
				}
				else
				{
					if (-1 == *cover_n)
					{
						(*cover_n)--;
						change_num++;
						embed_success = true;
					}
					else
					{
						(*cover_n)++;
						change_num++;
						embed_success = true;
					}
				}
			} while (!embed_success);
			//����n����Ϣ��д�ز���
			cover_n = cover_n_begin;
			for (int i = 0; i < n; i++)
			{
				*copy_back = *cover_n;
				copy_back++;
				cover_n++;
			}
		} while (!isend);

		cover_n = cover_n_begin;
		delete[] cover_n;
	}




	else  //��n=1,k=1ʱ�����lsbǶ�����
	{
		int  messagelen = strlen(message);
		int bit;
		for (int i = 0; i < messagelen; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				while (*valid_cover_embed_ptr == -1024 || *valid_cover_embed_ptr == 1023 || *valid_cover_embed_ptr == 1 || *valid_cover_embed_ptr == -1)
				{
					valid_cover_embed_ptr++;
				}
				bit = (int(*message) >> (7 - j)) & 0x00000001;
				*valid_cover_embed_ptr = (*valid_cover_embed_ptr & 0xfffffffe) + bit;
				valid_cover_embed_ptr++;
			}
			message++;
		}
	}

	///д���޸ĺ����Ч����
	img_jpeg_ptr->setValidCover(valid_cover);

	///д��ͼ������
	img_jpeg_ptr->write(jpeg_outputpath);

	///�ͷ����ָ��
	delete[] valid_cover;
}

char* f5_algorithm::f5_extract(const char* infilePath, int seed, int headLength)
{
	///������Ӧ�Ĳ���
	const char* jpeg_Inputpath = infilePath;

	///��ʼ��ͼ��
	img *img_jpeg_ptr = img_factory::createImage(jpeg_Inputpath, seed, headLength);

	///��ȡ��Ӧ����Ч����
	int *valid_cover_extract = img_jpeg_ptr->getValidCover();

	//**f5��Ч��ȡ
	//��ȡ��Ϣ�ĳ���
	int strbit = 0;
	int bit = 0;

	//��ȡmessage�ĳ���
	for (int i = 0; i < 32; i++)
	{
		bit = *valid_cover_extract & 0x00000001;
		strbit = (*valid_cover_extract & 0x00000001) + (strbit << 1);
		valid_cover_extract++;
	}
	int message_len = strbit;

	//�����ַ�����Ϣ�ڴ�
	char *message = new char[message_len];
	char *message_ptr = message;

	//��ȡͼ����Ч��ĳ���
	int valid_cover_extract_len = img_jpeg_ptr->getSize();

	///�������Ԫ��(1,n,k)
	int k = 1;
	int n = 1;
	double int_real_efiicient_rate = (message_len * 8.0 * 100.0) / (valid_cover_extract_len);
	if (int_real_efiicient_rate <= 1.76)
	{
		k = 9;
		n = 511;
	}
	else if (int_real_efiicient_rate <= 3.14)
	{
		k = 8;
		n = 255;
	}
	else if (int_real_efiicient_rate <= 5.51)
	{
		k = 7;
		n = 127;
	}
	else if (int_real_efiicient_rate <= 9.52)
	{
		k = 6;
		n = 63;
	}
	else if (int_real_efiicient_rate <= 16.13)
	{
		k = 5;
		n = 31;
	}
	else if (int_real_efiicient_rate <= 26.67)
	{
		k = 4;
		n = 15;
	}
	else if (int_real_efiicient_rate <= 42.86)
	{
		k = 3;
		n = 7;
	}
	else if (int_real_efiicient_rate <= 66.67)
	{
		k = 2;
		n = 3;
	}
	else if (int_real_efiicient_rate <= 100.00)
	{
		k = 1;
		n = 1;
	}

	//F5��ȡ�㷨��ȡ��n��cover����ȡk������Ϣ
	bool isEnd = false;
	int HASH;
	int message_extract_len = 0;
	int avalible_extracted_bitslen = 0;
	int byte_extracted_message = 0;
	do
	{
		//��ʼHASHΪ0
		HASH = 0;

		//��n��cover�У�����ɢ�к���f(a) = (ai*i)^....(an*n)����ȡk������Ϣ
		for (int i = 1; i <= n; i++)
		{
			//���*valid
			if (((*valid_cover_extract % 2) + 2) % 2 == 1)
			{
				HASH = HASH ^ i;
			}
			valid_cover_extract++;
		}

		//��k������Ϣ���
		for (int i = 0; i < k; i++)
		{
			byte_extracted_message |= (HASH >> i & 1) << avalible_extracted_bitslen++;
			if (8 == avalible_extracted_bitslen)
			{
				*message_ptr = char(byte_extracted_message);
				message_ptr++;

				//������صĲ�������
				byte_extracted_message = 0;
				avalible_extracted_bitslen = 0;
				message_extract_len++;
				if (message_extract_len == message_len)
				{
					isEnd = true;
					//������ȡ
					break;
				}
			}
		}
	} while (!isEnd);

	//�ַ���β������ս��
	*message_ptr = '\0';
	return message;
}