

#if !defined(IFEATURE_HEADER_INCLUDED)
#define IFEATURE_HEADER_INCLUDED


namespace uniclop
{

// ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
// Interfaces definition

// T is the class of the children
class IFeature
{
public:
    int x,y; ///< position

    virtual float distance(const IFeature& another_feature) const = 0;
    ///< measure distance to another feature of his own type
    ///< will raise a runtime error if the given feature type is not its own
	///< Will need to do a dynamic_cast to do type safe retrieval of the feature type.
	///< Could do this using a templated interface, but this complicates the code. Tradeoff simplicity versus speed.
	
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
