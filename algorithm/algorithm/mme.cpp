#include <iostream>
#include "img_factory.h"
#include "img_jpeg.h"
#include "img.h"
#include "mme.h"
using namespace std;

/*member function*/
void mme_algorithm::mme_embed(const char* infilePath, const char* outputPath, int seed, int headLength, char* message)
{
	///������ز���
	const char *jpeg_Inputpath = infilePath;
	const char *jpeg_Outputpath = outputPath;
	img *img_jpeg_ptr = img_factory::createImage(jpeg_Inputpath, seed, headLength);

	///��ȡͼ���width�Լ�height
	int img_jpeg_width = img_jpeg_ptr->getWidth();
	int img_jpeg_height = img_jpeg_ptr->getHeight();

	///��ȡͼ����ڸ�ͨ����coeff
	vector<mat2D<int> *> coeff_array = img_jpeg_ptr->getElementMatrix();

	///��ȡͼ�����Ч����
	int *valid_cover_mme = new int[img_jpeg_ptr->getSize()];
	valid_cover_mme = img_jpeg_ptr->getValidCover();
	int *valid_cover_mme_ptr = valid_cover_mme;

	///��ȡͼ���Yͨ��
	int channelSize = img_jpeg_width * img_jpeg_height;
	int *cover_Y = new int[channelSize];
	for (int i = 0; i < channelSize; i++)
	{
		int row = i / img_jpeg_width;
		int col = i%img_jpeg_width;
		cover_Y[i] = coeff_array[0]->Read(row, col);
	}

	///��Yͨ�����д��ۼ���
	double * cost_Y = new double[channelSize];
	int rows = img_jpeg_height;
	int cols = img_jpeg_width;
	double gia = 1.3;
	double gir = 1.0;

	for (int i = 0; i < channelSize; i++)
	{
		int row = i / img_jpeg_width;
		int col = i%img_jpeg_width;
		double c = abs(cover_Y[i]);
		double dia = 0.0;
		double dir = 0.0;
		double p_1 = 0.0;
		double p_8 = 0.0;
		//����  1/|c| + |dia| + |gia|
		//step 1
		if ((row + 1) >= rows)
		{
			dia = 0.0;
		}
		else
		{
			dia = abs(cover_Y[i + img_jpeg_width]);
		}
		p_1 = 1 / (c + dia + gia);

		//step2
		if ((row - 1) < 0)
		{
			dia = 0.0;
		}
		else
		{
			dia = abs(cover_Y[i - img_jpeg_width]);
		}
		p_1 = 1 / (c + dia + gia) + p_1;

		//step3
		if ((col + 1) >= cols)
		{
			dia = 0.0;
		}
		else
		{
			dia = abs(cover_Y[i + 1]);
		}
		p_1 = 1 / (c + dia + gia) + p_1;

		//step4
		if ((col - 1) < 0)
		{
			dia = 0.0;
		}
		else
		{
			dia = abs(cover_Y[i - 1]);
		}
		p_1 = 1 / (c + dia + gia) + p_1;

		//���� 1/|c + dir + gir|
		//step1
		if ((row + 8) >= rows)
		{
			dir = 0.0;
		}
		else
		{
			dir = abs(cover_Y[i + 8 * img_jpeg_width]);
		}
		p_8 = 1 / (c + dir + gir);

		//step2
		if ((row - 8) < 0)
		{
			dir = 0.0;
		}
		else
		{
			dir = abs(cover_Y[i - 8 * img_jpeg_width]);
		}
		p_8 = 1 / (c + dir + gir) + p_8;

		//step3
		if ((col + 8) >= cols)
		{
			dir = 0.0;
		}
		else
		{
			dir = abs(cover_Y[i + 8]);
		}
		p_8 = 1 / (c + dir + gir) + p_8;

		//step4
		if ((col - 8) < 0)
		{
			dir = 0.0;
		}
		else
		{
			dir = abs(cover_Y[i - 8]);
		}
		p_8 = 1 / (c + dir + gir) + p_8;

		cost_Y[i] = p_1 + p_8;
	}

	///��ȡ��Ч����Ĵ���
	//��ȡ�������Ч��С
	int valid_size = img_jpeg_ptr->getSize();
	double *valid_cover_cost = new double[valid_size];
	vector<int> indexRand = img_jpeg_ptr->getValidCoverIndex();
	for (int i = 0; i < valid_size; i++)
	{
		valid_cover_cost[i] = cost_Y[indexRand.at(i)];
	}
	int message_len = strlen(message);

	//**mme��ЧǶ��
	//��ȡ��Ϣ�ĳ���
	int valid_cover_cost_Count = 0;

	//�������Ԫ��
	int k = 1;
	int n = 1;
	double int_real_efiicient_rate = (message_len * 8.0 * 100.0) / (valid_size);
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

	///����lsb�㷨Ƕ���ļ���Ϣ����
	//�洢message�ĳ���
	int bit = 0;
	for (int i = 0; i < 32; i++)
	{
		bit = (message_len >> (31 - i)) & 0x00000001;
		if (0 == bit)
		{
			if (1 == *valid_cover_mme_ptr)
			{
				(*valid_cover_mme_ptr)++;
				valid_cover_mme_ptr++;
			}
			else if (1 == ((*valid_cover_mme_ptr % 2) + 2) % 2)
			{
				(*valid_cover_mme_ptr)--;
				valid_cover_mme_ptr++;
			}
			else
			{
				valid_cover_mme_ptr++;
			}
		}
		else
		{
			if (0 == ((*valid_cover_mme_ptr % 2) + 2) % 2)
			{
				(*valid_cover_mme_ptr)++;
				valid_cover_mme_ptr++;
			}
			else
			{
				valid_cover_mme_ptr++;
			}
		}
	}
	//Խ��32��cover
	valid_cover_cost_Count = valid_cover_cost_Count + 32;
	///mme����Ƕ�룬ѡȡn��coverǶ��k������Ϣ
	int change_num = 0;                                     //��Ч��Ƕ��->�������µ�0
	if (k != 1 && n != 1)
	{
		int *cover_n = new int[n];
		int *cover_n_begin = cover_n;                      //��¼�׵�ַ
		int *copy_back = valid_cover_mme_ptr;                //д�����ݵ�ַ
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
				*cover_n = *valid_cover_mme_ptr;
				int temp = *valid_cover_mme_ptr;
				cover_n++;
				valid_cover_mme_ptr++;
			}
			//Խ��n��cover
			valid_cover_cost_Count = valid_cover_cost_Count + n;

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

				if (change_loc == 0) break;
				//���д��۵ļ��㲢��ȡ������С�ĸı�λ��
				int *index_divide = new int[n];
				index_divide = indexForwardTwo(change_loc, n);
				int *two_index = new int[2];
				two_index = twoIndexGet(index_divide, valid_cover_cost_Count - n, valid_cover_cost, n);

				//����ϵ�����޸�(��Ϣ��Ƕ��)
				//���������0
				if (two_index[1] != 0)
				{
					//step1:���е�һ��ϵ�����޸�
					int change_loc_f = two_index[0];
					cover_n = cover_n_begin;
					for (int i = 1; i <= change_loc_f; i++)
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

					//step2:���еڶ���ϵ�����޸�
					int change_loc_s = two_index[1];
					cover_n = cover_n_begin;
					for (int i = 1; i <= change_loc_s; i++)
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
				}
				else
				{
					//���ϵ��Ϊ0��������ԭϵ�����иı�
					cover_n = cover_n_begin;
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
		//lsbǶ���㷨
		for (int i = 0; i < messagelen; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				bit = (int(*message) >> (7 - j)) & 0x00000001;
				if (0 == bit)
				{
					if (1 == *valid_cover_mme_ptr)
					{
						(*valid_cover_mme_ptr)++;
						valid_cover_mme_ptr++;
					}
					else if (1 == ((*valid_cover_mme_ptr % 2) + 2) % 2)
					{
						(*valid_cover_mme_ptr)--;
						valid_cover_mme_ptr++;
					}
					else
					{
						valid_cover_mme_ptr++;
					}
				}
				else
				{
					if (0 == ((*valid_cover_mme_ptr % 2) + 2) % 2)
					{
						(*valid_cover_mme_ptr)++;
						valid_cover_mme_ptr++;
					}
					else
					{
						valid_cover_mme_ptr++;
					}
				}

			}
		}
	}

	///д���޸ĺ����Ч����
	img_jpeg_ptr->setValidCover(valid_cover_mme);

	///д��ͼ������
	img_jpeg_ptr->write(jpeg_Outputpath);

	delete[] valid_cover_mme;
}

char* mme_algorithm::mme_extract(const char* infilePath, int seed, int headLength)
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
		cout << "*valid_cover_extractr:" << *valid_cover_extract << endl;
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

//**MMEcoding**//
int* mme_algorithm::indexForwardTwo(int index, int n)
{
	int *indexTwo = new int[n];
	indexTwo[0] = index;
	int indexTwoCount = 1;
	bool flageCotain = false;

	for (int i = 1; i <= n / 2; i++)
	{
		for (int j = 1; j <= n; j++)
		{
			if (j == index) continue;
			int a = index ^j;
			//����Ƿ��Ѱ�����ϵ����
			for (int k = 0; k < n; k++)
			{
				if (indexTwo[k] == a)
				{
					flageCotain = true;
				}
			}
			//û�а�������������
			if (!flageCotain)
			{
				indexTwo[indexTwoCount] = j;
				indexTwoCount++;
				indexTwo[indexTwoCount] = a;
				indexTwoCount++;
			}
			flageCotain = false;
		}
	}
	return indexTwo;
}

int* mme_algorithm::twoIndexGet(int indexTwo[], int cost_index_Count, double cost[], int n)
{
	int j = 1;
	double PRECISION = 0.000000000000001;
	int *two_index = new int[2];
	int *total_cost = new int[n / 2 + 1];
	int total_cost_Count = 1;
	total_cost[0] = cost[cost_index_Count + indexTwo[0] - 1];

	//��ȡ����ϵ���ԵĴ���֮��
	for (int i = 0; i < n / 2; i++)
	{
		double cost_f = cost[cost_index_Count + indexTwo[j] - 1];
		j++;
		double cost_s = cost[cost_index_Count + indexTwo[j] - 1];
		j++;
		total_cost[total_cost_Count] = cost_f + cost_s;
		total_cost_Count++;
	}

	//��ȡtotal cost��͵�ϵ����
	double min_cost = total_cost[0];
	int index_f = 0;
	for (int i = 0; i < total_cost_Count; i++)
	{
		if (total_cost[i] - min_cost < PRECISION)
		{
			min_cost = total_cost[i];
			index_f = i;
		}
	}

	//�õ���λϵ��ֵ
	if (0 == index_f)
	{
		two_index[0] = indexTwo[0];
		two_index[1] = 0;
	}
	else
	{
		two_index[0] = indexTwo[index_f * 2 - 1];
		two_index[1] = indexTwo[index_f * 2];
	}
	return two_index;
}