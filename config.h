#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QStringList>
#include <QPoint>

class Config
{
public:
    // 保存配置
    static bool save(
        const QString& fileName,
        int buttonCount,
        const QStringList& buttonTexts,
        const QPoint& targetPosition);

    // 读取配置
    static bool load(
        const QString& fileName,
        int& buttonCount,
        QStringList& buttonTexts,
        QPoint& targetPosition,
        bool& hasTargetPosition);
};

#endif // CONFIG_H