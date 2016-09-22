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
            send(MacMsg, "channelOut");
    }
}

void Transceiver::handleCSRequest(CSRequest* csRequest){
    //detect channel if channel busy, send back csResponse with result
    if(csRequest){

        //TEMPORARY for testing MAC
        //
        //
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

    //TODO: the transceiver should never get an app message; only MAC, physical and
    //carrier sense messages. Implement creation of MAC messages at MAC layer then replace
    //this with handleMACMessage()
    MacMessage* MacMsg = dynamic_cast<MacMessage*>(msg);
    handleMacMessage(MacMsg);

    CSRequest* csRequest = dynamic_cast<CSRequest*>(msg);
    handleCSRequest(csRequest);



    //NOTE: the following code is not implementing any of the specification
    //it is just holding together memory leaks until full implementation
    //is complete
    //
    //
    //cModule* parent = getParentModule();
    //const char* path = (parent -> getFullPath()).c_str();

    //int isTransmitter = (strstr(path, "transmitter") != NULL);
    //int id = -1;
//    if(isTransmitter){
//        id = parent->par("nodeIndetifier");
//    }
//
//    if(!isTransmitter || id != appMsg->getSenderId()){
//        delete msg;
//    }
    //
    //
    //end of memory leak code
}


Transceiver::Transceiver() {
    // TODO Auto-generated constructor stub

}

Transceiver::~Transceiver() {
    // TODO Auto-generated destructor stub
}

