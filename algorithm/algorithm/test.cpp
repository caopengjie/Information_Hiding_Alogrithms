#include "lsb.h"
#include "f5.h"
#include "mme.h"
#include <iostream>
using namespace std;

int main()
{
	const char* jpeg_inputPath = "C:\\Users\\Jeffci\\Desktop\\1.jpg";
	const char* jpeg_outputPath = "C:\\Users\\Jeffci\\Desktop\\1_f5.jpg";
	int seed = 123;
	char* message = "信息工程研究所曹鹏杰";

	//动态创建一个对象
	//lsb_algorithm* lsb = new lsb_algorithm();
	f5_algorithm* f5 = new f5_algorithm();
	//mme_algorithm* mme = new mme_algorithm();

    //lsb->lsb_embed(jpeg_inputPath, jpeg_outputPath, seed, 0, message);
	f5->f5_embed(jpeg_inputPath, jpeg_outputPath, seed, 0, message);
	//mme->mme_embed(jpeg_inputPath, jpeg_outputPath, seed, 0, message);

	//char* message1 = lsb->lsb_extract(jpeg_outputPath, seed, 0);
	//char* message1 = f5->f5_extract(jpeg_outputPath, seed, 0);
	//char* message1 = mme->mme_extract(jpeg_outputPath, seed, 0);

   //cout << message1 << endl;

	delete f5;

	return 0;
}