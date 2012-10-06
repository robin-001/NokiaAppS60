

#ifndef LBSREFERENCECONTAINER_H
#define LBSREFERENCECONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include "LBSPositionListener.h"

// FORWARD DECLARATIONS
class CAknDoubleStyleListBox;
class CLbsPositionRequestor;

// CONSTANTS

// Length of degree buffer
const TInt KDegreeLength = 19;

// Length of time buffer
const TInt KDateTimeLength = 125;

// maximum length a value string may have
const TInt KValueMaxLength = 30;

// CLASS DECLARATION

/**
*  CLbsReferenceContainer container control class.
*
*/
class CLbsReferenceContainer : public CCoeControl, 
                               public MLbsPositionListener
    {
    public: // Constructors and destructor

        /**
        * Constructor
        */
        CLbsReferenceContainer();

        /**
        * Symbian default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        ~CLbsReferenceContainer();
                
    private:

        /**
        * Changes latitude or longitude represented as degrees to 
        * a string of a form +DDD'MM''SS.SSSS
        * @param aDegrees Degrees to trasform
        * @param aDegreesString Degrees string to be returned
        */
        void GetDegreesString(
            const TReal64& aDegrees,TBuf<KDegreeLength>& aDegreesString) const;

        /**
        * Processes position info to string representation.
        * @param aPositionInfo Position information to be processed
        */
        void ProcessPositionInfoL( const TPositionInfo& aPositionInfo );

        /**
        * Processes satellite position info to string representation.
        * @param aSatelliteInfo Position information to be processed.
        */
        void ProcessSatelliteInfoL( 
            const TPositionSatelliteInfo & aSatelliteInfo );

    public: // Functions from base classes


        // -----------------------------------------------------------------------------
        // From MPositionListener
        // -----------------------------------------------------------------------------
        //
        void PositionInfoUpdatedL(TPositionInfoBase& aPosInfo,
            const TDesC& aModulename);   

        void ShowErrorL(const TDesC& aErrorString);

        void ShowMessageL(const TDesC& aMessage);
        
        // -----------------------------------------------------------------------------
        // From CCoeControl
        // -----------------------------------------------------------------------------
        //
        virtual TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);

    private:
        // Own helper functions
        void AddItemInListL(const TDesC& aLabel, const TDesC& aValue);

        // Functions from base classes

        // From CoeControl      
        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl(TInt aIndex) const;
        void Draw(const TRect& aRect) const;
		void HandleResourceChange(TInt aType);
		
    private: //data

        // list box control
        CAknDoubleStyleListBox* iListBox;

        // listbox item array, not owned        
        CDesCArray* iItemArray;

        // Buffer for storing values

        TBuf<KValueMaxLength> iValueBuffer;

        // Buffer dynamicly filling in a list item
        TBuf<KPositionMaxModuleName + KValueMaxLength> iListItemBuffer;
        
        // Module name
        TBuf<KPositionMaxModuleName> iModuleName;
        
        // Position requestor
        CLbsPositionRequestor* iPositionRequestor;
    };

#endif //LBSREFERENCECONTAINER_H

// End of File
