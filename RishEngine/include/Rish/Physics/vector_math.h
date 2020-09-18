#include <Rish/rlpch.h>

#ifndef vector_math_H_
#define vector_math_H_
/**
 * @file vector_math.h
 * @author  halloworld <william31212@gmail.com>
 * @brief 更詳細的向量數學計算function，繼承sf::Vector2<float>
 */

/** @brief 徑度與度度量的轉換 */
#define M_PI 3.14159265358979323846
#define degreesToRadians(angleDegrees) (angleDegrees * M_PI / 180.0)
#define radiansToDegrees(angleRadians) (angleRadians * 180.0 / M_PI)

/** @brief float_max, float_min常數 */
#define MAX_float std::numeric_limits<float>::max()
#define MIN_float std::numeric_limits<float>::min()

/**
 * @brief 補足sf::Vector2f無法做的計算，自行建立一個特化的Vector
 * @details 包括向量長度、內積、左右法向量等數學基本工具
 */
class Vec2 {
public:
    Vec2();
    /**
     * @brief 向量x, y
     */
    Vec2(float x_, float y_);

    /**
     * @brief Deconstructer
     */
    ~Vec2() = default;

    /**
    * @brief Two point calculate Vector
    */
    Vec2(Vec2 a, Vec2 b)
    {
        x = b.x - a.x;
        y = b.y - a.y;
    }

    /**
   * @brief Reverse Vector
   */
    Vec2 operator - ()  {
        Vec2 v;
        v.x = -x;
        v.y = -y;
        return v;
    }

    void operator = (const Vec2& v)
    {
        x = v.x; y = v.y;
    }

    Vec2 operator + (const Vec2& v)  {
        return Vec2(x + v.x, y + v.y);
    }

    Vec2 operator - (const Vec2& v)  {
        return Vec2(x - v.x, y - v.y);
    }

    void operator += (const Vec2& v)
    {
        x += v.x; y += v.y;
    }

    void operator -= (const Vec2& v)
    {
        x -= v.x; y -= v.y;
    }

    void operator *= (float a)
    {
        x *= a; y *= a;
    }
    /**
    * @brief set Vector
    */
    void Set(float x_, float y_) { x = x_; y = y_; }
    /**
     * @brief debug print
     */
    void print_Vector();
    /**
     * @brief get length of vector
     */
    float getLength();
    /**
     * @brief vector dot
     */
    float dot(Vec2 &vec2);
    /**
     * @brief 計算vec在vec2上的正射影(投影長)
     */
    float projectLengthOnto(Vec2 &vec2);
    /**
     * @brief 左法向量
     */
    Vec2 normalL();
    /**
     * @brief 右法向量
     */
    Vec2 normalR();
    /**
     * @brief 向量放大
     */
    void Times(float K){
        x *= K;
        y *= K;
    }
    /**
    * @brief 確認是否兩項量夾角大於90度，是否同邊
    */
    bool AtTheSameSide(Vec2 A){
        // vec is same side with A or not
        if( x * A.y - y * A.x > 0.0f)
            return true;
        return false;
    }
    /**
     * @brief 旋轉
     * @param angle 旋轉角度(徑度)
     * @retval float 旋轉過後的向量
     */
    void rotate(float angle);
    //x′1=[(x1−x0)cosθ−(y1−y0)sinθ]+x0
    //y′1=[(y1−y0)cosθ+(x1−x0)sinθ]+y0
    /**
     * @brief 旋轉
     * @details 依照參考點旋轉該向量
     * @param ref 參考點
     * @param angle 旋轉角度(徑度)
     * @retval float 旋轉過後的向量
     */
    void rotate_ref(float angle, Vec2 &ref);
    int x, y;
};


std::pair<float, float> getMinMax(Vec2 &axis, std::deque<Vec2> corner);
int randomint(int min, int max);


class Mat22 {
public:
    Mat22()
    {
        col1.x = 0;
        col2.x = 0;
        col1.y = 0;
        col2.y = 0;
    }

    Mat22(float angle)
    {
        float c = cosf(angle), s = sinf(angle);
        col1.x = c;
        col2.x = -s;
        col1.y = s;
        col2.y = c;
    }

    Mat22(const Vec2 &col1, const Vec2 &col2) : col1(col1), col2(col2) {}

    //轉至矩陣
    Mat22 Transpose() const
    {
        return Mat22(Vec2(col1.x, col2.x), Vec2(col1.y, col2.y));
    }
    Mat22 Invert() const
    {
        float a = col1.x, b = col2.x, c = col1.y, d = col2.y;
        Mat22 B;
        float det = a * d - b * c;
        if(det == 0.0f)
        {
            det = 0;
        }
        else
        {
            det = 1.0f / det;
        }
        B.col1.x = det * d;
        B.col2.x = -det * b;
        B.col1.y = -det * c;
        B.col2.y = det * a;
        return B;
    }

    Vec2 col1, col2;
};

inline float Dot(const Vec2& a, const Vec2& b)
{
    return a.x * b.x + a.y * b.y;
}

inline float Cross(const Vec2& a, const Vec2& b)
{
    return a.x * b.y - a.y * b.x;
}

inline Vec2 Cross(const Vec2& a, float s)
{
    return Vec2(s * a.y, -s * a.x);
}

inline Vec2 Cross(float s, const Vec2& a)
{
    return Vec2(-s * a.y, s * a.x);
}

inline Vec2 operator * (const Mat22& A, const Vec2& v)
{
    return Vec2(A.col1.x * v.x + A.col2.x * v.y, A.col1.y * v.x + A.col2.y * v.y);
}

inline Vec2 operator + (const Vec2& a, const Vec2& b)
{
    return Vec2(a.x + b.x, a.y + b.y);
}

inline Vec2 operator - (const Vec2& a, const Vec2& b)
{
    return Vec2(a.x - b.x, a.y - b.y);
}

inline Vec2 operator * (float s, const Vec2& v)
{
    return Vec2(s * v.x, s * v.y);
}

inline Mat22 operator + (const Mat22& A, const Mat22& B)
{
    return Mat22(A.col1 + B.col1, A.col2 + B.col2);
}

inline Mat22 operator * (const Mat22& A, const Mat22& B)
{
    return Mat22(A * B.col1, A * B.col2);
}

inline float Abs(float a)
{
    return a > 0.0f ? a : -a;
}

inline Vec2 Abs(const Vec2& a)
{
    return Vec2(fabsf(a.x), fabsf(a.y));
}

inline Mat22 Abs(const Mat22& A)
{
    return Mat22(Abs(A.col1), Abs(A.col2));
}

inline float Sign(float x)
{
    return x < 0.0f ? -1.0f : 1.0f;
}

inline float Min(float a, float b)
{
    return a < b ? a : b;
}

inline float Max(float a, float b)
{
    return a > b ? a : b;
}

inline float Clamp(float a, float low, float high)
{
    return Max(low, Min(a, high));
}

template<typename T> inline void Swap(T& a, T& b)
{
    T tmp = a;
    a = b;
    b = tmp;
}
#endif