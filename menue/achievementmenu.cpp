#include "menue/achievementmenu.h"
#include "menue/mainwindow.h"

#include "coreengine/mainapp.h"
#include "coreengine/console.h"
#include "coreengine/audiothread.h"
#include "coreengine/userdata.h"

#include "resource_management/backgroundmanager.h"
#include "resource_management/objectmanager.h"
#include "resource_management/fontmanager.h"

#include "game/gamemap.h"

#include "wiki/wikidatabase.h"

#include "objects/base/label.h"
#include "objects/base/progressinfobar.h"

Achievementmenu::Achievementmenu()
    : Basemenu()
{
    setObjectName("Achievementmenu");
    Mainapp* pApp = Mainapp::getInstance();
    pApp->pauseRendering();
    moveToThread(pApp->getWorkerthread());
    CONSOLE_PRINT("Entering Achievement Menue", Console::eDEBUG);
    Interpreter::setCppOwnerShip(this);

    BackgroundManager* pBackgroundManager = BackgroundManager::getInstance();
    // load background
    oxygine::spSprite sprite = oxygine::spSprite::create();
    addChild(sprite);
    oxygine::ResAnim* pBackground = pBackgroundManager->getResAnim("achievementmenu");
    if (pBackground != nullptr)
    {
        sprite->setResAnim(pBackground);
        // background should be last to draw
        sprite->setPriority(static_cast<qint32>(Mainapp::ZOrder::Background));
        sprite->setScaleX(Settings::getWidth() / pBackground->getWidth());
        sprite->setScaleY(Settings::getHeight() / pBackground->getHeight());
    }
    pApp->getAudioThread()->clearPlayList();
    pApp->getAudioThread()->loadFolder("resources/music/credits_options");
    pApp->getAudioThread()->playRandom();


    oxygine::spButton pButtonExit = ObjectManager::createButton(tr("Exit"));
    addChild(pButtonExit);
    pButtonExit->setPosition(Settings::getWidth()  / 2.0f - pButtonExit->getWidth() / 2.0f,
                             Settings::getHeight() - pButtonExit->getHeight() - 10);
    pButtonExit->addEventListener(oxygine::TouchEvent::CLICK, [=](oxygine::Event * )->void
    {
        emit sigExitMenue();
    });
    connect(this, &Achievementmenu::sigExitMenue, this, &Achievementmenu::exitMenue, Qt::QueuedConnection);

    oxygine::TextStyle style = oxygine::TextStyle(FontManager::getMainFont24());
    style.color = FontManager::getFontColor();
    style.vAlign = oxygine::TextStyle::VALIGN_DEFAULT;
    style.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    style.multiline = false;

    qint32 y = 10;
    qint32 width = 150;
    spLabel pTextfield = spLabel::create(width - 10);
    pTextfield->setStyle(style);
    pTextfield->setHtmlText(tr("Search: "));
    pTextfield->setPosition(10, y);
    addChild(pTextfield);
    m_SearchString = spTextbox::create(Settings::getWidth() - 380);
    m_SearchString->setTooltipText(tr("Text that will be searched for in the title of each wikipage."));
    m_SearchString->setPosition(150, y);
    connect(m_SearchString.get(), &Textbox::sigTextChanged, this, &Achievementmenu::searchChanged, Qt::QueuedConnection);
    addChild(m_SearchString);
    oxygine::spButton pButton = ObjectManager::createButton(tr("Search"));
    addChild(pButton);
    pButton->setPosition(m_SearchString->getWidth() + m_SearchString->getX() + 10, y);
    pButton->addEventListener(oxygine::TouchEvent::CLICK, [=](oxygine::Event * )->void
    {
        emit sigSearch();
    });
    connect(this, &Achievementmenu::sigSearch, this, &Achievementmenu::search, Qt::QueuedConnection);
    y += 50;

    QSize size(Settings::getWidth() - 20, Settings::getHeight() - 150);
    m_MainPanel = spPanel::create(true, size, size);
    m_MainPanel->setPosition(10, 90);
    addChild(m_MainPanel);

    qint32 singleWidth = Settings::getWidth() - 80;
    Userdata* pUserdata = Userdata::getInstance();
    const auto achievements = pUserdata->getAchievements();
    qint32 achieveCount = 0;
    for (const auto & achievement : qAsConst(*achievements))
    {
        if (achievement.loaded)
        {
            if (achievement.progress >= achievement.targetValue)
            {
                achieveCount += 1;
            }
        }
    }
    pTextfield = spLabel::create(singleWidth);
    pTextfield->setStyle(style);
    pTextfield->setHtmlText(tr("Achievement Progress: ") + QString::number(achieveCount) + " / " + QString::number(achievements->length()));
    pTextfield->setPosition(10, 50);
    addChild(pTextfield);

    searchChanged("");
    pApp->continueRendering();
}

void Achievementmenu::onEnter()
{
    Interpreter* pInterpreter = Interpreter::getInstance();
    QString object = "Init";
    QString func = "achievementMenu";
    if (pInterpreter->exists(object, func))
    {
        CONSOLE_PRINT("Executing:" + object + "." + func, Console::eDEBUG);
        QJSValueList args;
        QJSValue value = pInterpreter->newQObject(this);
        args << value;
        pInterpreter->doFunction(object, func, args);
    }
}

void Achievementmenu::exitMenue()
{    
    CONSOLE_PRINT("Leaving Achievement Menue", Console::eDEBUG);
    auto window = spMainwindow::create();
    oxygine::Stage::getStage()->addChild(window);
    oxygine::Actor::detach();    
}

void Achievementmenu::search()
{
    searchChanged(m_SearchString->getCurrentText());
}

void Achievementmenu::searchChanged(QString text)
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->pauseRendering();
    CONSOLE_PRINT("Achievementmenu::searchChanged " + text, Console::eDEBUG);
    oxygine::TextStyle style = oxygine::TextStyle(FontManager::getMainFont24());
    style.color = FontManager::getFontColor();
    style.vAlign = oxygine::TextStyle::VALIGN_DEFAULT;
    style.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    style.multiline = false;

    oxygine::TextStyle styleLarge = oxygine::TextStyle(FontManager::getMainFont48());
    styleLarge.color = FontManager::getFontColor();
    styleLarge.vAlign = oxygine::TextStyle::VALIGN_DEFAULT;
    styleLarge.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    styleLarge.multiline = false;

    m_MainPanel->clearContent();
    text = text.toLower();

    Userdata* pUserdata = Userdata::getInstance();
    const auto achievements = pUserdata->getAchievements();
    qint32 x = 10;
    qint32 y = 10;
    qint32 singleWidth = Settings::getWidth() - 80;
    for (const auto & achievement : qAsConst(*achievements))
    {
        if (achievement.loaded)
        {
            bool achieved = achievement.progress >= achievement.targetValue;
            QString lowerName = achievement.name.toLower();
            QString lowerDescription = achievement.description.toLower();
            if (text.isEmpty() ||
                ((lowerName.contains(text) ||
                  lowerDescription.contains(text)) &&
                 (!achievement.hide || achieved)))
            {
                oxygine::spActor pParent = oxygine::spActor::create();
                if (achieved)
                {
                    WikiDatabase* pWikiDatabase = WikiDatabase::getInstance();
                    oxygine::spSprite pIcon = pWikiDatabase->getIcon(achievement.icon, GameMap::defaultImageSize * 2);
                    pIcon->setPosition(x, y + 16);
                    pParent->addChild(pIcon);
                }
                else
                {
                    spLabel pTextfield = spLabel::create(50);
                    pTextfield->setStyle(styleLarge);
                    pTextfield->setHtmlText("?");
                    pTextfield->setPosition(x, y + 8);
                    pParent->addChild(pTextfield);
                }

                spLabel pTextfield = spLabel::create(singleWidth - 60);
                pTextfield->setStyle(style);
                if (achievement.hide && !achieved)
                {
                    pTextfield->setHtmlText("?");
                }
                else
                {
                    pTextfield->setHtmlText(achievement.name);
                }
                pTextfield->setPosition(x + 60, y);
                pParent->addChild(pTextfield);

                pTextfield = spLabel::create(singleWidth - 60);
                pTextfield->setStyle(style);
                if (achievement.hide && !achieved)
                {
                    pTextfield->setHtmlText("?");
                }
                else
                {
                    pTextfield->setHtmlText(achievement.description);
                }
                pTextfield->setPosition(x + 60, y + 40);
                pParent->addChild(pTextfield);

                QString info = QString::number(achievement.progress) + " / " + QString::number(achievement.targetValue);
                spProgressInfoBar pProgressInfoBar = spProgressInfoBar::create(singleWidth, 32, info, static_cast<float>(achievement.progress) / static_cast<float>(achievement.targetValue));
                pProgressInfoBar->setPosition(x, y + 80);
                pParent->addChild(pProgressInfoBar);
                pParent->setSize(Settings::getWidth(), 120);
                m_MainPanel->addItem(pParent);
                y += 120;
            }
        }
    }
    m_MainPanel->setContentHeigth(y + 50);
    pApp->continueRendering();
}
