#include "multiwindowservice.h"
#include <MDebug>
#include <MApplicationService>
#include <MApplicationIfAdaptor>
#include "mainpage.h"
#include "mywindow.h"

MultiWindowService::MultiWindowService()
    : QObject(0), 
    mApp(MApplication::instance())
{
    mDebug("MultiWindowService") << "MultiWindowService created.";

    QDBusConnection connection = QDBusConnection::sessionBus();
    QString serviceName("com.nokia.MultiWindow3");

    bool ret = connection.registerService(serviceName);
    mDebug("MultiWindowService") << "registerService:" << serviceName << ret;
    ret = connection.registerObject("/", this);
    mDebug("MultiWindowService") << "registerObject: /: " << ret;

    // Windows and pages could be handled in another class like
    // "MyMultiWindowManager" These are inside that file just to
    // make example more simple.
    m_windows[0] = MApplicationWindowPtr(new MApplicationWindow());
    m_windows[1] = MApplicationWindowPtr(new MApplicationWindow());
    m_windows[2] = MApplicationWindowPtr(new MyWindow());
    m_windows[2]->setCloseOnLazyShutdown(true);

    for (int i = 0; i < 3; ++i)
    {
        QString name = QString("Window %1").arg(i+1);

        m_windows[i]->setWindowTitle(name);
        m_windows[i]->setObjectName(name);

        m_pages[i] = MainPagePtr(new MainPage(name));
        m_pages[i]->appear(m_windows[i].data());
    }
}

MultiWindowService::~MultiWindowService()
{
}

void MultiWindowService::activateWidgets() 
{
    mDebug("MultiWindowService") << "Prestart released";
}

void MultiWindowService::deactivateWidgets() 
{
    mDebug("MultiWindowService") << "Prestart restored";

    for (int i = 0; i < 3; ++i)
        m_pages[i]->deactivateWidgets();
}

void MultiWindowService::activateWindow(MApplicationWindow *window, MainPage *page)
{
    Q_UNUSED(page)

    // Force release prestart - we should add a new method to
    // libmeegotouch for this
    if(mApp->isPrestarted()) {
        mDebug("MultiWindowService") << "Force release prestart.";
        mApp->setPrestarted(false);
    }    
 
    window->show();
    window->activateWindow();
    window->raise();
}

void MultiWindowService::launchWindow(int windowId)
{
    mDebug("MultiWindowService") << "launchWindow "<< windowId;

    if (windowId < 1 || windowId > 3)
    {
        mDebug("MultiWindowService") << "WindowId " << windowId
                                     << " not supported.";
    }
    else
    {
        --windowId;
        activateWindow(m_windows[windowId].data(), m_pages[windowId].data());
    }
}
