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

#include <PacketGenerator.h>


Define_Module(PacketGenerator);

void PacketGenerator::initialize(){
    //this is called at the beginning of the simulation
    cMessage* msg = new cMessage("NEW_MSG");
    scheduleAt(simTime(), msg);

}

void PacketGenerator::handleMessage(cMessage* msg){
    //this is called whenever a msg arrives at the computer
    AppMessage* appMsg = dynamic_cast<AppMessage*>(msg);

    if(!appMsg){
        //not an app message, so must be a transmission schedule message
        AppMessage* appMsg = new AppMessage("assignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsgassignmentMsg");
        int id = getParentModule()->par("nodeIndetifier");
        appMsg -> setSenderId(id);
        send(appMsg, "out");

        //schedule next transmission
        cMessage* newMsg = new cMessage("NEW_MSG");
        scheduleAt(simTime() + 1, newMsg);
    }
    delete msg;
}


PacketGenerator::PacketGenerator() {
    // TODO Auto-generated constructor stub

}

PacketGenerator::~PacketGenerator() {
    // TODO Auto-generated destructor stub
}

