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
    AppMessage* msg = new AppMessage("assignmentMsg");
    int id = getParentModule()->par("nodeIndetifier");

    msg -> setSenderId(id);
    scheduleAt(simTime(), msg);

}

void PacketGenerator::handleMessage(cMessage* msg){
    //this is called whenever a msg arrives at the computer

    AppMessage* appMsg = dynamic_cast<AppMessage*>(msg);
    int id = getParentModule()->par("nodeIndetifier");
    ASSERT(appMsg -> getSenderId() == id);
    send(appMsg, "out");

    //schedule next transmission
    AppMessage* newMsg = new AppMessage("assignmentMsg");
    newMsg -> setSenderId(id);
    scheduleAt(simTime() + 1, newMsg);


}


PacketGenerator::PacketGenerator() {
    // TODO Auto-generated constructor stub

}

PacketGenerator::~PacketGenerator() {
    // TODO Auto-generated destructor stub
}

