#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QGridLayout>
#include <QVector>
#include <QStringList>
#include <QFrame>

class TargetWidget;
class SearchManager;


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void createButtons();
    void loadConfig();
    void saveConfig();
    void updateButtons();
    void updateLayout();
    void changeButtonCount(int delta);

    // 处理按钮右键修改文字
    void editButtonText(QPushButton* button);
    void executeButtonSearch(QPushButton* button);
    void resetTargetPosition();

private:
    int m_buttonCount = 6;
    QStringList m_buttonTexts;
    QPoint m_targetPosition;
    bool m_hasSavedTargetPosition = false;
    TargetWidget* m_targetWidget = nullptr;
    QPushButton* m_addButton = nullptr;
    QPushButton* m_removeButton = nullptr;
    QPushButton* m_resetButton = nullptr;

    QLabel* m_countLabel = nullptr;

    QCheckBox* m_autoEnterCheckBox = nullptr;
    QFrame* m_targetFrame = nullptr;

    QGridLayout* m_gridLayout = nullptr;

    QVector<QPushButton*> m_buttons;
};

#endif // MAINWINDOW_H