#ifndef ACHIEVEMENTMENU_H
#define ACHIEVEMENTMENU_H


#include "3rd_party/oxygine-framework/oxygine-framework.h"
#include "menue/basemenu.h"

#include "objects/base/panel.h"
#include "objects/base/textbox.h"

class Achievementmenu;
using spAchievementmenu = oxygine::intrusive_ptr<Achievementmenu>;

class Achievementmenu : public Basemenu
{
    Q_OBJECT
public:
    explicit Achievementmenu();
    virtual ~Achievementmenu() = default;
signals:
    void sigExitMenue();
    void sigSearch();
public slots:
    void exitMenue();
    void search();
    void searchChanged(QString);
    virtual void onEnter() override;
private:
    spPanel m_MainPanel;
    spTextbox m_SearchString;
};

#endif // ACHIEVEMENTMENU_H
