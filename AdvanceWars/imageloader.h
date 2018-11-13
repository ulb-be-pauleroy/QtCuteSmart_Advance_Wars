#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QImage>



class ImageLoader  //Load images for map
{
public:
    ImageLoader();
    QImage* loadImage(int a);
};

#endif // IMAGELOADER_H
