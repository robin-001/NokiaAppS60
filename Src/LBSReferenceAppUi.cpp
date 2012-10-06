// INCLUDE FILES
#include <avkon.hrh>
#include "LBSReferenceAppUi.h"
#include "LBSReferenceContainer.h"



// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CLbsReferenceAppUi::ConstructL()
// Symbian OS two phased constructor
// ----------------------------------------------------------
//
void CLbsReferenceAppUi::ConstructL()
    {
    // Perform the base class construction
    BaseConstructL(EAknEnableSkin);

    //Create container
    iAppContainer = new (ELeave) CLbsReferenceContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect() );

    // Allow the appview to receive keyboard input
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CLbsReferenceAppUi::~CLbsReferenceAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CLbsReferenceAppUi::~CLbsReferenceAppUi()
    {
    // Cleanup
    if ( iAppContainer )
        {
        //Remove from stack
        RemoveFromStack( iAppContainer );

        //Delete the object
        delete iAppContainer;
        }
   }


// ----------------------------------------------------
// CLbsReferenceAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
//
// ----------------------------------------------------
//
TKeyResponse CLbsReferenceAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    // The key event was not handled
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CLbsReferenceAppUi::HandleCommandL(TInt aCommand)
//
// ----------------------------------------------------
//
void CLbsReferenceAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        // Exit the application
        case EEikCmdExit:
        case EAknSoftkeyBack:
        case EAknSoftkeyExit:
            {
            //Exit
            Exit();
            break;
            }
        // Do nothing.
        default:
            break;
        }
    }

// End of File
