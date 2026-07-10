#include "rankmanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <algorithm>

RankManager::RankManager()
{
}

QString RankManager::settingsFilePath() const
{
    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dirPath);
    return dirPath + QStringLiteral("/ranking.ini");
}

QList<RankRecord> RankManager::loadRecords() const
{
    QList<RankRecord> records;

    QSettings settings(settingsFilePath(), QSettings::IniFormat);

    int count = settings.value(QStringLiteral("rank/count"), 0).toInt();
    if (count < 0)
    {
        count = 0;
    }
    if (count > 10)
    {
        count = 10;
    }

    for (int i = 0; i < count; ++i)
    {
        QString nameKey  = QStringLiteral("rank/%1/name").arg(i);
        QString scoreKey = QStringLiteral("rank/%1/score").arg(i);

        RankRecord record;
        record.nickname  = settings.value(nameKey, QString()).toString();
        record.bestScore = settings.value(scoreKey, 0).toInt();

        // 跳过无效记录
        if (record.nickname.trimmed().isEmpty())
        {
            continue;
        }
        if (record.bestScore < 0)
        {
            continue;
        }

        records.append(record);
    }

    // 重新排序去重
    sortAndTrim(records);

    return records;
}

void RankManager::saveRecords(const QList<RankRecord> &records) const
{
    QSettings settings(settingsFilePath(), QSettings::IniFormat);
    settings.clear();

    settings.setValue(QStringLiteral("rank/count"), records.size());

    for (int i = 0; i < records.size(); ++i)
    {
        settings.setValue(QStringLiteral("rank/%1/name").arg(i), records.at(i).nickname);
        settings.setValue(QStringLiteral("rank/%1/score").arg(i), records.at(i).bestScore);
    }

    settings.sync();
}

void RankManager::sortAndTrim(QList<RankRecord> &records) const
{
    // 去掉重名（保留最高分）
    QList<RankRecord> deduped;
    for (int i = 0; i < records.size(); ++i)
    {
        const RankRecord &cur = records.at(i);
        bool found = false;
        for (int j = 0; j < deduped.size(); ++j)
        {
            if (deduped[j].nickname == cur.nickname)
            {
                found = true;
                if (cur.bestScore > deduped[j].bestScore)
                {
                    deduped[j].bestScore = cur.bestScore;
                }
                break;
            }
        }
        if (!found)
        {
            deduped.append(cur);
        }
    }

    // 按分数降序，同分按昵称升序
    std::sort(deduped.begin(), deduped.end(),
              [](const RankRecord &a, const RankRecord &b) {
                  if (a.bestScore != b.bestScore)
                  {
                      return a.bestScore > b.bestScore;
                  }
                  return a.nickname < b.nickname;
              });

    // 最多保留前10名
    while (deduped.size() > 10)
    {
        deduped.removeLast();
    }

    records = deduped;
}

void RankManager::updateBestScore(const QString &nickname, int score)
{
    QString trimmed = nickname.trimmed();
    if (trimmed.isEmpty())
    {
        return;
    }
    if (score < 0)
    {
        return;
    }

    QList<RankRecord> records = loadRecords();

    // 查找同名记录
    bool found = false;
    for (int i = 0; i < records.size(); ++i)
    {
        if (records[i].nickname == trimmed)
        {
            found = true;
            if (score > records[i].bestScore)
            {
                records[i].bestScore = score;
            }
            break;
        }
    }

    if (!found)
    {
        RankRecord newRecord;
        newRecord.nickname  = trimmed;
        newRecord.bestScore = score;
        records.append(newRecord);
    }

    sortAndTrim(records);
    saveRecords(records);
}

int RankManager::bestScoreFor(const QString &nickname) const
{
    QList<RankRecord> records = loadRecords();
    for (int i = 0; i < records.size(); ++i)
    {
        if (records[i].nickname == nickname.trimmed())
        {
            return records[i].bestScore;
        }
    }
    return 0;
}
