#include "targetwidget.h"

#include <QPainter>
#include <QMouseEvent>

TargetWidget::TargetWidget(QWidget* parent)
    : QWidget(parent)
{
    // 红点大小
    setFixedSize(16, 16);

    // 设置为无边框、工具窗口、置顶
    setWindowFlags(
        Qt::Tool |
        Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint |
        Qt::WindowDoesNotAcceptFocus);

    // 背景透明
    setAttribute(Qt::WA_TranslucentBackground);
}


void TargetWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // 开启抗锯齿
    painter.setRenderHint(QPainter::Antialiasing);

    // 红色填充
    painter.setBrush(Qt::red);

    // 不画边框
    painter.setPen(Qt::NoPen);

    // 画圆
    painter.drawEllipse(rect());
}


void TargetWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = true;

        m_dragOffset =
            event->position().toPoint();

        event->accept();
    }
}


void TargetWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_dragging)
        return;

    QPoint globalPos =
        event->globalPosition().toPoint();

    move(globalPos - m_dragOffset);

    event->accept();
}


void TargetWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = false;

        emit positionChanged(pos());

        event->accept();
    }
}