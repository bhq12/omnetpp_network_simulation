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
#ifndef MAC_H_
#define MAC_H_
#include <omnetpp.h>
#include <string.h>
#include <queue>
#include "AppMessage_m.h"
#include "MacMessage_m.h"
#include "CSResponse_m.h"
#include "CSRequest_m.h"
#include "TransmissionRequest_m.h"
#include "TransmissionConfirm_m.h"
#include "TransmissionIndication_m.h"
#include <iostream>
#include <fstream>
using namespace omnetpp;
class MAC : public cSimpleModule {
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage* msg);
        virtual bool addToBuffer(AppMessage* msg);
        virtual void transmit(void);
        virtual void handleAppMessage(AppMessage* appMsg);
        virtual void handleCSResponse(CSResponse* csResponse);
        virtual void handleTransmissionIndication(TransmissionIndication* transmissionIndication);
        virtual void handleTransmissionConfirm(TransmissionConfirm* transmissionConfirm);
        virtual void handleInternalSignals(cMessage* msg);
        virtual void refreshDisplay() const;
        virtual void finish();
    public:
        MAC();
        virtual ~MAC();
        int bufferSize;
        int maxBackoffs;
        double backoffDistribution;

        std::queue<AppMessage*> buffer;
        int backoffs;
        long droppedPackets;
        const char* logFileName;


};

#endif /* MEDIUMACCESSCONTROL_H_ */
