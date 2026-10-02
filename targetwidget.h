#ifndef TARGETWIDGET_H
#define TARGETWIDGET_H

#include <QWidget>

class TargetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TargetWidget(QWidget* parent = nullptr);
signals:
    void positionChanged(const QPoint& position);

protected:
    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    bool m_dragging = false;

    QPoint m_dragOffset;
};

#endif // TARGETWIDGET_H