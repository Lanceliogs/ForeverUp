#ifndef ProgramTableModel_H
#define ProgramTableModel_H

#include <QAbstractTableModel>
#include <QString>
#include <QDateTime>

#ifndef __PROGRAMITEM__
#define __PROGRAMITEM__
struct ProgramItem
{
    enum Status {Stopped, Running};

    int index= 0;
    QString name;
    QString description;
    bool enabled = false;
    int status = Stopped;
    int pid = 0;

    int hangingCount = 0;
    int maxHangingCount = 5;

    int restartCount = 0;
    QDateTime lastRestartDateTime = QDateTime();

    bool restartError = false;

    QString program;
    QString arguments;
    QString startIn;
    int checkInterval;
    int memoryLimit;
    QString logFile;
    QString scriptBeforeRestart;
};
#endif

class ProgramTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ProgramTableModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void clear();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QList<ProgramItem> &data();

signals:

public slots:

private:
    QList<ProgramItem> m_data;
    QStringList m_headers;
};

#endif // ProgramTableModel_H
