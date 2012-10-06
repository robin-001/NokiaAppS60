// INCLUDE FILES
#include <eikedwin.h>
#include <eikappui.h>
#include <aknutils.h>
#include <AknAppUi.h>
#include <aknlists.h>
#include <barsread.h>
#include <aknnotewrappers.h>
#include "LBSPositionRequestor.h"
#include <LbsReference.rsg>
#include "LBSReferenceContainer.h"

// CONSTANTS

// Degrees sign delimeter used in formatting methods
_LIT(KDelimDegree,"\xb0"); // "°" symbol

// Dot delimeter used in formatting methods
_LIT(KDelimDot,"\x2e"); // "." symbol

// Plus sign delimeter used in formatting methods
_LIT(KDelimPlus,"\x2b"); // "+" symbol

// Minus sign delimeter used in formatting methods
_LIT(KDelimMinus,"\x2d"); // "-" symbol

// Quotation sign delimeter used in formatting methods
_LIT(KDelimQuot,"\x22"); // "\"" symbol

// Apostrophe sign delimeter used in formatting methods
_LIT(KApostrophe,"\x27"); // "'" symbol

// Not-a-number string
_LIT(KNan,"NaN");

// Date format
_LIT(KDateString,"%F%D.%M.%Y  %:0%H%:1%T%:2%S%:3");

// String constants
_LIT(KTxtModuleName,        "Module name");
_LIT(KTxtLatitude,          "Latitude");
_LIT(KTxtLongitude,         "Longitude");
_LIT(KTxtAltitude,          "Altitude");
_LIT(KTxtHRMS,              "HRMS");
_LIT(KTxtVRMS,              "VRMS");
_LIT(KTxtTime,              "Time");
_LIT(KTxtSpeed,             "Speed");
_LIT(KTxtSpeedAccuracy,     "Speed accuracy");
_LIT(KTxtHeading,           "Heading");
_LIT(KTxtHeadingAccuracy,   "Heading Accuracy");
_LIT(KTxtSatelliteTime,     "Satellite time");
_LIT(KTxtSatellitesInView,  "Satellites in view");
_LIT(KTxtSatellitesUsed,    "Satellites used");

// ================= MEMBER FUNCTIONS =======================

CLbsReferenceContainer::CLbsReferenceContainer()
    :    iListBox(NULL), iItemArray(NULL), iValueBuffer(), iListItemBuffer(),
        iModuleName(), iPositionRequestor(NULL)
    {
    }



// ---------------------------------------------------------
// CLbsReferenceContainer::ConstructL(const TRect& aRect)
// Symbian OS two phased constructor
// ---------------------------------------------------------
//
void CLbsReferenceContainer::ConstructL(const TRect& aRect)
    {
    TResourceReader rr;

     // Create a window for this application view
    CreateWindowL();

    iEikonEnv->CreateResourceReaderLC(rr, R_LOC_LISTBOX_RESOURCE);

    iListBox = new (ELeave) CAknDoubleStyleListBox ();
    iListBox->SetContainerWindowL( *this);
    iListBox->ConstructFromResourceL( rr );
    CleanupStack::PopAndDestroy(); // resource stuffs.

    CEikScrollBarFrame* sb = iListBox->CreateScrollBarFrameL();
    sb->SetScrollBarVisibilityL (CEikScrollBarFrame::EOn, CEikScrollBarFrame::EOn);
    
    MDesCArray* itemList = iListBox->Model()->ItemTextArray();
    iItemArray = (CDesCArray*) itemList;

    // Set the windows size
    SetRect(aRect);

    // Activate the window, which makes it visible
    ActivateL();

       // Create and initialise position requestor
       iPositionRequestor = CLbsPositionRequestor::NewL(*this);
    }

// ----------------------------------------------------
// CLbsReferenceContainer::~CLbsReferenceContainer()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CLbsReferenceContainer::~CLbsReferenceContainer()
    {
    //Delete text control
    delete iListBox;
    
    //Delete requestor
    delete iPositionRequestor;
    }



void CLbsReferenceContainer::AddItemInListL(const TDesC& aLabel, const TDesC& aValue)
    {
       	_LIT(KTab, "\t");
       	iListItemBuffer.Zero();
        iListItemBuffer.Append(KTab);
        iListItemBuffer.Append(aLabel);
        iListItemBuffer.Append(KTab);
        iListItemBuffer.Append(aValue);
        
        iItemArray->AppendL(iListItemBuffer);
    }

// ---------------------------------------------------------
// CLbsReferenceAppUi::PositionInfoUpdatedL() 
// implements MPositionListener::PositionInfoUpdatedL
// Processes given position information and displays it to user
// This method handles TPositionInfo and TPositionSatelliteInfo
// ---------------------------------------------------------
//
void CLbsReferenceContainer::PositionInfoUpdatedL(TPositionInfoBase& aPosInfo, 
    const TDesC& aModulename)
    {
    TInt count = iItemArray->Count();
    iItemArray->Reset();

    // Set the module name
    iModuleName = aModulename;
    AddItemInListL(KTxtModuleName, iModuleName);
    
    // Check the type of the updated position information
    // and process that information.
    
    // Check if position information class type is TPositionSatelliteInfo
    if (aPosInfo.PositionClassType() & EPositionSatelliteInfoClass)
        {
        // Cast the TPositionInfoBase object to TPositionSatelliteInfo
        TPositionSatelliteInfo* posInfo = static_cast<TPositionSatelliteInfo*>(&aPosInfo);

        // Process the satellite information
        ProcessSatelliteInfoL( *posInfo );

        }
    // Check if position information class type is TPositionInfo
    else if (aPosInfo.PositionClassType() & EPositionInfoClass)
        {

        //Cast the TPositionInfoBase object to TPositionInfo
        TPositionInfo* posInfo = static_cast<TPositionInfo*>(&aPosInfo);

        //Process the position information
        ProcessPositionInfoL( *posInfo );

        }
    if (count < iItemArray->Count())
        {
        iListBox->HandleItemAdditionL();
        }
    else if (count > iItemArray->Count())
        {
        iListBox->HandleItemRemovalL();
        }
    else
        {
        iListBox->DrawNow();
        }
    }

// ---------------------------------------------------------
// CLbsReferenceAppUi::ShowErrorL
// Implements MPositionListener::ShowErrorL
// Informs the listener about occurred error during position 
// request process
// @param aErrorString The error in string format
// ---------------------------------------------------------
//
void CLbsReferenceContainer::ShowErrorL(const TDesC& aErrorString)
    {
    TInt count = iItemArray->Count();
    iItemArray->Reset();

	_LIT( KLocError, "Location error");
    AddItemInListL( KLocError, aErrorString);
    
    if (iItemArray->Count() < count)
        {
        iListBox->HandleItemRemovalL();
        }
	iListBox->DrawNow();
    }

// ---------------------------------------------------------
// CLbsReferenceAppUi::ShowMessageL
// Implements MPositionListener::ShowMessageL
// Informs the listener about occurred error during position 
// request process
// @param aMessage The event in string format
// ---------------------------------------------------------
//
void CLbsReferenceContainer::ShowMessageL(const TDesC& aMessage)
    {
    CAknInformationNote* note = new (ELeave) CAknInformationNote();
    note->SetTextL(aMessage);
    note->ExecuteLD();
    }


// ---------------------------------------------------------
// CLbsReferenceContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CLbsReferenceContainer::SizeChanged()
    {
    // Make control use the whole client area
      iListBox->SetRect(Rect());
    }

// ---------------------------------------------------------
// CLbsReferenceContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CLbsReferenceContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CLbsReferenceContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CLbsReferenceContainer::ComponentControl(TInt aIndex) const
    {
    CCoeControl* control = NULL;
    switch ( aIndex )
        {
        // Return label control
        case 0:
            control = iListBox;
            break;
        // return NULL, no components
        default:
            control = NULL;
        }
    return control;
    }

// ---------------------------------------------------------
// CLbsReferenceContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CLbsReferenceContainer::Draw(const TRect& aRect) const
    {
    //Get the graphics context
    CWindowGc& gc = SystemGc();

    //Set pen style
    gc.SetPenStyle(CGraphicsContext::ENullPen);

    //Set brush color
    gc.SetBrushColor(KRgbWhite);

    //Set brush style
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);

    //Draw the window area
    gc.DrawRect(aRect);
    }

// -----------------------------------------------------------------------------
// CLbsReferenceContainer::OfferKeyEventL
// Handles key events. Basically it just forwards them to listbox component.
// -----------------------------------------------------------------------------
//
TKeyResponse CLbsReferenceContainer::OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType)
    {
    return iListBox->OfferKeyEventL(aKeyEvent, aType);            
    }

// -----------------------------------------------------------------------------
// CLbsReferenceContainer::ProcessPositionInfo
// Processes position info in to a string representation.
// -----------------------------------------------------------------------------
//
void CLbsReferenceContainer::ProcessPositionInfoL(
    const TPositionInfo& aPositionInfo )
    {       
    const TInt KRealNumberFigureAmount = 2;
    const TInt KRealNumberWidth = 5;

    // Standard location information
    TPosition position;
    TRealFormat format;
    format.iPlaces = KRealNumberFigureAmount;
    format.iWidth = KRealNumberWidth;

    // Get basic position data
    aPositionInfo.GetPosition(position);

    // Get the latitude and format it
    TBuf<KDegreeLength> latitude;
    GetDegreesString(position.Latitude(), latitude);

    // Get the longitude and format it
    TBuf<KDegreeLength> longitude;
    GetDegreesString(position.Longitude(), longitude);

    //Get the altitude
    TReal altitude = position.Altitude();

    // Get the horizontal accuracy
    TReal horizontalAccuracy = position.HorizontalAccuracy();

    // Get the vertical accuracy
    TReal verticalAccuracy = position.VerticalAccuracy();

    // Get the timestamp of the fix
    TTime time = position.Time();
    
    // Temporary buffer for date formatting
    TBuf<KDateTimeLength> positionDateTime;
    
    //Format dates and times
    time.FormatL(positionDateTime, KDateString);

    // Clear the buffer
    iValueBuffer.Zero();

    AddItemInListL(KTxtLatitude, latitude);
    AddItemInListL(KTxtLongitude, longitude);

    iValueBuffer.Zero();
    iValueBuffer.AppendNum(altitude, format);
    AddItemInListL(KTxtAltitude, iValueBuffer);

    iValueBuffer.Zero();
    iValueBuffer.AppendNum(horizontalAccuracy, format);
    AddItemInListL(KTxtHRMS, iValueBuffer);

    iValueBuffer.Zero();
    iValueBuffer.AppendNum(verticalAccuracy, format);
    AddItemInListL(KTxtVRMS, iValueBuffer);

    iValueBuffer.Zero();
    iValueBuffer.Append(positionDateTime);
    AddItemInListL(KTxtTime, positionDateTime);
    
    }

// -----------------------------------------------------------------------------
// CLbsReferenceContainer::ProcessPositionInfo
// Processes satellite position info in to a string representation.
// -----------------------------------------------------------------------------
//

void CLbsReferenceContainer::ProcessSatelliteInfoL( 
    const TPositionSatelliteInfo& aSatelliteInfo )
    {
    const TReal SpeedRatio = 36.0/10.0;
    const TInt KRealNumberFigureAmount = 2;
    const TInt KRealNumberWidth = 5;
    
    // Process the basic info
    ProcessPositionInfoL(static_cast<TPositionInfo>(aSatelliteInfo));
          
    // The course info
    TCourse course;

    // Get the satellite info
    aSatelliteInfo.GetCourse(course);

    // Get the speed (m/s)
    TReal speed = course.Speed();

    // Convert to km/h if the speedAccuracy is not NaN
    if ( !Math::IsNaN(STATIC_CAST(TReal64, speed)) )
        {
        // Convert to km/h
        speed = speed * SpeedRatio;
        }

    // Get the speed accuracy (m/s)
    TReal speedAccuracy = course.SpeedAccuracy();

    // Convert to km/h if the speedAccuracy is not NaN
    if ( !Math::IsNaN(speedAccuracy) )
        {
        // Convert to km/h
        speedAccuracy = speedAccuracy * SpeedRatio;
        }

    // Get the heading (degrees)
    TReal heading = course.Heading();

    // Get the heading accuracy (degrees)
    TReal headingAccuracy = course.HeadingAccuracy();

    // Get the satellite time
    TTime time = aSatelliteInfo.SatelliteTime();

    // Format the date
    TBuf<KDateTimeLength> satelliteDateTime;

     //Format dates and times
    time.FormatL(satelliteDateTime,KDateString);

    // Get the number of visible satellites
    TInt numSatellitesInView = aSatelliteInfo.NumSatellitesInView();

    // Get the number of satellites utilized
    TInt numSatellitesUsed = aSatelliteInfo.NumSatellitesUsed();

    TRealFormat format;
    format.iPlaces = KRealNumberFigureAmount;
    format.iWidth = KRealNumberWidth;
    
    // Append the information in list box item array
    iValueBuffer.Zero();
    iValueBuffer.AppendNum(speed, format);
    AddItemInListL(KTxtSpeed, iValueBuffer);

    iValueBuffer.Zero();
    iValueBuffer.AppendNum(speedAccuracy, format);
    AddItemInListL(KTxtSpeedAccuracy, iValueBuffer);

    iValueBuffer.Zero();
    iValueBuffer.AppendNum(heading, format);
    AddItemInListL(KTxtHeading, iValueBuffer);

    iValueBuffer.Zero();
    iValueBuffer.AppendNum(headingAccuracy, format);
    AddItemInListL(KTxtHeadingAccuracy, iValueBuffer);

    AddItemInListL(KTxtSatelliteTime, satelliteDateTime);

    TInt64 value = numSatellitesInView;
    iValueBuffer.Zero();
    iValueBuffer.AppendNum(value);
    AddItemInListL(KTxtSatellitesInView, iValueBuffer);

    value = numSatellitesUsed;
    iValueBuffer.Zero();
    iValueBuffer.AppendNum(value);
    AddItemInListL(KTxtSatellitesUsed, iValueBuffer);

    }

// -----------------------------------------------------------------------------
// CLbsReferenceContainer::GetDegreesString
// Changes latitude or longitude represented as degrees to a string of 
// a form +DDD'MM''SS.SSSS
// -----------------------------------------------------------------------------
//

void CLbsReferenceContainer::GetDegreesString(
    const TReal64& aDegrees,TBuf<KDegreeLength>& aDegreesString) const
    {
    const TReal KSecondsInMinute = 60.0;
    const TInt KNumWidth = 3;
    
    // If the aDegree is a proper number
    if ( !Math::IsNaN(aDegrees) )
        {
        // Integer part of the degrees
        TInt intDegrees = static_cast<TInt>(aDegrees);

        // Positive float of the degrees
        TReal64 realDegrees = aDegrees;
        
        // Convert to positive values
        if ( intDegrees < 0 )
            {
            intDegrees = -intDegrees;
            realDegrees = -realDegrees;
            }

        // Minutes
        TReal64 realMinutes = (realDegrees - intDegrees) * KSecondsInMinute;
          
        // Integer part of the minutes
        TInt intMinutes = static_cast<TInt>(realMinutes);

        // Seconds
        TReal64 realSeconds = (realMinutes - intMinutes) * KSecondsInMinute;
        TInt intSeconds = static_cast<TInt>((realMinutes - intMinutes) * KSecondsInMinute);

        // Check the sign of the result
        if ( aDegrees >= 0 )
            {
            aDegreesString.Append(KDelimPlus); 
            }
        else
            {
            aDegreesString.Append(KDelimMinus);
            }

        // Add the degrees
        TInt64 value = intDegrees;
        aDegreesString.AppendNum(value);

        // Add the separator
        aDegreesString.Append(KDelimDegree);
    
        // Add the minutes
        value = intMinutes;
        aDegreesString.AppendNum(value);

        // Add the separator
        aDegreesString.Append(KApostrophe);
        
        // Add the seconds
        value = intSeconds;
        aDegreesString.AppendNum(value);

        // Add the separator
        aDegreesString.Append(KDelimQuot);

        // Add the separator
        aDegreesString.Append(KDelimDot);
        
        // Get six last digits
        realSeconds -= intSeconds;
        realSeconds *= 1000;
        
        // Add the seconds
        aDegreesString.AppendNumFixedWidth(static_cast<TInt>(realSeconds), EDecimal, KNumWidth);
        }
    else
        {
        // The conversion can not be done, return NaN
        aDegreesString = KNan;
        }
    }

// -----------------------------------------------------------------------------
// CLbsReferenceContainer::HandleResourceChange
// Handle Dynamic Screen Size change and Skin Change events and update
// controls accordingly. 
// -----------------------------------------------------------------------------
//
void CLbsReferenceContainer::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);
    switch( aType )
    	{
    	case KAknsMessageSkinChange:
        	{             
        	iListBox->HandleResourceChange( aType );
        	break;
        	}
    	case KEikDynamicLayoutVariantSwitch:
    		{
    		SetRect(iEikonEnv->EikAppUi()->ClientRect());
			break;
    		}
    	default:
    		{
    		break;
    		}
    	}
    }
    
// End of File
