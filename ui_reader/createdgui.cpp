#include "ui_reader/createdgui.h"

#include "coreengine/mainapp.h"
#include "objects/base/moveinbutton.h"

CreatedGui::~CreatedGui()
{
    for (auto & pItem : m_factoryUiItem)
    {
        pItem->detach();
    }
}

void CreatedGui::addFactoryUiItem(oxygine::spActor pItem)
{
    pItem->setPriority(static_cast<qint32>(Mainapp::ZOrder::Objects));
    m_factoryUiItem.append(pItem);
    addChild(pItem);
}

void CreatedGui::setEnabled(bool value)
{
    for (auto & item : m_factoryUiItem)
    {
        spMoveInButton pMoveInButton = oxygine::dynamic_pointer_cast<MoveInButton>(item);
        if (pMoveInButton.get() == nullptr)
        {
            item->setEnabled(value);
        }
    }
}
