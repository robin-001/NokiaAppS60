
#ifndef LBSREFERENCEDOCUMENT_H
#define LBSREFERENCEDOCUMENT_H

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  This class creates the user interface of the application
*/
class CLbsReferenceDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CLbsReferenceDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CLbsReferenceDocument();

    private:

        /**
        * Symbian OS default constructor.
        */
        CLbsReferenceDocument(CEikApplication& aApp);
        
        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CLBSReferenceAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif //LBSREFERENCEDOCUMENT_H

// End of File

