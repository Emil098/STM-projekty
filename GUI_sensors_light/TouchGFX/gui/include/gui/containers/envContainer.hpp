#ifndef ENVCONTAINER_HPP
#define ENVCONTAINER_HPP

#include <gui_generated/containers/envContainerBase.hpp>

#ifndef SIMULATOR
#include "gui_defs.h"
#endif

class envContainer : public envContainerBase
{
public:
    envContainer();
    virtual ~envContainer() {}

    virtual void initialize();

#ifndef SIMULATOR
    virtual void updateEnvContent(enviroment_struct e_info);
#endif

protected:
};

#endif // ENVCONTAINER_HPP
