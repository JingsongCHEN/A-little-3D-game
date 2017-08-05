#ifndef Vector3D_H
#define Vector3D_H
#include <iostream>
#include <cmath>
using namespace std;

class Vector3D
{
	public:
	float x;
	float y;
	float z;
	// ȱʡ���캯��
	Vector3D();
	~Vector3D();
	// �û����캯��
	Vector3D(float posX, float posY, float posZ);
	//ʸ���ӷ�
	Vector3D operator+(Vector3D v);
	//ʸ������
	Vector3D operator-(Vector3D v);
	//����
	Vector3D operator*(float n);
	//����
	Vector3D operator/(float n);
};

#endif
