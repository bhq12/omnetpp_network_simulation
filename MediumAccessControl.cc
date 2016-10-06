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

#include <MediumAccessControl.h>

Define_Module(MAC);

void MAC::initialize(){
    //this is called at the beginning of the simulation
    bufferSize  = (int)par("bufferSize");
    maxBackoffs = par("maxBackoffs");
    backoffDistribution = par("backoffDistribution");
    backoffs = 0;
    droppedPackets = 0;
}

void MAC::handleAppMessage(AppMessage* appMsg){
    if(appMsg){
            addToBuffer(appMsg);
    }
}

void MAC::handleTransmissionConfirm(TransmissionConfirm* transmissionConfirm){
    if(transmissionConfirm){
        //TODO: probably need to do something here? Or maybe do something if no confirmation is received
        delete transmissionConfirm;
    }
}

void MAC::refreshDisplay() const
{
    char buf[40];
    sprintf(buf, "Dropped: %ld      ", droppedPackets);
    getDisplayString().setTagArg("t", 0, buf);
}

void MAC::handleTransmissionIndication(TransmissionIndication* transmissionIndication){
    if(transmissionIndication){
        MacMessage* macMsg = dynamic_cast<MacMessage*>(transmissionIndication->decapsulate());
        delete transmissionIndication;
        EV_INFO << "Packet decapsulated (mac to app)" << endl;

        AppMessage* appMsg = dynamic_cast<AppMessage*>(macMsg->decapsulate());
        send(appMsg, "packetsOut");

        EV_INFO << "Packet deleted (delete mac msg)" << endl;

        delete macMsg;
    }
}

void MAC::handleCSResponse(CSResponse* csResponse){
    //detect channel if channel busy, send back csResponse with result
    if(csResponse){
        bool isBusy = csResponse->getBusyChannel();
        EV_INFO << "Packet deleted (reponse)" << endl;

        delete csResponse;

        if(isBusy){

            if(backoffs < maxBackoffs){
                double waitTime = exponential(backoffDistribution);
                scheduleAt(simTime() + waitTime, new cMessage("RETRY_CARRIER_SENSE"));

            }
            else{
                //max backoffs exceeded, drop the packet
                AppMessage* msg = buffer.front();
                buffer.pop();
                delete msg;
            }
            backoffs++;
        }
        else{
            transmit();
        }


    }

}

void MAC::handleInternalSignals(cMessage* msg){
    const char* name = msg->getName();
    if (strcmp("RETRY_CARRIER_SENSE", name) == 0){

        CSRequest* csRequest = new CSRequest();
        send(csRequest, "transceiverOut");
        delete msg;


    }
}

void MAC::handleMessage(cMessage* msg){
    //this is called whenever a msg arrives at the computer

    AppMessage* appMsg = dynamic_cast<AppMessage*>(msg);
    TransmissionIndication* transmissionIndication = dynamic_cast<TransmissionIndication*>(msg);
    CSResponse* csResponse = dynamic_cast<CSResponse*>(msg);
    TransmissionConfirm* transmissionConfirm = dynamic_cast<TransmissionConfirm*>(msg);

    handleInternalSignals(msg);
    handleAppMessage(appMsg);
    handleTransmissionIndication(transmissionIndication);
    handleCSResponse(csResponse);
    handleTransmissionConfirm(transmissionConfirm);


}

void MAC::transmit(void){
    AppMessage* msg = buffer.front();
    buffer.pop();
    MacMessage* macMsg = new MacMessage();
    macMsg->setBitLength(msg->getMsgSize());

    TransmissionRequest* transmit = new TransmissionRequest();
    EV_INFO << "Packet encapsulated app to mac" << endl;

    macMsg->encapsulate(msg);
    transmit->encapsulate(macMsg);
    msg =  nullptr;
    macMsg = nullptr;
    send(transmit, "transceiverOut");

}

bool MAC::addToBuffer(AppMessage* msg){
    //returns true if packet successfully buffered, false if dropped
    if(buffer.size() < bufferSize){
        buffer.push(msg);

        CSRequest* csRequest = new CSRequest();
        send(csRequest, "transceiverOut");
        return true;
    }
    else{
        //drop packet
        droppedPackets++;
        delete msg;
        return false;

    }
}


MAC::MAC() {

}

MAC::~MAC() {
    // TODO Auto-generated destructor stub
}

