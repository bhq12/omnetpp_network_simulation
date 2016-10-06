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
        int numTransmitters = 10;
        int radius = default(200);
        @display("bgb=0,0");
        
    submodules:

        transmitter[numTransmitters]: TransmitterNode {
            nodeXPosition = radius * cos(2 * 3.14159265 * index / (numTransmitters - 1));
            nodeYPosition = radius * sin(2 * 3.14159265 * index / (numTransmitters - 1));
            mac.backoffDistribution = 0.003;
            mac.maxBackoffs = 5;
            mac.bufferSize = 5;
            transceiver.bitRate = 250000;
            transceiver.csThreshDBm = -50;
            transceiver.csTime = 0.000125;
            transceiver.noisePowerDBm = -120;
            transceiver.turnAroundTime = 0.001;
            transceiver.txPowerDBm = 0;
            generator.messageSize = 64;
            generator.seqno = 0;
            nodeIndetifier = index + 1;
            @display("p=$nodeXPosition,$nodeYPosition"); 
        }
        receiver: ReceiverNode {
            mac.backoffDistribution = 0.003;
            mac.maxBackoffs = 5;
            mac.bufferSize = 5;
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
            @display("p=0,0"); 
        }
    connections:
        for i=0..numTransmitters - 1 {
            transmitter[i].out --> chan.in++;
            chan.out++ --> transmitter[i].in;
        }

        receiver.out --> chan.in++;
        chan.out++ --> receiver.in;
}

