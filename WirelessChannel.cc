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

#include <WirelessChannel.h>

Define_Module(WirelessChannel);

void WirelessChannel::initialize(){
    //this is called at the beginning of the simulation
    //if(strcmp("computer1", getName()) == 0){
        //cMessage* msg = new cMessage("assignmentMsg");
        //send(msg, "out");
    //}

}

void WirelessChannel::handleMessage(cMessage* msg){
    //this is called whenever a msg arrives at the computer

    int gates = gateCount() / 2;
    int senderId = findSenderId(msg);


    for (int i = 0; i < gates; i++)
    {

        if(senderId == -1 || senderId != i){
            EV_INFO << "Packet duplicated" << endl;
            cMessage* copy = msg->dup();
            send(copy, "out", i);
        }
    }
    if(dynamic_cast<SignalEndMessage*>(msg)){
        delete msg;
    }
    EV_INFO << "Packet deleted" << endl;
}

int WirelessChannel::findSenderId(cMessage* msg){
    int senderId = -1;

    SignalStartMessage* startMsg = dynamic_cast<SignalStartMessage*>(msg);
    SignalEndMessage* endMsg = dynamic_cast<SignalEndMessage*>(msg);

    if(startMsg){
        MacMessage* macMsg = dynamic_cast<MacMessage*>(startMsg->getEncapsulatedPacket());
        AppMessage* appMsg = dynamic_cast<AppMessage*>(macMsg->getEncapsulatedPacket());
        senderId = appMsg -> getSenderId();
    }
    else if(endMsg){
        senderId = endMsg -> getSenderId();
    }
    return senderId;
}

WirelessChannel::WirelessChannel() {
    // TODO Auto-generated constructor stub

}

WirelessChannel::~WirelessChannel() {
    // TODO Auto-generated destructor stub
}

