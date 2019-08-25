/*
f5 algorithm
completed by jeffci 2016/7/7
*/

class f5_algorithm {

public:
	void f5_embed(const char* infilePath, const char* outfilePath, int seed, int headLength, char* message);
	char* f5_extract(const char* infilePath, int seed, int headLength);
};
