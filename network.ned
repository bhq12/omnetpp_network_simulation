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

//
// TODO documentation
//
simple Computer
{
    gates:
        input in;
        output out;
}

//
// TODO documentation
//
network Network
{
    submodules:
        computer1: Computer {
            @display("p=132,96");
        }
        computer2: Computer {
            @display("p=390,96");
        }
    connections:
        computer1.out --> computer2.in;
        computer2.out --> computer1.in;
}

network NodeNetwork
{
    
    submodules:
        transmitter1: TransmitterNode {
            @display("p=36,38");
        }
        transmitter2: TransmitterNode {
            @display("p=244,38");
        }
        receiver: ReceiverNode {
            @display("p=140,139");
        }
        chan: WirelessChannel;
    connections:
        transmitter1.port++ <--> chan.connection++;
        transmitter2.port++ <--> chan.connection++;
        receiver.port++ <--> chan.connection++;


}

