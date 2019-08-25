#ifndef __IMG_H__
#define __IMG_H__

#include <vector>
#include"mat2D.h"





class img {
public:
    virtual ~img() {

    }

public:
    /// <summary>
    ///  获取头部像素或系数
    /// </summary>
    /// <returns>该函数申请内存, 需要调用者释放</returns>
    virtual int* getHead() = 0;
    virtual void setHead( const int *head ) = 0;

    /// <summary>
    /// 获取有效的载体
    /// JPEG图像有效载体是非零交流系数
    /// 空域图像有效载体是所有非透明像素
    /// </summary>
    /// <returns>该函数申请内存, 需要调用者释放</returns>
    virtual int* getValidCover() = 0;
    virtual void setValidCover( const int *cover) = 0;

    /// <summary>
    /// 获取图像有效载体的个数
    /// </summary>
    /// <returns>有效载体长度, 不包括协议头部</returns>
    virtual int getSize() = 0;

    // 将图像存回硬板
    virtual void write( const char *filename ) = 0;

    virtual int maxCoverElement() = 0;
    virtual int minCoverElement() = 0;

    virtual std::vector<mat2D<int> *> getElementMatrix() = 0;

    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
    virtual int getChannels() = 0;

    // 主要是为了根据该索引, 获得对应的代价的索引
    virtual std::vector<int> getValidCoverIndex()= 0;
};

#endif