#ifndef ISHAPEUICONTROLLER_H
#define ISHAPEUICONTROLLER_H

struct IShapeUIController
{
    virtual ~IShapeUIController() = default;

    virtual void RenderUI() = 0;
};

#endif