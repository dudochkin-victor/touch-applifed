#include "mywindow.h"
#include <MMessageBox>
#include <QCloseEvent>
#include <MDebug>

MyWindow::MyWindow() :
    messageBox(0) 
{
}

void MyWindow::closeEvent(QCloseEvent *event)
{
    MMessageBox *messageBox = new MMessageBox("Do you really want to exit?", (M::YesButton | M::NoButton));
 
    int retValue = messageBox->exec();

    mDebug("MyWindow") << "Message Box:" << retValue;

    // Temporary solution to prevent applications crashing when answering 
    // to dialog with a very short tap (TDriver test tool will do this).
    //    delete messageBox;

    // Return event->accept() and the window is closed / hidden
    if (retValue == M::YesButton) event->accept();

    // Return event->ignore() and the window's closing is terminated
    // and the window is not closed.
    if (retValue == M::NoButton) event->ignore();
}

