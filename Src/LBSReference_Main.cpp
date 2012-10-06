
#include "LbsReferenceApp.h"
#include <eikstart.h>

// ---------------------------------------------------------
// NewApplication()
// Constructs CLbsReferenceApp
// Returns: created application object
// ---------------------------------------------------------
//

EXPORT_C CApaApplication* NewApplication()
    {
    // Create a Reference application , and return a pointer to it
    return new CLbsReferenceApp;
    }


// ---------------------------------------------------------
// E32Main()
// The entry point for the application code. It creates, via a
// call to NewApplication() an instance of the CApaApplication
// derived class, CLbsReferenceApp.
// ---------------------------------------------------------

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }


