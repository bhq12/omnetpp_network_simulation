/*
 * computer.cc
 *
 *  Created on: 12/09/2016
 *      Author: Brook
 */

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Computer : public cSimpleModule {
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage* msg);
};

Define_Module(Computer);

void Computer::initialize(){
    //this is called at the beginning of the simulation
    if(strcmp("computer1", getName()) == 0){
        cMessage* msg = new cMessage("assignmentMsg");
        send(msg, "out");
    }

}

void Computer::handleMessage(cMessage* msg){
    //this is called whenever a msg arrives at the computer
    send(msg, "out");
}
