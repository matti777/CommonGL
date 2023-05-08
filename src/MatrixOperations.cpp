#include <memory.h>
#include <math.h>

#include "MatrixOperations.h"

/*
  NOTE: THE FUNCTIONS IN THIS FILE DEAL WITH ROW MAJOR MATRICES.

  Indices of a OpenGL 4x4 matrix (float[16]) as follows:

  [  0   1   2   3  ]
  [  4   5   6   7  ]
  [  8   9  10  11  ]
  [ 12  13  14  15  ]

  Indices of a OpenGL 3x3 matrix (float[9]) as follows:

  [ 0  1  2 ]
  [ 3  4  5 ]
  [ 6  7  8 ]

  Indices of a OpenGL 3x1 vector (float[3]) as follows:

  [ 0 ]
  [ 1 ]
  [ 2 ]
*/

// Constrants
static const size_t Matrix4x4Size = sizeof(float[16]);
static const size_t Vector4x1Size = sizeof(float[4]);
static const size_t Vector3x1Size = sizeof(float[3]);

void MatrixSetIdentity(float* matrix)
{
    memset(matrix, 0, Matrix4x4Size);
    matrix[0] = 1.0;
    matrix[5] = 1.0;
    matrix[10] = 1.0;
    matrix[15] = 1.0;
}

void CalculateInverseTransform(const float* matrix, float* result)
{
    // This method is based on MathGem's Fast Matrix Inversion:
    // http://content.gpwiki.org/index.php/MathGem:Fast_Matrix_Inversion
    // It assumes non-scaled orthogonal transform matrix

    // Transpose the rotation 3x3 submatrix
    MatrixSetIdentity(result);
    for ( int i = 0; i < 3; i++ ) {
        for ( int j = 0; j < 3; j++ ) {
            result[i*4 + j] = matrix[i + j*4];
        }
    }

    // Calculate inverse of the translation
    const float* originalTranslation = &matrix[12];
    float inverseTranslatin[3];
    Transformv3(result, originalTranslation, inverseTranslatin);
    result[12] = -inverseTranslatin[0];
    result[13] = -inverseTranslatin[1];
    result[14] = -inverseTranslatin[2];

    //TODO obviously we cannot just negate the translation..?
    // see http://www.opengl.org/discussion_boards/showthread.php/177324-Inverse-Orthogonal-Matrix
    // Negate the translation component
//    result[12] = -matrix[12];
//    result[13] = -matrix[13];
//    result[14] = -matrix[14];

//    // Copy the rest of the values
//    result[3] = matrix[3];
//    result[7] = matrix[7];
//    result[11] = matrix[11];
//    result[15] = 1.0;
}

void MatrixSetLookat(float* matrix, const float* position, const float* target)
{
    MatrixSetIdentity(matrix);

    // Calculate a "forward" vector from position to target and normalize it
    float forward[3];
    forward[0] = target[0] - position[0];
    forward[1] = target[1] - position[1];
    forward[2] = target[2] - position[2];
    float forwardLen = 0;
    NormalizeVector(forward, &forwardLen);
    if ( forwardLen < 0.00001 )
    {
        return;
    }

    // Calculate "right" as a cross product between "forward" and "world up"
    float worldUp[] = { 0.0, 1.0, 0.0 };
    float right[3];
    CrossProduct(forward, worldUp, right);
    NormalizeVector(right);

    // Calculate "up" as the cross product between "right" and "forward"
    float up[3];
    CrossProduct(right, forward, up);

    // Copy the vectors into the matrix to form a rotation; transpose this
    // already so this matrix can be used as "inverse camera matrix"
    // without inverse-transforming it later.
    matrix[0] = right[0];
    matrix[4] = right[1];
    matrix[8] = right[2];

    matrix[1] = up[0];
    matrix[5] = up[1];
    matrix[9] = up[2];

    matrix[2] = -forward[0];
    matrix[6] = -forward[1];
    matrix[10] = -forward[2];

    // Add translation
    float translation[16];
    MatrixCreateTranslation(translation,
                            -position[0], -position[1], -position[2]);
    MatrixMultiply(translation, matrix, matrix);
}

void NormalizeVector(float* vector, float* vectorOrigLength)
{
    float len = sqrt(vector[0]*vector[0] + vector[1]*vector[1] +
                     vector[2]*vector[2]);
    float invLen = 1.0 / len;
    vector[0] = vector[0] * invLen;
    vector[1] = vector[1] * invLen;
    vector[2] = vector[2] * invLen;

    if ( vectorOrigLength != NULL )
    {
        *vectorOrigLength = len;
    }
}

float DotProduct(const float* vector1, const float* vector2)
{
    return ((vector1[0] * vector2[0]) + (vector1[1] * vector2[1]) +
            (vector1[2] * vector2[2]));
}

void CrossProduct(const float* vector1, const float* vector2, float* output)
{
    float i = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
    float j = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
    float k = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

    output[0] = i;
    output[1] = j;
    output[2] = k;
}

void Transformv4(const float* matrix, const float* vector, float* result)
{
    float x = matrix[0] * vector[0] +
              matrix[4] * vector[1] +
              matrix[8] * vector[2] +
              matrix[12] * vector[3];
    float y = matrix[1] * vector[0] +
              matrix[5] * vector[1] +
              matrix[9] * vector[2] +
              matrix[13] * vector[3];
    float z = matrix[2] * vector[0] +
              matrix[6] * vector[1] +
              matrix[10] * vector[2] +
              matrix[14] * vector[3];
    float w = matrix[3] * vector[0] +
              matrix[7] * vector[1] +
              matrix[11] * vector[2] +
              matrix[15] * vector[3];
    
    result[0] = x;
    result[1] = y;
    result[2] = z;
    result[3] = w;
}

void Transformv3(const float* matrix, const float* vector, float* result)
{
    float x = matrix[0] * vector[0] +
              matrix[4] * vector[1] +
              matrix[8] * vector[2] +
              matrix[12];
    float y = matrix[1] * vector[0] +
              matrix[5] * vector[1] +
              matrix[9] * vector[2] +
              matrix[13];
    float z = matrix[2] * vector[0] +
              matrix[6] * vector[1] +
              matrix[10] * vector[2] +
              matrix[14];

    result[0] = x;
    result[1] = y;
    result[2] = z;
}

void CopyVector(const float* input, float* output)
{
    memcpy(output, input, Vector3x1Size);
}

void CopyMatrix(const float* input, float* output)
{
    memcpy(output, input, Matrix4x4Size);
}

void MatrixExtractRotation(const float* transform, float* rotation)
{
    MatrixSetIdentity(rotation);
    rotation[0] = transform[0];
    rotation[1] = transform[1];
    rotation[2] = transform[2];
    rotation[4] = transform[4];
    rotation[5] = transform[5];
    rotation[6] = transform[6];
    rotation[8] = transform[8];
    rotation[9] = transform[9];
    rotation[10] = transform[10];
}

void MatrixExtractTranslation(const float* transform, float* translation)
{
    translation[0] = transform[12];
    translation[1] = transform[13];
    translation[2] = transform[14];
}

void TransposeMatrix(const float* input, float* output)
{
    for ( int i = 0; i < 4; i++ )
    {
        for( int j = 0; j < 4; j++ )
        {
            output[j + i*4] = input[j*4 + i];
        }
    }
}

void NormalMatrix(float* normalMatrix, const float* modelViewMatrix,
                  bool isOrthogonal)
{
    if ( isOrthogonal )
    {
        // we can use the top 3x3 of model view matrix directly
        normalMatrix[0] = modelViewMatrix[0];
        normalMatrix[1] = modelViewMatrix[1];
        normalMatrix[2] = modelViewMatrix[2];
        normalMatrix[3] = modelViewMatrix[4];
        normalMatrix[4] = modelViewMatrix[5];
        normalMatrix[5] = modelViewMatrix[6];
        normalMatrix[6] = modelViewMatrix[8];
        normalMatrix[7] = modelViewMatrix[9];
        normalMatrix[8] = modelViewMatrix[10];
    }
    else
    {
        //TODO calculate 3x3 normal matrix as transpose of the inverse of
        // the top 3x3 of the model view matrix
    }
}

void MatrixMultiply(const float* left, const float* right, float* result)
{
    float tmp[16];

    for ( int i = 0; i < 4; i++ ) // i = row (in result matrix)
    {
      for( int j = 0; j < 4; j++ ) // j = column (in result matrix)
        {
          tmp[j + i*4] = left[0 + i*4] * right[j + 0] +
                         left[1 + i*4] * right[j + 4] +
                         left[2 + i*4] * right[j + 8] +
                         left[3 + i*4] * right[j + 12];
      }
    }

    CopyMatrix(tmp, result);
}

void MatrixCreateTranslation(float* matrix, float x, float y, float z)
{
    MatrixSetIdentity(matrix);
    matrix[12] = x;
    matrix[13] = y;
    matrix[14] = z;
}

void MatrixCreateScaling(float* matrix, float x, float y, float z)
{
    MatrixSetIdentity(matrix);
    matrix[0] = x;
    matrix[5] = y;
    matrix[10] = z;
}

void MatrixCreateRotation(float* matrix, float angleInRadians,
                          float x, float y, float z)
{
    MatrixSetIdentity(matrix);

    float length = sqrtf((x*x) + (y*y) + (z*z));
    if ( length == 0.0 )
    {
        x = 1.0;
        y = 0.0;
        z = 0.0;
    }
    else if ( length != 1.0 )
    {
        // normalize the axis vector
        float inv_len = 1.0 / length;
        x *= inv_len;
        y *= inv_len;
        z *= inv_len;
    }

    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float c = cosf(angleInRadians);
    float s = sinf(angleInRadians);
    float z_negcos = z * (1.0 - c);
    float y_negcos = y * (1.0 - c);

    matrix[0] = xx + ((1.0 - xx) * c);
    matrix[1] = (x * y_negcos) - (z * s);
    matrix[2] = (x * z_negcos) + (y * s);
    matrix[4] = (x * y_negcos) + (z * s);
    matrix[5] = yy + ((1.0 - yy) * c);
    matrix[6] = (y * z_negcos) - (x * s);
    matrix[8] = (x * z_negcos) - (y * s);
    matrix[9] = (y * z_negcos) + (x * s);
    matrix[10] = zz + ((1.0 - zz) * c);
}

void MatrixOrthographicProjection(float* matrix, float left, float right,
                                  float bottom, float top, float near,
                                  float far)
{
    MatrixSetIdentity(matrix);

    float a = 2.0 / (right - left);
    float b = 2.0 / (top - bottom);
    float c = -2.0 / (far - near);

    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    float tz = -(far + near) / (far - near);

    matrix[0] = a;
    matrix[5] = b;
    matrix[10] = c;
    matrix[12] = tx;
    matrix[13] = ty;
    matrix[14] = tz;
}

// This is pretty much the code from gluPerspective() - M_PI / 360.0 is correct
void MatrixPerspectiveProjection(float* matrix, float fovInDegrees,
                                 float aspectRatio, float near, float far)
{
    float ymax = near * tanf(fovInDegrees * M_PI / 360.0);
    float xmax = ymax * aspectRatio;
    MatrixFrustumProjection(matrix, -xmax, xmax, -ymax, ymax, near, far);
}

void MatrixFrustumProjection(float* matrix, float left, float right,
                             float bottom, float top, float near, float far)
{
    float temp, temp2, temp3, temp4;
    temp = 2.0 * near;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = far - near;
    matrix[0] = temp / temp2;
    matrix[1] = 0.0;
    matrix[2] = 0.0;
    matrix[3] = 0.0;
    matrix[4] = 0.0;
    matrix[5] = temp / temp3;
    matrix[6] = 0.0;
    matrix[7] = 0.0;
    matrix[8] = (right + left) / temp2;
    matrix[9] = (top + bottom) / temp3;
    matrix[10] = (-far - near) / temp4;
    matrix[11] = -1.0;
    matrix[12] = 0.0;
    matrix[13] = 0.0;
    matrix[14] = (-temp * far) / temp4;
    matrix[15] = 0.0;
}


