StatsGenerator Microstructure Presets Developer Guide {#micropreset}
===========


## Introduction
The implementation uses the **Factory** design pattern where a singleton instance of MicrostructurePresetManager is responsible for registering each of the preset classes and also for creating an instance of each of those classes given the name of the class. Each class that wants to implement a **MicrostructurePreset** needs to implement 2 classes:
- A subclass of AbstractMicrostructurePresetFactory
- A subclass of AbstractMicrostructurePreset

The _AbstractMicrostructurePresetFactory_ is the actual factory class that can instantiate an actual concrete implementation of _AbstractMicrostructurePreset_. There is a convenience macro (DECLARE_FACTORY_CLASS) that the programmer can use to quickly create the factory implementation. Each programmer will need to implement the following pure virtual methods from the AbstractMicrostructurePreset class:

    initializeOmega3TableModel(...)
    initializeBOverATableModel(...)
    initializeCOverATableModel(...)
    initializeCOverBTableModel(...)
    initializeNeighborTableModel(...)


There are also optional methods that have a default empty implementation in the AbstractMicrostructurePreset class:

    virtual void initializeAxisODFTableModel(SGAxisODFWidget* widget);
    virtual void initializeODFTableModel(StatsGenODFWidget* widget);
    virtual void initializeMDFTableModel(StatsGenMDFWidget* widget);


If your preset wishes to pre-populate the ODF, Axis ODF and/or MDF Tables then your subclass should reimplement each of the methods that you need. Below is an example class where the programmer just wants to provide data for the various plots but not any preset table entries for the ODF, Axis ODF or MDF tables.

    class EquiaxedPreset : public AbstractMicrostructurePreset
    {
      public:
    MXA_SHARED_POINTERS(EquiaxedPreset);
    MXA_STATIC_NEW_SUPERCLASS(AbstractMicrostructurePreset, EquiaxedPreset);

    virtual ~EquiaxedPreset();

    void displayUserInputDialog();

    void initializeOmega3TableModel(StatsGenPlotWidget* plot,
                            QVector<double> binNumbers);

    void initializeBOverATableModel(StatsGenPlotWidget* plot,
                                QVector<double> binNumbers);

    void initializeCOverATableModel(StatsGenPlotWidget* plot,
                                QVector<double> binNumbers);

    void initializeCOverBTableModel(StatsGenPlotWidget* plot,
                                QVector<double> binNumbers);

    void initializeNeighborTableModel(StatsGenPlotWidget* plot,
                                  QVector<double> binNumbers);

    protected:
    EquiaxedPreset();

    private:
    // Copy Constructor Not Implemented
    EquiaxedPreset(const EquiaxedPreset&);
    // Operator '=' Not Implemented
    void operator=(const EquiaxedPreset&);
    };
    
    // THIS MACRO MUST BE USED IN ORDER TO GET THE FACTORY CLASS CREATED FOR THIS
    // SUBCLASS OF AbstractMicrostructurePreset
    DECLARE_FACTORY_CLASS(EquiaxedPresetFactory, EquiaxedPreset, "Equiaxed");


After that code is written then the programmer needs to write the actual implementation of each of the generateXXXData(...) methods and the optional initializeXXX methods. These methods will generate a set of data that will be inserted into the proper SGAbstractTableModel instance for the given preset. After it is all written we have something like the following to get an instance of the DefaultStatsPreset class:


    AbstractMicrostructurePresetFactory::Pointer presetFactory
                                            = DefaultStatsPresetFactory::New();
    MicrostructurePresetManager::registerFactory(presetFactory);


###  User Input Display
If the programmer wishes they can create a QDialog based class to display options for the preset class to the user. The programmer simply supplies an actual implementation of the **displayUserInputDialog** method in their Preset class. Inside of this method the programmer can do what ever they want to display a Dialog. Some classes use some prebuilt dialogs for their own purpose. For example from the RolledPreset class we have the following:


    void RolledPreset::displayUserInputDialog()
    {
    RolledPresetDialog d(NULL);
    int ret = d.exec();
    if (ret == QDialog::Accepted)
    {
    // The user clicked the OK button so transfer the values from the
    //dialog into this class
    m_AspectRatio = d.getAspectRatio();
    m_RollingFraction = d.getRollingFraction();
    }
    else
    {
    // Perform any cancellation actions if the user canceled the dialog box
    }
    }


This code instantiates a **RolledPresetDialog** class and presents it to the user. If the user clicks the _OK_ button then the code extracts the values from the dialog and stores it in the class. The programmer can then use these values when the default set of data is being generated.

### User Interface Implementation Methods for Preset Dialogs
The programmer is welcome to use any technique to develop their dialog. They can use the QDesigner program to graphically layout their dialog then use the Multiple Inheritance model for the actual implementation details. For simply dialog the programmer may find it just as fast to implement a class deriving from **QDialog** and layout the input features by hand. Both the Rolled Preset and the Recrystallized Preset dialog boxes are done this way. During construction of these codes do not forget to update the CMake files with the necessary entries for the source files and optionally the _uic_ calls.

 








