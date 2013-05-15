#include <MApplication>
#include <MApplicationService>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <QDebug>

int main(int argc, char ** argv)
{
    QString servicename(QString("com.nokia.fali_multiapp") + QString(argv[1]));
    QString title(QString("App ")+ QString(argv[1])); 
    MApplicationService *mService = new MApplicationService(servicename, 0);
    MApplication app(argc, argv, mService);
    MApplication::setPrestartMode(M::LazyShutdown);
    MApplicationWindow window;
    MApplicationPage mainPage;
    window.show();
    mainPage.setTitle(title);
    mainPage.appear();
  
    return app.exec();
}
