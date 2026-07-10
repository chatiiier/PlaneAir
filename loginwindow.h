#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCloseEvent>

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void startGame();
    void showRankDialog();

private:
    void setupUI();

    QLineEdit   *m_nicknameEdit;
    QPushButton *m_startButton;
    QPushButton *m_rankButton;
    QPushButton *m_exitButton;
};

#endif // LOGINWINDOW_H
