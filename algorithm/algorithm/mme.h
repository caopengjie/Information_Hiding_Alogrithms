/*
mme algorithm
completed by jeffci 2016/7/7
*/

class mme_algorithm {

public:
	void mme_embed(const char* infilePath, const char* outfilePath, int seed, int headLength, char* message);

	char* mme_extract(const char* infilePath, int seed, int headLength);

	//�������п��ܵ�ϵ���ԣ�ע�⣺����ֻ��Զ�ά�ģ�
	int* indexForwardTwo(int index, int n);

	//���������С�ķ���
	int* twoIndexGet(int indexTwo[], int cost_index_Count, double cost[], int n);
};
