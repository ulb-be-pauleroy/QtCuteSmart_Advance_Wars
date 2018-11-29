#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QImage>

class Game;

class ImageLoader  //Load images for map
{
public:
    ImageLoader();
    static QImage* loadImage(int a);
};

#endif // IMAGELOADER_H
