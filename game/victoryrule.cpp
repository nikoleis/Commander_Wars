#include "victoryrule.h"

#include "coreengine/mainapp.h"
#include "coreengine/interpreter.h"

#include "game/gamemap.h"

#include "game/player.h"

#include "game/co.h"

const QString VictoryRule::checkbox = "checkbox";
const QString VictoryRule::spinbox = "spinbox";

VictoryRule::VictoryRule()
{
    Mainapp* pApp = Mainapp::getInstance();
    this->moveToThread(pApp->getWorkerthread());
    Interpreter::setCppOwnerShip(this);
}

VictoryRule::VictoryRule(QString ruleID)
    : QObject(),
      m_RuleID(ruleID)
{
    init();
}

void VictoryRule::init()
{
    Mainapp* pApp = Mainapp::getInstance();
    this->moveToThread(pApp->getWorkerthread());
    Interpreter::setCppOwnerShip(this);
    QString function1 = "init";
    QJSValueList args1;
    QJSValue obj1 = pApp->getInterpreter()->newQObject(this);
    args1 << obj1;
    QJSValue erg = pApp->getInterpreter()->doFunction(m_RuleID, function1, args1);
}

QStringList VictoryRule::getRuleType()
{
    Mainapp* pApp = Mainapp::getInstance();
    QString function1 = "getRuleType";
    QJSValue ret = pApp->getInterpreter()->doFunction(m_RuleID, function1);
    if (ret.isString())
    {
        return QStringList(ret.toString());
    }
    else
    {
        return ret.toVariant().toStringList();
    }
}

QString VictoryRule::getRuleName(qint32 itemNumber)
{
    Mainapp* pApp = Mainapp::getInstance();
    QString function1 = "getRuleName";
    QJSValueList args;
    args << itemNumber;
    QJSValue ret = pApp->getInterpreter()->doFunction(m_RuleID, function1, args);
    if (ret.isString())
    {
        return ret.toString();
    }
    else
    {
        return "";
    }
}

void VictoryRule::setRuleValue(qint32 value, qint32 itemNumber)
{
    Mainapp* pApp = Mainapp::getInstance();
    QString function1 = "setRuleValue";
    QJSValueList args;
    QJSValue obj1 = pApp->getInterpreter()->newQObject(this);
    args << obj1;
    args << value;
    args << itemNumber;
    QJSValue ret = pApp->getInterpreter()->doFunction(m_RuleID, function1, args);
}

qint32 VictoryRule::getInfiniteValue(qint32 itemNumber)
{
    Mainapp* pApp = Mainapp::getInstance();
    QString function1 = "getInfiniteValue";
    QJSValueList args;
    args << itemNumber;
    QJSValue ret = pApp->getInterpreter()->doFunction(m_RuleID, function1, args);
    if (ret.isNumber())
    {
        return ret.toInt();
    }
    else
    {
        return 0;
    }
}

qint32 VictoryRule::getDefaultValue(qint32 itemNumber)
{
    Mainapp* pApp = Mainapp::getInstance();
    QString function1 = "getDefaultRuleValue";
    QJSValueList args;
    args << itemNumber;
    QJSValue ret = pApp->getInterpreter()->doFunction(m_RuleID, function1, args);
    if (ret.isNumber())
    {
        return ret.toInt();
    }
    else
    {
        return 0;
    }
}

qint32 VictoryRule::getRuleValue(qint32 itemNumber)
{
    Mainapp* pApp = Mainapp::getInstance();
    QString function1 = "getRuleValue";
    QJSValueList args;
    QJSValue obj1 = pApp->getInterpreter()->newQObject(this);
    args << obj1;
    args << itemNumber;
    QJSValue ret = pApp->getInterpreter()->doFunction(m_RuleID, function1, args);
    if (ret.isNumber())
    {
        return ret.toInt();
    }
    else
    {
        return 0;
    }
}

QString VictoryRule::getRuleDescription(qint32 itemNumber)
{
    Mainapp* pApp = Mainapp::getInstance();
    QString function1 = "getRuleDescription";
    QJSValueList args;
    args << itemNumber;
    QJSValue ret = pApp->getInterpreter()->doFunction(m_RuleID, function1, args);
    if (ret.isString())
    {
        return ret.toString();
    }
    else
    {
        return "";
    }
}

qint32 VictoryRule::getRuleProgress(Player* pPlayer)
{
    Mainapp* pApp = Mainapp::getInstance();
    QString function1 = "getRuleProgress";
    QJSValueList args1;
    QJSValue obj1 = pApp->getInterpreter()->newQObject(this);
    args1 << obj1;
    QJSValue obj2 = pApp->getInterpreter()->newQObject(pPlayer);
    args1 << obj2;
    QJSValue ret = pApp->getInterpreter()->doFunction(m_RuleID, function1, args1);
    if (ret.isNumber())
    {
        return ret.toInt();
    }
    else
    {
        return 0;
    }
}

void VictoryRule::serializeObject(QDataStream& pStream)
{
    pStream << getVersion();
    pStream << m_RuleID;
    m_Variables.serializeObject(pStream);
}

void VictoryRule::deserializeObject(QDataStream& pStream)
{
    qint32 version = 0;
    pStream >> version;
    pStream >> m_RuleID;
    m_Variables.deserializeObject(pStream);
}

void VictoryRule::checkDefeat()
{
    GameMap* pMap = GameMap::getInstance();
    for (qint32 i = 0; i < pMap->getPlayerCount(); i++)
    {
        Player* pPlayer = pMap->getPlayer(i);
        Mainapp* pApp = Mainapp::getInstance();
        QString function1 = "checkDefeat";
        QJSValueList args1;
        QJSValue obj1 = pApp->getInterpreter()->newQObject(this);
        args1 << obj1;
        QJSValue obj2 = pApp->getInterpreter()->newQObject(pPlayer);
        args1 << obj2;
        QJSValue erg = pApp->getInterpreter()->doFunction(m_RuleID, function1, args1);
        if (erg.isNumber())
        {
            GameEnums::DefeatType type = static_cast<GameEnums::DefeatType>(erg.toNumber());
            switch (type)
            {
                case GameEnums::DefeatType_Alive:
                {
                    break;
                }
                case GameEnums::DefeatType_Defeated:
                {
                    pPlayer->defeatPlayer(nullptr);
                    break;
                }
                case GameEnums::DefeatType_ByCurrentPlayer:
                {
                    pPlayer->defeatPlayer(pMap->getCurrentPlayer());
                    break;
                }
                case GameEnums::DefeatType_Domination:
                {
                    pPlayer->defeatPlayer(pMap->getCurrentPlayer(), true);
                    break;
                }
            }
        }
    }
}

QString VictoryRule::getRuleID() const
{
    return m_RuleID;
}

