#include "config.h"

#include <QFile>
#include <QTextStream>

bool Config::save(
    const QString& fileName,
    int buttonCount,
    const QStringList& buttonTexts,
    const QPoint& targetPosition)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream out(&file);

    out << "BUTTON_COUNT=" << buttonCount << "\n";
    out << "TARGET_X=" << targetPosition.x() << "\n";
    out << "TARGET_Y=" << targetPosition.y() << "\n";

    for (int i = 0; i < buttonTexts.size(); ++i)
    {
        out << "TEXT"
            << (i + 1)
            << "="
            << buttonTexts[i]
            << "\n";
    }

    file.close();

    return true;
}


bool Config::load(
    const QString& fileName,
    int& buttonCount,
    QStringList& buttonTexts,
    QPoint& targetPosition,
    bool& hasTargetPosition)
{
    QFile file(fileName);
    hasTargetPosition = false;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream in(&file);

    buttonTexts.clear();

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.startsWith("BUTTON_COUNT="))
        {
            QString value =
                line.mid(QString("BUTTON_COUNT=").length());

            buttonCount = value.toInt();
        }
        else if (line.startsWith("TEXT"))
        {
            int equalPos = line.indexOf('=');

            if (equalPos >= 0)
            {
                QString text =
                    line.mid(equalPos + 1);

                buttonTexts.append(text);
            }
        }
        else if (line.startsWith("TARGET_X="))
        {
            QString value =
                line.mid(QString("TARGET_X=").length());

            targetPosition.setX(value.toInt());

            hasTargetPosition = true;
        }
        else if (line.startsWith("TARGET_Y="))
        {
            QString value =
                line.mid(QString("TARGET_Y=").length());

            targetPosition.setY(value.toInt());
        }
    }

    file.close();

    return true;
}