#include <MApplication>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MComboBox>
#include <MDebug>
#include <QStringList>

class MyWindow : public MApplicationWindow
{
public:
   MyWindow();
   void createPage();

protected:
    //! \reimp
    virtual void closeEvent(QCloseEvent *event);

private:
    MComboBox *comboBox;
};


MyWindow::MyWindow() 
{
}

void MyWindow::createPage()
{
    MApplicationPage *page=new MApplicationPage();
    page->setTitle("close-event");

    comboBox = new MComboBox();
    comboBox->setTitle("What should closeEvent do?");
    QStringList stringList;
    stringList << "Ignore closeEvent" << "Hide window (Lazy Shutdown)" << "Really close window";
    comboBox->addItems(stringList);
    page->setCentralWidget(comboBox);
    page->appear();
}

void MyWindow::closeEvent(QCloseEvent *event)
{
    switch (comboBox->currentIndex()) {
    case 0: 
        mDebug("MyWindow") << "Ignore closeEvent(). Window is not closed.";
        event->ignore(); 
        break;
    case 1: 
        mDebug("MyWindow") << "Lazy shutdown - hide window";
        setCloseOnLazyShutdown(false);
        event->accept(); 
        break;
    case 2:
    default: 
        mDebug("MyWindow") << "Really close window";
        setCloseOnLazyShutdown(true);
        event->accept(); 
        break;
    }
}

int main(int argc, char **argv)
{
    MApplication app(argc, argv);
    MApplication::setPrestartMode(M::LazyShutdown);

    MyWindow *window = new MyWindow();
    window->show();
    window->createPage();
 
    return app.exec();
}
