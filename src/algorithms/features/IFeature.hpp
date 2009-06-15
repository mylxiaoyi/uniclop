

#if !defined(IFEATURE_HEADER_INCLUDED)
#define IFEATURE_HEADER_INCLUDED


namespace uniclop
{
namespace algorithms
{
namespace features
{

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Interfaces definition

// T is the class of the children
template<typename T>
class IFeature
{
public:
    int x,y; ///< position

    virtual float distance(const T& another_feature) const = 0;
    ///< measure distance to another feature of his own type

    IFeature()
    {
        return;
    }
    virtual ~IFeature()
    {
        return;
    }
};

}
}
}

#endif // IFEATURE_HEADER_INCLUDED
