#include "rankdialog.h"
#include <QHeaderView>
#include <QDialogButtonBox>

RankDialog::RankDialog(QWidget *parent)
    : QDialog(parent)
    , m_table(0)
    , m_emptyLabel(0)
    , m_closeButton(0)
{
    setupUI();
    loadData();
}

RankDialog::~RankDialog()
{
}

void RankDialog::setupUI()
{
    setWindowTitle(QStringLiteral("排行榜"));
    setFixedSize(400, 420);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 表格
    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels(
        QStringList() << QStringLiteral("排名")
                      << QStringLiteral("玩家昵称")
                      << QStringLiteral("历史最高分"));
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    mainLayout->addWidget(m_table);

    // 空数据提示
    m_emptyLabel = new QLabel(QStringLiteral("暂无排行榜记录"), this);
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setStyleSheet(QStringLiteral("QLabel { color: gray; font-size: 18px; }"));
    m_emptyLabel->setVisible(false);
    mainLayout->addWidget(m_emptyLabel);

    // 关闭按钮
    m_closeButton = new QPushButton(QStringLiteral("关闭"), this);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(m_closeButton);
}

void RankDialog::loadData()
{
    QList<RankRecord> records = m_rankManager.loadRecords();

    if (records.isEmpty())
    {
        m_table->setVisible(false);
        m_emptyLabel->setVisible(true);
        return;
    }

    m_table->setVisible(true);
    m_emptyLabel->setVisible(false);

    m_table->setRowCount(records.size());
    for (int i = 0; i < records.size(); ++i)
    {
        // 排名
        QTableWidgetItem *rankItem = new QTableWidgetItem(
            QString::number(i + 1));
        rankItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 0, rankItem);

        // 昵称
        QTableWidgetItem *nameItem = new QTableWidgetItem(
            records.at(i).nickname);
        nameItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 1, nameItem);

        // 分数
        QTableWidgetItem *scoreItem = new QTableWidgetItem(
            QString::number(records.at(i).bestScore));
        scoreItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 2, scoreItem);
    }
}
