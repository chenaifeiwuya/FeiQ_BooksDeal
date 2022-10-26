#include "feiqdialog.h"
#include"form.h"
#include <QApplication>
#include"ckernel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



 //   FeiQDialog w;
  //  w.show();

    CKernel kernel;


    return a.exec();
}
