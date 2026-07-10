#ifndef RANKDIALOG_H
#define RANKDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "rankmanager.h"

class RankDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RankDialog(QWidget *parent = 0);
    ~RankDialog();

private:
    void setupUI();
    void loadData();

    QTableWidget *m_table;
    QLabel       *m_emptyLabel;
    QPushButton  *m_closeButton;

    RankManager m_rankManager;
};

#endif // RANKDIALOG_H
