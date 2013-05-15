#include <MApplication>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MButton>
#include <MDebug>

int main(int argc, char **argv)
{
    MApplication app(argc, argv);
    MApplication::setPrestartMode(M::LazyShutdown);

    MApplicationWindow *window = new MApplicationWindow; 
    MApplicationPage *page = new MApplicationPage();
    page->setTitle("fali_close");

    MButton *button = new MButton("close");
    QObject::connect(button, SIGNAL(clicked()), window, SLOT(close()));
    page->setCentralWidget(button);
    page->appear();
    window->show();
 
    return app.exec();
}
