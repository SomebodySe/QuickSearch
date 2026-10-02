#pragma once
#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include <QString>
#include <QPoint>

class SearchManager
{
public:
    static void executeSearch(
        const QString& text,
        const QPoint& targetPosition,
        bool autoEnter);
};

#endif // SEARCHMANAGER_H