// INCLUDE FILES
#include <Lbs.h>
#include "LBSPositionRequestor.h"
#include "LBSPositionlistener.h"

// CONSTANTS

//Second
const TInt KSecond = 1000000;

//Update interval
const TInt KUpdateInterval = KSecond;

//Update time out
const TInt KUpdateTimeOut = 15*KSecond;

//MaxAge
const TInt KMaxAge = 500000;

// Unknown string used when module name is not known
_LIT(KUnknown,"Unknown");

//The name of the requestor
_LIT(KRequestor,"Location Reference Application");

// Event messages
_LIT(KLastPosUnknown,"Last known position unknown.");

//Error messages
_LIT(KLbsErrLocRequest,     "Request error: %d.");
_LIT(KLbsErrAccess,         "Access denied: %d.");
_LIT(KLbsErrPosServConn,    "Connecting to server: %d.");
_LIT(KLbsErrOpenPos,        "Opening a positioner: %d.");
_LIT(KLbsErrSetRequestor,   "Setting requestor: %d.");
_LIT(KLbsErrSetUpOpt,       "Setting update options: %d.");
_LIT(KLbsErrCanceled,       "Location request canceled.");
_LIT(KLbsErrQualityLoss,    "KPositionQualityLoss.");
_LIT(KLbsErrTimedout,       "Timed out.");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLbsPositionRequestor::CLbsPositionRequestor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLbsPositionRequestor::CLbsPositionRequestor( MLbsPositionListener& aPositionListener )
    : CActive(CActive::EPriorityStandard),
    iPositionListener( aPositionListener ),
    iPosInfoBase( &iPositionInfo ),
    iGettingLastknownPosition( ETrue )
    {
    // Set update interval to one second to receive one position data per second
    iUpdateops.SetUpdateInterval(TTimeIntervalMicroSeconds(KUpdateInterval));

    // If position server could not get position
    // In two minutes it will terminate the position request
    iUpdateops.SetUpdateTimeOut(TTimeIntervalMicroSeconds(KUpdateTimeOut));

    // Positions which have time stamp below KMaxAge can be reused
    iUpdateops.SetMaxUpdateAge(TTimeIntervalMicroSeconds(KMaxAge));

    // Enables location framework to send partial position data
    iUpdateops.SetAcceptPartialUpdates(ETrue);

    // Add this position requestor to the active scheduler
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CLbsPositionRequestor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLbsPositionRequestor::ConstructL()
    {
    // Initialise the position request sequence
    DoInitialiseL();
    }

// -----------------------------------------------------------------------------
// CLbsPositionRequestor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLbsPositionRequestor* CLbsPositionRequestor::NewL(
                                        MLbsPositionListener& aPositionListener )
    {
    //Create the object
    CLbsPositionRequestor* self = new( ELeave ) CLbsPositionRequestor(
        aPositionListener);

    //Push to the cleanup stack
    CleanupStack::PushL( self );

    //Construct the object
    self->ConstructL();

    //Remove from cleanup stack
    CleanupStack::Pop( self );

    //Return pointer to the created object
    return self;
    }

// -----------------------------------------------------------------------------
// CLbsPositionRequestor::~CLbsPositionRequestor
// Destructor
// -----------------------------------------------------------------------------
//
CLbsPositionRequestor::~CLbsPositionRequestor()
    {
    // Cancel active object
    Cancel();

    // Close the positioner
    iPositioner.Close();

    // Close the session to the position server
    iPosServer.Close();
    }

// -----------------------------------------------------------------------------
// DoCancel() implements CActive DoCancel()
// Implements the cancellation of an outstanding request.
// -----------------------------------------------------------------------------
//
void CLbsPositionRequestor::DoCancel()
    {
    // When a client application wishes to close one of its connections to Location
    // Server, there can be no outstanding requests on that particular connection
    // If a client application attempts to close a connection before outstanding
    // requests have been cancelled or completed, it is panicked

    //If we are getting the last known position
    if ( iGettingLastknownPosition )
        {
        //Cancel the last known position request
        iPositioner.CancelRequest(EPositionerGetLastKnownPosition);
        }
    else
        {
        iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
        }
    }

// -----------------------------------------------------------------------------
// RunL() implements CActive RunL()
// Handles an active object’s request completion event.
// -----------------------------------------------------------------------------
//
void CLbsPositionRequestor::RunL()
    {
    TBuf<KPositionMaxModuleName> buffer;
    
    switch ( iStatus.Int() )
        {
        // The fix is valid
        case KErrNone:
        // The fix has only partially valid information.
        // It is guaranteed to only have a valid timestamp
        case KPositionPartialUpdate:
            {

            // Pre process the position information
            PositionUpdatedL();

            break;
            }
        // The data class used was not supported
        case KErrArgument:
            {
            // Set info base to position info
            iPosInfoBase = &iPositionInfo;

             // Request next position
            iPositioner.NotifyPositionUpdate( *iPosInfoBase, iStatus );

            // Set this object active
            SetActive();
            break;
            }
        // The position data could not be delivered
        case KPositionQualityLoss:
            {
            // Send event to position listener
            iPositionListener.ShowErrorL( KLbsErrQualityLoss );

            if ( iGettingLastknownPosition )
                {
                //Change the data class type
                iPosInfoBase = &iSatelliteInfo;
                }

            // Request position
            iPositioner.NotifyPositionUpdate( *iPosInfoBase, iStatus );

            // Set this object active
            SetActive();
            break;
            }
        // Access is denied
        case KErrAccessDenied:
            {
            // Send error to position listener
            buffer.Format(KLbsErrAccess, iStatus.Int());
            iPositionListener.ShowErrorL(buffer);
            break;
            }
        // Request timed out
        case KErrTimedOut:
            {
            // Send error to position listener
            iPositionListener.ShowErrorL(KLbsErrTimedout);

            if ( iGettingLastknownPosition )
                {
                //Change the data class type
                iPosInfoBase = &iSatelliteInfo;
                }

            // Request position
            iPositioner.NotifyPositionUpdate( *iPosInfoBase, iStatus );

            // Set this object active
            SetActive();
            break;
            }
        // The request was canceled
        case KErrCancel:
            {
            // Send error to position listener
            iPositionListener.ShowErrorL( KLbsErrCanceled );
            break;
            }
        // There is no last known position
        case KErrUnknown:
            {
            // Send event to position listener
            iPositionListener.ShowMessageL(KLastPosUnknown);

            if ( iGettingLastknownPosition )
                {
                //Change the data class type
                iPosInfoBase = &iSatelliteInfo;

                //Mark that we are not requesting NotifyPositionUpdate
                iGettingLastknownPosition = EFalse;
                }

            // Request next position
            iPositioner.NotifyPositionUpdate( *iPosInfoBase, iStatus );

            // Set this object active
            SetActive();
            break;
            }
        // Unrecoverable errors.
        default:
            {
            // Send error to position listener
            buffer.Format(KLbsErrLocRequest, iStatus.Int());
            iPositionListener.ShowErrorL( buffer );
            break;
            }
        }

         //We are not going to query the last known position anymore.
        if ( iGettingLastknownPosition )
            {
            //Mark that we are not requesting NotifyPositionUpdate
            iGettingLastknownPosition = EFalse;
            }
    }


// -----------------------------------------------------------------------------
// CLbsPositionRequestor::DoInitialiseL
// Initialises position server and positioner and
// begins the position request sequence.
// -----------------------------------------------------------------------------
//
void CLbsPositionRequestor::DoInitialiseL()
    {
    // Connect to the position server
    TInt error = iPosServer.Connect( );
    TBuf<100> buffer;
    
    // The connection failed
    if ( KErrNone != error )
        {
        // Show error to the user and leave
        buffer.Format(KLbsErrPosServConn, error);
        iPositionListener.ShowErrorL( buffer );
        return;
        }

    // Open subsession to the position server
    error = iPositioner.Open(iPosServer);

    // The opening of a subsession failed
    if ( KErrNone != error )
        {
        // Show error to the user and leave
        buffer.Format(KLbsErrOpenPos, error);
        iPositionListener.ShowErrorL( buffer );
        iPosServer.Close();
        return;
        }

    // Set position requestor
    error = iPositioner.SetRequestor( CRequestor::ERequestorService ,
         CRequestor::EFormatApplication , KRequestor );

    // The requestor could not be set
    if ( KErrNone != error )
        {
        // Show error to the user and leave

        buffer.Format(KLbsErrSetRequestor, error);
        iPositionListener.ShowErrorL( buffer );
        iPositioner.Close();
        iPosServer.Close();
        return;
        }

    // Set update options
    error =  iPositioner.SetUpdateOptions( iUpdateops );

    // The options could not be updated
    if ( KErrNone != error  )
        {
        // Show error to the user and leave
        buffer.Format(KLbsErrSetUpOpt, error);
        iPositionListener.ShowErrorL( buffer );
        iPositioner.Close();
        iPosServer.Close();
        return;
        }

    // Get last known position. The processing of the result
    // is done in RunL method
    iPositioner.GetLastKnownPosition(*iPosInfoBase,iStatus);

    // Set this active object active
    SetActive();
    }

// -----------------------------------------------------------------------------
// CLbsPositionRequestor::GetModuleName
// Gets the name of the positioning module by id.
// -----------------------------------------------------------------------------
//
void CLbsPositionRequestor::GetModuleName(const TPositionModuleId& aModuleId)
    {
    // Module information of the position module
    TPositionModuleInfo modInfo;

    // Get module info by module id
    TInt error = iPosServer.GetModuleInfoById( *&aModuleId, modInfo );

    // If error occured
    if ( KErrNone != error )
        {
        // Set the name of the module to 'Unknown'
        iModuleName = KUnknown;
        }
    else
        {
        // Get the name of the position module
        modInfo.GetModuleName(iModuleName);
        }
    }


// -----------------------------------------------------------------------------
// CLbsPositionRequestor::PositionUpdatedL
// Pre process the position information
// -----------------------------------------------------------------------------
//
void CLbsPositionRequestor::PositionUpdatedL()
    {
    //If we are getting the last known position
    if ( iGettingLastknownPosition )
        {
        // Set the module name to 'Unknown' because last
        // known position has no module name
        iModuleName = KUnknown;

        // Send position information to registered listener
        iPositionListener.PositionInfoUpdatedL(*iPosInfoBase,iModuleName);

        //Change the data class type
        iPosInfoBase = &iSatelliteInfo;

        // Request next position
        iPositioner.NotifyPositionUpdate( *iPosInfoBase, iStatus );

        // Set this object active
        SetActive();
        }
    else
        {
        // Check if the id of the used PSY is 0
        if ( 0 == iUsedPsy.iUid)
            {
            // Set the id of the currently used PSY
            iUsedPsy = iPosInfoBase->ModuleId();
            }
        // Check if the position module has changed
        else if ( iPosInfoBase->ModuleId() != iUsedPsy )
            {
            // Set the id of the currently used PSY
            iUsedPsy = iPosInfoBase->ModuleId();

            //Position module info of new module
            TPositionModuleInfo moduleInfo;

            // Get module info
            iPosServer.GetModuleInfoById(iUsedPsy,moduleInfo);

            // Get classes supported
            TInt32 moduleInfoFamily = moduleInfo.ClassesSupported(EPositionInfoFamily);

            iPosInfoBase = &iSatelliteInfo;

            // Check if the new module supports
            // TPositionSatelliteInfo class
            if ( EPositionSatelliteInfoClass & moduleInfoFamily )
                {
                // Set info base to satellite info
                iPosInfoBase = &iSatelliteInfo;
                }
            // The position module must support atleast
            // TPositionInfo class
            else
                {
                // Set info base to position info
                iPosInfoBase = &iPositionInfo;
                }
            }

        // Process the position information
        // and request next position

        // Get module name
        GetModuleName(iUsedPsy);

        // Send position information to registered listener
        iPositionListener.PositionInfoUpdatedL(*iPosInfoBase,iModuleName);

        // Request next position
        iPositioner.NotifyPositionUpdate( *iPosInfoBase, iStatus );

        // Set this object active
        SetActive();
        }
    }

//  End of File
