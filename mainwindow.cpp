#include "mainwindow.h"
#include "targetwidget.h"
#include "config.h"
#include "searchmanager.h"
#include <QInputDialog>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <windows.h>

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle("右键按钮修改");
    setWindowIcon(QIcon(":/icon.png"));
    setWindowFlags(
        Qt::Window |
        Qt::WindowStaysOnTopHint);

    loadConfig();


    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->setSpacing(1);

    m_removeButton =
        new QPushButton("−");

    m_addButton =
        new QPushButton("+");

    m_removeButton->setFixedSize(24, 24);
    m_addButton->setFixedSize(24, 24);

    m_countLabel =
        new QLabel(
            QString("按钮数量：%1")
            .arg(m_buttonCount));

    m_autoEnterCheckBox =
        new QCheckBox("自动Enter");

    m_resetButton =
        new QPushButton("复位");
    m_resetButton->setFixedSize(48, 24);
    m_autoEnterCheckBox->setChecked(false);

    m_targetFrame = new QFrame;
    m_targetFrame->setFixedSize(24, 24);
    m_targetFrame->setFrameShape(QFrame::Box);
    m_targetFrame->setFrameShadow(QFrame::Plain);

    //topLayout->addWidget(m_countLabel);

    //topLayout->addSpacing(2);

    topLayout->addWidget(m_removeButton);
    topLayout->addWidget(m_addButton);

    topLayout->addSpacing(7);

    topLayout->addWidget(m_autoEnterCheckBox);

    topLayout->addSpacing(7);
    topLayout->addStretch();

    topLayout->addWidget(m_resetButton);

    topLayout->addSpacing(1);

    topLayout->addWidget(m_targetFrame);

    m_gridLayout = new QGridLayout;
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    m_gridLayout->setHorizontalSpacing(1);
    m_gridLayout->setVerticalSpacing(1);
    m_targetWidget = new TargetWidget();

    // 红点显示以后，再把它放到主窗口右下角
    m_targetWidget->show();

    m_targetWidget->show();

    createButtons();

    QVBoxLayout* mainLayout = new QVBoxLayout;

    mainLayout->setContentsMargins(
        6, 0, 6, 6);

    mainLayout->setSpacing(6);

    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(m_gridLayout);

    setLayout(mainLayout);
    setMinimumWidth(260);

    int minHeight = mainLayout->sizeHint().height();

    resize(220, minHeight);

    connect(
        m_addButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            changeButtonCount(1);
        });

    connect(
        m_removeButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            changeButtonCount(-1);
        });
    connect(
        m_resetButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            resetTargetPosition();
        });
    connect(
        m_targetWidget,
        &TargetWidget::positionChanged,
        this,
        [this](const QPoint& position)
        {
            m_targetPosition = position;
            m_hasSavedTargetPosition = true;

            saveConfig();
        });
}

MainWindow::~MainWindow()
{}

void MainWindow::createButtons()
{
    // 删除旧按钮
    for (QPushButton* button : m_buttons)
    {
        delete button;
    }

    m_buttons.clear();

    // 创建新的按钮
    for (int i = 0; i < m_buttonCount; ++i)
    {
        QString text;

        if (i < m_buttonTexts.size())
        {
            text = m_buttonTexts[i];
        }
        else
        {
            text = QString::number(i + 1);
        }

        QPushButton* button =
            new QPushButton(text, this);
        button->setFixedSize(80, 30);
        adjustButtonFont(button);
        m_buttons.append(button);

        // 左键点击
        connect(
            button,
            &QPushButton::clicked,
            this,
            [this, button]()
            {
                executeButtonSearch(button);
            });

        // 允许右键菜单事件
        button->setContextMenuPolicy(
            Qt::CustomContextMenu);

        // 右键点击
        connect(
            button,
            &QPushButton::customContextMenuRequested,
            this,
            [this, button](const QPoint& pos)
            {
                Q_UNUSED(pos);

                editButtonText(button);
            });
    }

    // 重新排列按钮
    updateLayout();
}

void MainWindow::changeButtonCount(int delta)
{
    int newCount =
        m_buttonCount + delta;

    if (newCount < 1)
        newCount = 1;

    if (newCount > 30)
        newCount = 30;

    if (newCount == m_buttonCount)
        return;

    m_buttonCount = newCount;

    m_countLabel->setText(
        QString("按钮数量：%1")
        .arg(m_buttonCount));

    createButtons();
}

void MainWindow::updateLayout()
{
    if (!m_gridLayout)
        return;

    // 先把旧的按钮布局关系清掉
    while (m_gridLayout->count() > 0)
    {
        QLayoutItem* item = m_gridLayout->takeAt(0);

        if (item)
        {
            delete item;
        }
    }

    // 直接使用 MainWindow 自己的宽度
    int availableWidth = this->width();

    // 搜索按钮的大概宽度
    int buttonWidth = 80;

    // 按钮之间的间隔
    int gap = 6;

    // 根据窗口宽度计算一行放几个按钮
    int columns =
        qMax(
            1,
            availableWidth / (buttonWidth + gap));

    // 重新把按钮放回 GridLayout
    for (int i = 0; i < m_buttons.size(); ++i)
    {
        int row = i / columns;
        int column = i % columns;

        m_gridLayout->addWidget(
            m_buttons[i],
            row,
            column);
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    updateLayout();
}


void MainWindow::editButtonText(QPushButton* button)
{
    if (!button)
        return;

    bool ok = false;

    QString text =
        QInputDialog::getText(
            this,
            "修改文本",
            "请输入要搜索的内容:",
            QLineEdit::Normal,
            button->text(),
            &ok);

    if (!ok)
        return;

    // 修改按钮文字
    button->setText(text);

    // 修改后立即重新计算字体
    adjustButtonFont(button);

    // 找到这个按钮在 m_buttons 中的位置
    int index = m_buttons.indexOf(button);

    if (index >= 0)
    {
        // 确保 m_buttonTexts 数量足够
        while (m_buttonTexts.size() <= index)
        {
            m_buttonTexts.append(
                QString::number(
                    m_buttonTexts.size() + 1));
        }

        // 同步保存到文字列表
        m_buttonTexts[index] = text;
    }

    saveConfig();
}

void MainWindow::saveConfig()
{
    QStringList texts;

    for (QPushButton* button : m_buttons)
    {
        texts.append(button->text());
    }

    if (m_targetWidget)
    {
        m_targetPosition =
            m_targetWidget->pos();
    }

    Config::save(
        "search_conf.txt",
        m_buttonCount,
        texts,
        m_targetPosition);
}

void MainWindow::loadConfig()
{
    int count = 6;
    QStringList texts;

    bool success =
        Config::load(
            "search_conf.txt",
            count,
            texts,
            m_targetPosition,
            m_hasSavedTargetPosition);

    if (!success)
    {
        // 没有配置文件，使用默认值
        m_buttonCount = 6;

        m_buttonTexts.clear();

        for (int i = 0; i < 6; ++i)
        {
            m_buttonTexts.append(
                QString::number(i + 1));
        }

        return;
    }

    // 限制按钮数量
    if (count < 1)
        count = 1;

    if (count > 30)
        count = 30;

    m_buttonCount = count;
    m_buttonTexts = texts;
}

void MainWindow::executeButtonSearch(QPushButton* button)
{
    if (!button)
        return;

    QString text = button->text();

    // 获取红点的 Windows 窗口句柄
    HWND hwnd =
        reinterpret_cast<HWND>(m_targetWidget->winId());

    // 获取红点在 Windows 屏幕中的位置
    RECT rect{};

    if (!GetWindowRect(hwnd, &rect))
    {
        qDebug() << "无法获取红点位置";
        return;
    }

    // 计算红点中心
    QPoint targetPosition(
        (rect.left + rect.right) / 2,
        (rect.top + rect.bottom) / 2);

    qDebug() << "准备搜索:" << text;
    qDebug() << "红点 Windows 坐标:"
        << targetPosition;

    // 隐藏红点
    m_targetWidget->hide();

    // 执行搜索
    bool autoEnter =
        m_autoEnterCheckBox->isChecked();

    SearchManager::executeSearch(
        text,
        targetPosition,
        autoEnter);

    // 重新显示红点
    m_targetWidget->show();
    m_targetWidget->raise();
}

void MainWindow::resetTargetPosition()
{
    if (!m_targetWidget || !m_targetFrame)
        return;

    QPoint center =
        m_targetFrame->mapToGlobal(
            m_targetFrame->rect().center());

    QPoint targetPos =
        center -
        QPoint(
            m_targetWidget->width() / 2,
            m_targetWidget->height() / 2);

    m_targetWidget->move(targetPos);

    m_targetPosition = targetPos;

    m_hasSavedTargetPosition = true;

    saveConfig();

    m_targetWidget->raise();
}
void MainWindow::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (m_hasSavedTargetPosition)
    {
        m_targetWidget->move(
            m_targetPosition);

        m_targetWidget->raise();
    }
    else
    {
        resetTargetPosition();
    }
}

void MainWindow::adjustButtonFont(QPushButton* button)
{
    if (!button)
        return;

    QFont font = button->font();

    // 正常情况下的字体大小
    const int normalSize = 14;

    // 最小字体大小
    const int minSize = 6;

    // 按钮固定 80x30，所以留一点左右空间
    const int availableWidth = 80 - 10;

    font.setPointSize(normalSize);

    while (font.pointSize() > minSize)
    {
        QFontMetrics metrics(font);

        if (metrics.horizontalAdvance(button->text())
            <= availableWidth)
        {
            break;
        }

        font.setPointSize(font.pointSize() - 1);
    }

    button->setFont(font);
}