#include "img_factory.h"
#include "img_jpeg.h"
#include"img.h"



img *img_factory::createImage( const char *filename, int seed, int headLength ) {
     img *img = nullptr;
	 std::string lowerExt = "";

	 ///获取后缀
	 char *endptr = new char[strlen(filename)+ 1];
	 strcpy(endptr, filename);
	 int len_endptr = strlen(endptr);
	 while (len_endptr >0)
	 {
		 if (*endptr == '.')
		 {
		 	 do
		  	 {
			 	 lowerExt += *endptr;
				 endptr++;
				 len_endptr--;
			 } while (len_endptr >0);
		 }
			 endptr++;
			 len_endptr--;
	 }

	 ///判定后缀
    if ( lowerExt == ".jpg" || lowerExt == ".jpeg"||lowerExt == ".JPG" ) 
	  {
         img = new img_jpeg( filename, seed, headLength );
      }
	else 
	  {
         throw std::exception( "Only JPEG are supported" );
      }

    return img;
}


/// <summary>
/// 获取图像的域
/// </summary>
/// <param name="filename">图像绝对路径</param>
/// <returns> 1 表示 空域, 2 表示 JPEG域</returns>
ImageDomain img_factory::getImageDomain( const char *filename ) {
    std::string lowerExt = "";

	///获取后缀
	char *endptr = new char[strlen(filename) + 1];
	strcpy(endptr, filename);
	int len_endptr = strlen(endptr);
	while (len_endptr >0)
	{
		if (*endptr == '.')
		{
			do
			{
				lowerExt += *endptr;
				endptr++;
				len_endptr--;
			} while (len_endptr >0);
		}
		endptr++;
		len_endptr--;
	}

    if ( lowerExt == ".jpg" || lowerExt == ".jpeg" ) {
        return ImageDomain::JPEG;
    } else {
        return ImageDomain::UNKNOWN;
    }
}