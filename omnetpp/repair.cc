#include <omnetpp.h>
#include <iostream>

using namespace omnetpp;

double avg_time;

class Server : public cSimpleModule
{
    simtime_t timeoffline = 0;
    simtime_t timeon;
    simtime_t timeoff;
    cMessage *timer;
    int attack_interval;
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

Define_Module(Server);

void Server::initialize()
{
    attack_interval = par("attack");
    // Waiting for attack
    timer = new cMessage("server_attack");
    scheduleAt(simTime() + exponential(attack_interval), timer);
}

void Server::handleMessage(cMessage *msg)
{
    if (msg == timer) {
        int percentVal = intuniform(1, 100);
        if (percentVal >= 1 && percentVal <= 20) { // 20%, hard attack
            bubble("HARD DOWN");
            cMessage *server_down_hard = new cMessage("server_down_hard");
            timeoff = simTime();
            send(server_down_hard, "out");
        } else if (percentVal >= 21 && percentVal <= 50) { // 30%, low attack
            bubble("LOW DOWN");
            cMessage *server_down_low = new cMessage("server_down_low");
            timeoff = simTime();
            send(server_down_low, "out");
        } else if (percentVal >= 51) { // 50%, auto-repelled the attack
            // Waiting for further attack
            bubble("REPELLED");
            scheduleAt(simTime() + exponential(attack_interval), timer);
        }
    }

    if (strcmp(msg->getName(), "server_repaired") == 0) {
        bubble("REPAIRED");
        timeon = simTime();
        timeoffline += (timeon - timeoff);
//        EV << "TIMEOFFLINE: " << timeoffline  << endl;
//        EV << "TIMEDIFF: " << (timeon - timeoff)  << endl;
        // Waiting for further attack
        cancelAndDelete(msg);
        scheduleAt(simTime() + exponential(attack_interval), timer);
    }
}

void Server::finish() {
    avg_time += timeoffline.dbl();
    cancelAndDelete(timer);
//    EV << "Server offline time: " << timeoffline << endl;
}

class Masters : public cSimpleModule
{
    int masters;
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

Define_Module(Masters);

void Masters::initialize()
{
    masters = par("number_of_masters");
    avg_time = 0;
}

void Masters::handleMessage(cMessage *msg)
{
    cMessage *server_repaired = new cMessage("server_repaired");

    if (strcmp(msg->getName(), "server_down_hard") == 0) {
        sendDelayed(server_repaired, simTime() + intuniform(6, 18), "out", msg->getArrivalGate()->getIndex()); // 12+-6h
        cancelAndDelete(msg);
    } else if (strcmp(msg->getName(), "server_down_low") == 0) {
        sendDelayed(server_repaired, simTime() + intuniform(1, 9), "out", msg->getArrivalGate()->getIndex()); // 5+-4h
        cancelAndDelete(msg);
    }
}

void Masters::finish() {
    // EV << "MASTERS UTIL: " << avg_time / (5*masters*8766) << endl;
    std::cout << "MASTERS: " << masters << endl;
    std::cout << "MASTERS UTIL: " << avg_time / (5*masters*8766) << endl;
}
