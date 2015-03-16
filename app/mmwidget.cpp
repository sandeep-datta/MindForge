#include <QBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QDebug>
#include <QEvent>


#include "mmwidget.h"
#include "common.h"


MmWidget::MmWidget(QSettings &settings, QWidget *parent)
    : m_settings(settings)
    , QWidget(parent)
    , m_blackPen(Qt::black)
    , m_selectedNode(&m_rootNode)
    , m_editor(this)
    , m_bAddNode(false)
#ifdef DUMP_FRAMES
    , m_img(800, 550, QImage::Format_RGB32)
    , m_px(400)
    , m_py(0)
#endif
{
    m_blackPen.setWidth(2);
    connect(&m_editor, SIGNAL(editAccepted()), this, SLOT(editAccepted()));
    connect(&m_editor, SIGNAL(editRejected()), this, SLOT(editRejected()));
    m_editor.hide();
}

MmWidget::~MmWidget()
{

}

void MmWidget::setData(const MmNode &node)
{
    m_rootNode = node;
}


void MmWidget::setBackGround(QColor color)
{
    QPalette p(palette());
    p.setColor(QPalette::Background, color);
    setPalette(p);
}



void MmWidget::resizeEvent(QResizeEvent *)
{

}

MmNode* MmWidget::getSelectedNode()
{
    return m_selectedNode;
}

void MmWidget::editNode()
{
    QRect rect = getSelectedNode()->getTextRect();
    m_editor.setPlainText(getSelectedNode()->getText());
    m_editor.move(rect.topLeft());
    m_editor.show();
    m_editor.setFocus();
}

void MmWidget::addNode()
{
    m_bAddNode = true;
    m_selectedNode = &getSelectedNode()->addChild("");
    m_rootNode.updateLayout();
    editNode();
}

void MmWidget::editAccepted()
{
    qDebug()<<"Edit accepted.";
    m_bAddNode = false;
    m_selectedNode->setText(m_editor.toPlainText());
    m_editor.hide();
    update();
}

void MmWidget::editRejected()
{
    if (m_bAddNode) {
        m_selectedNode = getSelectedNode()->getParent();
        m_selectedNode->removeLastChild();
        m_bAddNode = false;
    }
    m_editor.hide();
    update();
}


#ifdef DUMP_FRAMES
void MmWidget::imgPrint(QString str, QPainter &painter)
{
    QRect targetRect(m_px, m_py, 800, 800);
    QRect br;
    painter.drawText(targetRect
                     , Qt::TextWordWrap
                     , str
                     , &br);

    m_py += br.height();
    save();
}

void MmWidget::save()
{
    m_img.save(QString("mm%1.png").arg(m_serial++));
}
#endif


void MmWidget::paintEvent(QPaintEvent *)
{
#ifdef DUMP_FRAMES
    QPainter painter(&m_img);
#else
    QPainter painter(this);
#endif

    //Paint background
    painter.fillRect(rect(), palette().background());
    painter.drawRect(rect());

    painter.setRenderHint(QPainter::Antialiasing, true);

    m_rootNode.setPos(m_rootNode.xMargin()
                      , height()/2 - m_rootNode.getTextRect().height()/2);
    m_rootNode.updateLayout();
    m_rootNode.paint(painter);
    SAVE();
}