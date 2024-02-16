#ifndef COLORCONTAINER_HPP
#define COLORCONTAINER_HPP

#include <gui_generated/containers/ColorContainerBase.hpp>

typedef struct
{
  uint32_t color;
  const char *name;
}btns_info_struct;

const btns_info_struct btn_info[15] =
{
    {0x008000, "Green"        },
    {0x00BFFF, "Deep blue sky"},
    {0x00FF7F, "Spring green" },
    {0x7CFC00, "Lawn Green"   },
    {0xFFFF00, "Yellow"       },
    {0x0000FF, "Blue"         },
    {0x1E90FF, "Dodger Blue"  },
    {0xEE82EE, "Violet"       },
    {0xFF00FF, "Magenta"      },
    {0x800080, "Purple"       },
    {0xFF1493, "Pink"         },
    {0xFFA500, "Orange"       },
    {0xFF4500, "Orange red"   },
    {0xDC143C, "Crimson"      },
    {0xFF0000, "Red"          }
};

class ColorContainer : public ColorContainerBase
{
public:
    ColorContainer();
    virtual ~ColorContainer() {}

    virtual void initialize();

    virtual void select_button(uint32_t color,char *btn_name);
    virtual void get_button_name(char *name);
protected:



};

#endif // COLORCONTAINER_HPP
