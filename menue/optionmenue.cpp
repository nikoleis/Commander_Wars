#include "menue/optionmenue.h"
#include "menue/mainwindow.h"

#include "coreengine/mainapp.h"
#include "coreengine/console.h"
#include "coreengine/settings.h"

#include "resource_management/backgroundmanager.h"
#include "resource_management/objectmanager.h"
#include "resource_management/fontmanager.h"

#include "objects/checkbox.h"
#include "objects/slider.h"
#include "objects/dropdownmenu.h"

#include <QDir>
#include <QFileInfoList>
#include <QTextStream>
#include <QProcess>

OptionMenue::OptionMenue()
{
    Mainapp* pApp = Mainapp::getInstance();
    this->moveToThread(pApp->getWorkerthread());
    Console::print("Entering Option Menue", Console::eDEBUG);

    BackgroundManager* pBackgroundManager = BackgroundManager::getInstance();
    // load background
    oxygine::spSprite sprite = new oxygine::Sprite();
    addChild(sprite);
    oxygine::ResAnim* pBackground = pBackgroundManager->getResAnim("Background+1");
    sprite->setResAnim(pBackground);
    sprite->setPosition(0, 0);
    // background should be last to draw
    sprite->setPriority(static_cast<short>(Mainapp::ZOrder::Background));
    sprite->setScaleX(pApp->getSettings()->getWidth() / pBackground->getWidth());
    sprite->setScaleY(pApp->getSettings()->getHeight() / pBackground->getHeight());

    pApp->getAudioThread()->clearPlayList();
    pApp->getAudioThread()->loadFolder("resources/music/hauptmenue");
    pApp->getAudioThread()->playRandom();


    oxygine::spButton pButtonExit = ObjectManager::createButton(tr("Exit"));
    pButtonExit->attachTo(this);
    pButtonExit->setPosition(pApp->getSettings()->getWidth()  / 2.0f - pButtonExit->getWidth() / 2.0f,
                             pApp->getSettings()->getHeight() - pButtonExit->getHeight() - 10);
    pButtonExit->addEventListener(oxygine::TouchEvent::CLICK, [=](oxygine::Event * )->void
    {
        emit sigExitMenue();
    });
    connect(this, &OptionMenue::sigExitMenue, this, &OptionMenue::exitMenue, Qt::QueuedConnection);



    oxygine::spButton pButtonMods = ObjectManager::createButton(tr("Mods"));
    pButtonMods->attachTo(this);
    pButtonMods->setPosition(pApp->getSettings()->getWidth() - pButtonMods->getWidth() - 10, 10);
    pButtonMods->addEventListener(oxygine::TouchEvent::CLICK, [=](oxygine::Event * )->void
    {
        emit sigShowMods();
    });
    connect(this, &OptionMenue::sigShowMods, this, &OptionMenue::showMods, Qt::QueuedConnection);

    oxygine::spButton pButtonSettings = ObjectManager::createButton(tr("Settings"));
    pButtonSettings->attachTo(this);
    pButtonSettings->setPosition(10, 10);
    pButtonSettings->addEventListener(oxygine::TouchEvent::CLICK, [=](oxygine::Event * )->void
    {
        emit sigShowSettings();
    });
    connect(this, &OptionMenue::sigShowSettings, this, &OptionMenue::showSettings, Qt::QueuedConnection);
    connect(this, &OptionMenue::sigChangeScreenSize, this, &OptionMenue::changeScreenSize, Qt::QueuedConnection);

    QSize size(pApp->getSettings()->getWidth() - 20,
               pApp->getSettings()->getHeight() - (20 + pButtonMods->getHeight()) * 2);
    m_pOptions = new  Panel(true,  size, size);
    m_pOptions->setPosition(10, 20 + pButtonMods->getHeight());
    addChild(m_pOptions);

    showSettings();
}

void OptionMenue::exitMenue()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    // save changed settings :)
    pApp->getSettings()->saveSettings();
    if (restartNeeded)
    {
        restart();
    }
    else
    {
        Console::print("Leaving Option Menue", Console::eDEBUG);
        oxygine::getStage()->addChild(new Mainwindow());
        oxygine::Actor::detach();
    }
    pApp->continueThread();
}

void OptionMenue::changeScreenMode(qint32 mode)
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    switch (mode)
    {
        case 1:
        {
            SDL_SetWindowBordered(oxygine::core::getWindow(), SDL_bool::SDL_FALSE);
            SDL_SetWindowFullscreen(oxygine::core::getWindow(), 0);
            pApp->getSettings()->setFullscreen(false);
            pApp->getSettings()->setBorderless(true);
            break;
        }
        case 2:
        {
            SDL_SetWindowFullscreen(oxygine::core::getWindow(), SDL_WINDOW_FULLSCREEN);
            pApp->getSettings()->setFullscreen(true);
            pApp->getSettings()->setBorderless(true);
            break;
        }
        default:
        {
            SDL_SetWindowBordered(oxygine::core::getWindow(), SDL_bool::SDL_TRUE);
            SDL_SetWindowFullscreen(oxygine::core::getWindow(), 0);
            pApp->getSettings()->setFullscreen(false);
            pApp->getSettings()->setBorderless(false);
        }
    }
    pApp->continueThread();
}

void OptionMenue::changeScreenSize(qint32 width, qint32 heigth)
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    SDL_SetWindowSize(oxygine::core::getWindow(), width, heigth);
    pApp->getSettings()->setWidth(width);
    pApp->getSettings()->setHeight(heigth);
    pApp->getSettings()->saveSettings();
    Console::print("Leaving Option Menue", Console::eDEBUG);
    oxygine::getStage()->addChild(new OptionMenue());
    oxygine::Actor::detach();
    pApp->continueThread();
}

void OptionMenue::showSettings()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    m_pOptions->clearContent();
    AudioThread* pAudio = pApp->getAudioThread();
    Settings* pSettings = pApp->getSettings();
    oxygine::TextStyle style = FontManager::getMainFont();
    style.color = oxygine::Color(255, 255, 255, 255);
    style.vAlign = oxygine::TextStyle::VALIGN_DEFAULT;
    style.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    style.multiline = false;

    qint32 y = 10;
    // cache all supported display modes
    SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, nullptr};
    qint32 modes = SDL_GetNumDisplayModes(0);
    QVector<QSize> supportedSizes;
    for  (qint32 i = 0; i < modes; i++)
    {
        if (SDL_GetDisplayMode(0, i, &mode) == 0)
        {
            QSize newSize(mode.w, mode.h);
            if (!supportedSizes.contains(newSize) &&
                newSize.width() >= 1152 &&
                newSize.height() >= 864)
            {

                supportedSizes.append(newSize);
            }
        }
        else
        {
            Console::print(QString("SDL_GetDisplayMode failed: ") + QString(SDL_GetError()), Console::eLogLevels::eERROR);
        }
    }
    QVector<QString> displaySizes;
    qint32 currentDisplayMode = 0;
    for  (qint32 i = 0; i < supportedSizes.size(); i++)
    {
        if (supportedSizes[i].width() == pSettings->getWidth() &&
            supportedSizes[i].height() == pSettings->getHeight())
        {
            currentDisplayMode = i;
        }
        displaySizes.append(QString::number(supportedSizes[i].width()) + " x " + QString::number(supportedSizes[i].height()));
    }
    qint32 sliderOffset = 400;

    oxygine::spTextField pTextfield = new oxygine::TextField();
    pTextfield->setStyle(style);
    pTextfield->setText(tr("Screen Settings").toStdString().c_str());
    pTextfield->setPosition(10, y);
    m_pOptions->addItem(pTextfield);
    y += 40;


    pTextfield = new oxygine::TextField();
    pTextfield->setStyle(style);
    pTextfield->setText(tr("Screen Resolution: ").toStdString().c_str());
    pTextfield->setPosition(10, y);
    m_pOptions->addItem(pTextfield);
    spDropDownmenu pScreenResolution = new DropDownmenu(400, displaySizes);
    pScreenResolution->setPosition(sliderOffset - 130, y);
    pScreenResolution->setCurrentItem(currentDisplayMode);
    m_pOptions->addItem(pScreenResolution);
    connect(pScreenResolution.get(), &DropDownmenu::sigItemChanged, [=](qint32)
    {
        QStringList itemData = pScreenResolution->getCurrentItemText().split(" x ");
        emit sigChangeScreenSize(itemData[0].toInt(), itemData[1].toInt());
    });
    y += 40;

    pTextfield = new oxygine::TextField();
    pTextfield->setStyle(style);
    pTextfield->setText(tr("Screen Mode: ").toStdString().c_str());
    pTextfield->setPosition(10, y);
    m_pOptions->addItem(pTextfield);
    QVector<QString> items = {tr("Window"), tr("Bordered"), tr("Fullscreen")};
    spDropDownmenu pScreenModes = new DropDownmenu(400, items);
    pScreenModes->setPosition(sliderOffset - 130, y);
    m_pOptions->addItem(pScreenModes);
    connect(pScreenModes.get(), &DropDownmenu::sigItemChanged, this, &OptionMenue::changeScreenMode, Qt::QueuedConnection);
    y += 40;

    pTextfield = new oxygine::TextField();
    pTextfield->setStyle(style);
    pTextfield->setText(tr("Audio Settings").toStdString().c_str());
    pTextfield->setPosition(10, y);
    m_pOptions->addItem(pTextfield);
    y += 40;
    pTextfield = new oxygine::TextField();
    pTextfield->setStyle(style);
    pTextfield->setText(tr("Global Volume: ").toStdString().c_str());
    pTextfield->setPosition(10, y);
    m_pOptions->addItem(pTextfield);
    spSlider pSlider = new Slider(pApp->getSettings()->getWidth() - 20 - sliderOffset, 0, 100);
    pSlider->setPosition(sliderOffset - 130, y);
    pSlider->setCurrentValue(pSettings->getTotalVolume());
    connect(pSlider.get(), &Slider::sliderValueChanged, [=](qint32 value)
    {
        pSettings->setTotalVolume(value);
        pAudio->setVolume(pSettings->getMusicVolume());
    });
    m_pOptions->addItem(pSlider);

    y += 40;
    pTextfield = new oxygine::TextField();
    pTextfield->setStyle(style);
    pTextfield->setText(tr("Music Volume: ").toStdString().c_str());
    pTextfield->setPosition(10, y);
    m_pOptions->addItem(pTextfield);
    pSlider = new Slider(pApp->getSettings()->getWidth() - 20 - sliderOffset, 0, 100);
    pSlider->setPosition(sliderOffset - 130, y);
    pSlider->setCurrentValue(pSettings->getMusicVolume());
    connect(pSlider.get(), &Slider::sliderValueChanged, [=](qint32 value)
    {
        pSettings->setMusicVolume(value);
        pAudio->setVolume(value);
    });
    m_pOptions->addItem(pSlider);

    y += 40;
    pTextfield = new oxygine::TextField();
    pTextfield->setStyle(style);
    pTextfield->setText(tr("Sound Volume: ").toStdString().c_str());
    pTextfield->setPosition(10, y);
    m_pOptions->addItem(pTextfield);
    pSlider = new Slider(pApp->getSettings()->getWidth() - 20 - sliderOffset, 0, 100);
    pSlider->setPosition(sliderOffset - 130, y);
    pSlider->setCurrentValue(pSettings->getSoundVolume());
    connect(pSlider.get(), &Slider::sliderValueChanged, [=](qint32 value)
    {
        pSettings->setSoundVolume(value);
    });
    m_pOptions->addItem(pSlider);
    pApp->continueThread();
}

void OptionMenue::showMods()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    m_pOptions->clearContent();
    QFileInfoList infoList = QDir("mods").entryInfoList(QDir::Dirs);

    oxygine::TextStyle style = FontManager::getMainFont();
    style.color = oxygine::Color(255, 255, 255, 255);
    style.vAlign = oxygine::TextStyle::VALIGN_DEFAULT;
    style.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    style.multiline = false;

    qint32 width = 0;
    qint32 mods = 0;
    Settings* pSettings = Mainapp::getInstance()->getSettings();
    QStringList currentMods = pSettings->getMods();

    for (qint32 i = 0; i < infoList.size(); i++)
    {
        QString folder = infoList[i].filePath();
        if (!folder.endsWith("."))
        {
            QString name = folder;
            QFile file(folder + "/mod.txt");
            if (file.exists())
            {
                file.open(QFile::ReadOnly);
                QTextStream stream(&file);
                while (!stream.atEnd())
                {
                    QString line = stream.readLine();
                    if (line.startsWith("name="))
                    {
                        name = line.split("=")[1];
                    }
                }
            }
            oxygine::spTextField pTextfield = new oxygine::TextField();
            pTextfield->setStyle(style);
            pTextfield->setText(name.toStdString().c_str());
            pTextfield->setPosition(10, 10 + mods * 40);
            m_pOptions->addItem(pTextfield);
            qint32 curWidth = pTextfield->getTextRect().getWidth() + 30;
            spCheckbox modCheck = new Checkbox();
            modCheck->setPosition(curWidth, pTextfield->getY());
            m_pOptions->addItem(modCheck);
            curWidth += modCheck->getWidth() + 10;
            if (currentMods.contains(folder))
            {
                modCheck->setChecked(true);
            }
            connect(modCheck.get(), &Checkbox::checkChanged, [=](bool checked)
            {
                if (checked)
                {
                    pSettings->addMod(folder);
                }
                else
                {
                    pSettings->removeMod(folder);
                }
                restartNeeded = true;
            });
            mods++;
            if (curWidth > width)
            {
                width = curWidth;
            }
        }
    }
    m_pOptions->setContentWidth(width);
    m_pOptions->setContentHeigth(20 + mods * 40);
    pApp->continueThread();
}

void OptionMenue::restart()
{
    QCoreApplication::exit(1);
}
