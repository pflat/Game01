#include "../Math/Vector3Ext.h"


namespace Toybox
{

Urho3D::Vector3 Bounce(const Urho3D::Vector3& vec, const Urho3D::Vector3& norm)
{
    float dp = vec.DotProduct(norm);
    //  vector vec projected in vector norm
    Urho3D::Vector3 vec_proj_norm = dp * norm;
    //  vector v reflected by vector n
    Urho3D::Vector3 vec_reflec = (2 * vec_proj_norm) - vec;
    //  invert vec_reflec in order to get a bounce, not a reflection
    return -vec_reflec;
}

Urho3D::Vector3 Slide(const Urho3D::Vector3& vec, const Urho3D::Vector3& norm)
{
    float dp = vec.DotProduct(norm);
    //  project vector v in vector n
    Urho3D::Vector3 vec_proj_norm = dp * norm;
    //  subtract the projection from v (in effect getting the vector v projected in the plane defined by the normal n)
    Urho3D::Vector3 vec_proj_plane = vec - vec_proj_norm;
    //  apply the length of v over the normalized projection into plane
    return vec.Length() * vec_proj_plane.Normalized();
}

Urho3D::Vector3 Project(const Urho3D::Vector3& vec, const Urho3D::Vector3& norm)
{
    float dp = vec.DotProduct(norm);
    //  project vector v in vector n
    Urho3D::Vector3 vec_proj_norm = dp * norm;
    //  subtract the projection from v (in effect getting the vector v projected in the plane defined by the normal n)
    return vec - vec_proj_norm;
}

Urho3D::Vector3 Align(const Urho3D::Vector3& vec1, const Urho3D::Vector3& vec2)
{
    return (vec1.Length() * vec2.Normalized());
}

}  //  namespace Toybox
