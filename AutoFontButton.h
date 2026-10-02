#pragma once

#include <QPushButton>
#include <QFontMetrics>
#include <QResizeEvent>

class AutoFontButton : public QPushButton
{
public:
    explicit AutoFontButton(const QString& text, QWidget* parent = nullptr)
        : QPushButton(text, parent)
    {
        m_normalPointSize = 14;
        updateFontSize();
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QPushButton::resizeEvent(event);
        updateFontSize();
    }

    void changeEvent(QEvent* event) override
    {
        QPushButton::changeEvent(event);

        if (event->type() == QEvent::FontChange)
            updateFontSize();
    }

private:
    void updateFontSize()
    {
        if (width() <= 0 || height() <= 0)
            return;

        QFont font = this->font();
        font.setPointSize(m_normalPointSize);

        // 按钮内部留一点空间
        int availableWidth = width() - 10;

        // 从正常字号开始逐渐缩小
        while (font.pointSize() > 6)
        {
            QFontMetrics metrics(font);

            if (metrics.horizontalAdvance(text()) <= availableWidth)
                break;

            font.setPointSize(font.pointSize() - 1);
        }

        setFont(font);
    }

private:
    int m_normalPointSize = 14;
};