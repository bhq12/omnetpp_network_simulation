//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef TRANSCEIVER_H_
#define TRANSCEIVER_H_
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omnetpp.h>
#include "globals.h"
#include "MACMessage_m.h"
#include "AppMessage_m.h"
#include "SignalStartMessage_m.h"
#include "SignalEndMessage_m.h"
#include "CSRequest_m.h"
#include "CSResponse_m.h"

using namespace omnetpp;


class Transceiver : public cSimpleModule {

public:
    typedef enum{
        Transmit = 0,
        Receive
    } TransceiverState_t;

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage* msg);
        virtual void handleCSRequest(CSRequest* csRequest);
        virtual void handleMacMessage(MacMessage* appMsg);
        virtual void handleInternalSignals(cMessage* msg);
        virtual void handleSignalStartMessage(SignalStartMessage* startMsg);
        virtual void handleSignalEndMessage(SignalEndMessage* endMsg);
        virtual double findChannelPowerDB();
        virtual double DBToRatio(double num);
        virtual double RatioToDB(double num);
        virtual double findPacketPowerDB(SignalStartMessage* msg);
        virtual double findTransmitterDistance(SignalStartMessage* msg);
        virtual double findPowerLossDB(double distance);

    public:
        Transceiver();
        virtual ~Transceiver();
        double txPowerDBm;
        int bitRate;//measured in bits/s
        double csThreshDBm;//threshold used to detect whether the medium is busy
        double noisePowerDBm;
        double turnAroundTime;
        double csTime;

        TransceiverState_t state;

        std::vector<SignalStartMessage*> currentTransmissions;
        int nodeXPosition;
        int nodeYPosition;
};

#endif /* TRANSCEIVER_H_ */
