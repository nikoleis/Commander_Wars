#include <QFileInfo>
#include <QDirIterator>

#include "resource_management/cospritemanager.h"

#include "coreengine/mainapp.h"

#include "spritingsupport/spritecreator.h"

COSpriteManager::COSpriteManager()
    : RessourceManagement<COSpriteManager>("/images/co/res.xml",
                                           "/scripts/cos")
{
    setObjectName("COSpriteManager");
    connect(this, &COSpriteManager::sigLoadResAnim, this, &COSpriteManager::loadResAnim, Qt::QueuedConnection);
}

void COSpriteManager::release()
{
    m_Ressources.clear();
}

QStringList COSpriteManager::getSpriteCOIDs()
{
    QStringList ret;
    for (auto iter = m_resourcesMap.begin(); iter != m_resourcesMap.end(); ++iter)
    {
        QString id(iter.key());
        if (id.endsWith("+face"))
        {
            QString name = id.toUpper().replace("+FACE", "");
            if (!ret.contains(name))
            {
                ret.append(name);
            }
        }
    }
    return ret;
}

QStringList COSpriteManager::getCOStyles(const QString & id)
{
    for (qint32 i = 0; i < m_loadedRessources.size(); i++)
    {
        if (m_loadedRessources[i] == id)
        {
            return getCOStyles(i);
        }
    }
    return QStringList();
}

QStringList COSpriteManager::getCOStyles(qint32 position)
{
    if ((position >= 0) && (position < m_loadedRessources.size()))
    {
        Interpreter* pInterpreter = Interpreter::getInstance();
        QJSValue value = pInterpreter->doFunction(m_loadedRessources[position], "getCOStyles");
        return value.toVariant().toStringList();
    }
    return QStringList();
}

void COSpriteManager::loadResAnim(const QString & coid, const QString & file, QImage colorTable, QImage maskTable, bool useColorBox)
{
    colorTable.convertTo(QImage::Format_ARGB32);
    maskTable.convertTo(QImage::Format_ARGB32);
    QString coidLower = coid.toLower();
    bool nrmFound = false;
    bool faceFound = false;
    bool infoFound = false;
    bool miniFound = false;
    QStringList filenameList = file.split("/");
    QString filename = filenameList[filenameList.size() - 1];
    oxygine::spResAnim pCOAnim;
    oxygine::spResAnim pAnim;
    for (qint32 i = 0; i < m_Ressources.size(); i++)
    {
        pCOAnim = nullptr;
        pAnim = nullptr;
        if (coidLower+ "+nrm" == m_Ressources[i].m_spriteId)
        {
            pAnim = oxygine::Resources::getResAnim(filename + "+nrm", oxygine::error_policy::ep_ignore_error);
            pCOAnim = SpriteCreator::createAnim(file + "+nrm.png", colorTable, maskTable, useColorBox, pAnim->getColumns(), pAnim->getRows(), pAnim->getScaleFactor(), false);
            m_Ressources[i] = CoSprite(coidLower + "+nrm", pCOAnim);
            nrmFound = true;
        }
        else if (coidLower + "+face" == m_Ressources[i].m_spriteId)
        {
            pAnim = oxygine::Resources::getResAnim(filename + "+face", oxygine::error_policy::ep_ignore_error);
            pCOAnim = SpriteCreator::createAnim(file + "+face.png", colorTable, maskTable, useColorBox, pAnim->getColumns(), pAnim->getRows(), pAnim->getScaleFactor(), true);
            m_Ressources[i] = CoSprite(coidLower + "+face", pCOAnim);
            faceFound = true;
        }
        else if (coidLower + "+info" == m_Ressources[i].m_spriteId)
        {
            pAnim = oxygine::Resources::getResAnim(filename + "+info", oxygine::error_policy::ep_ignore_error);
            pCOAnim = SpriteCreator::createAnim(file + "+info.png", colorTable, maskTable, useColorBox, pAnim->getColumns(), pAnim->getRows(), pAnim->getScaleFactor(), false);
            m_Ressources[i] = CoSprite(coidLower + "+info", pCOAnim);
            infoFound = true;
        }
        else if (coidLower + "+mini" == m_Ressources[i].m_spriteId)
        {
            pAnim = oxygine::Resources::getResAnim(filename + "+mini", oxygine::error_policy::ep_ignore_error);
            if (pAnim.get() != nullptr)
            {
                pCOAnim = SpriteCreator::createAnim(file + "+mini.png", colorTable, maskTable, useColorBox, pAnim->getColumns(), pAnim->getRows(), pAnim->getScaleFactor(), false);
                m_Ressources[i] = CoSprite(coidLower + "+mini", pCOAnim);
                infoFound = true;
            }
        }
    }
    pAnim = nullptr;
    pCOAnim = nullptr;
    if (!faceFound)
    {
        pAnim = oxygine::Resources::getResAnim(filename + "+face", oxygine::error_policy::ep_ignore_error);
        if (pAnim.get() != nullptr)
        {
            pCOAnim = SpriteCreator::createAnim(file + "+face.png", colorTable, maskTable, useColorBox, pAnim->getColumns(), pAnim->getRows(), pAnim->getScaleFactor(), true);
            if (pCOAnim.get() != nullptr)
            {
                m_Ressources.append(CoSprite(coidLower + "+face", pCOAnim));
            }
        }
    }
    pAnim = nullptr;
    pCOAnim = nullptr;
    if (!infoFound)
    {
        pAnim = oxygine::Resources::getResAnim(filename + "+info", oxygine::error_policy::ep_ignore_error);
        if (pAnim.get() != nullptr)
        {
            pCOAnim = SpriteCreator::createAnim(file + "+info.png", colorTable, maskTable, useColorBox, pAnim->getColumns(), pAnim->getRows(), pAnim->getScaleFactor(), false);
            if (pCOAnim.get() != nullptr)
            {
                m_Ressources.append(CoSprite(coidLower + "+info", pCOAnim));
            }
        }
    }
    pAnim = nullptr;
    pCOAnim = nullptr;
    if (!nrmFound)
    {
        oxygine::spResAnim pAnim = oxygine::spResAnim(oxygine::Resources::getResAnim(filename + "+nrm", oxygine::error_policy::ep_ignore_error));
        if (pAnim.get() != nullptr)
        {
            oxygine::spResAnim pCOAnim = SpriteCreator::createAnim(file + "+nrm.png", colorTable, maskTable, useColorBox, pAnim->getColumns(), pAnim->getRows(), pAnim->getScaleFactor(), false);
            if (pCOAnim.get() != nullptr)
            {
                m_Ressources.append(CoSprite(coidLower + "+nrm", pCOAnim));
            }
        }
    }
    pAnim = nullptr;
    pCOAnim = nullptr;
    if (!miniFound)
    {
        oxygine::spResAnim pAnim = oxygine::spResAnim(oxygine::Resources::getResAnim(filename + "+mini", oxygine::error_policy::ep_ignore_error));
        if (pAnim.get() != nullptr)
        {
            oxygine::spResAnim pCOAnim = SpriteCreator::createAnim(file + "+mini.png", colorTable, maskTable, useColorBox, pAnim->getColumns(), pAnim->getRows(), pAnim->getScaleFactor(), false);
            if (pCOAnim.get() != nullptr)
            {
                m_Ressources.append(CoSprite(coidLower + "+mini", pCOAnim));
            }
        }
    }
}

oxygine::ResAnim* COSpriteManager::getResAnim(const QString & id, oxygine::error_policy ep) const
{
    for (qint32 i = 0; i < m_Ressources.size(); i++)
    {
        if (id.toLower() == m_Ressources[i].m_spriteId.toLower())
        {
            return m_Ressources[i].m_sprite.get();
        }
    }
    return oxygine::Resources::getResAnim(id, ep);
}

void COSpriteManager::removeRessource(const QString & id)
{
    for (qint32 i = 0; i < m_loadedRessources.size(); ++i)
    {
        if (m_loadedRessources[i] == id)
        {
            m_loadedRessources.removeAt(i);
            break;
        }
    }
}

QStringList COSpriteManager::getArmyList(const QStringList & coids) const
{
    Interpreter* pInterpreter = Interpreter::getInstance();
    QString function1 = "getArmies";
    QJSValueList args1;
    QJSValue ret = pInterpreter->doFunction("PLAYER", function1, args1);
    QStringList armies = ret.toVariant().toStringList();
    QStringList allowedArmies;
    // remove unused armies
    // first search avaible armies if the selection is set like that
    for (qint32 i = 0; i < coids.size(); i++)
    {
        QString function1 = "getCOArmy";
        QJSValue ret = pInterpreter->doFunction(coids[i], function1);
        if (ret.isString())
        {
            QString army = ret.toString();
            if (!allowedArmies.contains(army))
            {
                allowedArmies.append(army);
            }
        }
    }
    // we have allowed armies? Else allow everything
    if (allowedArmies.size() > 0)
    {
        // remove all other armies
        qint32 iter = 0;
        while (iter < armies.size())
        {
            if (allowedArmies.contains(armies[iter]))
            {
                iter++;
            }
            else
            {
                armies.removeAt(iter);
            }
        }
    }
    return armies;
}
