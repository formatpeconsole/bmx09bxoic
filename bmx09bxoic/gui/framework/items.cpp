#include "items.h"

namespace gui::framework
{
bool RealItemPath::canRender(const RealItemPath& other)
{
    return tab == other.tab 
        && subTab == other.subTab 
        && childCategory == other.childCategory;
}

}