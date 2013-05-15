#ifndef MULTIWINDOWSERVICE_H
#define MULTIWINDOWSERVICE_H

#include <QObject>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MApplication>
#include <QSharedPointer>
#include "mainpage.h"

class MultiWindowService : public QObject
{
    Q_OBJECT
public:
    MultiWindowService();
    virtual ~MultiWindowService();

public Q_SLOTS:
    void launchWindow(int windowId);
    void activateWidgets();
    void deactivateWidgets();

private:
    void activateWindow(MApplicationWindow *window, MainPage *page);
    MApplication *mApp;

    typedef QSharedPointer<MApplicationWindow> MApplicationWindowPtr;
    typedef QSharedPointer<MainPage> MainPagePtr;

    MApplicationWindowPtr m_windows[3];
    MainPagePtr m_pages[3];
};
#endif


