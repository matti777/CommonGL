#include <QDebug>
#include <QFile>
#include <QUuid>

#include "CommonFunctions.h"

bool ReadBundleFile(const char* fileName, bool zeropad,
                    size_t* size, void** buffer)
{
    QString resFileName = QString(":/") + fileName;
    QFile file(resFileName);
    if ( !file.open(QIODevice::ReadOnly) )
    {
        LOG_DEBUG("ReadBundleFile(): Failed to open file: %s", fileName);
        return false;
    }

    size_t totalSize = file.size();
    if ( zeropad )
    {
        // One extra byte for zero padding just in case we're reading strings
        totalSize++;
    }

    char* data = (char*)malloc(totalSize);
    if ( data == NULL )
    {
        LOG_DEBUG("ReadBundleFile(): Failed to malloc() %d bytes", totalSize);
        file.close();
        return false;
    }

    memset(data, 0, totalSize);
    QByteArray byteArray = file.readAll();
    memcpy(data, byteArray.constData(), file.size());
    file.close();

    // Fill in the caller's data
    *buffer = data;
    *size = totalSize;

    return true;
}

bool LoadImageFromBundle(const char* imageName, QImage& image)
{
    LOG_DEBUG("LoadImageFromBundle(): loading '%s'", imageName);
    QString imageNameS = QString(imageName);

    QString resImgName = QString(":/") + imageNameS;
    if ( !image.load(resImgName) )
    {
        LOG_DEBUG("failed to load texture: %s", imageName);
        return false;
    }

    image = QGLWidget::convertToGLFormat(image);
    if ( image.isNull() )
    {
        LOG_DEBUG("failed to convert texture to GL format.");
        return false;
    }

    return true;
}

bool Load2DTextureFromBundle(const char* imageName, GLuint* texture,
                             bool clamp, bool useMipmaps)
{
    QImage image;

    // Load the texture image file
    if ( !LoadImageFromBundle(imageName, image) )
    {
        return false;
    }

    // Upload the texture to OpenGL and create the texture object
    Create2DTexture(image.width(), image.height(), image.bits(),
                    texture, clamp, useMipmaps);
    return true;
}

bool LoadCubeMapTargetTexture(GLenum target, const char* imageName)
{
    QImage image;

    if ( !LoadImageFromBundle(imageName, image) )
    {
        return false;
    }
    glTexImage2D(target, 0, GL_RGBA,
                 image.width(), image.height(), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image.bits());

    return true;
}

std::string RandomUuid()
{
    // .mid(1,36) strips the curly braces added by toString(), duh
    return std::string(QUuid::createUuid().toString().mid(1,36).toUtf8());
}

void PrintLogDebug(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char msg[1024*16];
    vsnprintf(msg, sizeof(msg) - 1, fmt, args);
    qDebug() << msg;
    va_end(args);
}

#ifdef DEBUG
void DebugAssert(bool criteria)
{
    Q_ASSERT(criteria);
}
#endif


