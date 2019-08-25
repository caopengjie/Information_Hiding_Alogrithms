/*
mme algorithm
completed by jeffci 2016/7/7
*/

class mme_algorithm {

public:
	void mme_embed(const char* infilePath, const char* outfilePath, int seed, int headLength, char* message);

	char* mme_extract(const char* infilePath, int seed, int headLength);

	//计算所有可能的系数对（注意：这里只针对二维的）
	int* indexForwardTwo(int index, int n);

	//计算代价最小的方案
	int* twoIndexGet(int indexTwo[], int cost_index_Count, double cost[], int n);
};
