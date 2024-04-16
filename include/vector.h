#ifndef VECTOR_H
#define VECTOR_H

typedef struct Vec2i {
    int x;
    int y;
} Vec2i;


/// @brief Rotates the given vector by 90 degrees CCW
/// @param vec given vector (out)
void rotLeft(Vec2i *vec);
/// @brief Rotates the given vector by 90 degrees CW
/// @param vec given vector (out)
void rotRight(Vec2i *vec);

/// @brief Gets a rotated vector by 90 degrees CCW
/// @param vec given vector
/// @return the rotated vector
Vec2i getLeftRot(Vec2i vec);
/// @brief Gets a rotated vector by 90 degrees CW
/// @param vec given vector
/// @return the rotated vector
Vec2i getRightRot(Vec2i vec);

/// @brief Adds dir to pos
/// @param dir direction to move towards
/// @param pos current position (out)
void advance(const Vec2i * const dir, Vec2i *pos);

Vec2i addVectors(Vec2i dir , Vec2i pos);

/// @brief Flattens 2D grid onto X axis with the purpose of
/// @brief returning a unique number from 0 to 3 for orthonormal
/// @brief unit vectors.
/// @param vec given orthonormal vector
/// @return index from 0 to 3
/// @link https://www.desmos.com/calculator/ecuq826hkn
int flattenStdBasis(const Vec2i vec);

/// @brief Returns associated char with Compass rose direction
/// @param dir 
/// @return n, w, s, or e
char compass(int dirSerial);

int inRange(const Vec2i * const pos, Vec2i low, Vec2i high);

int serialize(const Vec2i * const dir, int width);

int getSerial(Vec2i dir, int width);

Vec2i deserialize(int hash, int width);

int dotProd(Vec2i a, Vec2i b);

int getWinding(Vec2i a, Vec2i b);

#endif // VECTOR_H
