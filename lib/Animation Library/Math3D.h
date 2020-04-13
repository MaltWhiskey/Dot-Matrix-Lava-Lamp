#ifndef MATH3D_H_
#define MATH3D_H_
/*----------------------------------------------------------------------------------------------
 * Vector3 CLASS
 *----------------------------------------------------------------------------------------------
 * A vector V in physics is an (angel, magnitude) magnitude is length and the angle
 * is usually represented by Greek letter theta.
 *
 * In game development a vector is stored as (x,y,z) representing a movement
 * in the Cartesian plane across the x, y and z axis.
 *
 * A point or the position of an object can also be represented by a vector by moving
 * from the origin to the coordinates (x,y,z)
 *
 * The length = sqrt(x*x + y*y + z*z)
 *----------------------------------------------------------------------------------------------*/
class Vector3 {
public:
  float x,y,z;
public:
  // constructors
  Vector3();
  Vector3(const float x, const float y, const float z);
  Vector3(const Vector3& v);

  // moving
  Vector3 operator+(const Vector3& v) const;
  Vector3 operator-(const Vector3& v) const;
  Vector3& operator+=(const Vector3& v);
  Vector3& operator-=(const Vector3& v);
  // negate
  Vector3 operator-() const;

  // scaling
  Vector3 operator*(const float s) const;
  Vector3 operator/(const float s) const;
  Vector3& operator*=(const float s);
  Vector3& operator/=(const float s);

  // cross product
  Vector3 cross(const Vector3& v) const;
  Vector3 operator*(const Vector3& v) const;
  Vector3& operator*=(const Vector3& v);

  // dot product
  float dot(const Vector3& v) const;
  float operator%(const Vector3& v) const;

  // unit vector
  Vector3& normalize();
  Vector3 normalized() const;
  // magnitude or length of the vector
  float magnitude() const;
  float norm() const;

  // rotate v by angle and this axis vector
  Vector3 rotate(float angle, const Vector3& v) const;
 
  // test circle boundary of this vector
  bool inside(const Vector3& v, float radius) const;
  // test square boundary of this vector
  bool inside(const Vector3& l, const Vector3& h) const;
};

/*----------------------------------------------------------------------------------------------
 * Quaternion CLASS
 *----------------------------------------------------------------------------------------------
 * A Quaternion is a complex number in the form  w + xi + yj + zk, where w, x, y, z are real
 * numbers and i, j, k are imaginary.
 *
 * In the implementation i,j and k are ignored, w is a scalar and x,y,z is a vector
 *----------------------------------------------------------------------------------------------*/
class Quaternion {
public:
  float w;
  Vector3 v;
public:
  // constructors
  Quaternion();
  Quaternion(const Quaternion& q);
  Quaternion(const float a, const Vector3& v);
  Quaternion(const Vector3& v, float a);

  // moving (add subtract)
  Quaternion operator+(const Quaternion& q) const;
  Quaternion operator-(const Quaternion& q) const;
  Quaternion& operator+=(const Quaternion& q);
  Quaternion& operator-=(const Quaternion& q);

  // scaling (multiply divide by scalar)
  Quaternion operator*(const float s) const;
  Quaternion operator/(const float s) const;

  Quaternion& operator*=(const float s);
  Quaternion& operator/=(const float s);

  // multiply quaternions
  Quaternion operator*(const Quaternion& q) const;
  Quaternion operator/(const Quaternion& q) const;
  Quaternion& operator*=(const Quaternion& q);

  // dot product
  float dot(const Quaternion& q) const;
  float operator%(const Quaternion& q) const;

  // inverse
  Quaternion& inverse();
  Quaternion inversed() const;
  // get conjugate (negative imaginary part)
  Quaternion& conjugate();
  Quaternion conjugated() const;
  // unit quaternion
  Quaternion& normalize();
  Quaternion normalized() const;
  // magnitude or length of the quaterion
  float magnitude() const;
  float norm() const;
  // rotate v by quaternion
  Vector3 rotate(const Vector3& v) const;
};

/*-----------------------------------------------------------------------------------------------
 * OBJECT CLASS
 *-----------------------------------------------------------------------------------------------
 * The object class represents an object with a position and velocity
 * acting upon it. Forces get applied over time.
 *
 * Delta Time (float dt) is in seconds.
*----------------------------------------------------------------------------------------------*/
class Object2 {
public:
  Vector3 position = Vector3(0,0,0);
  Vector3 velocity = Vector3(0,0,0);
  
  uint8_t hue;
  float brightness; // (0.f - 1.0f) -> 0x00 - 0xff
  float seconds;    // seconds to fade brightness to zero
public:
  Object2(Vector3 p = Vector3(0,0,0), Vector3 v = Vector3(0,0,0), uint8_t h = 0,
    float b = 1.0f, float s = 1.0f) :
    position(p), velocity(v), hue(h), brightness(b), seconds(s) {}
  Object2& move(const float dt, const Vector3 gravity);
};
#endif