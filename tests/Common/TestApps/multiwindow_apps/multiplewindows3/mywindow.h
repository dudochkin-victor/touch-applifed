#ifndef MYWINDOW_H
#define MYWINDOW_H

#include "mapplicationwindow.h"

class MMessageBox;

class MyWindow : public MApplicationWindow
{
Q_OBJECT

public:
   MyWindow();

protected:
    //! \reimp
    virtual void closeEvent(QCloseEvent *event);

private:
    MMessageBox *messageBox;

};

#endif
