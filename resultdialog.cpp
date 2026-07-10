#include "resultdialog.h"
#include "rankdialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCloseEvent>
#include <QApplication>

ResultDialog::ResultDialog(
    GameResultType resultType,
    const QString &nickname,
    int finalScore,
    int bestScore,
    QWidget *parent)
    : QDialog(parent)
    , m_resultType(resultType)
    , m_nickname(nickname)
    , m_finalScore(finalScore)
    , m_bestScore(bestScore)
{
    setupUI();
}

void ResultDialog::setupUI()
{
    setWindowTitle(QStringLiteral("结算"));
    setFixedSize(320, 340);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(20, 20, 20, 20);

    // 标题
    QLabel *titleLabel = new QLabel(this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont;
    titleFont.setPixelSize(26);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    if (m_resultType == ResultVictory)
    {
        titleLabel->setText(QStringLiteral("任务完成"));
        titleLabel->setStyleSheet(QStringLiteral("QLabel { color: gold; }"));
    }
    else
    {
        titleLabel->setText(QStringLiteral("游戏结束"));
        titleLabel->setStyleSheet(QStringLiteral("QLabel { color: white; }"));
    }
    layout->addWidget(titleLabel);

    layout->addSpacing(5);

    // 玩家昵称
    QLabel *nicknameLabel = new QLabel(
        QStringLiteral("玩家：%1").arg(m_nickname), this);
    nicknameLabel->setAlignment(Qt::AlignCenter);
    QFont infoFont;
    infoFont.setPixelSize(16);
    nicknameLabel->setFont(infoFont);
    layout->addWidget(nicknameLabel);

    // 本局得分
    QLabel *scoreLabel = new QLabel(
        QStringLiteral("本局得分：%1").arg(m_finalScore), this);
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setFont(infoFont);
    layout->addWidget(scoreLabel);

    // 历史最高分
    QLabel *bestLabel = new QLabel(
        QStringLiteral("历史最高分：%1").arg(m_bestScore), this);
    bestLabel->setAlignment(Qt::AlignCenter);
    bestLabel->setFont(infoFont);
    bestLabel->setStyleSheet(QStringLiteral("QLabel { color: gold; }"));
    layout->addWidget(bestLabel);

    layout->addSpacing(10);

    // 重新开始
    QPushButton *restartBtn = new QPushButton(
        QStringLiteral("重新开始"), this);
    restartBtn->setMinimumHeight(36);
    connect(restartBtn, &QPushButton::clicked, this, [this]() {
        done(ActionRestart);
    });
    layout->addWidget(restartBtn);

    // 查看排行榜
    QPushButton *rankBtn = new QPushButton(
        QStringLiteral("查看排行榜"), this);
    rankBtn->setMinimumHeight(36);
    connect(rankBtn, &QPushButton::clicked, this, &ResultDialog::showRankDialog);
    layout->addWidget(rankBtn);

    // 返回登录
    QPushButton *loginBtn = new QPushButton(
        QStringLiteral("返回登录"), this);
    loginBtn->setMinimumHeight(36);
    connect(loginBtn, &QPushButton::clicked, this, [this]() {
        done(ActionReturnLogin);
    });
    layout->addWidget(loginBtn);

    // 退出游戏
    QPushButton *exitBtn = new QPushButton(
        QStringLiteral("退出游戏"), this);
    exitBtn->setMinimumHeight(36);
    connect(exitBtn, &QPushButton::clicked, this, [this]() {
        done(ActionExitGame);
    });
    layout->addWidget(exitBtn);
}

void ResultDialog::showRankDialog()
{
    RankDialog dialog(this);
    dialog.exec();
}

void ResultDialog::closeEvent(QCloseEvent *event)
{
    // 右上角关闭 → 返回登录
    done(ActionReturnLogin);
    event->accept();
}

void ResultDialog::reject()
{
    // Esc 键 → 返回登录
    done(ActionReturnLogin);
}
