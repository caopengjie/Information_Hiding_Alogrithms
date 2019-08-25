#include <iostream>
#include "img_factory.h"
#include "img_jpeg.h"
#include "img.h"
#include "lsb.h"
using namespace std;

/*member function*/
void lsb_algorithm::lsb_embed(const char* infilePath, const char* outfilePath, int seed, int headLength,char* message)
{
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
	int *valid_cover_embed = new int[img_jpeg_ptr->getSize()];
	valid_cover_embed = img_jpeg_ptr->getValidCover();

	//**�޸������Чλ//
	int bit = 0;
	int *coverPtr = valid_cover_embed;
	int messageLength = strlen(message);

	//�洢message�ĳ���
	for (int i = 0; i < 32; i++)
	{
		bit = (messageLength >> (31 - i)) & 0x00000001;
		if (0 == bit)
		{
			if (1 == *coverPtr)
			{
				(*coverPtr)++;
				coverPtr++;
			}
			else if (1 == ((*coverPtr % 2) + 2) % 2)
			{
				(*coverPtr)--;
				coverPtr++;
			}
			else
			{
				coverPtr++;
			}
		}
		else
		{
			if (0 == ((*coverPtr % 2) + 2) % 2)
			{
				(*coverPtr)++;
				coverPtr++;
			}
			else
			{
				coverPtr++;
			}
		}
	}

	//lsbǶ���㷨
	for (int i = 0; i < messageLength; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			bit = (int(*message) >> (7 - j))& 0x00000001;
			if (0 == bit)
			{
				if (1 == *coverPtr)
				{
					(*coverPtr)++;
					coverPtr++;
				}
				else if (1 == ((*coverPtr % 2) + 2) % 2)
				{
					(*coverPtr)--;
					coverPtr++;
				}
				else
				{
					coverPtr++;
				}
			}
			else
			{
				if (0 == ((*coverPtr % 2) + 2) % 2)
				{
					(*coverPtr)++;
					coverPtr++;
				}
				else
				{
					coverPtr++;
				}
			}
		}
		message++;
	}
	int * a = valid_cover_embed;
	for (int i = 0; i < 32; i++)
	{
		cout << "��" << i << "��:" << *a << endl;
		a++;
	}
	///д���޸ĺ����Ч����
	img_jpeg_ptr->setValidCover(valid_cover_embed);

	///д��ͼ������
	img_jpeg_ptr->write(outfilePath);
}

char* lsb_algorithm::lsb_extract(const char* infilePath, int seed, int headLength)
{
	///������ز���
	const char* jpeg_Inputpath = infilePath;

	///��ʼ��ͼ��
	img* img_jpeg_ptr = img_factory::createImage(jpeg_Inputpath, seed, headLength);

	///��ȡ��Ӧ����Ч����
	int* valid_cover_extract = img_jpeg_ptr->getValidCover();

	//**��ȡ�����ص�����**//
	int bit = 0;
	int messageLength = 0;

	//��ȡmessage�ĳ���
	for (int i = 0; i < 32; i++)
	{
		messageLength = (*valid_cover_extract & 0x00000001) + (messageLength << 1);
		valid_cover_extract++;
	}

	//��ȡmessage
	char *message	= new char[messageLength];
	char* messagePtr = message;
	for (int i = 0; i < messageLength; i++)
	{
		bit = 0;
		for (int j = 0; j < 8; j++)
		{
			bit = (*valid_cover_extract & 0x00000001) + (bit << 1);
			valid_cover_extract++;
		}
		*messagePtr = char(bit);
		messagePtr++;
	}
	*messagePtr = '\0';
	
	return message;
}
