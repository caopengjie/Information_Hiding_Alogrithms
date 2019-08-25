#ifndef JSTRUCT_H_
#define JSTRUCT_H_

#include <vector>

#include "img.h"
#include "mat2D.h"

// Struct for store an huff table
struct struct_huff_tables {
	std::vector<int> counts; 
	std::vector<int> symbols;
};

// 
struct struct_comp_info {
	int component_id;
	int h_samp_factor;
	int v_samp_factor;
	int quant_tbl_no;
	int dc_tbl_no;
	int ac_tbl_no;
};



class img_jpeg : public img {
    // inherite functions
public:
    int*    getHead();
    void    setHead( const int *head );
    int*    getValidCover();
    void    setValidCover( const int *cover );
    int     getSize();
    void    write( const char *filename );
    int     maxCoverElement();
    int     minCoverElement();

    std::vector<mat2D<int> *> getElementMatrix();
    int     getWidth();
    int     getHeight();
    int     getChannels();

    std::vector<int> getValidCoverIndex();

private:
	unsigned int	image_width;
	unsigned int	image_height;
	// 
	int				image_components;
	unsigned int	image_color_space;
	// 
	int				jpeg_components;
	unsigned int	jpeg_color_space;

	std::vector<char *>		markers;
	
	std::vector<mat2D<int> *>	coef_arrays;
	std::vector<mat2D<int> *>	spatial_arrays;
	std::vector<mat2D<int> *>	quant_tables;

    std::vector<struct_huff_tables *>	ac_huff_tables;
	std::vector<struct_huff_tables *>	dc_huff_tables;

	unsigned char					optimize_coding;
	std::vector<struct_comp_info *> comp_info;
	unsigned char					progressive_mode;

    int headLength;
    std::vector<int> randIndex;
    int seed;

public:
    img_jpeg( std::string filePath, int seed, int headLength, bool loadSpatial = false );
	~img_jpeg();

	void jpeg_write( std::string filePath, bool optimize_coding );

private:
	bool loadSpatial;
	void jpeg_load(std::string filePath);
	void spatial_load(std::string filePath);

};

#endif