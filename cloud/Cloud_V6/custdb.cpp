#include "custdb.h"

CUSTDB::CUSTDB(QObject *parent) : QObject(parent)
{
    query = NULL;
    model = NULL;
}

CUSTDB::~CUSTDB()
{
    if (query )
        delete query;
    if(model)
        delete model;
    query = NULL;
    model = NULL;
}

/*
 * create a database and two tables in it.
 */
bool CUSTDB::InitDatabase()
{
    if (QSqlDatabase::contains("DataBase"))
        DB_QB =  QSqlDatabase::database("DataBase");
    else
    {
        DB_QB = QSqlDatabase::addDatabase("QSQLITE");
        DB_QB.setDatabaseName("DataBase.db");
        DB_QB.setUserName("root");
        DB_QB.setPassword("mvr123456");
        DB_QB.open();
        query = new QSqlQuery(DB_QB);
        bool rst = query->exec("CREATE TABLE IF NOT EXISTS deviceinfo ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "curtime  VARCHAR(200) NULL, "
                               "longitude REAL NULL, "
                               "lattitude REAL NULL, "
                               "outtem  FLOAT NULL, "
                               "intem  FLOAT NULL, "
                               "outhum  FLOAT NULL, "
                               "inhum  FLOAT NULL, "
                               "viscov REAL NULL, "
                               "ircov REAL NULL, "
                               "cov REAL NULL, "
                               "vispath  VARCHAR(200) NULL, "
                               "irpath  VARCHAR(200) NULL, "
                               "status integer NULL)"); // status 0: waiting ,1: send ok.
        if (rst == false)
            return false;
    }
    model = new QSqlQueryModel();
    return true;
}

bool CUSTDB::InsertDataDB(QString txt,int Size)
{
    query->prepare("INSERT INTO sendbuff (send_data, data_size, status) VALUES (:send_data, :data_size, :status)");
    query->bindValue(":send_data", txt);
    query->bindValue(":data_size", Size);
    query->bindValue(":status", 0);
    query->exec();
    return true;
}

bool CUSTDB::SelectDataDB()
{
    query->prepare("SELECT id, send_data,data_size FROM sendbuff WHERE status = 0");
    query->exec();
    return true;
}

bool CUSTDB::UpdateDataDB(int id)
{
    query->prepare("UPDATE sendbuff SET status = :status WHERE id = :id");
    query->bindValue(":id", id);
    query->bindValue(":status", 1);
    query->exec();
    return true;
}

bool CUSTDB::DeleteDataDB()
{
    query->prepare("DELETE FROM sendbuff WHERE status = 1");
    query->exec();
    return true;
}

bool CUSTDB::InsertPicDB(QString q_curtime,double q_long,double q_latt,
                 float q_outtem,float q_intem,float q_outhum,float q_inhum,double q_viscov,
                 double q_ircov,double q_cov,QString q_vis_path,QString q_ir_path)
{
    query->prepare("INSERT INTO deviceinfo (curtime,longitude,lattitude,outtem,intem,outhum,inhum,viscov,ircov,cov,vispath,irpath,status) "
                   "VALUES (:curtime, :longitude, :lattitude, :outtem, :intem, :outhum, :inhum, :viscov, :ircov, :cov, :vispath, :irpath, :status)");

    query->bindValue(":curtime", q_curtime);
    query->bindValue(":longitude", q_long);
    query->bindValue(":lattitude", q_latt);
    query->bindValue(":outtem", q_outtem);
    query->bindValue(":intem", q_intem);
    query->bindValue(":outhum", q_outhum);
    query->bindValue(":inhum", q_inhum);
    query->bindValue(":viscov", q_viscov);
    query->bindValue(":ircov", q_ircov);
    query->bindValue(":cov", q_cov);
    query->bindValue(":vispath", q_vis_path);
    query->bindValue(":irpath", q_ir_path);
    query->bindValue(":status", 0);
    query->exec();
    return true;
}

bool CUSTDB::SelectPicDataDB()
{
    query->prepare("SELECT * FROM deviceinfo WHERE status = 0");
    query->exec();
    return true;
}
bool CUSTDB::UpdatePicDataDB(int id)
{
    query->prepare("UPDATE deviceinfo SET status = :status WHERE id = :id");
    query->bindValue(":id", id);
    query->bindValue(":status", 1);
    query->exec();
    return true;
}
