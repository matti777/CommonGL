#include <FBase.h>
#include <FApp.h>
#include <FIo.h>
#include <FGraphics.h>
#include <FMedia.h>

#include "CommonFunctions.h"

using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::Io;
using namespace Tizen::Graphics;
using namespace Tizen::Base::Utility;
using namespace Tizen::Media;

bool ReadBundleFile(const char* fileName, bool zeropad,
                    size_t* size, void** buffer)
{
    String dirPath = App::GetInstance()->GetAppResourcePath();
    File file;

    result r = file.Construct(dirPath + fileName, "r");
    if( IsFailed(r) )
    {
	return false;
    }

    // Get file size
    file.Seek(FILESEEKPOSITION_END, 0);
    int fileSize = file.Tell();
    int totalSize = fileSize;
    LOG_DEBUG("File length being read: %d", fileSize);
    file.Seek(FILESEEKPOSITION_BEGIN, 0);

    if ( zeropad )
    {
	// One extra byte for zero padding just in case we're reading strings
	totalSize++;
    }

    char* data = (char*)malloc(totalSize);
    if ( data == NULL )
    {
	LOG_DEBUG("ReadBundleFile(): Failed to malloc() %d bytes", totalSize);
	return false;
    }

    memset(data, 0, totalSize);
    file.Read(data, fileSize);

    // Fill in the caller's data
    *buffer = data;
    *size = totalSize;

    return true;
}

Bitmap* ReadImageFromBundle(String fileName)
{
    String filepath = App::GetInstance()->GetAppResourcePath() + fileName;
    AppLogDebug("Loading image from path: %ls", filepath.GetPointer());

    Image image;
    image.Construct();

    // Flip the image for OpenGL
    int imageWidth = 0;
    int imageHeight = 0;
    std::unique_ptr<ByteBuffer> originalBuffer(image.DecodeToBufferN(filepath,
                                                                     BITMAP_PIXEL_FORMAT_R8G8B8A8,
                                                                     imageWidth, imageHeight));
    result r;
    Dimension dimension(imageWidth, imageHeight);
    ByteBuffer flippedBuffer;
    r = flippedBuffer.Construct(originalBuffer->GetCapacity());
    if ( IsFailed(r) )
    {
	LOG_INFO("Failed to Construct() bytebuffer; result: %d", r);
    }

    r = ImageUtil::Flip(*originalBuffer, flippedBuffer, dimension, IMAGE_FLIP_VERTICAL,
                        MEDIA_PIXEL_FORMAT_RGBA8888);
    if ( IsFailed(r) )
    {
	LOG_INFO("Failed to flip bitmap; result: %d", r);
    }

    Bitmap* bitmap = new (std::nothrow) Bitmap();
    bitmap->Construct(flippedBuffer, dimension, BITMAP_PIXEL_FORMAT_ARGB8888);

    return bitmap;
}

bool Load2DTextureFromBundle(const char* imageName, GLuint* texture,
				 bool clamp, bool useMipmaps)
{
    std::unique_ptr<Bitmap> bitmap(ReadImageFromBundle(imageName));
    BufferInfo bufferInfo;
    if ( IsFailed(bitmap->Lock(bufferInfo)) )
    {
	LOG_DEBUG("Failed to lock the bitmap!");
	return false;
    }

    LOG_DEBUG("Read bitmap with dimensions: %d x %d",
              bufferInfo.width, bufferInfo.height);

    // Upload the texture to OpenGL and create the texture object
    Create2DTexture(bufferInfo.width, bufferInfo.height, bufferInfo.pPixels,
                    texture, clamp, useMipmaps);

    LOG_GL_ERROR();

    bitmap->Unlock();

    return true;
}

bool LoadCubeMapTargetTexture(GLenum target, const char* imageName)
{
    std::unique_ptr<Bitmap> bitmap(ReadImageFromBundle(imageName));
    BufferInfo bufferInfo;
    if ( IsFailed(bitmap->Lock(bufferInfo)) )
    {
	LOG_DEBUG("Failed to lock the bitmap!");
	return false;
    }

    // Upload the texture to OpenGL and create the texture object
    glTexImage2D(target, 0, GL_RGBA,
                 bufferInfo.width, bufferInfo.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, bufferInfo.pPixels);

    bitmap->Unlock();

    return true;
}

std::string RandomUuid()
{
    std::unique_ptr<UuId> uuid(UuId::GenerateN());
    String uuidString = uuid->ToString();
    std::unique_ptr<ByteBuffer> buffer(StringUtil::StringToUtf8N(uuidString));
    return std::string((const char*)buffer->GetPointer());
}

void PrintLogDebug(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char msg[1024*16];
    vsnprintf(msg, sizeof(msg) - 1, fmt, args);
    AppLogDebug(msg);
    va_end(args);
}

#ifdef DEBUG
void DebugAssert(bool criteria)
{
    AppAssert(criteria);
}
#endif


