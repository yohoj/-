#ifndef ZIGBEECOM_H
#define ZIGBEECOM_H

struct TempWet{
    double temp;
    double wet;
};
class ZigbeeCom
{
public:
    ZigbeeCom();
    bool Init();
    bool Start();
    bool GetTw(int zgbId,TempWet *tw);
    bool LedControl(int zgbId,int led);
};

#endif // ZIGBEECOM_H
