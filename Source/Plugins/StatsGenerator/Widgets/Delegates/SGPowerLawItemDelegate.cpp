#include "SGPowerLawItemDelegate.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_SGPowerLawItemDelegate.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGPowerLawItemDelegate::SGPowerLawItemDelegate(QObject* parent)
: QStyledItemDelegate(parent)
{
  Q_ASSERT_X(false, __FILE__, "POWERLAW TABLE MODEL IS NOT WORKING AND NEEDS FIXING.");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGPowerLawItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QStyledItemDelegate::paint(painter, option, index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* SGPowerLawItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QLineEdit* alpha;
  QLineEdit* beta;
  QDoubleValidator* alphaValidator;
  QDoubleValidator* betaValidator;
  QLineEdit* k;
  QDoubleValidator* kValidator;

  qint32 col = index.column();
  switch(col)
  {
  case SGPowerLawTableModel::BinNumber:
    return nullptr;
    break;

  case SGPowerLawTableModel::Alpha:
    alpha = new QLineEdit(parent);
    alpha->setFrame(false);
    alphaValidator = new QDoubleValidator(alpha);
    alphaValidator->setDecimals(6);
    alpha->setValidator(alphaValidator);
    return alpha;
  case SGPowerLawTableModel::K:
    k = new QLineEdit(parent);
    k->setFrame(false);
    kValidator = new QDoubleValidator(k);
    kValidator->setDecimals(6);
    k->setValidator(kValidator);
    return k;
  case SGPowerLawTableModel::Beta:
    beta = new QLineEdit(parent);
    beta->setFrame(false);
    betaValidator = new QDoubleValidator(beta);
    betaValidator->setDecimals(6);
    beta->setValidator(betaValidator);
    return beta;
  default:
    break;
  }
  return QStyledItemDelegate::createEditor(parent, option, index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGPowerLawItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  qint32 col = index.column();
  // bool ok = false;
  if(col == SGPowerLawTableModel::Alpha || col == SGPowerLawTableModel::K || col == SGPowerLawTableModel::Beta)
  {
    //    double value = index.model()->data(index).toFloat(&ok);
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    Q_ASSERT(lineEdit);
    lineEdit->setText(index.model()->data(index).toString());
  }
  else
  {
    QStyledItemDelegate::setEditorData(editor, index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGPowerLawItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  //  qDebug() << "SGPowerLawItemDelegate::setModelData" << "\n";
  qint32 col = index.column();
  //  bool ok = false;
  if(col == SGPowerLawTableModel::Alpha || col == SGPowerLawTableModel::K || col == SGPowerLawTableModel::Beta)
  {
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    Q_ASSERT(lineEdit);
    bool ok = false;
    double v = lineEdit->text().toFloat(&ok);
    model->setData(index, v);
  }
  else
  {
    QStyledItemDelegate::setModelData(editor, model, index);
  }
}
