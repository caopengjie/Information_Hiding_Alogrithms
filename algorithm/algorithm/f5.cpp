#include <iostream>
#include "img_factory.h"
#include "img_jpeg.h"
#include "img.h"
#include "f5.h"
using namespace std;

/*member function*/
void f5_algorithm::f5_embed(const char* infilePath, const char* outfilePath, int seed, int headLength, char* message)
{
	///设置相关参数
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
	int *valid_cover = img_jpeg_ptr->getValidCover();
	int * valid_cover_embed_ptr = valid_cover;

	//**f5有效嵌入
	///获取图像有效体的长度
	int valid_cover_len = img_jpeg_ptr->getSize();

	///获取信息长度
	int message_len = strlen(message);

	///计算出三元组(1,n,k)
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

	///利用lsb算法嵌入文件长度
	//存储message的长度
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


	//f5算法:取出有效的n个载体，k位嵌入信息位,并嵌入
	int change_num = 0;                                     //无效的嵌入->产生了新的0
	if (k != 1 && n != 1)
	{
		int *cover_n = new int[n];
		int *cover_n_begin = cover_n;                      //记录首地址
		int *copy_back = valid_cover_embed_ptr;                //写回数据地址
		int eight_bits_remain = 0;
		bool isend = false;
		int messagebits_k;                                       //存放kbit信息
		int charbits;
		int charnextbitget;

		do
		{
			messagebits_k = 0;
			//获取k位嵌入信息位
			for (int i = 0; i < k; i++)
			{
				if (eight_bits_remain == 0)
				{
					//如果读入字符串尾端的话就跳出循环
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

				//k bits 获取
				charnextbitget = charbits & 1;
				charbits >>= 1;
				eight_bits_remain--;
				messagebits_k |= charnextbitget << i;
			}

			//kbits信息嵌入
			cover_n = cover_n_begin;

			///初始获取n个嵌入位置
			for (int i = 0; i < n; i++)
			{
				*cover_n = *valid_cover_embed_ptr;
				int temp = *valid_cover_embed_ptr;
				cover_n++;
				valid_cover_embed_ptr++;
			}
			bool embed_success = false;
			do{
				//获取改变的位置change_loc,依据散列函数 ：f(a) = (ai*i)^....(an*n)
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
				//进行系数的修改(信息的嵌入)
				cover_n = cover_n_begin;
				if (change_loc == 0) break;
				for (int i = 1; i <= change_loc; i++)
				{
					cover_n++;
				}
				//操作多加上了1操作
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
			//进行n个信息的写回操作
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




	else  //当n=1,k=1时则进行lsb嵌入操作
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

	///写回修改后的有效载体
	img_jpeg_ptr->setValidCover(valid_cover);

	///写回图像数据
	img_jpeg_ptr->write(jpeg_outputpath);

	///释放相关指针
	delete[] valid_cover;
}

char* f5_algorithm::f5_extract(const char* infilePath, int seed, int headLength)
{
	///设置相应的参数
	const char* jpeg_Inputpath = infilePath;

	///初始化图像
	img *img_jpeg_ptr = img_factory::createImage(jpeg_Inputpath, seed, headLength);

	///获取相应的有效载体
	int *valid_cover_extract = img_jpeg_ptr->getValidCover();

	//**f5有效提取
	//获取信息的长度
	int strbit = 0;
	int bit = 0;

	//获取message的长度
	for (int i = 0; i < 32; i++)
	{
		bit = *valid_cover_extract & 0x00000001;
		strbit = (*valid_cover_extract & 0x00000001) + (strbit << 1);
		valid_cover_extract++;
	}
	int message_len = strbit;

	//分配字符串信息内存
	char *message = new char[message_len];
	char *message_ptr = message;

	//获取图像有效体的长度
	int valid_cover_extract_len = img_jpeg_ptr->getSize();

	///计算出三元组(1,n,k)
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

	//F5提取算法，取得n个cover，获取k比特信息
	bool isEnd = false;
	int HASH;
	int message_extract_len = 0;
	int avalible_extracted_bitslen = 0;
	int byte_extracted_message = 0;
	do
	{
		//初始HASH为0
		HASH = 0;

		//从n个cover中，利用散列函数f(a) = (ai*i)^....(an*n)，获取k比特信息
		for (int i = 1; i <= n; i++)
		{
			//如果*valid
			if (((*valid_cover_extract % 2) + 2) % 2 == 1)
			{
				HASH = HASH ^ i;
			}
			valid_cover_extract++;
		}

		//将k比特信息输出
		for (int i = 0; i < k; i++)
		{
			byte_extracted_message |= (HASH >> i & 1) << avalible_extracted_bitslen++;
			if (8 == avalible_extracted_bitslen)
			{
				*message_ptr = char(byte_extracted_message);
				message_ptr++;

				//重置相关的参数设置
				byte_extracted_message = 0;
				avalible_extracted_bitslen = 0;
				message_extract_len++;
				if (message_extract_len == message_len)
				{
					isEnd = true;
					//结束提取
					break;
				}
			}
		}
	} while (!isEnd);

	//字符串尾后添加终结符
	*message_ptr = '\0';
	return message;
}