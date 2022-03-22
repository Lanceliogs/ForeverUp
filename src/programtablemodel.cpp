#include "programtablemodel.h"

ProgramTableModel::ProgramTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_headers << tr("#") << tr("Name") << tr("Enabled") << tr("Status") << tr("Description");
}

QVariant ProgramTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    if (index.row() >= m_data.size()) return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column())
        {
        case 0: return QString::number(index.row() + 1);
        case 1: return m_data.at(index.row()).name;
        case 2: return m_data.at(index.row()).enabled;
        case 3: switch (m_data.at(index.row()).status) {
            case ProgramItem::Stopped: return tr("Stopped");
            case ProgramItem::Running: return tr("Running");
            }
        case 4: return m_data.at(index.row()).description;
        }
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case 0: return Qt::AlignCenter;
        case 1: return Qt::AlignCenter;
        case 2: return Qt::AlignCenter;
        case 3: return Qt::AlignCenter;
        default: return Qt::AlignVCenter;
        }
    }

    return QVariant();
}

bool ProgramTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        switch (index.column())
        {
        case 0: m_data[index.row()].index = value.toInt(); break;
        case 1: m_data[index.row()].name = value.toString(); break;
        case 2: m_data[index.row()].enabled = value.toBool(); break;
        case 3: m_data[index.row()].status = value.toInt(); break;
        case 4: m_data[index.row()].description = value.toString(); break;
        }
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool ProgramTableModel::insertRow(int row, const QModelIndex &parent)
{
    if (row > rowCount()) return false;

    beginInsertRows(parent, row, row);
    m_data.insert(row, ProgramItem());
    endInsertRows();
    return true;
}

bool ProgramTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row > rowCount()) return false;

    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0 ; i<count ; i++)
        m_data.insert(row, ProgramItem());
    endInsertRows();
    return true;
}

bool ProgramTableModel::removeRow(int row, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row);
    m_data.removeAt(row);
    endRemoveRows();
    return true;
}

bool ProgramTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    m_data.erase(m_data.begin() + row, m_data.begin() + row + count);
    endRemoveRows();
    return true;
}

void ProgramTableModel::clear()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}

int ProgramTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_data.size();
}

int ProgramTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 5;
}

QVariant ProgramTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation == Qt::Horizontal) return m_headers.at(section);
    else return QString::number(section + 1);
}

Qt::ItemFlags ProgramTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return NULL;
    return QAbstractTableModel::flags(index);
}

QList<ProgramItem> &ProgramTableModel::data()
{
    return m_data;
}
