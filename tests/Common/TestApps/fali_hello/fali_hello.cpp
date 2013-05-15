#include <MApplication>
#include <MApplicationService>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <QDebug>
#include <QString>
#include <QFile>
#include <sys/time.h>
#include <vector>


void FANGORNLOG(const char* s)
{
    QFile f("/tmp/applifed_perftest.log");
    f.open(QIODevice::Append);
    f.write(s, qstrlen(s));
    f.write("\n", 1);
    f.close();
}

void timestamp(const char *s)
{
    timeval tim;
    char msg[80];
    gettimeofday(&tim, NULL);
    snprintf(msg, 80, "%d.%06d %s\n",
    static_cast<int>(tim.tv_sec), static_cast<int>(tim.tv_usec), s);
    FANGORNLOG(msg);
}


class MyApplicationWindow: public MApplicationWindow
{
public:
    MyApplicationWindow() :
        MApplicationWindow()
    {
    }

    virtual ~MyApplicationWindow()
    {
    }

    void enterDisplayEvent()
    {
        timestamp("MyApplicationWindow::enterDisplayEvent()");
    }
};

int main(int argc, char ** argv)
{
    timestamp("main()");
    QString appName(argv[0]);

    MApplication app(argc, argv);
    MApplication::setPrestartMode(M::LazyShutdown);

    MyApplicationWindow window;
    MApplicationPage mainPage;
    window.show();
    if (appName.endsWith("fali_hello"))
    {
        mainPage.setTitle("fali_hello");
    }
    else if (appName.endsWith("fali_perf"))
    {
        mainPage.setTitle("fali_perf");
    }
    mainPage.appear();
  
    return app.exec();
}
