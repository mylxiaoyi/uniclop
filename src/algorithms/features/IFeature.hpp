

#if !defined(IFEATURE_HEADER_INCLUDED)
#define IFEATURE_HEADER_INCLUDED


namespace uniclop
{

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Interfaces definition

// T is the class of the children

template<typename T>
class IFeature
{
public:
    int x,y; ///< position

    virtual float distance(const T& another_feature) const;
    ///< measure distance to another feature of his own type
    ///< will raise a runtime error if the given feature type is not its own

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

#endif // IFEATURE_HEADER_INCLUDED
