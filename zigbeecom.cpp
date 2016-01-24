#include "zigbeecom.h"

ZigbeeCom::ZigbeeCom()
{

}
bool ZigbeeCom::Init()
{
    return true;
}
bool ZigbeeCom::Start()
{
    return true;
}
bool ZigbeeCom::GetTw(int zgbId,TempWet *tw)
{
    tw->temp = 18.5;
    tw->wet = 23.1;
    return true;
}
bool ZigbeeCom::LedControl(int zgbId,int led)
{

        return true;
}
