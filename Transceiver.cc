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
    channelPower  = 0;
    latestPacketLength = 0;
    collidedCount = 0;
    errorCount = 0;
}

//TODO: implement turn-around time
void Transceiver::handleTransmissionRequest(TransmissionRequest* transmissionRequest){
    if(transmissionRequest){
        if(state == Receive){
            MacMessage* macMsg = dynamic_cast<MacMessage*>(transmissionRequest->decapsulate());
            //latestPacketLength = macMsg -> getBitLength();
            nextTransmission = macMsg;

            //change transceiver state then wait turnaround time before continuing with transmission
            state = Transmit;
            scheduleAt(simTime() + turnAroundTime, new cMessage("TURNAROUND_TRANSMIT_START_WAIT"));

            delete transmissionRequest;
        }
        else if(state == Transmit){
            //TODO: The MAC with CSMA should ensure we never reach this state, but we should prepare for it in case of error
        }
    }
}

void Transceiver::handleInternalSignals(cMessage* msg){
    const char* name = msg->getName();
    if (strcmp("END_TRANSMISSION", name) == 0){
        delete msg;
        SignalEndMessage* endMessage = new SignalEndMessage();
        int id = getParentModule()->par("nodeIndetifier");
        endMessage -> setSenderId(id);
        SignalStartMessage* startMsg = findAssociatedTransmission(endMessage);
        delete startMsg;//transmission has ended, delete the message
        send(endMessage, "channelOut");
        //TODO: add status parameter to TransmissionConfirm packet, see 8.4 bullet pt 2
        send (new TransmissionConfirm(), "macOut");
        //change transceiver state to receive
        scheduleAt(simTime() + turnAroundTime, new cMessage("TURNAROUND_TRANSMIT_FINISHED_WAIT"));

    }
    else if (strcmp("CARRIER_SENSE_WAIT", name) == 0){
        delete msg;

        bool isBusy = channelPower > csThreshDBm;

        EV_INFO << "in recv state, Channel is busy: " << isBusy << "." << endl;

        CSResponse* response = new CSResponse();
        response -> setBusyChannel(isBusy);
        send(response, "macOut");
    }
    else if (strcmp("TURNAROUND_TRANSMIT_FINISHED_WAIT", name) == 0){
        //change transceiver state after waiting for turnaround time
        delete msg;

        state = Receive;

    }
    else if (strcmp("TURNAROUND_TRANSMIT_START_WAIT", name) == 0){
        //start transmission after waiting for the state turnaround time
        delete msg;
        SignalStartMessage* startMessage = new SignalStartMessage();
        startMessage -> encapsulate(nextTransmission);
        startMessage -> setTransmitPowerDBm(txPowerDBm);
        int id = getParentModule()->par("nodeIndetifier");
        startMessage -> setSenderId(id);
        startMessage -> setCollidedFlag(false);
        startMessage -> setPositionX(getParentModule()->par("nodeXPosition"));
        startMessage -> setPositionY(getParentModule()->par("nodeYPosition"));
        startMessage -> setTransmitPowerDBm(txPowerDBm);
        handleSignalStartMessage(startMessage);
        send(startMessage, "channelOut");
        double bitLength = nextTransmission -> getBitLength();
        double messageLength = bitLength / (double)bitRate;
        scheduleAt(simTime() + messageLength, new cMessage("END_TRANSMISSION"));
    }
}

void Transceiver::refreshDisplay() const
{
    char buf[40];
    sprintf(buf, "Errors: %ld   Collisions: %ld     ", errorCount, collidedCount);
    getDisplayString().setTagArg("t", 0, buf);
}

void Transceiver::handleSignalStartMessage(SignalStartMessage* startMsg){
    if (startMsg){
        //TODO: according to spec: when receiving a signal start msg, extract the ID field and check whether it
        //equals the ID field of any of the start messages stored in the current transmissions list. If so,
        //print an error message and abort the program. See section 8.5 paragraph 2

        bool hasCollided = currentTransmissions.size() != 0;

        currentTransmissions.push_back(startMsg);
        if(hasCollided){
            //other nodes already transmitting, collision has occured
            int i;
            for(i = 0; i < currentTransmissions.size(); i++){
                SignalStartMessage* startMsg = currentTransmissions[i];
                startMsg->setCollidedFlag(true);
            }
        }
    }
}

void Transceiver::handleSignalEndMessage(SignalEndMessage* endMsg){
    if (endMsg){
        SignalStartMessage* startMsg = findAssociatedTransmission(endMsg);


        if(startMsg -> getCollidedFlag()){
            //drop the packet
            EV_INFO << "collided packet dropped" << endl;
            collidedCount++;
        }
        else{
            MacMessage* macMsg = static_cast<MacMessage*>(startMsg->decapsulate());

            double receivedPower = findPacketPowerDB(startMsg);

            double bitRateDB = ratioToDB(bitRate);

            double Eb_N0 = receivedPower - (noisePowerDBm + bitRateDB);

            double snr = DBToRatio(Eb_N0);

            double bitErrorRate = erfc(sqrt(2 * snr));
            double packetClearRate = pow((1 - bitErrorRate), macMsg->getBitLength());
            double packetErrorRate = 1 - packetClearRate;

            //draw a random number to simulate whether the packet is erroneous
            double packetErrorIndicator = uniform(0,1);

            if(packetErrorIndicator < packetErrorRate){
                //drop the packet
                errorCount++;
                EV_INFO << "erroneous packet dropped" << endl;
            }
            else{
                TransmissionIndication* transmissionIndication = new TransmissionIndication();
                transmissionIndication->encapsulate(macMsg);
                send(transmissionIndication, "macOut");
            }


        }


        delete startMsg;
        delete endMsg;

    }
}


SignalStartMessage* Transceiver::findAssociatedTransmission(SignalEndMessage* endMsg){
    if(endMsg){
        int i;
        for(i = 0; i < currentTransmissions.size(); i++){
            int endMessageId = endMsg->getSenderId();
            SignalStartMessage* startMsg = currentTransmissions[i];
            int id = startMsg->getSenderId();
            if (id == endMessageId){
                currentTransmissions.erase(currentTransmissions.begin()+i-1);
                return startMsg;
            }
        }
    }
    return nullptr;
}

void Transceiver::handleCSRequest(CSRequest* csRequest){
    //detect channel if channel busy, send back csResponse with result
    if(csRequest){
        EV_INFO << "Packet deeleted request" << endl;

        delete csRequest;

        if(state == Receive){


            //calculate the current channel signal power
            channelPower = findChannelPowerDB();


            scheduleAt(simTime() + csTime, new cMessage("CARRIER_SENSE_WAIT"));

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

    return ratioToDB(totalPowerRatio);
}

double Transceiver::findPacketPowerDB(SignalStartMessage* msg){

    double distance = findTransmitterDistance(msg);

    double powerLossDb = findPowerLossDB(distance);

    double messagePower = msg->getTransmitPowerDBm() - powerLossDb;
    return messagePower;
}

double Transceiver::findTransmitterDistance(SignalStartMessage* msg){
    int transmitterX = msg->getPositionX();
    int transmitterY = msg->getPositionY();

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

    double powerLossDb = ratioToDB(powerLossRatio);
    return powerLossDb;
}

double Transceiver::DBToRatio(double num){
    return pow(10, num/10);
}

double Transceiver::ratioToDB(double num){
    return 10 * log10(num);
}
Transceiver::Transceiver() {
    // TODO Auto-generated constructor stub

}

Transceiver::~Transceiver() {
    // TODO Auto-generated destructor stub
}

