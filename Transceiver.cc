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

void Transceiver::handleMessage(cMessage* msg){
    //this is called whenever a msg arrives at the computer
    AppMessage* appMsg = dynamic_cast<AppMessage*>(msg);
    cModule* parent = getParentModule();
    const char* path = (parent -> getFullPath()).c_str();

    int isTransmitter = (strstr(path, "transmitter") != NULL);

    int id = -1;

    if(isTransmitter){
        id = getParentModule()->par("nodeIndetifier");
    }

    if((appMsg->getSenderId()) == id && isTransmitter){
        send(appMsg, "channelOut");
    }
    else{
        delete msg;
    }
}


Transceiver::Transceiver() {
    // TODO Auto-generated constructor stub

}

Transceiver::~Transceiver() {
    // TODO Auto-generated destructor stub
}

