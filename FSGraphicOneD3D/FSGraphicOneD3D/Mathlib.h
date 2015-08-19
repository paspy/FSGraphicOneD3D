#pragma once
#include <cmath>
#include <cassert>

#define EPSILON 0.0001f
#define PI 3.1415926f
#define FAR_PLANE 10.0f
#define NEAR_PLANE 0.1f

typedef struct Vertex4 {
	union {
		// x y z w
		float xyzw[4];
		struct {
			float x, y, z, w;
		};
	};
	union {
		float uv[2];
		struct {
			float u, v;
		};
	};
	unsigned int color;
}*Vertex4_ptr;

typedef struct Tri {
	Vertex4 a, b, c;
}*Triangle_ptr;

typedef union Matrix4x4 {
	float e[16];

	struct {
		float _e11, _e12, _e13, _e14;
		float _e21, _e22, _e23, _e24;
		float _e31, _e32, _e33, _e34;
		float _e41, _e42, _e43, _e44;
	};
}*Matrix4x4_ptr;


inline float DegreeToRadians(float _degree) {
	return (float)(_degree * PI / 180.0f);
}

inline int Convert2Dto1D(const unsigned int _x, const unsigned int _y, const unsigned int _width) {
	return _y*_width + _x;
}

inline int RandInRange(int _min, int _max) {
	return _min + (rand() % (int)(_max - _min + 1));
}

inline unsigned int Lerp_(unsigned int _A, unsigned int _B, float _ratio) {
	return (unsigned int)((((float)_B - (float)_A) * _ratio) + (float)_A);
}

inline unsigned int BGRA_To_ARGB(const unsigned int _inColor) {
	// BGRA to ARGB
	unsigned int ia = _inColor & 0x000000FF; ia <<= 24;
	unsigned int ir = _inColor & 0x0000FF00; ir <<= 8;
	unsigned int ig = _inColor & 0x00FF0000; ig >>= 8;
	unsigned int ib = _inColor & 0xFF000000; ib >>= 24;

	return (ia | ir | ig | ib);
}

inline unsigned int LerpTri(unsigned int _A, unsigned int _B, unsigned int _C, float _ratioA, float _ratioB, float _ratioC) {
	return (unsigned int)(((float)_A * _ratioA) + ((float)_B * _ratioB) + ((float)_C * _ratioC));
}

inline unsigned int ColorLerpTriangle(unsigned int _A, unsigned int _B, unsigned int _C, float _ratioA, float _ratioB, float _ratioC) {
	unsigned int A1 = (_A & 0xFF000000) >> 24;
	unsigned int R1 = (_A & 0x00FF0000) >> 16;
	unsigned int G1 = (_A & 0x0000FF00) >> 8;
	unsigned int B1 = (_A & 0x000000FF);

	unsigned int A2 = (_B & 0xFF000000) >> 24;
	unsigned int R2 = (_B & 0x00FF0000) >> 16;
	unsigned int G2 = (_B & 0x0000FF00) >> 8;
	unsigned int B2 = (_B & 0x000000FF);

	unsigned int A3 = (_C & 0xFF000000) >> 24;
	unsigned int R3 = (_C & 0x00FF0000) >> 16;
	unsigned int G3 = (_C & 0x0000FF00) >> 8;
	unsigned int B3 = (_C & 0x000000FF);

	unsigned int newA = LerpTri(A1, A2, A3, _ratioA, _ratioB, _ratioC) << 24;
	unsigned int newR = LerpTri(R1, R2, R3, _ratioA, _ratioB, _ratioC) << 16;
	unsigned int newG = LerpTri(G1, G2, G3, _ratioA, _ratioB, _ratioC) << 8;
	unsigned int newB = LerpTri(B1, B2, B3, _ratioA, _ratioB, _ratioC);

	return newA | newR | newG | newB;

}

inline Matrix4x4 SetIdentity() {
	Matrix4x4 m = { 0 };
	m._e11 = 1.0f;
	m._e22 = 1.0f;
	m._e33 = 1.0f;
	m._e44 = 1.0f;
	return m;
}

bool IsZero(float a) {
	return (fabs(a))<EPSILON;
}

Matrix4x4 MultiplyMatrixByMatrix(Matrix4x4 m, Matrix4x4 n) {
	Matrix4x4 r;

	r._e11 = m._e11 * n._e11 + m._e12 * n._e21 + m._e13 * n._e31 + m._e14 * n._e41;
	r._e12 = m._e11 * n._e12 + m._e12 * n._e22 + m._e13 * n._e32 + m._e14 * n._e42;
	r._e13 = m._e11 * n._e13 + m._e12 * n._e23 + m._e13 * n._e33 + m._e14 * n._e43;
	r._e14 = m._e11 * n._e14 + m._e12 * n._e24 + m._e13 * n._e34 + m._e14 * n._e44;

	r._e21 = m._e21 * n._e11 + m._e22 * n._e21 + m._e23 * n._e31 + m._e24 * n._e41;
	r._e22 = m._e21 * n._e12 + m._e22 * n._e22 + m._e23 * n._e32 + m._e24 * n._e42;
	r._e23 = m._e21 * n._e13 + m._e22 * n._e23 + m._e23 * n._e33 + m._e24 * n._e43;
	r._e24 = m._e21 * n._e14 + m._e22 * n._e24 + m._e23 * n._e34 + m._e24 * n._e44;

	r._e31 = m._e31 * n._e11 + m._e32 * n._e21 + m._e33 * n._e31 + m._e34 * n._e41;
	r._e32 = m._e31 * n._e12 + m._e32 * n._e22 + m._e33 * n._e32 + m._e34 * n._e42;
	r._e33 = m._e31 * n._e13 + m._e32 * n._e23 + m._e33 * n._e33 + m._e34 * n._e43;
	r._e34 = m._e31 * n._e14 + m._e32 * n._e24 + m._e33 * n._e34 + m._e34 * n._e44;

	r._e41 = m._e41 * n._e11 + m._e42 * n._e21 + m._e43 * n._e31 + m._e44 * n._e41;
	r._e42 = m._e41 * n._e12 + m._e42 * n._e22 + m._e43 * n._e32 + m._e44 * n._e42;
	r._e43 = m._e41 * n._e13 + m._e42 * n._e23 + m._e43 * n._e33 + m._e44 * n._e43;
	r._e44 = m._e41 * n._e14 + m._e42 * n._e24 + m._e43 * n._e34 + m._e44 * n._e44;

	return r;
}

float Matrix_Determinant(float e_11, float e_12, float e_13,
	float e_21, float e_22, float e_23,
	float e_31, float e_32, float e_33) {
	return e_11 * e_22 * e_33 + e_12 * e_23 * e_31 + e_13 * e_21 * e_32 - e_11 * e_23 * e_32 - e_12 * e_21 * e_33 - e_13 * e_22 * e_31;
}

float Matrix_Determinant(Matrix4x4 m) {
	return m._e11 * Matrix_Determinant(m._e22, m._e23, m._e24, m._e32, m._e33, m._e34, m._e42, m._e43, m._e44) -
		m._e12 * Matrix_Determinant(m._e21, m._e23, m._e24, m._e31, m._e33, m._e34, m._e41, m._e43, m._e44) +
		m._e13 * Matrix_Determinant(m._e21, m._e22, m._e24, m._e31, m._e32, m._e34, m._e41, m._e42, m._e44) -
		m._e14 * Matrix_Determinant(m._e21, m._e22, m._e23, m._e31, m._e32, m._e33, m._e41, m._e42, m._e43);
}

Matrix4x4 Matrix_Inverse(Matrix4x4 m) {
	float det = Matrix_Determinant(m);
	if ( IsZero(det) )
		return m;

	Matrix4x4 r;
	float inv_det = 1.0f / det;

	r._e11 =  Matrix_Determinant(m._e22, m._e23, m._e24, m._e32, m._e33, m._e34, m._e42, m._e43, m._e44) * inv_det;
	r._e12 = -Matrix_Determinant(m._e12, m._e13, m._e14, m._e32, m._e33, m._e34, m._e42, m._e43, m._e44) * inv_det;
	r._e13 =  Matrix_Determinant(m._e12, m._e13, m._e14, m._e22, m._e23, m._e24, m._e42, m._e43, m._e44) * inv_det;
	r._e14 = -Matrix_Determinant(m._e12, m._e13, m._e14, m._e22, m._e23, m._e24, m._e32, m._e33, m._e34) * inv_det;
	r._e21 = -Matrix_Determinant(m._e21, m._e23, m._e24, m._e31, m._e33, m._e34, m._e41, m._e43, m._e44) * inv_det;
	r._e22 =  Matrix_Determinant(m._e11, m._e13, m._e14, m._e31, m._e33, m._e34, m._e41, m._e43, m._e44) * inv_det;
	r._e23 = -Matrix_Determinant(m._e11, m._e13, m._e14, m._e21, m._e23, m._e24, m._e41, m._e43, m._e44) * inv_det;
	r._e24 =  Matrix_Determinant(m._e11, m._e13, m._e14, m._e21, m._e23, m._e24, m._e31, m._e33, m._e34) * inv_det;
	r._e31 =  Matrix_Determinant(m._e21, m._e22, m._e24, m._e31, m._e32, m._e34, m._e41, m._e42, m._e44) * inv_det;
	r._e32 = -Matrix_Determinant(m._e11, m._e12, m._e14, m._e31, m._e32, m._e34, m._e41, m._e42, m._e44) * inv_det;
	r._e33 =  Matrix_Determinant(m._e11, m._e12, m._e14, m._e21, m._e22, m._e24, m._e41, m._e42, m._e44) * inv_det;
	r._e34 = -Matrix_Determinant(m._e11, m._e12, m._e14, m._e21, m._e22, m._e24, m._e31, m._e32, m._e34) * inv_det;
	r._e41 = -Matrix_Determinant(m._e21, m._e22, m._e23, m._e31, m._e32, m._e33, m._e41, m._e42, m._e43) * inv_det;
	r._e42 =  Matrix_Determinant(m._e11, m._e12, m._e13, m._e31, m._e32, m._e33, m._e41, m._e42, m._e43) * inv_det;
	r._e43 = -Matrix_Determinant(m._e11, m._e12, m._e13, m._e21, m._e22, m._e23, m._e41, m._e42, m._e43) * inv_det;
	r._e44 =  Matrix_Determinant(m._e11, m._e12, m._e13, m._e21, m._e22, m._e23, m._e31, m._e32, m._e33) * inv_det;

	return r;
}

Matrix4x4 MatrixTranslation(float _x, float _y, float _z) {
	Matrix4x4 m = { 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		_x, _y, _z, 1 };
	return m;
}

Matrix4x4 MatrixRotation_X(float _degree) {
	float rad = DegreeToRadians(_degree);
	Matrix4x4 m = { 1, 0,			0,			0,
		0, cosf(rad),	-sinf(rad),	0,
		0, sinf(rad),	cosf(rad),	0,
		0, 0,			0,			1 };
	return m;
}

Matrix4x4 MatrixRotation_Y(float _degree) {
	float rad = DegreeToRadians(_degree);
	Matrix4x4 m = { cosf(rad),	0,	sinf(rad),	0,
		0,			1,	0,			0,
		-sinf(rad),	0,	cosf(rad),	0,
		0,			0,	0,			1 };
	return m;
}

Matrix4x4 MatrixRotation_Z(float _degree) {
	float rad = DegreeToRadians(_degree);
	Matrix4x4 m = { cosf(rad),	-sinf(rad),	0,	0,
		sinf(rad),	cosf(rad),	0,	0,
		0,			0,			1,	0,
		0,			0,			0,	1 };
	return m;
}

Matrix4x4 CreateProjectMatrix(float _nearPlane, float _farPlane, float _fovDegree, float _aspectRatio) {
	float fov_rad = DegreeToRadians(_fovDegree);
	float yScale = 1.0f / tanf(0.5f*fov_rad);
	float xScale = yScale * _aspectRatio;
	float zFar, zNear;
	assert(_nearPlane != 0 && _farPlane > _nearPlane);
	zNear = _nearPlane;
	zFar = _farPlane;
	Matrix4x4 m = { xScale, 0,		 0,								0,
		0	  , yScale,	 0,								0,
		0	  , 0,		 zFar / (zFar - zNear),			1,
		0	  , 0,		 -(zFar*zNear) / (zFar - zNear),	0 };
	return m;
}