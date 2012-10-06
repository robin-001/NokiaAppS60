/*
* ==============================================================================
*  Name        : LbsPositionListener.h
*  Part of     : LBSReference example application
*  Interface   : 
*  Description : Position listener interface
*  Version     : 1.0
*
*  Copyright (c) 2005-2006 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef LBSPOSITIONLISTENER_H
#define LBSPOSITIONLISTENER_H

#include <lbsSatellite.h>

class TPositionInfo;
class TPositionSatelliteInfo;

/**
*   Position data listener interface listens position information.
*   It can also be used to transfer error messages and codes.
*/
class MLbsPositionListener
    {
    public:

        /**
        * PositionInfoUpdatedL informs the listener about new location information
        * @param aModulename The name of the module which provided this information
        * @param aPosInfo Position information class
        */
        virtual void PositionInfoUpdatedL(TPositionInfoBase& aPosInfo,
            const TDesC& aModulename) = 0;

        /**
        * ShowErrorL informs the listener about occurred error during position 
        * request process
        * @param aErrorString The error in string format
        */
        virtual void ShowErrorL(const TDesC& aErrorString) = 0;
        
        /**
        * ShowMessageL informs the listener about occurred event during position 
        * request process
        * @param aMessage The message in string format
        */
        virtual void ShowMessageL(const TDesC& aMessage) = 0;
        
    };

#endif //LBSPOSITIONLISTENER_H


// End of File
