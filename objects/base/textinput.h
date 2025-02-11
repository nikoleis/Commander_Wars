#pragma once

#include "objects/base/tooltip.h"
#include <QObject>
#include <QLineEdit>
#include <QElapsedTimer>


class TextInput : public Tooltip
{
    Q_OBJECT
public:
    enum
    {
        BLINKFREQG = 250
    };
    explicit TextInput();
    virtual ~TextInput();

    QString getCurrentText() const;
    /**
     * @brief setCurrentText changes the text of this textbox
     * @param text
     */
    void setCurrentText(QString text);
    qint32 getCursorPosition() const;
    void setCursorPosition(qint32 position);

public slots:
    virtual void focusedLost() override;
private slots:
    void editFinished();
protected:
    virtual bool doHandleEvent(QEvent *event) override;
    virtual bool onEditFinished() = 0;
    virtual void focused() override;
    virtual void looseFocusInternal() override;
    /**
     * @brief getDrawText
     * @param text
     * @return the draw text in pseudo html format
     */
    QString getDrawText(QString text);
    virtual void inputMethodQuery(Qt::InputMethodQuery query, QVariant arg) override;
private:
    QLineEdit* m_lineEdit;
    bool m_LineEditFocused{false};
    QElapsedTimer m_toggle;
};

