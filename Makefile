CPP  = g++ -D__DEBUG__
CC   = gcc -D__DEBUG__
RES  = 
OBJ  = src/main.o src/main/simulator/common.o src/main/simulator/address/HypercubeAddress.o src/tests/simulator/address/AddressTest.o src/tests/UnitTest.o src/tests/AllTests.o src/tests/simulator/dataUnit/DataUnitTests.o src/main/simulator/layer/PhysicalLayer.o src/main/simulator/event/Event.o src/main/simulator/Units.o src/main/simulator/Simulator.o src/tests/simulator/layer/LayerTests.o src/main/simulator/address/HypercubeMaskAddress.o src/main/simulator/layer/DataLinkLayer.o src/main/simulator/layer/UDPTransportLayer.o src/main/simulator/command/Command.o src/tests/simulator/command/CommandTests.o src/main/simulator/notification/Notification.o src/tests/simulator/notification/NotificationTests.o src/tests/simulator/SimulatorTests.o src/main/simulator/layer/Connection.o src/main/simulator/message/Message.o src/main/simulator/TNode.o src/main/simulator/hypercube/dataUnit/HCPacket.o src/tests/simulator/hypercube/dataUnit/HCPacketTests.o src/main/simulator/hypercube/StateMachines.o src/main/simulator/address/MACAddress.o src/main/simulator/hypercube/Applications.o src/main/simulator/hypercube/routing/ReactiveRouting.o src/main/simulator/address/AddressSpace.o src/main/simulator/hypercube/dataUnit/RouteHeader.o src/main/simulator/hypercube/dataUnit/TOptionalHeader.o src/main/simulator/hypercube/dataUnit/DataPacket.o src/main/simulator/hypercube/routing/Entry.o src/main/simulator/hypercube/routing/NeighbourMapping.o src/main/simulator/hypercube/routing/VisitedBitmap.o src/main/simulator/command/Function.o src/main/simulator/address/UniversalAddress.o src/main/simulator/hypercube/TraceRoute.o src/main/simulator/dataUnit/Frame.o src/main/simulator/dataUnit/Data.o src/main/simulator/dataUnit/UDPSegment.o src/main/simulator/hypercube/RendezVousServer.o src/main/simulator/hypercube/RendezVousPacket.o src/main/simulator/hypercube/RendezVousClient.o src/main/simulator/hypercube/HypercubeControlLayer.o src/main/simulator/hypercube/HypercubeRoutingLayer.o src/main/simulator/hypercube/Neighbour.o src/main/simulator/hypercube/HypercubeParameters.o src/main/simulator/notification/TypeFilter.o src/main/simulator/hypercube/HypercubeNetwork.o src/main/simulator/hypercube/HypercubeNode.o $(RES)
LINKOBJ  = src/main.o src/main/simulator/common.o src/main/simulator/address/HypercubeAddress.o src/tests/simulator/address/AddressTest.o src/tests/UnitTest.o src/tests/AllTests.o src/tests/simulator/dataUnit/DataUnitTests.o src/main/simulator/layer/PhysicalLayer.o src/main/simulator/event/Event.o src/main/simulator/Units.o src/main/simulator/Simulator.o src/tests/simulator/layer/LayerTests.o src/main/simulator/address/HypercubeMaskAddress.o src/main/simulator/layer/DataLinkLayer.o src/main/simulator/layer/UDPTransportLayer.o src/main/simulator/command/Command.o src/tests/simulator/command/CommandTests.o src/main/simulator/notification/Notification.o src/tests/simulator/notification/NotificationTests.o src/tests/simulator/SimulatorTests.o src/main/simulator/layer/Connection.o src/main/simulator/message/Message.o src/main/simulator/TNode.o src/main/simulator/hypercube/dataUnit/HCPacket.o src/tests/simulator/hypercube/dataUnit/HCPacketTests.o src/main/simulator/hypercube/StateMachines.o src/main/simulator/address/MACAddress.o src/main/simulator/hypercube/Applications.o src/main/simulator/hypercube/routing/ReactiveRouting.o src/main/simulator/address/AddressSpace.o src/main/simulator/hypercube/dataUnit/RouteHeader.o src/main/simulator/hypercube/dataUnit/TOptionalHeader.o src/main/simulator/hypercube/dataUnit/DataPacket.o src/main/simulator/hypercube/routing/Entry.o src/main/simulator/hypercube/routing/NeighbourMapping.o src/main/simulator/hypercube/routing/VisitedBitmap.o src/main/simulator/command/Function.o src/main/simulator/address/UniversalAddress.o src/main/simulator/hypercube/TraceRoute.o src/main/simulator/dataUnit/Frame.o src/main/simulator/dataUnit/Data.o src/main/simulator/dataUnit/UDPSegment.o src/main/simulator/hypercube/RendezVousServer.o src/main/simulator/hypercube/RendezVousPacket.o src/main/simulator/hypercube/RendezVousClient.o src/main/simulator/hypercube/HypercubeControlLayer.o src/main/simulator/hypercube/HypercubeRoutingLayer.o src/main/simulator/hypercube/Neighbour.o src/main/simulator/hypercube/HypercubeParameters.o src/main/simulator/notification/TypeFilter.o src/main/simulator/hypercube/HypercubeNetwork.o src/main/simulator/hypercube/HypercubeNode.o $(RES)
INCS =   -I"src/main/simulator/address"  -I"src/main/simulator"  -I"src/tests"  -I"src/tests/simulator"  -I"src/tests/simulator/dataUnit"  -I"src/main/simulator/dataUnit"  -I"src/main/simulator/layer"  -I"src/tests/simulator/layer"  -I"src/main/simulator/event"  -I"src/main/simulator/command"  -I"src/tests/simulator/command"  -I"src/main/simulator/notification"  -I"src/main/simulator/message"  -I"src/main/simulator/hypercube/dataUnit"  -I"src/tests/simulator/hypercube/dataUnit"  -I"src/main/simulator/hypercube"  -I"src/main/simulator/hypercube/routing" 
CXXINCS =   -I"src/main/simulator/address"  -I"src/main/simulator"  -I"src/tests"  -I"src/tests/simulator"  -I"src/tests/simulator/dataUnit"  -I"src/main/simulator/dataUnit"  -I"src/main/simulator/layer"  -I"src/tests/simulator/layer"  -I"src/main/simulator/event"  -I"src/main/simulator/command"  -I"src/tests/simulator/command"  -I"src/main/simulator/notification"  -I"src/main/simulator/message"  -I"src/main/simulator/hypercube/dataUnit"  -I"src/tests/simulator/hypercube/dataUnit"  -I"src/main/simulator/hypercube"  -I"src/main/simulator/hypercube/routing" 
BIN  = quenas
CXXFLAGS = $(CXXINCS)   -g3
CFLAGS = $(INCS)   -g3
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before quenas all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "quenas" $(LIBS)

src/main.o: src/main.cpp
	$(CPP) -c src/main.cpp -o src/main.o $(CXXFLAGS)

src/main/simulator/common.o: src/main/simulator/common.cpp
	$(CPP) -c src/main/simulator/common.cpp -o src/main/simulator/common.o $(CXXFLAGS)

src/main/simulator/address/HypercubeAddress.o: src/main/simulator/address/HypercubeAddress.cpp
	$(CPP) -c src/main/simulator/address/HypercubeAddress.cpp -o src/main/simulator/address/HypercubeAddress.o $(CXXFLAGS)

src/tests/simulator/address/AddressTest.o: src/tests/simulator/address/AddressTest.cpp
	$(CPP) -c src/tests/simulator/address/AddressTest.cpp -o src/tests/simulator/address/AddressTest.o $(CXXFLAGS)

src/tests/UnitTest.o: src/tests/UnitTest.cpp
	$(CPP) -c src/tests/UnitTest.cpp -o src/tests/UnitTest.o $(CXXFLAGS)

src/tests/AllTests.o: src/tests/AllTests.cpp
	$(CPP) -c src/tests/AllTests.cpp -o src/tests/AllTests.o $(CXXFLAGS)

src/tests/simulator/dataUnit/DataUnitTests.o: src/tests/simulator/dataUnit/DataUnitTests.cpp
	$(CPP) -c src/tests/simulator/dataUnit/DataUnitTests.cpp -o src/tests/simulator/dataUnit/DataUnitTests.o $(CXXFLAGS)

src/main/simulator/layer/PhysicalLayer.o: src/main/simulator/layer/PhysicalLayer.cpp
	$(CPP) -c src/main/simulator/layer/PhysicalLayer.cpp -o src/main/simulator/layer/PhysicalLayer.o $(CXXFLAGS)

src/main/simulator/event/Event.o: src/main/simulator/event/Event.cpp
	$(CPP) -c src/main/simulator/event/Event.cpp -o src/main/simulator/event/Event.o $(CXXFLAGS)

src/main/simulator/Units.o: src/main/simulator/Units.cpp
	$(CPP) -c src/main/simulator/Units.cpp -o src/main/simulator/Units.o $(CXXFLAGS)

src/main/simulator/Simulator.o: src/main/simulator/Simulator.cpp
	$(CPP) -c src/main/simulator/Simulator.cpp -o src/main/simulator/Simulator.o $(CXXFLAGS)

src/tests/simulator/layer/LayerTests.o: src/tests/simulator/layer/LayerTests.cpp
	$(CPP) -c src/tests/simulator/layer/LayerTests.cpp -o src/tests/simulator/layer/LayerTests.o $(CXXFLAGS)

src/main/simulator/address/HypercubeMaskAddress.o: src/main/simulator/address/HypercubeMaskAddress.cpp
	$(CPP) -c src/main/simulator/address/HypercubeMaskAddress.cpp -o src/main/simulator/address/HypercubeMaskAddress.o $(CXXFLAGS)

src/main/simulator/layer/DataLinkLayer.o: src/main/simulator/layer/DataLinkLayer.cpp
	$(CPP) -c src/main/simulator/layer/DataLinkLayer.cpp -o src/main/simulator/layer/DataLinkLayer.o $(CXXFLAGS)

src/main/simulator/layer/UDPTransportLayer.o: src/main/simulator/layer/UDPTransportLayer.cpp
	$(CPP) -c src/main/simulator/layer/UDPTransportLayer.cpp -o src/main/simulator/layer/UDPTransportLayer.o $(CXXFLAGS)

src/main/simulator/command/Command.o: src/main/simulator/command/Command.cpp
	$(CPP) -c src/main/simulator/command/Command.cpp -o src/main/simulator/command/Command.o $(CXXFLAGS)

src/tests/simulator/command/CommandTests.o: src/tests/simulator/command/CommandTests.cpp
	$(CPP) -c src/tests/simulator/command/CommandTests.cpp -o src/tests/simulator/command/CommandTests.o $(CXXFLAGS)

src/main/simulator/notification/Notification.o: src/main/simulator/notification/Notification.cpp
	$(CPP) -c src/main/simulator/notification/Notification.cpp -o src/main/simulator/notification/Notification.o $(CXXFLAGS)

src/tests/simulator/notification/NotificationTests.o: src/tests/simulator/notification/NotificationTests.cpp
	$(CPP) -c src/tests/simulator/notification/NotificationTests.cpp -o src/tests/simulator/notification/NotificationTests.o $(CXXFLAGS)

src/tests/simulator/SimulatorTests.o: src/tests/simulator/SimulatorTests.cpp
	$(CPP) -c src/tests/simulator/SimulatorTests.cpp -o src/tests/simulator/SimulatorTests.o $(CXXFLAGS)

src/main/simulator/layer/Connection.o: src/main/simulator/layer/Connection.cpp
	$(CPP) -c src/main/simulator/layer/Connection.cpp -o src/main/simulator/layer/Connection.o $(CXXFLAGS)

src/main/simulator/message/Message.o: src/main/simulator/message/Message.cpp
	$(CPP) -c src/main/simulator/message/Message.cpp -o src/main/simulator/message/Message.o $(CXXFLAGS)

src/main/simulator/TNode.o: src/main/simulator/TNode.cpp
	$(CPP) -c src/main/simulator/TNode.cpp -o src/main/simulator/TNode.o $(CXXFLAGS)

src/main/simulator/hypercube/dataUnit/HCPacket.o: src/main/simulator/hypercube/dataUnit/HCPacket.cpp
	$(CPP) -c src/main/simulator/hypercube/dataUnit/HCPacket.cpp -o src/main/simulator/hypercube/dataUnit/HCPacket.o $(CXXFLAGS)

src/tests/simulator/hypercube/dataUnit/HCPacketTests.o: src/tests/simulator/hypercube/dataUnit/HCPacketTests.cpp
	$(CPP) -c src/tests/simulator/hypercube/dataUnit/HCPacketTests.cpp -o src/tests/simulator/hypercube/dataUnit/HCPacketTests.o $(CXXFLAGS)

src/main/simulator/hypercube/StateMachines.o: src/main/simulator/hypercube/StateMachines.cpp
	$(CPP) -c src/main/simulator/hypercube/StateMachines.cpp -o src/main/simulator/hypercube/StateMachines.o $(CXXFLAGS)

src/main/simulator/address/MACAddress.o: src/main/simulator/address/MACAddress.cpp
	$(CPP) -c src/main/simulator/address/MACAddress.cpp -o src/main/simulator/address/MACAddress.o $(CXXFLAGS)

src/main/simulator/hypercube/Applications.o: src/main/simulator/hypercube/Applications.cpp
	$(CPP) -c src/main/simulator/hypercube/Applications.cpp -o src/main/simulator/hypercube/Applications.o $(CXXFLAGS)

src/main/simulator/hypercube/routing/ReactiveRouting.o: src/main/simulator/hypercube/routing/ReactiveRouting.cpp
	$(CPP) -c src/main/simulator/hypercube/routing/ReactiveRouting.cpp -o src/main/simulator/hypercube/routing/ReactiveRouting.o $(CXXFLAGS)

src/main/simulator/address/AddressSpace.o: src/main/simulator/address/AddressSpace.cpp
	$(CPP) -c src/main/simulator/address/AddressSpace.cpp -o src/main/simulator/address/AddressSpace.o $(CXXFLAGS)

src/main/simulator/hypercube/dataUnit/RouteHeader.o: src/main/simulator/hypercube/dataUnit/RouteHeader.cpp
	$(CPP) -c src/main/simulator/hypercube/dataUnit/RouteHeader.cpp -o src/main/simulator/hypercube/dataUnit/RouteHeader.o $(CXXFLAGS)

src/main/simulator/hypercube/dataUnit/TOptionalHeader.o: src/main/simulator/hypercube/dataUnit/TOptionalHeader.cpp
	$(CPP) -c src/main/simulator/hypercube/dataUnit/TOptionalHeader.cpp -o src/main/simulator/hypercube/dataUnit/TOptionalHeader.o $(CXXFLAGS)

src/main/simulator/hypercube/dataUnit/DataPacket.o: src/main/simulator/hypercube/dataUnit/DataPacket.cpp
	$(CPP) -c src/main/simulator/hypercube/dataUnit/DataPacket.cpp -o src/main/simulator/hypercube/dataUnit/DataPacket.o $(CXXFLAGS)

src/main/simulator/hypercube/routing/Entry.o: src/main/simulator/hypercube/routing/Entry.cpp
	$(CPP) -c src/main/simulator/hypercube/routing/Entry.cpp -o src/main/simulator/hypercube/routing/Entry.o $(CXXFLAGS)

src/main/simulator/hypercube/routing/NeighbourMapping.o: src/main/simulator/hypercube/routing/NeighbourMapping.cpp
	$(CPP) -c src/main/simulator/hypercube/routing/NeighbourMapping.cpp -o src/main/simulator/hypercube/routing/NeighbourMapping.o $(CXXFLAGS)

src/main/simulator/hypercube/routing/VisitedBitmap.o: src/main/simulator/hypercube/routing/VisitedBitmap.cpp
	$(CPP) -c src/main/simulator/hypercube/routing/VisitedBitmap.cpp -o src/main/simulator/hypercube/routing/VisitedBitmap.o $(CXXFLAGS)

src/main/simulator/command/Function.o: src/main/simulator/command/Function.cpp
	$(CPP) -c src/main/simulator/command/Function.cpp -o src/main/simulator/command/Function.o $(CXXFLAGS)

src/main/simulator/address/UniversalAddress.o: src/main/simulator/address/UniversalAddress.cpp
	$(CPP) -c src/main/simulator/address/UniversalAddress.cpp -o src/main/simulator/address/UniversalAddress.o $(CXXFLAGS)

src/main/simulator/hypercube/TraceRoute.o: src/main/simulator/hypercube/TraceRoute.cpp
	$(CPP) -c src/main/simulator/hypercube/TraceRoute.cpp -o src/main/simulator/hypercube/TraceRoute.o $(CXXFLAGS)

src/main/simulator/dataUnit/Frame.o: src/main/simulator/dataUnit/Frame.cpp
	$(CPP) -c src/main/simulator/dataUnit/Frame.cpp -o src/main/simulator/dataUnit/Frame.o $(CXXFLAGS)

src/main/simulator/dataUnit/Data.o: src/main/simulator/dataUnit/Data.cpp
	$(CPP) -c src/main/simulator/dataUnit/Data.cpp -o src/main/simulator/dataUnit/Data.o $(CXXFLAGS)

src/main/simulator/dataUnit/UDPSegment.o: src/main/simulator/dataUnit/UDPSegment.cpp
	$(CPP) -c src/main/simulator/dataUnit/UDPSegment.cpp -o src/main/simulator/dataUnit/UDPSegment.o $(CXXFLAGS)

src/main/simulator/hypercube/RendezVousServer.o: src/main/simulator/hypercube/RendezVousServer.cpp
	$(CPP) -c src/main/simulator/hypercube/RendezVousServer.cpp -o src/main/simulator/hypercube/RendezVousServer.o $(CXXFLAGS)

src/main/simulator/hypercube/RendezVousPacket.o: src/main/simulator/hypercube/RendezVousPacket.cpp
	$(CPP) -c src/main/simulator/hypercube/RendezVousPacket.cpp -o src/main/simulator/hypercube/RendezVousPacket.o $(CXXFLAGS)

src/main/simulator/hypercube/RendezVousClient.o: src/main/simulator/hypercube/RendezVousClient.cpp
	$(CPP) -c src/main/simulator/hypercube/RendezVousClient.cpp -o src/main/simulator/hypercube/RendezVousClient.o $(CXXFLAGS)

src/main/simulator/hypercube/HypercubeControlLayer.o: src/main/simulator/hypercube/HypercubeControlLayer.cpp
	$(CPP) -c src/main/simulator/hypercube/HypercubeControlLayer.cpp -o src/main/simulator/hypercube/HypercubeControlLayer.o $(CXXFLAGS)

src/main/simulator/hypercube/HypercubeRoutingLayer.o: src/main/simulator/hypercube/HypercubeRoutingLayer.cpp
	$(CPP) -c src/main/simulator/hypercube/HypercubeRoutingLayer.cpp -o src/main/simulator/hypercube/HypercubeRoutingLayer.o $(CXXFLAGS)

src/main/simulator/hypercube/Neighbour.o: src/main/simulator/hypercube/Neighbour.cpp
	$(CPP) -c src/main/simulator/hypercube/Neighbour.cpp -o src/main/simulator/hypercube/Neighbour.o $(CXXFLAGS)

src/main/simulator/hypercube/HypercubeParameters.o: src/main/simulator/hypercube/HypercubeParameters.cpp
	$(CPP) -c src/main/simulator/hypercube/HypercubeParameters.cpp -o src/main/simulator/hypercube/HypercubeParameters.o $(CXXFLAGS)

src/main/simulator/notification/TypeFilter.o: src/main/simulator/notification/TypeFilter.cpp
	$(CPP) -c src/main/simulator/notification/TypeFilter.cpp -o src/main/simulator/notification/TypeFilter.o $(CXXFLAGS)

src/main/simulator/hypercube/HypercubeNetwork.o: src/main/simulator/hypercube/HypercubeNetwork.cpp
	$(CPP) -c src/main/simulator/hypercube/HypercubeNetwork.cpp -o src/main/simulator/hypercube/HypercubeNetwork.o $(CXXFLAGS)

src/main/simulator/hypercube/HypercubeNode.o: src/main/simulator/hypercube/HypercubeNode.cpp
	$(CPP) -c src/main/simulator/hypercube/HypercubeNode.cpp -o src/main/simulator/hypercube/HypercubeNode.o $(CXXFLAGS)
