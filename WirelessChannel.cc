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

    AppMessage* appMsg = dynamic_cast<AppMessage*>(msg);
    int senderId = appMsg->getSenderId();

    //TODO: detect the number of connections to the channel rather than hardcoded 3
    for (int i = 0; i < 3; i++)
    {
        //don't forward msgs back to the original sender
        if(i != senderId){
            cMessage *copy = msg->dup();
            send(copy, "out", i);
        }

    }
    delete msg;
}


WirelessChannel::WirelessChannel() {
    // TODO Auto-generated constructor stub

}

WirelessChannel::~WirelessChannel() {
    // TODO Auto-generated destructor stub
}

