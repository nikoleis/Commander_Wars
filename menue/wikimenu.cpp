#include "menue/wikimenu.h"
#include "menue/mainwindow.h"

#include "resource_management/backgroundmanager.h"
#include "resource_management/objectmanager.h"
#include "resource_management/fontmanager.h"

#include "coreengine/mainapp.h"
#include "coreengine/console.h"
#include "coreengine/settings.h"
#include "coreengine/audiothread.h"

Wikimenu::Wikimenu()
    : Basemenu()
{
    setObjectName("Wikimenu");
    Mainapp* pApp = Mainapp::getInstance();
    pApp->pauseRendering();
    Interpreter::setCppOwnerShip(this);
    moveToThread(pApp->getWorkerthread());
    CONSOLE_PRINT("Entering Wiki Menue", Console::eDEBUG);

    BackgroundManager* pBackgroundManager = BackgroundManager::getInstance();
    // load background
    oxygine::spSprite sprite = oxygine::spSprite::create();
    addChild(sprite);
    oxygine::ResAnim* pBackground = pBackgroundManager->getResAnim("wikimenu");
    if (pBackground != nullptr)
    {
        sprite->setResAnim(pBackground);
        // background should be last to draw
        sprite->setPriority(static_cast<qint32>(Mainapp::ZOrder::Background));
        sprite->setScaleX(Settings::getWidth() / pBackground->getWidth());
        sprite->setScaleY(Settings::getHeight() / pBackground->getHeight());
    }

    pApp->getAudioThread()->clearPlayList();
    pApp->getAudioThread()->loadFolder("resources/music/hauptmenue");
    pApp->getAudioThread()->playRandom();

    oxygine::spButton pButtonExit = ObjectManager::createButton(tr("Exit"));
    addChild(pButtonExit);
    pButtonExit->setPosition(Settings::getWidth()  / 2.0f - pButtonExit->getWidth() / 2.0f,
                             Settings::getHeight() - pButtonExit->getHeight() - 10);
    pButtonExit->addEventListener(oxygine::TouchEvent::CLICK, [=](oxygine::Event * )->void
    {
        emit sigExitMenue();
    });
    m_pWikiView = spWikiView::create(Settings::getWidth(), Settings::getHeight());
    addChild(m_pWikiView);
    connect(this, &Wikimenu::sigExitMenue, this, &Wikimenu::exitMenue, Qt::QueuedConnection);
    pApp->continueRendering();
}

void Wikimenu::onEnter()
{
    Interpreter* pInterpreter = Interpreter::getInstance();
    QString object = "Init";
    QString func = "wikiMenu";
    if (pInterpreter->exists(object, func))
    {
        CONSOLE_PRINT("Executing:" + object + "." + func, Console::eDEBUG);
        QJSValueList args;
        QJSValue value = pInterpreter->newQObject(this);
        args << value;
        pInterpreter->doFunction(object, func, args);
    }
}

void Wikimenu::exitMenue()
{
    CONSOLE_PRINT("Leaving Wiki Menue", Console::eDEBUG);
    auto window = spMainwindow::create();
    oxygine::Stage::getStage()->addChild(window);
    oxygine::Actor::detach();    
}
