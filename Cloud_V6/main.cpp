#include <QCoreApplication>
#include "custdevicemanagment.h"
#include <iostream>
using namespace std;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CUSTDeviceManagment dm;
    if(dm.Open_Device())
        dm.Start_Thread();
    return a.exec();
}
