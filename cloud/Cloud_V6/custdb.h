#ifndef CUSTDB_H
#define CUSTDB_H

#include "custdefine.h"
#include "custdb.h"
class CUSTDB : public QObject
{
    Q_OBJECT
public:
    explicit CUSTDB(QObject *parent = 0);
    ~CUSTDB();
    QSqlDatabase DB_QB;
    QSqlQuery *query;
    QSqlQueryModel *model;
    bool InitDatabase();
    bool InsertDataDB(QString txt,int Size);
    bool SelectDataDB();
    bool UpdateDataDB(int id);
    bool DeleteDataDB();

    bool InsertPicDB(QString q_curtime,double q_long,double q_latt,
                     float q_outtem,float q_intem,float q_outhum,float q_inhum,double q_viscov,
                     double q_ircov,double q_cov,QString q_vis_path,QString q_ir_path);
    bool SelectPicDataDB();
    bool UpdatePicDataDB(int id);

signals:

public slots:
    //void UpDateQTB(/*QString cardid, */QString wordid, int area, QString endtime);
};
#endif // CUSTDB_H
