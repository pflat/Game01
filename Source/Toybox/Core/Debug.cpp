#include "../Core/Debug.h"
#include <cstdio>
#include <ctime>


namespace Toybox
{

void PrintMessage(unsigned frame, const Urho3D::String& message)
{
    printf("%d - %s\n", frame, message.CString());
}


void PrintVector(const Urho3D::Vector3& vec)
{
    printf("(%.6f, %.6f, %.6f)\n", vec.x_, vec.y_, vec.z_);
}


void PrintVector(const Urho3D::String& message, const Urho3D::Vector3& vec)
{
    printf("%s : (%.6f, %.6f, %.6f)\n", message.CString(), vec.x_, vec.y_, vec.z_);
}


void PrintFormatedDate()
{
    char text[80];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(text, sizeof(text)-1, "%Y%m%d%H%M%S", t);
    printf("%s\n", text);
}


void PrintNodeTree(int tabs, Urho3D::Node* node)
{
    printf("%*s%s (%d)\n", tabs, "", node->GetName().CString(), node->GetChildren().Size());
    //printf("(%f, %f, %f)\n", node->GetPosition().x_, node->GetPosition().y_, node->GetPosition().z_);
    //printf("(%f, %f, %f)\n", node->GetRotation().PitchAngle(), node->GetRotation().YawAngle(), node->GetRotation().RollAngle());
    for (unsigned i = 0; i < node->GetChildren().Size(); ++i)
        PrintNodeTree(tabs + 2, node->GetChild(i));
}


void PrintSkeleton(int tabs, Urho3D::Skeleton* skel)
{
    for (unsigned i = 0; i < skel->GetNumBones(); ++i)
    {
        //printf("%s\n", skel->GetNumBones());
        printf("%s\n", skel->GetBone(i)->name_.CString());
    }
}

}
