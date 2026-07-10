#include "loginwindow.h"
#include "mainscence.h"
#include "rankdialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QApplication>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
    , m_nicknameEdit(0)
    , m_startButton(0)
    , m_rankButton(0)
    , m_exitButton(0)
{
    setupUI();
}

LoginWindow::~LoginWindow()
{
}

void LoginWindow::setupUI()
{
    setWindowTitle(QStringLiteral("飞机大战"));
    setFixedSize(360, 280);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setContentsMargins(30, 30, 30, 30);

    // 标题
    QLabel *titleLabel = new QLabel(QStringLiteral("✈ 飞机大战 ✈"), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont;
    titleFont.setPixelSize(28);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    layout->addSpacing(10);

    // 昵称输入框
    m_nicknameEdit = new QLineEdit(this);
    m_nicknameEdit->setPlaceholderText(QStringLiteral("请输入玩家昵称"));
    m_nicknameEdit->setMaxLength(12);
    m_nicknameEdit->setMinimumHeight(32);
    m_nicknameEdit->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_nicknameEdit);

    // 按钮
    m_startButton = new QPushButton(QStringLiteral("开始游戏"), this);
    m_startButton->setMinimumHeight(36);
    layout->addWidget(m_startButton);

    m_rankButton = new QPushButton(QStringLiteral("排行榜"), this);
    m_rankButton->setMinimumHeight(36);
    layout->addWidget(m_rankButton);

    m_exitButton = new QPushButton(QStringLiteral("退出游戏"), this);
    m_exitButton->setMinimumHeight(36);
    layout->addWidget(m_exitButton);

    // 信号连接
    connect(m_startButton, &QPushButton::clicked,
            this, &LoginWindow::startGame);
    connect(m_nicknameEdit, &QLineEdit::returnPressed,
            this, &LoginWindow::startGame);
    connect(m_rankButton, &QPushButton::clicked,
            this, &LoginWindow::showRankDialog);
    connect(m_exitButton, &QPushButton::clicked,
            qApp, &QApplication::quit);
}

void LoginWindow::startGame()
{
    QString nickname = m_nicknameEdit->text().trimmed();
    if (nickname.isEmpty())
    {
        QMessageBox::warning(this,
                             QStringLiteral("提示"),
                             QStringLiteral("昵称不能为空"));
        return;
    }

    // 禁用开始按钮，防止快速双击创建两个游戏窗口
    m_startButton->setEnabled(false);

    // 创建游戏窗口（parent = nullptr，避免随登录窗口隐藏）
    Mainscence *gameWindow = new Mainscence(nickname, nullptr);
    gameWindow->setAttribute(Qt::WA_DeleteOnClose);

    // 游戏窗口关闭后恢复登录窗口
    connect(gameWindow, &QObject::destroyed, this, [this]() {
        m_startButton->setEnabled(true);
        this->show();
        this->activateWindow();
        this->raise();
    });

    // 隐藏登录，显示游戏
    this->hide();
    gameWindow->show();
}

void LoginWindow::showRankDialog()
{
    RankDialog dialog(this);
    dialog.exec();
}

void LoginWindow::closeEvent(QCloseEvent *event)
{
    QApplication::quit();
    event->accept();
}
