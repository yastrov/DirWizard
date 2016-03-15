#include "filelisttablemodel.h"
#include <QApplication>
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif

namespace {
const int MaxColumns = 3;

QString getFileName(const QString &path)
{
#ifdef NATIVE_PATH_SEP
    QString fname = QDir::toNativeSeparators(path);
    return  fname.section(QDir::separator(), -1);
#else
    return  path.section('/', -1);
#endif
}

}

int FileListTableModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : items->count();
}


int FileListTableModel::columnCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : MaxColumns;
}

QVariant FileListTableModel::headerData(int section,
                                        Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case Column::fileName: return tr("FileName"); break;
        case Column::hash: return tr("Hash");  break;
        case Column::size: return tr("Size"); break;
        default: Q_ASSERT(false);
        }
    }
    return section + 1;
}

QVariant FileListTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() ||
            index.row() < 0 || index.row() >= items->count() ||
            index.column() < 0 || index.column() >= MaxColumns)
        return QVariant();
    const HashFileInfoStruct &item = items->at(index.row());
    switch(role) {
    case Qt::SizeHintRole: {
        QStyleOptionComboBox option;
        switch (index.column()) {
        case Column::fileName: option.currentText = getFileName(item.fileName); break;
        case Column::hash: option.currentText = item.hash; break;
        case Column::size: option.currentText = QString::number(item.size); break;
        default: Q_ASSERT(false);
        }
        QFontMetrics fontMetrics(data(index, Qt::FontRole)
                                 .value<QFont>());
        option.fontMetrics = fontMetrics;
        QSize size(fontMetrics.width(option.currentText),
                   fontMetrics.height());
        return qApp->style()->sizeFromContents(QStyle::CT_ComboBox,
                                               &option, size);
    }
    case Qt::DisplayRole: {
        switch (index.column()) {
        case Column::fileName: return getFileName(item.fileName);
        case Column::hash: return item.hash;
        case Column::size: return item.size;
        default: Q_ASSERT(false);
        }
    }
    case Qt::EditRole: {
        return QVariant();
    }
    case Qt::ToolTipRole: {
        switch (index.column()) {
        case Column::fileName: return item.fileName;
        case Column::hash: return item.hash;
        case Column::size: return item.size;
        default: Q_ASSERT(false);
        }
    }
    default: return QVariant();
    }
    return QVariant();
}

bool FileListTableModel::setData(const QModelIndex &index,
                                 const QVariant &value, int role)
{
    Q_UNUSED(value)
    Q_UNUSED(role)
    if (!index.isValid() ||
            index.row() < 0 || index.row() >= items->count() ||
            index.column() < 0 || index.column() > MaxColumns)
        return false;
    return false;
}

void FileListTableModel::sort(int column, Qt::SortOrder order)
{
#ifdef MYPREFIX_DEBUG
    if(order == Qt::AscendingOrder) {
        qDebug()<< "FileListTableModel::sort Qt::AscendingOrder";
    } else {
        qDebug()<< "FileListTableModel::sort Qt::DescendingOrder";
    }
#endif

    switch(column) {
    case Column::fileName:
        std::sort(items->begin(), items->end(), [](const HashFileInfoStruct &v1, const HashFileInfoStruct &v2)->bool {
            return v1.fileName > v2.fileName;
        });
        break;
    case Column::hash:
        std::sort(items->begin(), items->end(), [](const HashFileInfoStruct &v1, const HashFileInfoStruct &v2)->bool {
            return v1.hash > v2.hash;
        });
        break;
    case Column::size:
        std::sort(items->begin(), items->end(), [](const HashFileInfoStruct &v1, const HashFileInfoStruct &v2)->bool {
            return v1.size > v2.size;
        });
        break;
    default: return;
    }
    if(order != Qt::AscendingOrder)
        std::reverse(items->begin(), items->end());
    emit dataChanged(QModelIndex(), QModelIndex());
}

