
#ifndef LBSREFERENCEAPP_H
#define LBSREFERENCEAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS

// UID of the application
const TUid KUidLBSReference = { 0xA0000189 };

// CLASS DECLARATION

/**
* CLbsReferenceApp application class.
* Provides factory to create concrete document object.
*
*/
class CLbsReferenceApp : public CAknApplication
    {
    private:

        /**
        * From CApaApplication, returns application's UID (KUidLBSReference).
        * @return The value of KUidLBSReference.
        */
        TUid AppDllUid() const;

        /**
        * From CApaApplication, creates CLBSReferenceDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif //LBSREFERENCEAPP_H

// End of File

