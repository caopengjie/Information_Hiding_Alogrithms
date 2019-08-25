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

	///获取图像的width以及height
	int img_jpeg_width = img_jpeg_ptr->getWidth();
	int img_jpeg_height = img_jpeg_ptr->getHeight();

	///获取图像的channel
	int channel = img_jpeg_ptr->getChannels();

	///获取图像基于该通道的coeff
	vector<mat2D<int>*> coeff = img_jpeg_ptr->getElementMatrix();
	
	///获取图像的有效载体
	int *valid_cover_embed = new int[img_jpeg_ptr->getSize()];
	valid_cover_embed = img_jpeg_ptr->getValidCover();

	//**修改最低有效位//
	int bit = 0;
	int *coverPtr = valid_cover_embed;
	int messageLength = strlen(message);

	//存储message的长度
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

	//lsb嵌入算法
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
		cout << "第" << i << "个:" << *a << endl;
		a++;
	}
	///写回修改后的有效载体
	img_jpeg_ptr->setValidCover(valid_cover_embed);

	///写回图像数据
	img_jpeg_ptr->write(outfilePath);
}

char* lsb_algorithm::lsb_extract(const char* infilePath, int seed, int headLength)
{
	///设置相关参数
	const char* jpeg_Inputpath = infilePath;

	///初始化图像
	img* img_jpeg_ptr = img_factory::createImage(jpeg_Inputpath, seed, headLength);

	///获取相应的有效载体
	int* valid_cover_extract = img_jpeg_ptr->getValidCover();

	//**获取所隐藏的数据**//
	int bit = 0;
	int messageLength = 0;

	//获取message的长度
	for (int i = 0; i < 32; i++)
	{
		messageLength = (*valid_cover_extract & 0x00000001) + (messageLength << 1);
		valid_cover_extract++;
	}

	//提取message
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
