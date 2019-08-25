/*
	lsb algorithm 
	completed by jeffci 2016/7/6
*/

class lsb_algorithm {
	
public:
	void lsb_embed(const char* infilePath, const char* outfilePath, int seed, int headLength, char* message);
	char* lsb_extract(const char* infilePath, int seed, int headLength);
};

