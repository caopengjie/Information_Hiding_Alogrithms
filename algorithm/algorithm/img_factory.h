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
    /// 获取图像的域
    /// </summary>
    /// <param name="filename">图像绝对路径</param>
    /// <returns> 1 表示 空域, 2 表示 JPEG域</returns>
    static ImageDomain getImageDomain( const char *filename );
};

#endif