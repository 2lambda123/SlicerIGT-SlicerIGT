/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/


// Qt includes
#include <QtGui>

// SlicerQt includes
#include "qSlicerBreachWarningModuleWidget.h"
#include "ui_qSlicerBreachWarningModule.h"

#include "vtkSlicerBreachWarningLogic.h"

#include "vtkMRMLBreachWarningNode.h"
#include "vtkMRMLTransformNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_BreachWarning
class qSlicerBreachWarningModuleWidgetPrivate: public Ui_qSlicerBreachWarningModule
{
  Q_DECLARE_PUBLIC( qSlicerBreachWarningModuleWidget ); 
  
protected:
  qSlicerBreachWarningModuleWidget* const q_ptr;
public:
  qSlicerBreachWarningModuleWidgetPrivate( qSlicerBreachWarningModuleWidget& object );
  vtkSlicerBreachWarningLogic* logic() const;

  bool ModuleWindowInitialized;
};

//-----------------------------------------------------------------------------
// qSlicerBreachWarningModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerBreachWarningModuleWidgetPrivate::qSlicerBreachWarningModuleWidgetPrivate( qSlicerBreachWarningModuleWidget& object ) 
  : q_ptr( &object )
  , ModuleWindowInitialized( false )
{
}

//-----------------------------------------------------------------------------
vtkSlicerBreachWarningLogic* qSlicerBreachWarningModuleWidgetPrivate::logic() const
{
  Q_Q( const qSlicerBreachWarningModuleWidget );
  return vtkSlicerBreachWarningLogic::SafeDownCast( q->logic() );
}

//-----------------------------------------------------------------------------
// qSlicerBreachWarningModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerBreachWarningModuleWidget::qSlicerBreachWarningModuleWidget( QWidget* _parent )
  : Superclass( _parent )
  , d_ptr( new qSlicerBreachWarningModuleWidgetPrivate( *this ) )
{

}

//-----------------------------------------------------------------------------
qSlicerBreachWarningModuleWidget::~qSlicerBreachWarningModuleWidget()
{
 Q_D(qSlicerBreachWarningModuleWidget);
  disconnect( d->ParameterNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onParameterNodeChanged() ) );

  // Make connections to update the mrml from the widget
  disconnect( d->ModelNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onModelNodeChanged() ) );
  disconnect( d->ToolComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onToolTransformChanged() ) );
  disconnect( d->WarningColorPickerButton, SIGNAL( colorChanged( QColor ) ), this, SLOT( updateWarningColor( QColor ) ) );
  disconnect( d->SoundCheckBox, SIGNAL(toggled(bool)), this, SLOT(playWarningSound(bool)));
  disconnect( d->WarningCheckBox, SIGNAL(toggled(bool)), this, SLOT(displayWarningColor(bool)));
  disconnect( d->LineToClosestPointVisibilityCheckBox, SIGNAL(toggled(bool)), this, SLOT(lineToClosestPointVisibilityChanged(bool)));
  disconnect( d->LineToClosestPointColorPickerButton, SIGNAL( colorChanged( QColor ) ), this, SLOT(lineToClosestPointColorChanged( QColor ) ) );  
  disconnect( d->LineToClosestPointTextSizeSlider, SIGNAL( valueChanged (double ) ), this, SLOT(lineToClosestPointTextSizeChanged( double ) ) );  
  disconnect(d->LineToClosestPointThicknessSlider, SIGNAL(valueChanged(double)), this, SLOT(lineToClosestPointThicknessChanged(double)));
  disconnect(d->WarningDistanceMMSpinBox, SIGNAL(valueChanged(double)), this, SLOT(warningDistanceMMChanged(double)));
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::setup()
{
  Q_D(qSlicerBreachWarningModuleWidget);

  d->setupUi(this);
  this->Superclass::setup();

  this->setMRMLScene( d->logic()->GetMRMLScene() );

  connect( d->ParameterNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onParameterNodeChanged() ) );

  // Make connections to update the mrml from the widget
  connect( d->ModelNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onModelNodeChanged() ) );
  connect( d->ToolComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onToolTransformChanged() ) );
  connect( d->WarningColorPickerButton, SIGNAL( colorChanged( QColor ) ), this, SLOT( updateWarningColor( QColor ) ) );
  connect(d->SoundCheckBox, SIGNAL(toggled(bool)), this, SLOT(playWarningSound(bool)));
  connect(d->WarningCheckBox, SIGNAL(toggled(bool)), this, SLOT(displayWarningColor(bool)));
  connect(d->LineToClosestPointVisibilityCheckBox, SIGNAL(toggled(bool)), this, SLOT(lineToClosestPointVisibilityChanged(bool)));
  connect( d->LineToClosestPointColorPickerButton, SIGNAL( colorChanged( QColor ) ), this, SLOT(lineToClosestPointColorChanged( QColor ) ) );
  connect( d->LineToClosestPointTextSizeSlider, SIGNAL( valueChanged (double ) ), this, SLOT(lineToClosestPointTextSizeChanged( double ) ) );  
  connect( d->LineToClosestPointThicknessSlider, SIGNAL( valueChanged (double ) ), this, SLOT(lineToClosestPointThicknessChanged( double ) ) );  
  connect(d->WarningDistanceMMSpinBox, SIGNAL(valueChanged(double)), this, SLOT(warningDistanceMMChanged(double)));

  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::enter()
{
  Q_D(qSlicerBreachWarningModuleWidget);

  if ( this->mrmlScene() == NULL )
  {
    qCritical() << "Invalid scene!";
    return;
  }
  
  // Create a module MRML node if there is none in the scene.

  vtkMRMLNode* node = this->mrmlScene()->GetNthNodeByClass(0, "vtkMRMLBreachWarningNode");
  if ( node == NULL )
  {
    vtkSmartPointer< vtkMRMLBreachWarningNode > newNode = vtkSmartPointer< vtkMRMLBreachWarningNode >::New();
    this->mrmlScene()->AddNode( newNode );
  }

  node = this->mrmlScene()->GetNthNodeByClass( 0, "vtkMRMLBreachWarningNode" );
  if ( node == NULL )
  {
    qCritical( "Failed to create module node" );
    return;
  }

  // For convenience, select a default module.

  if ( d->ParameterNodeComboBox->currentNode() == NULL )
  {
    d->ParameterNodeComboBox->setCurrentNodeID( node->GetID() );
  }

  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::setMRMLScene( vtkMRMLScene* scene )
{
  Q_D( qSlicerBreachWarningModuleWidget );
  this->Superclass::setMRMLScene( scene );
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::onSceneImportedEvent()
{
  this->enter();
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::onParameterNodeChanged()
{
  Q_D( qSlicerBreachWarningModuleWidget );
  vtkMRMLBreachWarningNode* bwNode = vtkMRMLBreachWarningNode::SafeDownCast(d->ParameterNodeComboBox->currentNode());
  qvtkReconnect(bwNode, vtkMRMLBreachWarningNode::InputDataModifiedEvent, this, SLOT(updateWidgetFromMRML()));
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::onModelNodeChanged()
{
  Q_D( qSlicerBreachWarningModuleWidget );
  
  vtkMRMLBreachWarningNode* bwNode = vtkMRMLBreachWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( bwNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  
  vtkMRMLModelNode* currentNode = vtkMRMLModelNode::SafeDownCast(d->ModelNodeComboBox->currentNode());
  d->logic()->SetWatchedModelNode( currentNode, bwNode );
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::onToolTransformChanged()
{
  Q_D( qSlicerBreachWarningModuleWidget );
  
  vtkMRMLBreachWarningNode* parameterNode = vtkMRMLBreachWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( parameterNode == NULL )
  {
    qCritical( "Transform node should not be changed when no parameter node selected" );
    return;
  }
  
  vtkMRMLTransformNode* currentNode = vtkMRMLTransformNode::SafeDownCast(d->ToolComboBox->currentNode());
  parameterNode->SetAndObserveToolTransformNodeId( (currentNode!=NULL) ? currentNode->GetID() : NULL);
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::playWarningSound(bool playWarningSound)
{
  Q_D(qSlicerBreachWarningModuleWidget);
  vtkMRMLBreachWarningNode* parameterNode = vtkMRMLBreachWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( parameterNode == NULL )
  {
    qCritical( "No module node selected" );
    return;
  }
  parameterNode->SetPlayWarningSound(playWarningSound);
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::displayWarningColor(bool displayWarningColor)
{
  Q_D(qSlicerBreachWarningModuleWidget);
  vtkMRMLBreachWarningNode* parameterNode = vtkMRMLBreachWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( parameterNode == NULL )
  {
    qCritical( "No module node selected" );
    return;
  }
  parameterNode->SetDisplayWarningColor(displayWarningColor);
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::lineToClosestPointVisibilityChanged(bool visible)
{
  Q_D(qSlicerBreachWarningModuleWidget);
  vtkMRMLBreachWarningNode* parameterNode = vtkMRMLBreachWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( parameterNode == NULL )
  {
    qCritical( "No module node selected" );
    return;
  }
  d->logic()->SetLineToClosestPointVisibility( visible, parameterNode );
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::updateWarningColor( QColor newColor )
{
  Q_D(qSlicerBreachWarningModuleWidget);

  vtkMRMLBreachWarningNode* parameterNode = vtkMRMLBreachWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( parameterNode == NULL )
  {
    qCritical( "Color selected without module node" );
    return;
  }

  parameterNode->SetWarningColor( newColor.redF(), newColor.greenF(), newColor.blueF() );
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::lineToClosestPointColorChanged( QColor newColor )
{
  Q_D(qSlicerBreachWarningModuleWidget);

  vtkMRMLBreachWarningNode* parameterNode = vtkMRMLBreachWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( parameterNode == NULL )
  {
    qCritical( "Color selected without module node" );
    return;
  }

  d->logic()->SetLineToClosestPointColor( newColor.redF(), newColor.greenF(), newColor.blueF(), parameterNode );
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::lineToClosestPointTextSizeChanged( double size )
{
  Q_D(qSlicerBreachWarningModuleWidget);

  vtkMRMLBreachWarningNode* parameterNode = vtkMRMLBreachWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( parameterNode == NULL )
  {
    qCritical( "No module node selected" );
    return;
  }

  d->logic()->SetLineToClosestPointTextScale( size, parameterNode );
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::lineToClosestPointThicknessChanged( double thickness )
{
  Q_D(qSlicerBreachWarningModuleWidget);

  vtkMRMLBreachWarningNode* parameterNode = vtkMRMLBreachWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( parameterNode == NULL )
  {
    qCritical( "No module node selected" );
    return;
  }

  d->logic()->SetLineToClosestPointThickness( thickness, parameterNode);
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::warningDistanceMMChanged(double warningDistanceMM)
{
  Q_D(qSlicerBreachWarningModuleWidget);

  vtkMRMLBreachWarningNode* parameterNode = vtkMRMLBreachWarningNode::SafeDownCast(d->ParameterNodeComboBox->currentNode());
  if (parameterNode == NULL)
  {
    qCritical("No module node selected");
    return;
  }
  parameterNode->SetWarningDistanceMM(warningDistanceMM);
}

//-----------------------------------------------------------------------------
void qSlicerBreachWarningModuleWidget::updateWidgetFromMRML()
{
  Q_D( qSlicerBreachWarningModuleWidget );
  
  vtkMRMLBreachWarningNode* bwNode = vtkMRMLBreachWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( bwNode == NULL )
  {
    d->ToolComboBox->setCurrentNodeID( "" );
    d->ModelNodeComboBox->setCurrentNodeID( "" );
    d->ModelNodeComboBox->setEnabled( false );
    d->ToolComboBox->setEnabled( false );
    d->WarningCheckBox->setEnabled( false );
    d->WarningColorPickerButton->setEnabled( false );
    d->SoundCheckBox->setEnabled( false );        
    d->LineToClosestPointVisibilityCheckBox->setEnabled( false );
    d->LineToClosestPointColorPickerButton->setEnabled( false );
    d->LineToClosestPointTextSizeSlider->setEnabled( false );
    d->LineToClosestPointThicknessSlider->setEnabled( false );
    d->WarningDistanceMMSpinBox->setValue(0.0);
    return;
  }
    
  d->ModelNodeComboBox->setEnabled( true );
  d->ToolComboBox->setEnabled( true );
  d->WarningCheckBox->setEnabled( true );
  d->WarningColorPickerButton->setEnabled( true );
  d->SoundCheckBox->setEnabled( true );
  d->LineToClosestPointVisibilityCheckBox->setEnabled( true );
  d->LineToClosestPointColorPickerButton->setEnabled( true );
  d->LineToClosestPointTextSizeSlider->setEnabled( true );
  d->LineToClosestPointThicknessSlider->setEnabled( true );

  // Only set the node if it is different from current selection because otherwise
  // the node selector would keep jumping back to the currently referenced node in MRML
  // while the parent transforms are being modified.
  if (d->ToolComboBox->currentNode() != bwNode->GetToolTransformNode())
  {
    d->ToolComboBox->setCurrentNode(bwNode->GetToolTransformNode());
  }
  if (d->ModelNodeComboBox->currentNode() != bwNode->GetWatchedModelNode())
  {
    d->ModelNodeComboBox->setCurrentNode(bwNode->GetWatchedModelNode());
  }
  
  double* warningColor = bwNode->GetWarningColor();
  QColor warningColorQt;
  warningColorQt.setRgbF(warningColor[0],warningColor[1],warningColor[2]);
  d->WarningColorPickerButton->setColor(warningColorQt);

  double* lineToClosestPointColor = d->logic()->GetLineToClosestPointColor(bwNode);
  QColor lineToClosestPointColorQt;
  lineToClosestPointColorQt.setRgbF(lineToClosestPointColor[0],lineToClosestPointColor[1],lineToClosestPointColor[2]);
  d->LineToClosestPointColorPickerButton->setColor(lineToClosestPointColorQt);

  d->WarningCheckBox->setChecked( bwNode->GetDisplayWarningColor() );
  d->SoundCheckBox->setChecked( bwNode->GetPlayWarningSound() );  
  d->LineToClosestPointVisibilityCheckBox->setChecked( d->logic()->GetLineToClosestPointVisibility(bwNode) );
  d->LineToClosestPointTextSizeSlider->setValue( d->logic()->GetLineToClosestPointTextScale(bwNode) );
  d->LineToClosestPointThicknessSlider->setValue( d->logic()->GetLineToClosestPointThickness(bwNode) );

  d->WarningDistanceMMSpinBox->setValue(bwNode->GetWarningDistanceMM());
}
