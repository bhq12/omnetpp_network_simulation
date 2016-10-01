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

    txPowerDBm = par("txPowerDBm");
    bitRate = par("bitRate");//measured in bits/s
    csThreshDBm = par("csThreshDBm");//threshold used to detect whether the medium is busy
    noisePowerDBm = par("noisePowerDBm");
    turnAroundTime = par("turnAroundTime");
    csTime = par("csTime");

    state = Receive;

    nodeXPosition = getParentModule()->par("nodeXPosition");
    nodeYPosition = getParentModule()->par("nodeYPosition");
}

//TODO: implement turn-around time
void Transceiver::handleTransmissionRequest(TransmissionRequest* transmissionRequest){
    if(transmissionRequest){
        SignalStartMessage* startMessage = new SignalStartMessage();
        startMessage -> setTransmitPowerDBm(txPowerDBm);
        MacMessage* macMsg = dynamic_cast<MacMessage*>(transmissionRequest->decapsulate());
        int packetLength = macMsg -> getBitLength();
        delete transmissionRequest;
        delete macMsg;

        state = Transmit;
        send(startMessage, "channelOut");
        //TODO: implement transmission confirmation, probably need to do this on end transmission
        //send (new TransmissionConfirm(), "macOut");

        scheduleAt(simTime() + (packetLength / bitRate), new cMessage("END_TRANSMISSION"));
    }
}

void Transceiver::handleInternalSignals(cMessage* msg){
    const char* name = msg->getName();
    if (strcmp("END_TRANSMISSION", name) == 0){
        delete msg;
        SignalEndMessage* endMessage = new SignalEndMessage();
        send(endMessage, "channelOut");
        send (new TransmissionConfirm(), "macOut");
        state = Receive;

    } else if (strcmp("WAIT_OVER", name) == 0){
        delete msg;

        bool isBusy = channelPower > csThreshDBm;

        EV_INFO << "in recv state, Channel is busy: " << isBusy << "." << endl;

        CSResponse* response = new CSResponse();
        response -> setBusyChannel(isBusy);
        send(response, "macOut");
    }
}

void Transceiver::handleSignalStartMessage(SignalStartMessage* startMsg){
    if (startMsg){
        delete startMsg;

        //TODO: want to add to current transmissions vector, then delete after stop is received
        //currentTransmissions.insert(currentTransmissions.begin(), startMsg)
        state = Receive;
    }
}

void Transceiver::handleSignalEndMessage(SignalEndMessage* endMsg){
    if (endMsg){
        delete endMsg;
        state = Receive;
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
        //EV_INFO << "Packet created reponse" << endl;

        //CSResponse* response = new CSResponse();
        //response -> setBusyChannel(false);
        //send(response, "macOut");
        //
        //
        //TEMPORARY

        if(state == Receive){


            //calculate the current channel signal power
            channelPower = findChannelPowerDB();


            scheduleAt(simTime() + csTime, new cMessage("WAIT_OVER"));

        }
        else{
            EV_INFO << "in transmit state" << endl;
            CSResponse* response = new CSResponse();
            response -> setBusyChannel(true);
            send(response, "macOut");
            //need to decide what to do when in busy state, see spec section 8.3
        }

    }

}

void Transceiver::handleMessage(cMessage* msg){
    //this is called whenever a msg arrives at the computer

    EV_INFO << "Packet casted" << endl;

    TransmissionRequest* transmissionRequest = dynamic_cast<TransmissionRequest*>(msg);
    CSRequest* csRequest = dynamic_cast<CSRequest*>(msg);
    SignalEndMessage* endMsg = dynamic_cast<SignalEndMessage*>(msg);
    SignalStartMessage* startMsg = dynamic_cast<SignalStartMessage*>(msg);


    handleInternalSignals(msg);
    handleTransmissionRequest(transmissionRequest);
    handleCSRequest(csRequest);
    handleSignalEndMessage(endMsg);
    handleSignalStartMessage(startMsg);

}

double Transceiver::findChannelPowerDB(){
    //returns the current channel power in Db
    double totalPowerRatio = 0.0;

    auto i = currentTransmissions.begin();

    while(i != currentTransmissions.end()){

        double packetPowerDB = findPacketPowerDB(*i);
        double packetPowerRatio = DBToRatio(packetPowerDB);
        totalPowerRatio += packetPowerRatio;

        i++;
    }

    return RatioToDB(totalPowerRatio);
}

double Transceiver::findPacketPowerDB(SignalStartMessage* msg){

    double distance = findTransmitterDistance(msg);

    double powerLossDb = findPowerLossDB(distance);

    double messagePower = msg->getTransmitPowerDBm() - powerLossDb;
    return messagePower;
}

double Transceiver::findTransmitterDistance(SignalStartMessage* msg){
    int transmitterX = msg->getPositionX();
    int transmitterY = msg->getPositionX();

    int xDisplacement = abs(transmitterX - nodeXPosition);
    int yDisplacement = abs(transmitterY - nodeYPosition);

    //c^2 = a^2 + b^2 euclidian distance
    double distance = sqrt(xDisplacement * xDisplacement + yDisplacement * yDisplacement);

    return distance;
}

double Transceiver::findPowerLossDB(double distance){
    //d0, see section 8.5 page 14 of spec
    double referenceDistance = 1.0;
    double powerLossRatio = 1.0;

    if(distance < referenceDistance){
        powerLossRatio = 1.0; //no power loss
    }
    else{
        powerLossRatio = pow(distance, pathLossExponent);
    }

    double powerLossDb = RatioToDB(powerLossRatio);
    return powerLossDb;
}

double Transceiver::DBToRatio(double num){
    return pow(10, num/10);
}

double Transceiver::RatioToDB(double num){
    return 10 * log10(num);
}
Transceiver::Transceiver() {
    // TODO Auto-generated constructor stub

}

Transceiver::~Transceiver() {
    // TODO Auto-generated destructor stub
}

