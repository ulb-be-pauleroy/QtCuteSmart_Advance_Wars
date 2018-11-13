#include "imageloader.h"

ImageLoader::ImageLoader()
{

}

QImage *ImageLoader::loadImage(int a)
{
    switch (a){
    case 1 : return new QImage(":/Terrains/Images/Terrains/plain.gif");
    case 2 : return new QImage(":/Terrains/Images/Terrains/mountain.gif");
    case 3 : return new QImage(":/Terrains/Images/Terrains/wood.gif");
    case 4 : return new QImage(":/Terrains/Images/Terrains/hriver.gif");
    case 5 : return new QImage(":/Terrains/Images/Terrains/vriver.gif");
    case 6 : return new QImage(":/Terrains/Images/Terrains/criver.gif");
    case 7 : return new QImage(":/Terrains/Images/Terrains/esriver.gif");
    case 8 : return new QImage(":/Terrains/Images/Terrains/swriver.gif");
    case 9 : return new QImage(":/Terrains/Images/Terrains/wnriver.gif");
    case 10 : return new QImage(":/Terrains/Images/Terrains/neriver.gif");
    case 11 : return new QImage(":/Terrains/Images/Terrains/eswriver.gif");
    case 12 : return new QImage(":/Terrains/Images/Terrains/swnriver.gif");
    case 13 : return new QImage(":/Terrains/Images/Terrains/wneriver.gif");
    case 14 : return new QImage(":/Terrains/Images/Terrains/nesriver.gif");
    case 15 : return new QImage(":/Terrains/Images/Terrains/hroad.gif");
    case 16 : return new QImage(":/Terrains/Images/Terrains/vroad.gif");
    case 17 : return new QImage(":/Terrains/Images/Terrains/croad.gif");
    case 18 : return new QImage(":/Terrains/Images/Terrains/esroad.gif");
    case 19 : return new QImage(":/Terrains/Images/Terrains/swroad.gif");
    case 20 : return new QImage(":/Terrains/Images/Terrains/wnroad.gif");
    case 21 : return new QImage(":/Terrains/Images/Terrains/neroad.gif");
    case 22 : return new QImage(":/Terrains/Images/Terrains/eswroad.gif");
    case 23 : return new QImage(":/Terrains/Images/Terrains/swnroad.gif");
    case 24 : return new QImage(":/Terrains/Images/Terrains/wneroad.gif");
    case 25 : return new QImage(":/Terrains/Images/Terrains/nesroad.gif");
    case 26 : return new QImage(":/Terrains/Images/Terrains/hbridge.gif");
    case 27 : return new QImage(":/Terrains/Images/Terrains/vbridge.gif");
    case 28 : return new QImage(":/Terrains/Images/Terrains/sea.gif");
    case 29 : return new QImage(":/Terrains/Images/Terrains/hshoal.gif");
    case 30 : return new QImage(":/Terrains/Images/Terrains/hshoaln.gif");
    case 31 : return new QImage(":/Terrains/Images/Terrains/vshoal.gif");
    case 32 : return new QImage(":/Terrains/Images/Terrains/vshoale.gif");
    case 33 : return new QImage(":/Terrains/Images/Terrains/reef.gif");
    case 34 : return new QImage(":/Terrains/Images/Terrains/neutralcity.gif");
    case 35 : return new QImage(":/Terrains/Images/Terrains/neutralbase.gif");
    case 36 : return new QImage(":/Terrains/Images/Terrains/neutralairport.gif");
    case 37 : return new QImage(":/Terrains/Images/Terrains/neutralcity.gif");
    case 38 : return new QImage(":/Terrains/Images/Terrains/orangestarcity.gif");
    case 39 : return new QImage(":/Terrains/Images/Terrains/orangestarbase.gif");
    case 40 : return new QImage(":/Terrains/Images/Terrains/orangestarairport.gif");
    case 41 ... 42 : return new QImage(":/Terrains/Images/Terrains/orangestarcity.gif");
    case 43 : return new QImage(":/Terrains/Images/Terrains/bluemooncity.gif");
    case 44 : return new QImage(":/Terrains/Images/Terrains/bluemoonbase.gif");
    case 45 : return new QImage(":/Terrains/Images/Terrains/bluemoonairport.gif");
    case 46 ... 47 : return new QImage(":/Terrains/Images/Terrains/bluemooncity.gif");
    case 101 : return new QImage(":/Terrains/Images/Terrains/vpipe.gif");
    case 102 : return new QImage(":/Terrains/Images/Terrains/hpipe.gif");
    case 103 : return new QImage(":/Terrains/Images/Terrains/nepipe.gif");
    case 104 : return new QImage(":/Terrains/Images/Terrains/espipe.gif");
    case 105 : return new QImage(":/Terrains/Images/Terrains/swpipe.gif");
    case 106 : return new QImage(":/Terrains/Images/Terrains/wnpipe.gif");
    case 107 : return new QImage(":/Terrains/Images/Terrains/npipeend.gif");
    case 108 : return new QImage(":/Terrains/Images/Terrains/epipeend.gif");
    case 109 : return new QImage(":/Terrains/Images/Terrains/spipeend.gif");
    case 110 : return new QImage(":/Terrains/Images/Terrains/wpipeend.gif");
    case 111 ... 112 : return new QImage(":/Terrains/Images/Terrains/plain.gif");
    case 113 : return new QImage(":/Terrains/Images/Terrains/hpipeseam.gif");
    case 114 : return new QImage(":/Terrains/Images/Terrains/vpipeseam.gif");
    case 115 ... 116 : return new QImage(":/Terrains/Images/Terrains/plain.gif");
    case 129 :  return new QImage(":/Terrains/Images/Terrains/bluemooncity.gif");
    case 133 : return new QImage(":/Terrains/Images/Terrains/neutralcity.gif");
    case 134 : return new QImage(":/Terrains/Images/Terrains/orangestarcity.gif");
    case 140 : return new QImage(":/Terrains/Images/Terrains/bluemooncity.gif");
    case 145 : return new QImage(":/Terrains/Images/Terrains/neutralcity.gif");
    case 146 : return new QImage(":/Terrains/Images/Terrains/orangestarcity.gif");



    }
    return nullptr;
}
