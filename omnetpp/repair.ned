simple Server
{
    parameters:
        int attack;
        @display("i=block/routing");
    gates:
        input in;
        output out;
}

simple Masters
{
    parameters:
        int number_of_masters;
        @display("i=block/process");
    gates:
        input in[];
        output out[];
}

network Repair
{
    @display("bgb=506,220");
    submodules:
        server1: Server {
            parameters:
                attack = 20;
                @display("i=,cyan");
        }
        server2: Server {
            parameters:
                attack = 30;
                @display("i=,cyan");
        }
        server3: Server {
            parameters:
                attack = 26;
                @display("i=,cyan");
        }
        server4: Server {
            parameters:
                attack = 50;
                @display("i=,cyan");
        }
        server5: Server {
            parameters:
                attack = 64;
                @display("i=,cyan");
        }
        masters: Masters {
            parameters:
                @display("i=,gold");
        }
    connections:
        masters.out++ --> server1.in;
        masters.in++ <-- server1.out;

        masters.out++ --> server2.in;
        masters.in++ <-- server2.out;

        masters.out++ --> server3.in;
        masters.in++ <-- server3.out;

        masters.out++ --> server4.in;
        masters.in++ <-- server4.out;

        masters.out++ --> server5.in;
        masters.in++ <-- server5.out;
}

