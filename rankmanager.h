#ifndef RANKMANAGER_H
#define RANKMANAGER_H

#include <QString>
#include <QList>

// 排行榜记录
struct RankRecord
{
    QString nickname;
    int bestScore;
};

// 排行榜管理器（不继承QObject，不创建窗口）
class RankManager
{
public:
    RankManager();

    QList<RankRecord> loadRecords() const;
    void updateBestScore(const QString &nickname, int score);
    int bestScoreFor(const QString &nickname) const;

private:
    QString settingsFilePath() const;
    void saveRecords(const QList<RankRecord> &records) const;
    void sortAndTrim(QList<RankRecord> &records) const;
};

#endif // RANKMANAGER_H
