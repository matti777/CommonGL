//
//  CommonFunctionsIOS.mm
//  MMark
//
//  Created by Matti Dahlbom on 22.6.2012.
//

#include <stdio.h>

#import "ImageHelper.h"

#include "CommonFunctions.h"

static const size_t RGBAPixelSize = 4;

bool ReadBundleFile(const char* fileName, bool zeropad, 
                    size_t* size, void** buffer) 
{
    NSBundle* bundle = [NSBundle mainBundle];
    NSString* path = [bundle pathForResource:[NSString stringWithUTF8String:fileName] ofType:nil];
    if ( path == nil ) {
        LOG_DEBUG("ReadBundleFile(): Failed to find file: %s", fileName);
        return false;
    }

    NSData* nsdata = [NSData dataWithContentsOfFile:path];
    
    size_t totalSize = nsdata.length;
    if ( zeropad ) 
    {
        totalSize++;
    }
    
    char* data = (char*)malloc(totalSize);
    if ( data == NULL )
    {
        LOG_DEBUG("ReadBundleFile(): Failed to malloc() %d bytes", totalSize);
        return false;
    }

    // Copy the file data into the newly allocated buffer
    memset(data, 0, totalSize);
    memcpy(data, nsdata.bytes, nsdata.length);
    
    // Fill in the caller's data
    *buffer = data;
    *size = totalSize;
    
    return true;
}

bool LoadImageFromBundle(const char* imageName, void** data, 
                         int* imageWidth, int* imageHeight) 
{
    UIImage* image = [UIImage imageNamed:[NSString stringWithUTF8String:imageName]];
    if ( image == nil ) {
        LOG_DEBUG("LoadImageFromBundle(): Failed to load texture file %s", imageName);
        return false;
    }
    
    int width = (int)image.size.width;
    int height = (int)image.size.height;
    int scanlineSize = width * RGBAPixelSize;
    
    // Extract RGBA data
    unsigned char* imageData = [ImageHelper convertUIImageToBitmapRGBA8:image];
    
    // Flip the image data scanlines around to make it OpenGL format
    unsigned char* flippedData = (unsigned char*)malloc(width * height * RGBAPixelSize);
    if ( flippedData == NULL ) 
    {
        LOG_DEBUG("LoadImageFromBundle(): memory allocation failed.");
        return false;
    }
    
    unsigned char* srcPtr = imageData + (height - 1) * scanlineSize;
    unsigned char* dstPtr = flippedData; 
    
    for ( int i = 0; i < height; i++ ) 
    {
        memcpy(dstPtr, srcPtr, scanlineSize);
        srcPtr -= scanlineSize;
        dstPtr += scanlineSize;
    }
    
    free(imageData);
    *data = flippedData;
    *imageWidth = width;
    *imageHeight = height;
    
    return true;
}

bool Load2DTextureFromBundle(const char* imageName, GLuint* texture,
                             bool clamp, bool useMipmaps) 
{
    void* data; 
    int width, height; 
    
    LOG_DEBUG("Load2DTextureFromBundle(): imageName: %s", imageName);
    
    if ( !LoadImageFromBundle(imageName, &data, &width, &height) ) 
    {
        return false;
    }
    
    // Upload the texture to OpenGL and create the texture object
    Create2DTexture(width, height, data, texture, clamp, useMipmaps);
    free(data);
    
    return true;
}

bool LoadCubeMapTargetTexture(GLenum target, const char* imageName)
{
    void* data; 
    int width, height; 
    
    if ( !LoadImageFromBundle(imageName, &data, &width, &height) ) 
    {
        return false;
    }
    
    // Upload the texture to OpenGL and create the texture object
    glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    free(data);
    
    return true;
}

std::string RandomUuid()
{
    CFUUIDRef theUUID = CFUUIDCreate(NULL);
    CFStringRef cfuuid = CFUUIDCreateString(NULL, theUUID);
    CFRelease(theUUID);
    NSString* uuid = (NSString*)cfuuid;
    [uuid autorelease];
    
    return std::string([uuid UTF8String]);
}

void PrintLogDebug(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char msg[1024 * 10];
    vsnprintf(msg, sizeof(msg) - 1, fmt, args);    
    va_end(args);
    
    NSString* s = [[NSString alloc] initWithUTF8String:msg];
    NSLog(@"%@", s);
    [s release];
}

void DebugAssert(bool criteria)
{
    assert(criteria);
}

