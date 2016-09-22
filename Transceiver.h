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
#include <string.h>
#include <omnetpp.h>
#include "AppMessage_m.h"
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
        virtual void handleAppMessage(AppMessage* appMsg);
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
};

#endif /* TRANSCEIVER_H_ */
