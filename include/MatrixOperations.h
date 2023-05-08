#ifndef MATRIXOPERATIONS_H
#define MATRIXOPERATIONS_H

#include <stdlib.h>

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

// Offset in a 4x4 matrix to the translation component
const int MatrixTranslationOffset = 12;

/**
 * Sets the given matrix to identity.
 * @param matrix float[16]
 */
void MatrixSetIdentity(float* matrix);

/**
 * Calculates an inverse of a transformation matrix.
 * @param matrix float[16]
 * @param result float[16] - can't be the same as the first argument
 */
void CalculateInverseTransform(const float* matrix, float* result);

/**
 * Creates a look-at matrix.
 * @param matrix float[16]
 * @param position float[3]
 * @param target float[3]
 */
void MatrixSetLookat(float* matrix, const float* position, const float* target);

/**
 * Normalizes a vector into unit length.
 * @param vector float[3]
 * @param vectorOrigLength if not NULL, store vector's original length here
 */
void NormalizeVector(float* vector, float* vectorOrigLength = NULL);

/**
 * Calculates the dot product of two vectors.
 * @param vector1 float[3]
 * @param vector2 float[3]
 */
float DotProduct(const float* vector1, const float* vector2);

/**
 * Calculates a cross product of two vectors.
 * @param vector1 float[3]
 * @param vector2 float[3]
 * @param output float[3] - can be same as either of the inputs
 */
void CrossProduct(const float* vector1, const float* vector2, float* output);

/**
 * Transforms a 4-element vector by a transformation matrix.
 * @param matrix float[16]
 * @param vector float[4]
 * @param result float[4] - can be same as source vector
 */
void Transformv4(const float* matrix, const float* vector, float* result);

/**
 * Transforms a 3-element vector by a transformation matrix.
 * @param matrix float[16]
 * @param vector float[3]
 * @param result float[3] - can be same as source vector
 */
void Transformv3(const float* matrix, const float* vector, float* result);

/**
 * Copies a 4x4 matrix to another.
 * @param input float[16]
 * @param output float[16] - cannot be the same as input
 */
void CopyMatrix(const float* input, float* output);

/**
 * Copies a 3x1 vector to another.
 * @param input float[3]
 * @param output float[3] - cannot be the same as input
 */
void CopyVector(const float* input, float* output);

/**
 * Extracts the rotation part out of a transformation matrix.
 * @param transform float[16]
 * @param rotation float[16] - cannot be the same as the other parameter!
 */
void MatrixExtractRotation(const float* transform, float* rotation);

/**
 * Extracts the translation part out of a transformation matrix.
 *
 * @param transform float[16]
 * @param translation float[3]
 */
void MatrixExtractTranslation(const float* transform, float* translation);

/**
 * Transposes a matrix. The result matrix may NOT be the same as the input matrix.
 * @param matrix float[16]
 * @param output float[16]
 */
void TransposeMatrix(const float* matrix, float* output);

/**
 * Multiplies two matrices and places the result into a third. The result
 * matrix may be the same as any of the other parameters.
 * @param matrix left[16]
 * @param matrix right[16]
 * @param matrix result[16]
 */
void MatrixMultiply(const float* left, const float* right, float* result);

/**
 * Creates a translation transformation. (see glTranslatef())
 * @param matrix float[16]
 */
void MatrixCreateTranslation(float* matrix, float x, float y, float z);

/**
 * Creates a scaling transformation. (see glScalef())
 * @param matrix float[16]
 */
void MatrixCreateScaling(float* matrix, float x, float y, float z);

/**
 * Creates a rotation transformation around a given axis. (see glRotatef())
 * @param matrix float[16]
 */
void MatrixCreateRotation(float* matrix, float angleInRadians,
                          float x, float y, float z);

/**
 * Creates a orthographic ("2D") projection. (see glOrtho())
 * @param matrix float[16]
 */
void MatrixOrthographicProjection(float* matrix, float left, float right,
                                  float bottom, float top, float near,
                                  float far);

/**
 * Creates a perspective projection. (see gluPerspective())
 * @param matrix float[16]
 */
void MatrixPerspectiveProjection(float* matrix, float fovInDegrees,
                                 float aspectRatio, float near, float far);

/**
 * Creates a perspective projection using a frustum. (see glFrustum())
 * @param matrix float[16]
 */
void MatrixFrustumProjection(float* matrix, float left, float right,
                             float bottom, float top, float near, float far);

/**
 * Creates a Normal Matrix out of a ModelView Matrix
 * @param normalMatrix float[9]
 * @param modelViewMatrix float[16]
 */
void NormalMatrix(float* normalMatrix, const float* modelViewMatrix,
                  bool isOrthogonal = true);

#endif // MATRIXOPERATIONS_H
