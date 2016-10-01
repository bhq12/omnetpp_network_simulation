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
    backoffs = 0;
}

void MAC::handleAppMessage(AppMessage* appMsg){
    if(appMsg){
            addToBuffer(appMsg);
    }
}

void MAC::handleMacMessage(MacMessage* macMsg){
    if(macMsg){
        //delete macMsg;
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

                scheduleAt(simTime() + waitTime, new cMessage("retry carrier sense"));


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

void MAC::handleMessage(cMessage* msg){
    //this is called whenever a msg arrives at the computer

    AppMessage* appMsg = dynamic_cast<AppMessage*>(msg);
    MacMessage* macMsg = dynamic_cast<MacMessage*>(msg);
    CSResponse* csResponse = dynamic_cast<CSResponse*>(msg);


    handleAppMessage(appMsg);
    handleMacMessage(macMsg);
    handleCSResponse(csResponse);

    if(buffer.size() > 0){
        //perform carrier sense in order to send packets
        EV_INFO << "Packet created (request)" << endl;

        CSRequest* csRequest = new CSRequest();
        send(csRequest, "transceiverOut");
    }
    else{
        //wait for new packets to arrive from generator
    }
}

void MAC::transmit(void){
    AppMessage* msg = buffer.front();
    buffer.pop();
    MacMessage* MacMsg = new MacMessage();
    TransmissionRequest* transmit = new TransmissionRequest();
    EV_INFO << "Packet encapsulated app to mac" << endl;

    MacMsg->encapsulate(msg);
    transmit->encapsulate(MacMsg);
    msg =  nullptr;
    MacMsg = nullptr;
    send(transmit, "transceiverOut");

}

bool MAC::addToBuffer(AppMessage* msg){
    //returns true if packet successfully buffered, false if dropped
    if(buffer.size() < bufferSize){
        buffer.push(msg);
        return true;
    }
    else{
        //drop packet
        delete msg;
        return false;

    }
}


MAC::MAC() {

}

MAC::~MAC() {
    // TODO Auto-generated destructor stub
}

