#include <MApplication>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MApplicationService>

int main(int argc, char ** argv)
{
    MApplication app(argc, argv, "fali_toc", new MApplicationService ("com.nokia.fali_toc") );
    MApplicationPage mainPage;
    MApplicationWindow window;

    MApplication::setPrestartMode(M::TerminateOnClose);

    sleep(4);

    window.show();

    mainPage.setTitle("fali_toc");
    mainPage.appear();
  
    return app.exec();
}
