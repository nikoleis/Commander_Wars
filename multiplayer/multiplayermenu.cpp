#include <qcryptographichash.h>

#include "multiplayer/multiplayermenu.h"
#include "multiplayer/lobbymenu.h"
#include "multiplayer/networkcommands.h"

#include "coreengine/mainapp.h"
#include "coreengine/console.h"
#include "coreengine/settings.h"
#include "coreengine/filesupport.h"
#include "coreengine/globalutils.h"

#include "menue/gamemenue.h"

#include "network/tcpclient.h"
#include "network/tcpserver.h"
#include "network/localserver.h"

#include "game/gamemap.h"
#include "game/player.h"
#include "game/co.h"

#include "objects/dialogs/dialogconnecting.h"
#include "objects/dialogs/filedialog.h"
#include "objects/dialogs/dialogmessagebox.h"
#include "objects/base/moveinbutton.h"

#include "ingamescriptsupport/genericbox.h"

#include "resource_management/backgroundmanager.h"
#include "resource_management/objectmanager.h"
#include "resource_management/fontmanager.h"

Multiplayermenu::Multiplayermenu(QString adress, QString password, bool host)
    : MapSelectionMapsMenue(Settings::getSmallScreenDevice() ? Settings::getHeight() - 80 : Settings::getHeight() - 380),
      m_Host(host),
      m_local(true),
      m_password(password)
{
    init();
    if (!host)
    {
        m_NetworkInterface = spTCPClient::create(nullptr);
        m_NetworkInterface->moveToThread(Mainapp::getInstance()->getNetworkThread());
        m_pPlayerSelection->attachNetworkInterface(m_NetworkInterface);
        initClientAndWaitForConnection();
        emit m_NetworkInterface->sig_connect(adress, Settings::getGamePort());
    }
    else
    {
        m_pHostAdresse = ObjectManager::createButton("Show Adresses");
        addChild(m_pHostAdresse);
        m_pHostAdresse->addEventListener(oxygine::TouchEvent::CLICK, [=](oxygine::Event * )->void
        {
            emit sigShowIPs();
        });
        m_pHostAdresse->setPosition(m_pButtonStart->getX() - m_pHostAdresse->getWidth() - 10,
                                    Settings::getHeight() - m_pHostAdresse->getHeight() - 10);
        m_pHostAdresse->setVisible(false);
        connect(this, &Multiplayermenu::sigShowIPs, this, &Multiplayermenu::showIPs, Qt::QueuedConnection);
    }
}

Multiplayermenu::Multiplayermenu(spNetworkInterface pNetworkInterface, QString password, bool host)
    : MapSelectionMapsMenue(Settings::getHeight() - 380),
      m_Host(host),
      m_local(false),
      m_password(password)
{
    m_NetworkInterface = pNetworkInterface.get();
    init();
    if (!host)
    {
        initClientAndWaitForConnection();
    }
    else
    {
        dynamic_cast<Label*>(m_pButtonStart->getFirstChild()->getFirstChild().get())->setHtmlText(tr("Ready"));
        m_pPlayerSelection->setIsServerGame(true);
    }
}

void Multiplayermenu::initClientAndWaitForConnection()
{
    createChat();
    hideRuleSelection();
    Multiplayermenu::hideMapSelection();
    m_MapSelectionStep = MapSelectionStep::selectPlayer;
    // change the name of the start button
    dynamic_cast<Label*>(m_pButtonStart->getFirstChild()->getFirstChild().get())->setHtmlText(tr("Ready"));
    // quit on host connection lost
    connect(m_NetworkInterface.get(), &NetworkInterface::sigDisconnected, this, &Multiplayermenu::disconnected, Qt::QueuedConnection);
    connect(m_NetworkInterface.get(), &NetworkInterface::recieveData, this, &Multiplayermenu::recieveData, Qt::QueuedConnection);
    connect(m_pPlayerSelection.get(), &PlayerSelection::sigDisconnect, this, &Multiplayermenu::buttonBack, Qt::QueuedConnection);
    // wait 10 minutes till timeout
    spDialogConnecting pDialogConnecting = spDialogConnecting::create(tr("Connecting"), 1000 * 60 * 5);
    addChild(pDialogConnecting);
    connect(pDialogConnecting.get(), &DialogConnecting::sigCancel, this, &Multiplayermenu::buttonBack, Qt::QueuedConnection);
    connect(this, &Multiplayermenu::sigConnected, pDialogConnecting.get(), &DialogConnecting::connected, Qt::QueuedConnection);
}

void Multiplayermenu::init()
{
    Mainapp* pApp = Mainapp::getInstance();
    moveToThread(pApp->getWorkerthread());
    Interpreter::setCppOwnerShip(this);
    CONSOLE_PRINT("Entering Multiplayer Menue", Console::eDEBUG);
    m_pButtonLoadSavegame = ObjectManager::createButton(tr("Load Savegame"));
    m_pButtonLoadSavegame->setPosition(Settings::getWidth() - m_pButtonLoadSavegame->getWidth() - m_pButtonNext->getWidth() - 20, Settings::getHeight() - 10 - m_pButtonLoadSavegame->getHeight());
    addChild(m_pButtonLoadSavegame);
    m_pButtonLoadSavegame->addEventListener(oxygine::TouchEvent::CLICK, [=](oxygine::Event * )->void
    {
        emit sigLoadSaveGame();
    });
    connect(this, &Multiplayermenu::sigLoadSaveGame, this, &Multiplayermenu::showLoadSaveGameDialog, Qt::QueuedConnection);
    connect(&m_GameStartTimer, &QTimer::timeout, this, &Multiplayermenu::countdown, Qt::QueuedConnection);
}

void Multiplayermenu::showIPs()
{
    spGenericBox pGenericBox = spGenericBox::create();
    QStringList items = NetworkInterface::getIPAdresses();
    QSize size(Settings::getWidth() - 40, Settings::getHeight() - 80);
    spPanel pPanel = spPanel::create(true, size, size);
    pPanel->setPosition(20, 20);
    oxygine::TextStyle style = oxygine::TextStyle(FontManager::getMainFont24());
    style.color = FontManager::getFontColor();
    style.vAlign = oxygine::TextStyle::VALIGN_DEFAULT;
    style.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    style.multiline = true;
    oxygine::spTextField info = oxygine::spTextField::create();
    info->setStyle(style);
    info->setHtmlText((tr("Please use one of the following IP-Adresses to connect to this Host. Not all IP-Adresses") +
                       tr(" may work for each client depending on the network settings. Please use cmd and the ping command to verify if an IP-Adress may work")));
    info->setSize(Settings::getWidth() - 80, 500);
    info->setPosition(10, 10);
    pPanel->addItem(info);
    qint32 starty = 10 + info->getTextRect().getHeight();
    for (qint32 i = 0; i < items.size(); i++)
    {
        oxygine::spTextField text = oxygine::spTextField::create();
        text->setStyle(style);
        text->setHtmlText((tr("Host Adress: ") + items[i]));
        text->setPosition(10, starty + i * 40);
        pPanel->addItem(text);
    }
    pPanel->setContentHeigth(items.size() * 40 + 40);
    pGenericBox->addItem(pPanel);
    addChild(pGenericBox);
}

void Multiplayermenu::showLoadSaveGameDialog()
{
    
    // dummy impl for loading
    QStringList wildcards;
    wildcards.append("*.msav");
    QString path = Settings::getUserPath() + "savegames";
    spFileDialog saveDialog = spFileDialog::create(path, wildcards);
    addChild(saveDialog);
    connect(saveDialog.get(), &FileDialog::sigFileSelected, this, &Multiplayermenu::loadSaveGame, Qt::QueuedConnection);
    
}

void Multiplayermenu::loadSaveGame(QString filename)
{    
    if (filename.endsWith(".msav"))
    {
        QFile file(filename);
        QFileInfo info(filename);
        if (file.exists())
        {
            m_pMapSelectionView->loadMap(info, false);
            hideMapSelection();
            m_saveGame = true;
            m_pPlayerSelection->setSaveGame(m_saveGame);
            m_MapSelectionStep = MapSelectionStep::selectRules;
            buttonNext();
        }
    }    
}

void Multiplayermenu::hideMapSelection()
{    
    m_pButtonLoadSavegame->setVisible(false);
    MapSelectionMapsMenue::hideMapSelection();    
}

void Multiplayermenu::showMapSelection()
{    
    m_pButtonLoadSavegame->setVisible(true);
    MapSelectionMapsMenue::showMapSelection();    
}

void Multiplayermenu::playerJoined(quint64 socketID)
{
    if (m_NetworkInterface->getIsServer() && Mainapp::getSlave())
    {
        if (m_slaveGameReady)
        {
            sendSlaveReady();
        }
    }
    else if (m_NetworkInterface->getIsServer() && m_local)
    {
        if (m_pPlayerSelection->hasOpenPlayer())
        {
            acceptNewConnection(socketID);
        }
        else
        {
            // reject connection by disconnecting
            emit m_NetworkInterface.get()->sigDisconnectClient(socketID);
        }
    }
}

void Multiplayermenu::acceptNewConnection(quint64 socketID)
{
    CONSOLE_PRINT("Accepting connection for socket " + QString::number(socketID), Console::eDEBUG);
    QCryptographicHash myHash(QCryptographicHash::Sha3_512);
    QString file = m_pMapSelectionView->getCurrentFile().filePath();
    QString fileName = m_pMapSelectionView->getCurrentFile().fileName();
    QString scriptFile = m_pMapSelectionView->getCurrentMap()->getGameScript()->getScriptFile();
    QFile mapFile(file);
    mapFile.open(QIODevice::ReadOnly);
    myHash.addData(&mapFile);
    mapFile.close();
    QString command = QString(NetworkCommands::MAPINFO);
    CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
    QByteArray hash = myHash.result();
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << command;
    stream << Mainapp::getGameVersion();
    QStringList mods = Settings::getMods();
    QStringList versions = Settings::getActiveModVersions();
    bool filter = m_pMapSelectionView->getCurrentMap()->getGameRules()->getCosmeticModsAllowed();
    filterCosmeticMods(mods, versions, filter);
    stream << filter;
    stream << static_cast<qint32>(mods.size());
    for (qint32 i = 0; i < mods.size(); i++)
    {
        stream << mods[i];
        stream << versions[i];
    }
    Filesupport::writeByteArray(stream, Filesupport::getRuntimeHash(mods));
    stream << m_saveGame;
    if (m_saveGame)
    {
        m_pMapSelectionView->getCurrentMap()->serializeObject(stream);
    }
    else
    {
        stream << fileName;
        stream << hash;
        stream << scriptFile;
        if (QFile::exists(scriptFile))
        {
            // create hash for script file
            QFile scriptData(scriptFile);
            QCryptographicHash myScriptHash(QCryptographicHash::Sha3_512);
            scriptData.open(QIODevice::ReadOnly);
            myScriptHash.addData(&scriptData);
            scriptData.close();
            QByteArray scriptHash = myScriptHash.result();
            stream << scriptHash;
        }
    }
    // send map data to client
    emit m_NetworkInterface->sig_sendData(socketID, data, NetworkInterface::NetworkSerives::Multiplayer, false);
}

void Multiplayermenu::filterCosmeticMods(QStringList & mods, QStringList & versions, bool filter)
{
    if (filter)
    {
        qint32 i = 0;
        while (i < mods.length())
        {
            if (Settings::getIsCosmetic(mods[i]))
            {
                mods.removeAt(i);
                versions.removeAt(i);
            }
            else
            {
                ++i;
            }
        }
    }
}

void Multiplayermenu::recieveData(quint64 socketID, QByteArray data, NetworkInterface::NetworkSerives service)
{
    // data for us?
    if (service == NetworkInterface::NetworkSerives::Multiplayer)
    {
        if (!m_pPlayerSelection->hasNetworkInterface())
        {
            m_pPlayerSelection->attachNetworkInterface(m_NetworkInterface);
        }
        QDataStream stream(&data, QIODevice::ReadOnly);
        QString messageType;
        stream >> messageType;
        CONSOLE_PRINT("Local Network Command received: " + messageType + " for socket " + QString::number(socketID), Console::eDEBUG);
        if (messageType == NetworkCommands::MAPINFO)
        {
            clientMapInfo(stream, socketID);
        }
        else if (messageType == NetworkCommands::REQUESTRULE)
        {
            requestRule(socketID);
        }
        else if (messageType == NetworkCommands::SENDINITUPDATE)
        {
            sendInitUpdate(stream, socketID);
        }
        else if (messageType == NetworkCommands::REQUESTMAP)
        {
            requestMap(socketID);
        }
        else if (messageType == NetworkCommands::MAPDATA)
        {
            recieveMap(stream, socketID);
        }
        else if (messageType == NetworkCommands::INITGAME)
        {
            // initializes the game on the client
            if (!m_NetworkInterface->getIsServer() ||
                !m_local)
            {
                initClientGame(socketID, stream);
                oxygine::Actor::detach();
            }
        }
        else if (messageType == NetworkCommands::STARTSERVERGAME ||
                 messageType == NetworkCommands::CLIENTREADY)
        {
            if (messageType == NetworkCommands::CLIENTREADY)
            {
                m_pPlayerSelection->recievePlayerReady(socketID, stream);
            }
            if (!m_local)
            {
                CONSOLE_PRINT("Checking if server game should start", Console::eDEBUG);
                startCountdown();
            }
        }
    }
    else if (service == NetworkInterface::NetworkSerives::ServerHosting)
    {
        QDataStream stream(&data, QIODevice::ReadOnly);
        QString messageType;
        stream >> messageType;
        CONSOLE_PRINT("Recieving data from Master. Command received: " + messageType + " for socket " + QString::number(socketID), Console::eDEBUG);
        if (messageType == NetworkCommands::LAUNCHGAMEONSERVER)
        {
            launchGameOnServer(stream);
        }
        else if (messageType == NetworkCommands::PLAYERJOINEDGAMEONSERVER)
        {
            playerJoinedServer(stream, socketID);
        }
        else if (messageType == NetworkCommands::PLAYERDISCONNECTEDGAMEONSERVER)
        {
            quint64 socket;
            stream >> socket;
            m_pPlayerSelection->disconnected(socket);
        }
    }
}

void Multiplayermenu::playerJoinedServer(QDataStream & stream, quint64 socketID)
{
    if (m_pPlayerSelection->hasOpenPlayer())
    {
        CONSOLE_PRINT("Player joined game for socket " + QString::number(socketID), Console::eDEBUG);
        quint64 socketId;
        stream >> socketId;
        dynamic_cast<LocalServer*>(m_NetworkInterface.get())->addSocket(socketID);
        acceptNewConnection(socketID);
    }
    else
    {
        CONSOLE_PRINT("Player rejected cause no player is open for socket " + QString::number(socketID), Console::eDEBUG);
        QString command = QString(NetworkCommands::PLAYERDISCONNECTEDGAMEONSERVER);
        CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << command;
        stream << socketID;
        emit m_NetworkInterface->sig_sendData(0, data, NetworkInterface::NetworkSerives::ServerHosting, false);
    }
}

void Multiplayermenu::requestRule(quint64 socketID)
{
    // a client requested the current map rules set by the server
    if (m_NetworkInterface->getIsServer())
    {
        QString command = QString(NetworkCommands::SENDINITUPDATE);
        CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
        QByteArray sendData;
        QDataStream sendStream(&sendData, QIODevice::WriteOnly);
        sendStream << command;
        m_pMapSelectionView->getCurrentMap()->getGameRules()->serializeObject(sendStream);
        if (m_pMapSelectionView->getCurrentMap()->getCampaign() != nullptr)
        {
            sendStream << true;
            m_pMapSelectionView->getCurrentMap()->getCampaign()->serializeObject(sendStream);
        }
        else
        {
            sendStream << false;
        }
        for (qint32 i = 0; i < m_pMapSelectionView->getCurrentMap()->getPlayerCount(); i++)
        {
            GameEnums::AiTypes aiType = m_pPlayerSelection->getPlayerAiType(i);
            if (aiType == GameEnums::AiTypes_Human)
            {
                sendStream << Settings::getUsername();
                sendStream << static_cast<qint32>(GameEnums::AiTypes_ProxyAi);
            }
            else
            {
                sendStream << m_pPlayerSelection->getPlayerAiName(i);
                if (m_pPlayerSelection->isOpenPlayer(i))
                {
                    sendStream << static_cast<qint32>(GameEnums::AiTypes_Open);
                }
                else if (m_pPlayerSelection->isClosedPlayer(i))
                {
                    sendStream << static_cast<qint32>(GameEnums::AiTypes_Closed);
                }
                else
                {
                    sendStream << static_cast<qint32>(GameEnums::AiTypes_ProxyAi);
                }
            }
            m_pMapSelectionView->getCurrentMap()->getPlayer(i)->serializeObject(sendStream);
        }
        emit m_NetworkInterface->sig_sendData(socketID, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
    }
}

void Multiplayermenu::sendInitUpdate(QDataStream & stream, quint64 socketID)
{
    // the client recieved the initial map data read it and make it visible
    if (!m_NetworkInterface->getIsServer() || !m_local)
    {
        m_pMapSelectionView->getCurrentMap()->getGameRules()->deserializeObject(stream);
        if (!m_password.areEqualPassword(m_pMapSelectionView->getCurrentMap()->getGameRules()->getPassword()))
        {
            CONSOLE_PRINT("Incorrect Password found.", Console::eDEBUG);
            // quit game with wrong version
            buttonBack();
        }
        else
        {

            bool campaign = false;
            stream >> campaign;
            if (campaign)
            {
                m_pMapSelectionView->getCurrentMap()->setCampaign(spCampaign::create());
                m_pMapSelectionView->getCurrentMap()->getCampaign()->deserializeObject(stream);
            }
            CONSOLE_PRINT(("Reading players count: " + QString::number(m_pMapSelectionView->getCurrentMap()->getPlayerCount())), Console::eDEBUG);
            for (qint32 i = 0; i < m_pMapSelectionView->getCurrentMap()->getPlayerCount(); i++)
            {
                QString name;
                qint32 aiType;
                stream >> name;
                stream >> aiType;
                m_pPlayerSelection->setPlayerAiName(i, name);
                m_pMapSelectionView->getCurrentMap()->getPlayer(i)->deserializeObject(stream);
                m_pMapSelectionView->getCurrentMap()->getPlayer(i)->setBaseGameInput(BaseGameInputIF::createAi(static_cast<GameEnums::AiTypes>(aiType)));
                m_pPlayerSelection->updatePlayerData(i);
            }
            m_pPlayerSelection->sendPlayerRequest(socketID, -1, GameEnums::AiTypes_Human);
            emit sigConnected();
            emit sigHostGameLaunched();
        }
    }
}

void Multiplayermenu::clientMapInfo(QDataStream & stream, quint64 socketID)
{
    // we recieved map info from a server check if we have this map already
    if (!m_NetworkInterface->getIsServer() || !m_local)
    {
        QString version;
        stream >> version;
        bool filter = false;
        stream >> filter;
        qint32 size = 0;
        stream >> size;
        QStringList mods;
        QStringList versions;
        for (qint32 i = 0; i < size; i++)
        {
            QString mod;
            stream >> mod;
            mods.append(mod);
            QString version;
            stream >> version;
            versions.append(version);
        }
        bool sameMods = checkMods(mods, versions, filter);
        bool differentHash = false;
        QByteArray hostRuntime = Filesupport::readByteArray(stream);
        if (hostRuntime != Filesupport::getRuntimeHash(mods))
        {
            differentHash = false;
        }
        if (version == Mainapp::getGameVersion() && sameMods && !differentHash)
        {
            stream >> m_saveGame;
            m_pPlayerSelection->setSaveGame(m_saveGame);
            if (m_saveGame)
            {
                QString command = QString(NetworkCommands::REQUESTRULE);
                CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
                m_pMapSelectionView->setCurrentMap(spGameMap::create<QDataStream &, bool>(stream, m_saveGame));
                loadMultiplayerMap();
                QByteArray sendData;
                QDataStream sendStream(&sendData, QIODevice::WriteOnly);
                sendStream << command;
                emit m_NetworkInterface->sig_sendData(socketID, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
            }
            else
            {
                QString fileName;
                stream >> fileName;
                QByteArray hash;
                stream >> hash;
                QString scriptFile;
                stream >> scriptFile;
                QByteArray scriptHash;
                CONSOLE_PRINT("Checking for map " + fileName + " and script " + scriptFile , Console::eDEBUG);
                if (!scriptFile.isEmpty())
                {
                    stream >> scriptHash;
                }
                if (!fileName.startsWith(NetworkCommands::RANDOMMAPIDENTIFIER) &&
                    !fileName.startsWith(NetworkCommands::SERVERMAPIDENTIFIER) &&
                    existsMap(fileName, hash, scriptFile, scriptHash))
                {
                    QString command = QString(NetworkCommands::REQUESTRULE);
                    CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
                    QByteArray sendData;
                    QDataStream sendStream(&sendData, QIODevice::WriteOnly);
                    sendStream << command;
                    emit m_NetworkInterface->sig_sendData(socketID, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
                }
                else
                {
                    QString command = QString(NetworkCommands::REQUESTMAP);
                    CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
                    QByteArray sendData;
                    QDataStream sendStream(&sendData, QIODevice::WriteOnly);
                    sendStream << command;
                    emit m_NetworkInterface->sig_sendData(socketID, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
                }
            }
        }
        else
        {
            // quit game with wrong version
            spDialogMessageBox pDialogMessageBox;
            if (sameMods)
            {
                pDialogMessageBox = spDialogMessageBox::create(tr("Host has a different game version. Leaving the game again."));
            }
            else if (differentHash)
            {
                pDialogMessageBox = spDialogMessageBox::create(tr("Host has a different version of a mod or the game resource folder has been modified by one of the games."));
            }
            else
            {
                QString hostMods;
                for (auto & mod : mods)
                {
                    hostMods += Settings::getModName(mod) + "\n";
                }
                mods = Settings::getMods();
                QString myMods;
                for (auto & mod : mods)
                {
                    myMods += Settings::getModName(mod) + "\n";
                }
                pDialogMessageBox = spDialogMessageBox::create(tr("Host has  different mods. Leaving the game again.\nHost mods: ") + hostMods + "\nYour Mods:" + myMods);
            }

            connect(pDialogMessageBox.get(), &DialogMessageBox::sigOk, this, &Multiplayermenu::buttonBack, Qt::QueuedConnection);
            addChild(pDialogMessageBox);
            
        }
    }
}

bool Multiplayermenu::checkMods(const QStringList & mods, const QStringList & versions, bool filter)
{
    QStringList myVersions = Settings::getActiveModVersions();
    QStringList myMods = Settings::getMods();
    filterCosmeticMods(myMods, myVersions, filter);
    bool sameMods = true;
    if (myMods.size() != mods.size())
    {
        sameMods = false;
    }
    else
    {
        // check mods in both directions
        for (qint32 i = 0; i < myMods.size(); i++)
        {
            if (!mods.contains(myMods[i]))
            {
                sameMods = false;
                break;
            }
            else
            {
                for (qint32 i2 = 0; i2 < mods.size(); i2++)
                {
                    if (mods[i2] == myMods[i])
                    {
                        if (versions[i2] != myVersions[i])
                        {
                            sameMods = false;
                            break;
                        }
                    }
                }
            }
        }
        for (qint32 i = 0; i < mods.size(); i++)
        {
            if (!myMods.contains(mods[i]))
            {
                sameMods = false;
                break;
            }
        }
    }
    return sameMods;
}

void Multiplayermenu::requestMap(quint64 socketID)
{
    // someone requested the current map data from the server
    if (m_NetworkInterface->getIsServer())
    {
        QString command = QString(NetworkCommands::MAPDATA);
        CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
        QString file = GlobalUtils::makePathRelative(m_pMapSelectionView->getCurrentFile().filePath(), false);
        QString scriptFile = m_pMapSelectionView->getCurrentMap()->getGameScript()->getScriptFile();
        QByteArray sendData;
        QDataStream sendStream(&sendData, QIODevice::WriteOnly);
        sendStream << command;
        sendStream << file;
        sendStream << scriptFile;
        QFile mapFile(m_pMapSelectionView->getCurrentFile().filePath());
        if (file.startsWith(NetworkCommands::RANDOMMAPIDENTIFIER) ||
            file.startsWith(NetworkCommands::SERVERMAPIDENTIFIER))
        {
            GameMap::getInstance()->serializeObject(sendStream);
        }
        else
        {
            mapFile.open(QIODevice::ReadOnly);
            QByteArray data = mapFile.readAll();
            mapFile.close();
            Filesupport::writeByteArray(sendStream, data);
        }
        if (!scriptFile.isEmpty())
        {
            QFile script(scriptFile);
            script.open(QIODevice::ReadOnly);
            QByteArray data = script.readAll();
            Filesupport::writeByteArray(sendStream, data);
            script.close();
        }
        emit m_NetworkInterface->sig_sendData(socketID, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
    }
}

void Multiplayermenu::recieveMap(QDataStream & stream, quint64 socketID)
{
    if (!m_NetworkInterface->getIsServer())
    {
        QString mapFile;
        stream >> mapFile;
        QString scriptFile;
        stream >> scriptFile;
        spGameMap pNewMap;
        if (mapFile.startsWith(NetworkCommands::RANDOMMAPIDENTIFIER) ||
            mapFile.startsWith(NetworkCommands::SERVERMAPIDENTIFIER))
        {
            pNewMap = spGameMap::create<QDataStream &, bool>(stream, m_saveGame);
        }
        else
        {
            QFile map(mapFile);
            QFile script(scriptFile);
            if (!map.exists() &&
                (scriptFile.isEmpty() ||
                 !script.exists()))
            {
                QByteArray mapData;
                mapData = Filesupport::readByteArray(stream);
                QFileInfo mapInfo(mapFile);
                QDir dir;
                QString fileDir = mapInfo.filePath().replace(mapInfo.fileName(), "");
                dir.mkdir(fileDir);
                map.open(QIODevice::WriteOnly);
                QDataStream mapFilestream(&map);
                Filesupport::writeBytes(mapFilestream, mapData);
                map.close();
                if (!scriptFile.isEmpty())
                {
                    QByteArray scriptData;
                    scriptData = Filesupport::readByteArray(stream);
                    QFileInfo scriptInfo(scriptFile);
                    fileDir = scriptInfo.filePath().replace(scriptInfo.fileName(), "");
                    dir.mkdir(fileDir);
                    script.open(QIODevice::WriteOnly);
                    QDataStream scriptFilestream(&script);
                    Filesupport::writeBytes(scriptFilestream, scriptData);
                    script.close();
                }
                pNewMap = spGameMap::create(mapFile, true, false, m_saveGame);
            }
            else
            {
                pNewMap = createMapFromStream(mapFile, scriptFile, stream);
            }
        }
        QString command = QString(NetworkCommands::REQUESTRULE);
        CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
        m_pMapSelectionView->setCurrentMap(pNewMap);
        loadMultiplayerMap();
        QByteArray sendData;
        QDataStream sendStream(&sendData, QIODevice::WriteOnly);
        sendStream << command;
        emit m_NetworkInterface->sig_sendData(socketID, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
    }
}

void Multiplayermenu::launchGameOnServer(QDataStream & stream)
{
    CONSOLE_PRINT("Launching Game on Slave", Console::eDEBUG);
    hideMapSelection();
    QStringList mods;
    mods = Filesupport::readVectorList<QString, QList>(stream);
    spGameMap pMap = spGameMap::create<QDataStream &, bool>(stream, m_saveGame);
    m_pMapSelectionView->setCurrentMap(pMap);
    m_pMapSelectionView->setCurrentFile(NetworkCommands::SERVERMAPIDENTIFIER);
    m_pPlayerSelection->attachNetworkInterface(m_NetworkInterface);
    m_pPlayerSelection->setIsServerGame(true);
    loadMultiplayerMap();
    createChat();
    m_pPlayerSelection->setPlayerAi(0, GameEnums::AiTypes::AiTypes_Open, "");
    sendSlaveReady();
    m_slaveGameReady = true;
}

void Multiplayermenu::sendSlaveReady()
{
    QString command = QString(NetworkCommands::GAMERUNNINGONSERVER);
    CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
    spGameMap pMap = GameMap::getInstance();
    QByteArray sendData;
    QDataStream sendStream(&sendData, QIODevice::WriteOnly);
    sendStream << command;
    sendStream << pMap->getGameRules()->getDescription();
    if (pMap->getGameRules()->getPassword().isValidPassword(""))
    {
        sendStream << false;
    }
    else
    {
        sendStream << true;
    }
    emit m_NetworkInterface->sig_sendData(0, sendData, NetworkInterface::NetworkSerives::ServerHosting, true);
}

void Multiplayermenu::slotCancelHostConnection()
{
    buttonBack();
}

void Multiplayermenu::slotHostGameLaunched()
{
    // we're hosting a game so we get the same rights as a local host
    m_NetworkInterface->setIsServer(true);
    createChat();
    MapSelectionMapsMenue::buttonNext();
}

spGameMap Multiplayermenu::createMapFromStream(QString mapFile, QString scriptFile, QDataStream &stream)
{
    spGameMap pNewMap;
    mapFile = getNewFileName(mapFile);
    QFile map(mapFile);
    QFileInfo mapInfo(mapFile);
    QDir dir;
    QString fileDir = mapInfo.filePath().replace(mapInfo.fileName(), "");
    dir.mkdir(fileDir);
    map.open(QIODevice::WriteOnly);
    QDataStream mapFilestream(&map);
    QByteArray mapData;
    mapData = Filesupport::readByteArray(stream);
    Filesupport::writeBytes(mapFilestream, mapData);
    map.close();
    pNewMap = spGameMap::create(mapFile, true, false, m_saveGame);
    if (!scriptFile.isEmpty())
    {
        QFile script(scriptFile);
        scriptFile = getNewFileName(scriptFile);
        QByteArray scriptData;
        scriptData = Filesupport::readByteArray(stream);
        QFileInfo scriptInfo(scriptFile);
        fileDir = scriptInfo.filePath().replace(scriptInfo.fileName(), "");
        dir.mkdir(fileDir);
        script.open(QIODevice::WriteOnly);
        QDataStream scriptFilestream(&script);
        Filesupport::writeBytes(scriptFilestream, scriptData);
        script.close();
        scriptFile = GlobalUtils::makePathRelative(scriptFile, true);
        // save script file
        pNewMap->getGameScript()->setScriptFile(scriptFile);
        map.open(QIODevice::WriteOnly);
        QDataStream stream(&map);
        pNewMap->serializeObject(stream);
        map.close();
    }
    return pNewMap;
}

QString Multiplayermenu::getNewFileName(QString filename)
{
    bool found = false;
    qint32 index = 0;
    QString newName;
    filename = filename.replace(".map", "");
    while (!found)
    {
        newName = filename + "+" + QString::number(index) + ".map";
        found = !QFile::exists(newName);
        index++;
    }
    return newName;
}

void Multiplayermenu::loadMultiplayerMap()
{    
    m_pMapSelectionView->getCurrentMap()->getGameScript()->init();
    m_pMapSelectionView->updateMapData();
    showPlayerSelection();
}

void Multiplayermenu::initClientGame(quint64, QDataStream &stream)
{
    
    spGameMap pMap = GameMap::getInstance();
    pMap->setVisible(false);
    if (!m_saveGame)
    {
        pMap->initPlayers();
    }
    quint32 seed;
    stream >> seed;
    for (qint32 i = 0; i < m_pMapSelectionView->getCurrentMap()->getPlayerCount(); i++)
    {
        GameEnums::AiTypes aiType = GameEnums::AiTypes::AiTypes_Closed;
        auto* baseGameInput = m_pMapSelectionView->getCurrentMap()->getPlayer(i)->getBaseGameInput();
        if (baseGameInput != nullptr)
        {
            aiType = baseGameInput->getAiType();
        }
        CONSOLE_PRINT("Creating AI for player " + QString::number(i) + " of type " + QString::number(aiType), Console::eDEBUG);
        m_pMapSelectionView->getCurrentMap()->getPlayer(i)->deserializeObject(stream);
        m_pMapSelectionView->getCurrentMap()->getPlayer(i)->setBaseGameInput(BaseGameInputIF::createAi(aiType));
    }
    GlobalUtils::seed(seed);
    GlobalUtils::setUseSeed(true);

    if (!m_saveGame)
    {
        pMap->getGameScript()->gameStart();
    }
    pMap->updateSprites();
    // start game
    m_NetworkInterface->setIsServer(false);
    CONSOLE_PRINT("Leaving Map Selection Menue", Console::eDEBUG);
    auto window = spGameMenue::create(m_saveGame, m_NetworkInterface);
    oxygine::Stage::getStage()->addChild(window);
    // send game started
    QString command = QString(NetworkCommands::CLIENTINITGAME);
    CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
    QByteArray sendData;
    QDataStream sendStream(&sendData, QIODevice::WriteOnly);
    sendStream << command;
    sendStream << m_NetworkInterface->getSocketID();
    emit m_NetworkInterface->sig_sendData(0, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
    
}

bool Multiplayermenu::existsMap(QString& fileName, QByteArray& hash, QString& scriptFileName, QByteArray& scriptHash)
{
    QString path = "maps";
    QStringList filter;
    filter << "*" + fileName;
    QDirIterator dirIter(Settings::getUserPath() + path, filter, QDir::Files, QDirIterator::Subdirectories);
    bool found = findAndLoadMap(dirIter, hash, m_saveGame);
    if (!found)
    {
        QDirIterator virtDirIter(oxygine::Resource::RCC_PREFIX_PATH + path, filter, QDir::Files, QDirIterator::Subdirectories);
        found = findAndLoadMap(virtDirIter, hash, m_saveGame);
    }
    if (found  && !m_saveGame)
    {
        if (!scriptFileName.isEmpty())
        {
            QFile scriptFile;
            QByteArray myHashArray;
            // check real drive and virt drive for script
            if (QFile::exists(Settings::getUserPath() + scriptFileName))
            {
                scriptFile.setFileName(Settings::getUserPath() + scriptFileName);
                scriptFile.open(QIODevice::ReadOnly);
                QCryptographicHash myHash(QCryptographicHash::Sha3_512);
                myHash.addData(&scriptFile);
                scriptFile.close();
                myHashArray = myHash.result();

            }
            if (myHashArray != scriptHash && QFile::exists(oxygine::Resource::RCC_PREFIX_PATH + scriptFileName))
            {
                scriptFile.setFileName(oxygine::Resource::RCC_PREFIX_PATH + scriptFileName);
                scriptFile.open(QIODevice::ReadOnly);
                QCryptographicHash myHash(QCryptographicHash::Sha3_512);
                myHash.addData(&scriptFile);
                scriptFile.close();
                QByteArray myHashArray = myHash.result();
                if (myHashArray != scriptHash)
                {
                    found = false;
                }
            }
            else
            {
                found = false;
            }
        }
    }
    return found;
}

bool Multiplayermenu::findAndLoadMap(QDirIterator & dirIter, QByteArray& hash, bool m_saveGame)
{
    bool found = false;
    while (dirIter.hasNext() && !found)
    {
        dirIter.next();
        QString file = dirIter.fileInfo().absoluteFilePath();
        QFile mapFile(file);
        mapFile.open(QIODevice::ReadOnly);
        QCryptographicHash myHash(QCryptographicHash::Sha3_512);
        myHash.addData(&mapFile);
        mapFile.close();
        QByteArray myHashArray = myHash.result();
        if (hash == myHashArray)
        {
            m_pMapSelectionView->setCurrentMap(spGameMap::create(file, true, false, m_saveGame));
            loadMultiplayerMap();
            found = true;
        }
    }
    return found;
}

void Multiplayermenu::showRuleSelection()
{    
    m_pRuleSelection->setVisible(true);
    m_pButtonSaveRules->setVisible(true);
    m_pButtonLoadRules->setVisible(true);
    m_pRuleSelection->clearContent();
    m_pRuleSelectionView = spRuleSelection::create(Settings::getWidth() - 80, RuleSelection::Mode::Multiplayer);
    connect(m_pRuleSelectionView.get(), &RuleSelection::sigSizeChanged, this, &Multiplayermenu::ruleSelectionSizeChanged, Qt::QueuedConnection);
    m_pRuleSelection->addItem(m_pRuleSelectionView);
    m_pRuleSelection->setContentHeigth(m_pRuleSelectionView->getHeight() + 40);
    m_pRuleSelection->setContentWidth(m_pRuleSelectionView->getWidth());    
}

void Multiplayermenu::disconnected(quint64)
{
    if (m_Host)
    {
        // handled in player selection
    }
    else
    {
        disconnectNetwork();
        CONSOLE_PRINT("Leaving Map Selection Menue", Console::eDEBUG);
        oxygine::Stage::getStage()->addChild(spLobbyMenu::create());
        oxygine::Actor::detach();
    }
}

void Multiplayermenu::buttonBack()
{
    if (!m_Host ||
        m_MapSelectionStep == MapSelectionStep::selectMap ||
        !m_local)
    {
        disconnectNetwork();
        CONSOLE_PRINT("Leaving Map Selection Menue", Console::eDEBUG);
        oxygine::Stage::getStage()->addChild(spLobbyMenu::create());
        oxygine::Actor::detach();
    }
    else if (m_Host)
    {
        m_pHostAdresse->setVisible(false);
        if (m_Chat.get() != nullptr)
        {
            m_Chat->detach();
            m_Chat = nullptr;
        }
        disconnectNetwork();
        MapSelectionMapsMenue::buttonBack();
        if (m_saveGame)
        {
            MapSelectionMapsMenue::buttonBack();
            m_saveGame = false;
            m_pPlayerSelection->setSaveGame(m_saveGame);
        }
    }
}

void Multiplayermenu::buttonNext()
{
    if (m_Host && m_MapSelectionStep == MapSelectionStep::selectRules)
    {
        spGameMap pMap = GameMap::getInstance();
        m_password.setPassword(pMap->getGameRules()->getPassword());
        if (m_local)
        {
            m_pHostAdresse->setVisible(true);
            m_NetworkInterface = spTCPServer::create(nullptr);
            m_NetworkInterface->moveToThread(Mainapp::getInstance()->getNetworkThread());
            m_pPlayerSelection->attachNetworkInterface(m_NetworkInterface);
            createChat();
            emit m_NetworkInterface->sig_connect("", Settings::getGamePort());
            connectNetworkSlots();
            MapSelectionMapsMenue::buttonNext();
        }
        else
        {
            MapSelectionMapsMenue::hideRuleSelection();
            connectNetworkSlots();
            startGameOnServer();
        }
    }
    else
    {
        MapSelectionMapsMenue::buttonNext();
    }
}

void Multiplayermenu::connectNetworkSlots()
{
    connect(m_NetworkInterface.get(), &NetworkInterface::sigConnected, this, &Multiplayermenu::playerJoined, Qt::QueuedConnection);
    connect(m_NetworkInterface.get(), &NetworkInterface::recieveData, this, &Multiplayermenu::recieveData, Qt::QueuedConnection);
    connect(m_NetworkInterface.get(), &NetworkInterface::sigDisconnected, this, &Multiplayermenu::disconnected, Qt::QueuedConnection);
}

void Multiplayermenu::startGameOnServer()
{
    QString command = QString(NetworkCommands::LAUNCHGAMEONSERVER);
    CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
    QByteArray sendData;
    QDataStream sendStream(&sendData, QIODevice::WriteOnly);
    sendStream << command;
    Filesupport::writeVectorList(sendStream, Settings::getMods());
    spGameMap pMap = GameMap::getInstance();
    pMap->serializeObject(sendStream);
    emit m_NetworkInterface->sig_sendData(0, sendData, NetworkInterface::NetworkSerives::ServerHosting, false);

    spDialogConnecting pDialogConnecting = spDialogConnecting::create(tr("Launching game on server"), 1000 * 60 * 5);
    addChild(pDialogConnecting);
    connect(pDialogConnecting.get(), &DialogConnecting::sigCancel, this, &Multiplayermenu::slotCancelHostConnection, Qt::QueuedConnection);
    connect(this, &Multiplayermenu::sigHostGameLaunched, pDialogConnecting.get(), &DialogConnecting::connected, Qt::QueuedConnection);
    connect(pDialogConnecting.get(), &DialogConnecting::sigConnected, this, &Multiplayermenu::slotHostGameLaunched);
}

void Multiplayermenu::createChat()
{
    if (Settings::getSmallScreenDevice())
    {
        m_Chat = spChat::create(m_NetworkInterface,
                                QSize(Settings::getWidth() - 60, Settings::getHeight() - 90),
                                NetworkInterface::NetworkSerives::GameChat);
        m_Chat->setPosition(-m_Chat->getWidth() + 1, 10);
        auto moveButton = spMoveInButton::create(m_Chat.get(), m_Chat->getScaledWidth(), 1, -1, 1.0f);
        m_Chat->addChild(moveButton);
    }
    else
    {
        m_Chat = spChat::create(m_NetworkInterface,
                                QSize(Settings::getWidth() - 20, 300),
                                NetworkInterface::NetworkSerives::GameChat);
        m_Chat->setPosition(10, Settings::getHeight() - 360);
    }
    addChild(m_Chat);    
}

void Multiplayermenu::disconnectNetwork()
{    
    m_GameStartTimer.stop();
    if (m_NetworkInterface.get() != nullptr)
    {
        if (m_Chat.get())
        {
            m_Chat->detach();
            m_Chat = nullptr;
        }
        m_pPlayerSelection->attachNetworkInterface(spNetworkInterface());
        m_NetworkInterface = nullptr;
    }    
}

bool Multiplayermenu::getGameReady()
{
    bool gameReady = true;
    for (qint32 i = 0; i < m_pMapSelectionView->getCurrentMap()->getPlayerCount(); i++)
    {
        GameEnums::AiTypes aiType = m_pPlayerSelection->getPlayerAiType(i);
        if (aiType == GameEnums::AiTypes_Open)
        {
            CONSOLE_PRINT("Game not ready cause player " + QString::number(i) + " is open", Console::eDEBUG);
            gameReady = false;
            break;
        }
        else if (aiType == GameEnums::AiTypes_ProxyAi)
        {
            if (m_pPlayerSelection->getReady(i) == false)
            {
                CONSOLE_PRINT("Game not ready cause proxy player " + QString::number(i) + " is not ready", Console::eDEBUG);
                gameReady = false;
                break;
            }
        }
    }
    CONSOLE_PRINT("Game ready", Console::eDEBUG);
    return gameReady;
}

void Multiplayermenu::startGame()
{
    if (!m_Host)
    {
        markGameReady();
        changeButtonText();
    }
    else if (m_local)
    {
        if (getGameReady())
        {
            markGameReady();
            startCountdown();
        }
    }
    else
    {
        QString command = QString(NetworkCommands::STARTSERVERGAME);
        CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
        markGameReady();
        changeButtonText();
        QByteArray sendData;
        QDataStream sendStream(&sendData, QIODevice::WriteOnly);
        sendStream << command;
        emit m_NetworkInterface->sig_sendData(0, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
    }
}

void Multiplayermenu::markGameReady()
{
    QString command = QString(NetworkCommands::CLIENTREADY);
    CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
    m_pPlayerSelection->setPlayerReady(!m_pPlayerSelection->getPlayerReady());
    QByteArray sendData;
    QDataStream sendStream(&sendData, QIODevice::WriteOnly);
    sendStream << command;
    sendStream << m_pPlayerSelection->getPlayerReady();
    emit m_NetworkInterface->sig_sendData(0, sendData, NetworkInterface::NetworkSerives::Multiplayer, false);
}

void Multiplayermenu::changeButtonText()
{
    if (m_pPlayerSelection->getPlayerReady())
    {
        dynamic_cast<Label*>(m_pButtonStart->getFirstChild()->getFirstChild().get())->setHtmlText(tr("Not Ready"));
    }
    else
    {
        dynamic_cast<Label*>(m_pButtonStart->getFirstChild()->getFirstChild().get())->setHtmlText(tr("Ready"));
    }
}

void Multiplayermenu::startCountdown()
{
    m_counter = 5;
    // can we start the game?
    if (getGameReady())
    {
        if (!m_GameStartTimer.isActive())
        {
            CONSOLE_PRINT("Starting countdown", Console::eDEBUG);
            sendServerReady(true);
            m_GameStartTimer.setInterval(std::chrono::seconds(1));
            m_GameStartTimer.setSingleShot(false);
            m_GameStartTimer.start();
            emit m_Chat->sigSendText(QString::number(m_counter) + "...");
        }
    }
    else
    {
        CONSOLE_PRINT("Stoping countdown", Console::eDEBUG);
        m_GameStartTimer.stop();
        sendServerReady(false);
    }
}

void Multiplayermenu::sendServerReady(bool value)
{
    if (m_NetworkInterface->getIsServer())
    {
        if (value)
        {
            emit m_NetworkInterface.get()->sigPauseListening();
        }
        else
        {
            emit m_NetworkInterface.get()->sigContinueListening();
        }
        QVector<qint32> player;
        for (qint32 i = 0; i < m_pMapSelectionView->getCurrentMap()->getPlayerCount(); i++)
        {
            GameEnums::AiTypes aiType = m_pPlayerSelection->getPlayerAiType(i);
            if (aiType != GameEnums::AiTypes_Open &&
                aiType != GameEnums::AiTypes_ProxyAi)
            {
                player.append(i);
            }
        }
        CONSOLE_PRINT("Setting player ready information to local players with value "  + QString::number(value), Console::eDEBUG);
        m_pPlayerSelection->setPlayerReady(value);
        CONSOLE_PRINT("Sending ready information to all players with value " + QString::number(value), Console::eDEBUG);
        m_pPlayerSelection->sendPlayerReady(0, player, value);
    }
}

void Multiplayermenu::countdown()
{
    if (getGameReady())
    {
        m_counter--;
        if (m_Chat.get() != nullptr)
        {
            CONSOLE_PRINT("Sending game counter..." + QString::number(m_counter), Console::eDEBUG);
            emit m_Chat->sigSendText(QString::number(m_counter) + "...");
        }
        if (m_counter == 0 && m_NetworkInterface.get() != nullptr)
        {
            CONSOLE_PRINT("Starting game on server", Console::eDEBUG);
            defeatClosedPlayers();
            spGameMap pMap = GameMap::getInstance();
            pMap->setVisible(false);
            if (!m_saveGame)
            {
                pMap->initPlayersAndSelectCOs();
            }
            QString command = QString(NetworkCommands::INITGAME);
            CONSOLE_PRINT("Sending command " + command, Console::eDEBUG);
            QByteArray data;
            QDataStream stream(&data, QIODevice::WriteOnly);
            stream << command;
            quint32 seed = QRandomGenerator::global()->bounded(std::numeric_limits<quint32>::max());
            stream << seed;
            for (qint32 i = 0; i < m_pMapSelectionView->getCurrentMap()->getPlayerCount(); i++)
            {
                CONSOLE_PRINT("AI on server for player " + QString::number(i) + " is " + QString::number(pMap->getPlayer(i)->getBaseGameInput()->getAiType()), Console::eDEBUG);
                pMap->getPlayer(i)->serializeObject(stream);
            }
            GlobalUtils::seed(seed);
            GlobalUtils::setUseSeed(true);
            if (!m_saveGame)
            {
                pMap->getGameScript()->gameStart();
            }
            pMap->updateSprites(-1, -1, false, true);
            // start game
            CONSOLE_PRINT("Leaving Map Selection Menue", Console::eDEBUG);
            auto window = spGameMenue::create(m_saveGame, m_NetworkInterface);
            oxygine::Stage::getStage()->addChild(window);
            QThread::msleep(200);
            CONSOLE_PRINT("Sending init game to clients", Console::eDEBUG);
            emit m_NetworkInterface->sig_sendData(0, data, NetworkInterface::NetworkSerives::Multiplayer, false);
            oxygine::Actor::detach();
        }
    }
    else
    {
        m_counter = 5;
        m_GameStartTimer.stop();
        sendServerReady(false);
    }
}
