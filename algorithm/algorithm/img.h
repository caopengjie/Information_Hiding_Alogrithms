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
    ///  ��ȡͷ�����ػ�ϵ��
    /// </summary>
    /// <returns>�ú��������ڴ�, ��Ҫ�������ͷ�</returns>
    virtual int* getHead() = 0;
    virtual void setHead( const int *head ) = 0;

    /// <summary>
    /// ��ȡ��Ч������
    /// JPEGͼ����Ч�����Ƿ��㽻��ϵ��
    /// ����ͼ����Ч���������з�͸������
    /// </summary>
    /// <returns>�ú��������ڴ�, ��Ҫ�������ͷ�</returns>
    virtual int* getValidCover() = 0;
    virtual void setValidCover( const int *cover) = 0;

    /// <summary>
    /// ��ȡͼ����Ч����ĸ���
    /// </summary>
    /// <returns>��Ч���峤��, ������Э��ͷ��</returns>
    virtual int getSize() = 0;

    // ��ͼ����Ӳ��
    virtual void write( const char *filename ) = 0;

    virtual int maxCoverElement() = 0;
    virtual int minCoverElement() = 0;

    virtual std::vector<mat2D<int> *> getElementMatrix() = 0;

    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
    virtual int getChannels() = 0;

    // ��Ҫ��Ϊ�˸��ݸ�����, ��ö�Ӧ�Ĵ��۵�����
    virtual std::vector<int> getValidCoverIndex()= 0;
};

#endif