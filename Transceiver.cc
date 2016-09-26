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

#include <Transceiver.h>

Define_Module(Transceiver);

void Transceiver::initialize(){
    //this is called at the beginning of the simulation
    /*if(strcmp("computer1", getName()) == 0){
        cMessage* msg = new cMessage("assignmentMsg");
        send(msg, "out");
    }*/

}

void Transceiver::handleMacMessage(MacMessage* MacMsg){
    if(MacMsg){
            EV_INFO << "Packet encapsulated mac to phys" << endl;

            PhysicalMessage* phyMsg = new PhysicalMessage();
            phyMsg->encapsulate(MacMsg);
            MacMsg = nullptr;
            send(phyMsg, "channelOut");
    }
}

void Transceiver::handlePhysicalMessage(PhysicalMessage* phyMsg){
    if(phyMsg){
        EV_INFO << "Packet decapsulated phys to mac" << endl;

        MacMessage* MacMsg = dynamic_cast<MacMessage*>(phyMsg->decapsulate());
        send(MacMsg, "macOut");
        delete phyMsg;

    }
}

void Transceiver::handleCSRequest(CSRequest* csRequest){
    //detect channel if channel busy, send back csResponse with result
    if(csRequest){
        EV_INFO << "Packet deeleted request" << endl;

        delete csRequest;


        //TEMPORARY for testing MAC
        //
        //
        EV_INFO << "Packet created reponse" << endl;

        CSResponse* response = new CSResponse();
        response -> setBusyChannel(false);
        send(response, "macOut");
        //
        //
        //TEMPORARY

        if(state == Receive){
            //calculate the current channel signal power
            //double channlePower = gimmePowerPlz();

            //wait for carrier sense time delay
            //threadWait(csTime); //or maybe schedule an event?

            //decide if the channel is busy
            //bool isBusy = channelPower > csThreshDBm;

            //CsResponse* response = new CSResponse();
            //response -> setBusyChannel(isBusy);
            //send(response, "macOut");

        }
        else{
            //need to decide what to do when in busy state, see spec section 8.3
        }

    }

}

void Transceiver::handleMessage(cMessage* msg){
    //this is called whenever a msg arrives at the computer
    EV_INFO << "Packet casted" << endl;

    MacMessage* MacMsg = dynamic_cast<MacMessage*>(msg);
    CSRequest* csRequest = dynamic_cast<CSRequest*>(msg);
    PhysicalMessage* phyMsg = dynamic_cast<PhysicalMessage*>(msg);

    handleMacMessage(MacMsg);
    handleCSRequest(csRequest);
    handlePhysicalMessage(phyMsg);
}


Transceiver::Transceiver() {
    // TODO Auto-generated constructor stub

}

Transceiver::~Transceiver() {
    // TODO Auto-generated destructor stub
}

