#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

#include <QDialog>
#include <QString>

// 结算结果类型
enum GameResultType
{
    ResultDefeat,
    ResultVictory
};

// 结算操作
enum ResultAction
{
    ActionRestart    = 1001,
    ActionReturnLogin = 1002,
    ActionExitGame   = 1003
};

class ResultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResultDialog(
        GameResultType resultType,
        const QString &nickname,
        int finalScore,
        int bestScore,
        QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *event);
    void reject();

private:
    void setupUI();
    void showRankDialog();

    GameResultType m_resultType;
    QString m_nickname;
    int    m_finalScore;
    int    m_bestScore;
};

#endif // RESULTDIALOG_H
