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


module TransmitterNode extends Node{
    parameters:
        int nodeIndetifier;      
    submodules:
        generator: PacketGenerator;
    connections:
        generator.out++ --> mac.in++;
        mac.out++ --> transceiver.in++;
        mac.in++ <-- transceiver.out++;
        transceiver.out++ --> out++;
}
