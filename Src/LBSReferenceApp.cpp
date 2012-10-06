// INCLUDE FILES
#include "LBSReferenceApp.h"
#include "LBSReferenceDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CLbsReferenceApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CLbsReferenceApp::AppDllUid() const
    {
    // Return the UID for the Animation application
    return KUidLBSReference;
    }

// ---------------------------------------------------------
// CLbsReferenceApp::CreateDocumentL()
// Creates CLBSReferenceDocument object
// ---------------------------------------------------------
//
CApaDocument* CLbsReferenceApp::CreateDocumentL()
    {
    // Create an Reference application document, and return a pointer to it
    return CLbsReferenceDocument::NewL( *this );
    }


// End of File

