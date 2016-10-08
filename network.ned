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


network NodeNetwork
{
    parameters:
        int numTransmitters = default(10);
        int radius = default(20);
        @display("bgb=0,0");
        
    submodules:

        transmitter[numTransmitters]: TransmitterNode {
            nodeIndetifier = index;
            nodeXPosition = radius * cos(2 * 3.14159265 * index / (numTransmitters - 1));
            nodeYPosition = radius * sin(2 * 3.14159265 * index / (numTransmitters - 1));
            mac.backoffDistribution = 0.003;
            mac.maxBackoffs = 5;
            mac.bufferSize = 5;
            mac.logFileName = "analysis/outputMac";
            transceiver.bitRate = 250000;
            transceiver.csThreshDBm = -50;
            transceiver.csTime = 0.000125;
            transceiver.noisePowerDBm = -120;
            transceiver.turnAroundTime = 0.001;
            transceiver.txPowerDBm = 0;
            transceiver.logFileName = "analysis/outputTx.txt";
            generator.messageSize = 512;
            generator.seqno = 0;
            generator.iatDistribution = 0.01;
            @display("p=$nodeXPosition,$nodeYPosition"); 
        }
        receiver: ReceiverNode {
            mac.backoffDistribution = 0.003;
            mac.maxBackoffs = 5;
            mac.bufferSize = 5;
            mac.logFileName = "analysis/useless";  
            transceiver.bitRate = 250000;
            transceiver.csThreshDBm = -50;
            transceiver.csTime = 0.000125;
            transceiver.noisePowerDBm = -120;
            transceiver.turnAroundTime = 0.001;
            transceiver.txPowerDBm = 0;
            generator.messageSize = 64;
            generator.seqno = 0;
            nodeXPosition = 0;
            nodeYPosition = 0;
            @display("p=$nodeXPosition,$nodeYPosition"); 
        }
        chan: WirelessChannel {
            @display("p=0,100"); 
        }
    connections:
        for i=0..numTransmitters - 1 {
            transmitter[i].out --> chan.in++;
        }
        for i=0..numTransmitters - 1 {
            chan.out++ --> transmitter[i].in;
        }

        receiver.out --> chan.in++;
        chan.out++ --> receiver.in;
}

