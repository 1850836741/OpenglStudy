#pragma once
#include<fstream>
#include<math.h>
struct Vec3
{
	float arr[3];
	Vec3() {
		arr[0] = arr[1] = arr[2] = 0.0f;
	}
	Vec3(float x, float y, float z) {
		arr[0] = x; arr[1] = y; arr[2] = z;
	}

	float x() const { return arr[0]; }
	float y() const { return arr[1]; }
	float z() const { return arr[2]; }

	float operator[](int i) { return arr[i]; }

	Vec3& operator+= (Vec3& v) {
		for (int i = 0; i < 3; i++)
		{
			this->arr[i] += v[i];
		}
		return *this;
	}

	Vec3& operator-= (Vec3& v) {
		for (int i = 0; i < 3; i++)
		{
			this->arr[i] -= v[i];
		}
		return *this;
	}

	Vec3& operator*= (float n) {
		for (int i = 0; i < 3; i++)
		{
			this->arr[i] *= n;
		}
		return *this;
	}

	Vec3& operator/= (float n) {
		for (int i = 0; i < 3; i++)
		{
			this->arr[i] *= 1/n;
		}
		return *this;
	}

	Vec3 operator+ (Vec3& v) {
		return Vec3(this->arr[0] + v[0], this->arr[1] + v[1], this->arr[2] + v[2]);
	}

	Vec3 operator- (Vec3& v) {
		return Vec3(this->arr[0] - v[0], this->arr[1] - v[1], this->arr[2] - v[2]);
	}
	
	Vec3 operator* (float n) {
		return Vec3(this->arr[0]*n, this->arr[1]*n, this->arr[2]*n);
	}

	Vec3 operator/ (float n) {
		return Vec3(this->arr[0] / n, this->arr[1] / n, this->arr[2] / n);
	}

	/*获取向量的模*/
	float moudleLength() {
		return sqrt(arr[0]*arr[0] + arr[1]*arr[1] + arr[2]*arr[2]);
	}
	
};

using Point = Vec3;

/*向量点积*/
inline float dot(const Vec3 v1, const Vec3 v2) {
	return v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z();
}
/*向量叉积*/
inline Vec3 cross(const Vec3 v1,const Vec3 v2) {
	return Vec3(v1.y()*v2.z()- v1.z() * v2.y(),
		v1.z()*v2.x()-v1.x()*v2.z(),
		v1.x()*v2.y()-v1.y()*v2.x());
}

/*输出向量*/
inline std::ostream& operator<< (std::ostream& out, Vec3& v) {
	return out << v.x() << " " << v.y() << " " << v.z() << "\n";
}

/*获取单位向量*/
inline Vec3 unit_vec(Vec3& v) {
	return Vec3(v.x()/v.moudleLength(), v.y() / v.moudleLength(), v.z() / v.moudleLength());
}
