#ifndef __IMG_SPA_FACTORY_H__
#define __IMG_SPA_FACTORY_H__

#include "img.h"

enum ImageDomain {
    JPEG,
    SPATIAL,
    UNKNOWN
};

class img_factory {
public:
    static img *createImage( const char *filename, int seed, int headLength );

    /// <summary>
    /// ��ȡͼ�����
    /// </summary>
    /// <param name="filename">ͼ�����·��</param>
    /// <returns> 1 ��ʾ ����, 2 ��ʾ JPEG��</returns>
    static ImageDomain getImageDomain( const char *filename );
};

#endif