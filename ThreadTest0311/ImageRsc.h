//#pragma once
#ifndef __IMAGE_RSC__H
#define __IMAGE_RSC__H

#include <QtGlobal>

namespace cluster
{
namespace resource
{

struct imgToRscEntry
{
    const char* nameImage;
    qint64 offsetImg;
    int lengthImg;
    int formatImg;
    int widthImg;
    int heightImg;
    int imgSize;
};

extern imgToRscEntry tableImg[];

} // namespace resource
} // namespace cluster

#endif// __IMAGE_RSC__H
